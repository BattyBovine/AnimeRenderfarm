#include "serverthread.h"

ServerThread::ServerThread(QObject *parent) :
    QThread(parent), server(NULL), client(NULL)
{
    connect(this, SIGNAL(initServer()), this, SLOT(startServer()));
    connect(this, SIGNAL(serverStatus(QString)), parent, SLOT(getStatus(QString)));
}

ServerThread::~ServerThread()
{
    if(client) {
        client->disconnectFromHost();
        client->deleteLater();
    }
    if(server) {
        server->close();
        server->deleteLater();
    }
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
    if(!server->listen(QHostAddress::Any, 26463))
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
        cl->disconnectFromHost();   cl->deleteLater();
    }

    // If we're free, get the new connection and tell it to clean the pointer when disconnected
    client = server->nextPendingConnection();
    connect(client, SIGNAL(disconnected()),
            this, SLOT(cleanClientSocket()));
    // Send a request for the project file
    writeString(client, "project");
}

void ServerThread::cleanClientSocket() {
    client->deleteLater();
    client = NULL;
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
