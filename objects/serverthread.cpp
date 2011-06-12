/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Anime Renderfarm - A remote batch renderer for Anime Studio            *
 *  Copyright (C) 2011 Batty Bovine Productions                            *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "serverthread.h"

ServerThread::ServerThread(QObject *parent) :
    QThread(parent),
    server(NULL), client(NULL), bindport(26463)
{
    temppath = QDir::toNativeSeparators(QDesktopServices::storageLocation(
        QDesktopServices::TempLocation)) + QDir::separator();

    connect(this, SIGNAL(initServer()), this, SLOT(startServer()));
    connect(this, SIGNAL(serverStatus(QString)), parent, SLOT(getStatus(QString)));
}

ServerThread::~ServerThread()
{
    QDir path; path.rmpath(temppath);

    if(client)
        client->deleteLater();
    if(server)
        server->deleteLater();
}



void ServerThread::setBindIP(QString in)
{
    if(!in.isEmpty())
        bindip = in;
}

void ServerThread::setBindPort(quint16 in)
{
    bindport = in;
}



void ServerThread::run()
{
    if(server && server->isListening())
        return;

    emit initServer();
}

void ServerThread::startServer()
{
    server = new QTcpServer(this);
    server->setMaxPendingConnections(1);
    if(bindip.isEmpty())
        server->listen(QHostAddress::Any, bindport);
    else
        server->listen(QHostAddress(bindip), bindport);

    if(server->isListening())
        emit serverStatus(tr("Awaiting connection on %1:%2")
                          .arg(server->serverAddress().toString())
                          .arg(QString::number(server->serverPort())));
    else
        emit serverStatus(tr("Server could not be started: %1").arg(server->errorString()));

    connect(server, SIGNAL(newConnection()), this, SLOT(beginTransfer()));
}



void ServerThread::beginTransfer()
{
    // If the server already has an active connection, tell it we're busy right now
    if(client && client->isValid()) {
        QTcpSocket *cl = server->nextPendingConnection();
        comm.writeData(cl, "busy");
        cl->deleteLater();
    }

    // If we're free, get the new connection and tell it to clean the pointer when disconnected
    client = server->nextPendingConnection();
    emit serverStatus(tr("Connected to client: ")+client->localAddress().toString());
    connect(client, SIGNAL(disconnected()), this, SLOT(cleanup()));
    // Send a request for the project file
    connect(client, SIGNAL(readyRead()), this, SLOT(getProjectName()));
    comm.writeData(client, "filename");
}

void ServerThread::getProjectName()
{
    // Read the data from the client; if there is none prepared, wait
    filename = comm.readData(client);
    if(filename.isNull())
        return;

    // Reconnect the readyRead() signal, and request the project file
    if(disconnect(SIGNAL(readyRead()))) {
        // Update our temp path to include a new directory for our rendered project
        temppath += QCryptographicHash::hash(filename.toUtf8(), QCryptographicHash::Sha1).toHex();
        temppath += QDir::separator();
        QDir path; path.mkpath(temppath);

        connect(client, SIGNAL(readyRead()), this, SLOT(getProjectFile()));
    }
    comm.writeData(client, "project");
}

void ServerThread::getProjectFile()
{
    // Read the data from the client; if there is none prepared, wait
    QByteArray data = comm.readData(client);
    if(data.isNull())
        return;

    QFile infile(temppath+filename);
    if(!infile.open(QFile::WriteOnly))
        return;
    infile.write(data);
    infile.close();
}



void ServerThread::cleanup() {
    // At this point, we will no longer need the client pointer
    client->deleteLater();
    client = NULL;
    // Reset the temporary path to the root of the user's temporary location
    temppath = QDir::toNativeSeparators(QDesktopServices::storageLocation(
        QDesktopServices::TempLocation)) + QDir::separator();
    // Set the filename to a null string
    filename = QString::null;
}
