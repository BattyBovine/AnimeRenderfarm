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
    server(NULL), client(NULL), renderproc(NULL), bindport(26463)
{
    temppath = QDir::toNativeSeparators(QDesktopServices::storageLocation(
        QDesktopServices::TempLocation)) + QDir::separator();

    connect(this, SIGNAL(initServer()), this, SLOT(startServer()));
    connect(this, SIGNAL(renderStart()), this, SLOT(startRenderThread()));
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
    if(client && client->isValid() && client->isOpen()) {
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
    comm.writeData(client, "projectname");
}

void ServerThread::getProjectName()
{
    // Read the data from the client; if there is none prepared, wait
    projectname = comm.readData(client);
    if(projectname.isNull())
        return;

    // If the readyRead() signal is disconnected properly...
    if(client->disconnect(SIGNAL(readyRead()))) {
        // ...update our temp path to include a new directory for our rendered project...
        temppath += QCryptographicHash::hash(projectname.toUtf8(),
                                             QCryptographicHash::Sha1).toHex();
        temppath += QDir::separator();
        QDir path; path.mkpath(temppath);
        // ...and get ready to receive the actual file
        connect(client, SIGNAL(readyRead()), this, SLOT(getProjectFile()));
        comm.writeData(client, "project");
    }
}

void ServerThread::getProjectFile()
{
    // Read the data from the client; if there is none prepared, wait
    QByteArray data = comm.readData(client);
    if(data.isNull())
        return;

    // Save the file to our temporary directory with the given project name
    QFile infile(temppath+projectname);
    if(!infile.open(QIODevice::WriteOnly))
        return;
    infile.write(data);
    infile.close();

    embedlist = processEmbeddedFiles(temppath+projectname);

    if(client->disconnect(SIGNAL(readyRead()))) {
        if(embedlist.isEmpty()) {
            connect(client, SIGNAL(readyRead()), this, SLOT(getCommandLine()));
            comm.writeData(client, "cmd");
        } else {
            connect(client, SIGNAL(readyRead()), this, SLOT(getEmbeddedFile()));
            comm.writeData(client, "embed:"+embedlist.first().first.toUtf8());
        }
    }
}

void ServerThread::getEmbeddedFile()
{
    // Read the data from the client; if there is none prepared, wait
    QByteArray data = comm.readData(client);
    if(data.isNull())
        return;

    // Save the embedded file to our temporary directory with its original filename
    QFile infile(temppath+embedlist.first().second);
    if(!infile.open(QIODevice::WriteOnly))
        return;
    infile.write(data);
    infile.close();

    embedlist.removeFirst();

    if(client->disconnect(SIGNAL(readyRead()))) {
        if(embedlist.isEmpty()) {
            connect(client, SIGNAL(readyRead()), this, SLOT(getCommandLine()));
            comm.writeData(client, "cmd");
        } else {
            connect(client, SIGNAL(readyRead()), this, SLOT(getEmbeddedFile()));
            comm.writeData(client, "embed:"+embedlist.first().first.toUtf8());
        }
    }
}

void ServerThread::getCommandLine()
{
    // Read the data from the client; if there is none prepared, wait
    QString data = comm.readData(client);
    if(data.isNull())
        return;

    QStringList cmd = data.split(":");

    emit renderStart();
}

void ServerThread::startRenderThread()
{
//    renderproc = new RenderThread(this);
//    renderproc->setExe(exe);
//    renderproc->setCmd(cmd);
//    connect(renderproc, SIGNAL(renderComplete(QPair<QString,QString>)),
//            this, SLOT(cleanup()));
//    renderproc->start();
}



QList< QPair<QString,QString> > ServerThread::processEmbeddedFiles(QString filepath)
{
    QFile in(filepath);
    if(!in.open(QIODevice::ReadOnly))
        return QList< QPair<QString,QString> >();
    QTextStream intext(&in);

    // First, we're going to get the original file, and collect and replace the embed paths
    embedlist.clear();
    QStringList inlist;
    while(!intext.atEnd()) {
        QString line = intext.readLine();
        QRegExp regex("(image|audio_file|external_model)\\s+\"(.*)\"");
        if(regex.indexIn(line)>=0) {
            if(!regex.cap(2).isEmpty()) {
                // The files will be represented by a QString pair
                QPair<QString,QString> fnames;
                // The first in the pair will be the full path as embedded into the project file
                fnames.first = regex.cap(2);
                // The second in the pair will be the filename only
                // (separators are always '/' in project files)
                fnames.second = fnames.first.mid(fnames.first.lastIndexOf('/')+1);
                // This new, file-name only path will be the new path of the file
                line.replace(fnames.first, fnames.second);
                if(!embedlist.contains(fnames))
                    embedlist << fnames;
                if(fnames.first.endsWith(".obj")) {
                    fnames.first.replace(".obj", ".mtl");
                    fnames.second.replace(".obj", ".mtl");
                    embedlist << fnames;
                }
            }
        }
        inlist << line;
    }
    in.close();

    // Next, we re-save the old file using our QStringList containing the lines
    QFile out(filepath);
    if(!out.open(QIODevice::WriteOnly))
        return embedlist;
    QTextStream outtext(&out);
    foreach(QString ln, inlist)
        outtext << ln << endl;
    out.close();

    return embedlist;
}

void ServerThread::cleanup()
{
    // At this point, we will no longer need the client or render pointers
    client->deleteLater();
    client = NULL;
    renderproc->deleteLater();
    renderproc = NULL;
    // Reset the temporary path to the root of the user's temporary location
    temppath = QDir::toNativeSeparators(QDesktopServices::storageLocation(
        QDesktopServices::TempLocation)) + QDir::separator();
    // Set the filename to a null string
    projectname = QString::null;
    embedlist.clear();
}
