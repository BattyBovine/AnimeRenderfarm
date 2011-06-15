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

#include "clientthread.h"

ClientThread::ClientThread(QObject *parent) :
    QThread(parent),
    socket(NULL)
{
}

ClientThread::~ClientThread()
{
    if(socket)
        socket->deleteLater();
}



void ClientThread::setProject(QPair<QString,QString> in)
{
    project = in;
}

void ClientThread::setOutputDirectory(QString in)
{
    outputPath = in;
}

void ClientThread::setFormat(int in)
{
    format = in;
}

void ClientThread::setFrameRange(int start, int end)
{
    if(end>start)
        emit renderError("Start frame must be smaller than end frame");
    frameStart = start;
    frameEnd = end;
}

void ClientThread::setSwitches(bool aa, bool sfx, bool lfx, bool hsize, bool hfps,
                               bool fewpart, bool xsmooth, bool ntsc, bool nopmult, bool varw)
{
    switchAA = RenderThread::boolToString(aa);
    switchShapeFX = RenderThread::boolToString(sfx);
    switchLayerFX = RenderThread::boolToString(lfx);
    switchHalfSize = RenderThread::boolToString(hsize);
    switchHalfFPS = RenderThread::boolToString(hfps);
    switchFewParticles = RenderThread::boolToString(fewpart);
    switchExtraSmooth = RenderThread::boolToString(xsmooth);
    switchNTSCSafe = RenderThread::boolToString(ntsc);
    switchPremultiply = RenderThread::boolToString(!nopmult);   // This option is weird.
    switchVariableWidths = RenderThread::boolToString(varw);
}

void ClientThread::setServerIP(QString in)
{
    serverIP = in;
}

void ClientThread::setServerPort(quint16 in)
{
    serverPort = in;
}



void ClientThread::start()
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(handleServerResponse()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(connectionError(QAbstractSocket::SocketError)));
    socket->abort();
    socket->connectToHost(serverIP, serverPort);
}



void ClientThread::handleServerResponse()
{
    QString response = comm.readData(socket);

    if(response=="projectname") {
        // Send the project name when requested
        comm.writeData(socket, project.second.toUtf8());
        emit renderProgress(tr("Uploading %1 to the server").arg(project.second), 0);
        return;
    } else if(response=="project") {
        // After that, send the full project file
        comm.writeFile(socket, project.first+project.second);
        return;
    } else if(response.startsWith("embed:")) {
        // If the server sends us a request for an embedded file, send it
        QString filepath = response.mid(response.indexOf(":")+1,response.length());
        QString file = response.mid(response.lastIndexOf("/")+1,response.length());
        emit renderProgress(tr("Uploading embedded file %1 to the server")
                            .arg(file), 0);
        if(QFile::exists(filepath))
            comm.writeFile(socket, filepath);
        else
            comm.writeData(socket, "");
        return;
    } else if(response=="cmd") {
        // Now send the command line string -- without the executable, of course
        QString cmdline = RenderThread::generateCommand("*}PRJ{*",
                                                        RenderThread::indexToFormat(format),
                                                        "@)RND(@",frameStart,frameEnd,
                                                        switchAA,switchShapeFX,switchLayerFX,
                                                        switchHalfSize,switchHalfFPS,
                                                        switchFewParticles,switchExtraSmooth,
                                                        switchNTSCSafe,switchPremultiply,
                                                        switchVariableWidths).join(":");
        comm.writeData(socket, cmdline.toUtf8());
        return;
    }
}



void ClientThread::connectionError(QAbstractSocket::SocketError e)
{
    switch(e) {
    case QAbstractSocket::RemoteHostClosedError:
        QMessageBox::information(NULL, tr("Connection Closed"),
                                 tr("The remote host closed the connection"));
        emit renderCancel();
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::critical(NULL, tr("Connection Refused"),
                              tr("The connection was refused by the remote server.")+"\n\n"+
                              tr("Please make sure the server program is running, and "
                                 "that the IP address and port are correct."));
        break;
    default:
        QMessageBox::critical(NULL, tr("Connection Error"),
                              tr("The following error occurred when connecting to the server: %1")
                              .arg(socket->errorString()));
    }
}
