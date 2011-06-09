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
    QString response = readString();

    if(response=="project")
        sendData(project.first+project.second);
}

QString ClientThread::readString()
{
    quint16 blocksize = 0;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_0);

    if(blocksize==0) {
        if(socket->bytesAvailable() < (int)sizeof(quint16))
            return NULL;
        in >> blocksize;
    }

    if(socket->bytesAvailable() < blocksize)
        return NULL;

    QString data;
    in >> data;
    return data;
}

void ClientThread::sendData(QString filepath)
{
//    QByteArray block;
//    QDataStream outstream(&block, QIODevice::WriteOnly);
//    outstream.setVersion(QDataStream::Qt_4_0);

//    QFile outfile(filepath);
//    if(!outfile.open(QIODevice::ReadOnly))
//        return;

//    outstream << (qint64)outfile.size();
//    outstream << outfile.readAll();

//    socket->write(block);
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly))
        return;

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (qint64)file.size();
    out << file.readAll();

    qint64 sent = 0;
    while(sent < sizeof(data)) {
        qint64 sentnow = socket->write(data);
        if(sentnow >= 0) {
            sent += sentnow;
        } else {
            emit renderError("Could not write data to server");
            return;
        }
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
