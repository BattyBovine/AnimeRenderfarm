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
    outputDirectory = in;
}

void ClientThread::setFormat(int in)
{
    format = indexToFormat(in);
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
    switchAA=(aa?"yes":"no");
    switchShapeFX=(sfx?"yes":"no");
    switchLayerFX=(lfx?"yes":"no");
    switchHalfSize=(hsize?"yes":"no");
    switchHalfFPS=(hfps?"yes":"no");
    switchFewParticles=(fewpart?"yes":"no");
    switchExtraSmooth=(xsmooth?"yes":"no");
    switchNTSCSafe=(ntsc?"yes":"no");
    switchPremultiply=(!nopmult?"yes":"no");    // This option is weird.
    switchVariableWidths=(varw?"yes":"no");
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

    // Send the project filename when requested
    if(response=="filename") {
        comm.writeData(socket, project.second.toUtf8()); return; }

    // After that, send the full project file
    if(response=="project") {
        comm.writeFile(socket, project.first+project.second); return; }
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



bool ClientThread::isImageSequence()
{
    return (format=="JPEG" || format=="TGA" || format=="BMP" || format=="PNG");
}

QString ClientThread::indexToFormat(int index)
{
    switch(index) {
    case 0:
        return "JPEG";
    case 1:
        return "BMP";
    case 2:
        return "TGA";
    case 3:
    default:
        return "PNG";
    case 4:
        return "QT";
    case 5:
        return "SWF";
    }
}

QString ClientThread::extension()
{
    if(format=="JPEG")
        return ".jpg";
    if(format=="BMP")
        return ".bmp";
    if(format=="TGA")
        return ".tga";
    if(format=="PNG")
        return ".png";
    if(format=="QT")
        return ".mov";
    if(format=="SWF")
        return ".swf";

    return "";
}
