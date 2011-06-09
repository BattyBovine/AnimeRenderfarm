#include "serverthread.h"

ServerThread::ServerThread(QObject *parent) :
    QThread(parent),
    server(NULL), client(NULL), port(26463), filelen(0)
{
    temppath = QDir::toNativeSeparators(QDesktopServices::storageLocation(
                                            QDesktopServices::TempLocation));
    temppath += QDir::separator();
    temppath += "1234567890";
//            QString(QCryptographicHash::hash("1234567890", QCryptographicHash::Sha1));
    QDir path; path.mkpath(temppath);

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



void ServerThread::setPort(quint16 in)
{
    port = in;
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
    if(!server->listen(QHostAddress::Any, port))
        emit serverStatus(tr("Server could not be started: %1").arg(server->errorString()));

    if(server->isListening())
        emit serverStatus(tr("Awaiting connection on port %1")
                          .arg(QString::number(server->serverPort())));

    connect(server, SIGNAL(newConnection()), this, SLOT(beginTransfer()));
}



void ServerThread::trackStatus()
{
    if(server && server->isListening())
        emit serverStatus(tr("Awaiting connection on port ")+
                          QString::number(server->serverPort()));
    else
        emit serverStatus(tr("Offline"));
}

void ServerThread::beginTransfer()
{
    // If the server already has an active connection, tell it we're busy right now
    if(client && client->isValid()) {
        QTcpSocket *cl = server->nextPendingConnection();
        writeString(cl, "busy");
        cl->deleteLater();
    }

    // If we're free, get the new connection and tell it to clean the pointer when disconnected
    client = server->nextPendingConnection();
    connect(client, SIGNAL(disconnected()), this, SLOT(cleanClientSocket()));
    // Send a request for the project file
    connect(client, SIGNAL(readyRead()), this, SLOT(getProjectFile()));
    writeString(client, "project");
}

void ServerThread::getProjectFile()
{
    QByteArray data = readData(client);
    if(data.isNull())
        return;

    QFile infile(temppath+QDir::separator()+"project.anme");
    if(!infile.open(QFile::WriteOnly)) {
        filelen = 0;
        return;
    }
    infile.write(data);
    infile.close();
}

void ServerThread::cleanClientSocket() {
    client->deleteLater();
    client = NULL;
}



void ServerThread::writeData(QTcpSocket *cl, QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return;

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (qint64)file.size();
    out << file.readAll();

    qint64 sent = 0;
    while(sent < sizeof(data)) {
        qint64 sentnow = cl->write(data);
        if(sentnow >= 0)
            sent += sentnow;
        else
            return;
    }
}

void ServerThread::writeString(QTcpSocket *cl, QString data)
{
    if(!cl || !cl->isValid())
        return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << (quint16)data.size();
    out << data;

    cl->write(block);
}

QByteArray ServerThread::readData(QTcpSocket *cl)
{
    QDataStream in(cl);
    in.setVersion(QDataStream::Qt_4_0);

    if(filelen==0) {
        if(cl->bytesAvailable() < (int)sizeof(qint64))
            return NULL;
        in >> filelen;
    }

    if(cl->bytesAvailable() < filelen)
        return NULL;

    QByteArray data;
    in >> data;

    filelen = 0;
    return data;
}
