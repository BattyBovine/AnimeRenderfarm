#include "clientthread.h"

ClientThread::ClientThread(QObject *parent) :
    QThread(parent)
{
    socket = NULL;

    connect(this, SIGNAL(initClient(QString,int)), this, SLOT(startClient(QString,int)));
}

ClientThread::~ClientThread()
{
    if(socket) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
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
//    if(end>start)
//        emit renderError("Start frame must be smaller than end frame");
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



void ClientThread::start()
{
//    emit initClient("192.168.169.150", 26463);

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readServerData()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(connectionError(QAbstractSocket::SocketError)));
    socket->abort();
    socket->connectToHost("192.168.169.150", 26463);
}



void ClientThread::startClient(QString, int)
{
}

void ClientThread::readServerData()
{
    quint16 blocksize = 0;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_0);

    if(blocksize==0) {
        if(socket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blocksize;
    }

    if(socket->bytesAvailable() < blocksize)
        return;

    QString data;
    in >> data;

    emit renderProgress(data, 0);
}



void ClientThread::connectionError(QAbstractSocket::SocketError e)
{
    switch(e) {
    case QAbstractSocket::RemoteHostClosedError:
        QMessageBox::information(NULL, tr("Connection Closed"),
                                 tr("The remote host closed the connection"));
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
