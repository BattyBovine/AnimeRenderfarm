#include "serverthread.h"

ServerThread::ServerThread(QObject *parent) :
    QThread(parent)
{
    server = NULL;
    statustimer = NULL;

    connect(this, SIGNAL(initServer()),
            this, SLOT(startServer()));
    connect(this, SIGNAL(serverStatus(QString)),
            parent, SLOT(getStatus(QString)));
}

ServerThread::~ServerThread()
{
    if(server)
        server->deleteLater();
}

void ServerThread::run()
{
    if(server) {
        if(!server->isListening())
            server->listen(QHostAddress::Any, 26463);
    } else {
        emit initServer();
    }
}

void ServerThread::startServer()
{
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, 26463);

    statustimer = new QTimer(this);
    statustimer->setInterval(1000);
    connect(statustimer, SIGNAL(timeout()),
            this, SLOT(trackStatus()));
    statustimer->start();
}



void ServerThread::trackStatus()
{
    if(server && server->isListening())
        emit serverStatus(tr("Online on port ") + QString::number(server->serverPort()));
    else
        emit serverStatus(tr("Offline"));
}
