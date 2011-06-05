#include "serverthread.h"

ServerThread::ServerThread(QObject *parent) :
    QTcpServer(parent)
{
}

ServerThread::~ServerThread()
{
}



void ServerThread::incomingConnection(int)
{
}
