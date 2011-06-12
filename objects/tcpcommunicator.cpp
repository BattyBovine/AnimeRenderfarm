#include "tcpcommunicator.h"

TcpCommunicator::TcpCommunicator() :
    data(NULL), size(0)
{
}

TcpCommunicator::~TcpCommunicator()
{
}



void TcpCommunicator::reset()
{
    data = NULL;
    size = 0;
}



QByteArray TcpCommunicator::readData(QTcpSocket *sock)
{
    if(!sock || !sock->isValid())
        return NULL;

    QDataStream in(sock);
    in.setVersion(QDataStream::Qt_4_0);

    if(size==0) {
        if(sock->bytesAvailable() < (int)sizeof(qint64))
            return NULL;
        in >> size;
    }

    if(sock->bytesAvailable() < size)
        return NULL;

    QByteArray data;
    in >> data;

    size = 0;

    return data;
}

void TcpCommunicator::writeFile(QTcpSocket *sock, QString filepath)
{
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly))
        return;

    writeData(sock, file.readAll());
    file.close();
}

void TcpCommunicator::writeData(QTcpSocket *sock, QByteArray data)
{
    if(!sock || !sock->isValid())
        return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << (qint64)data.size();
    out << data;

    qint64 sent = 0;
    while(sent < sizeof(block)) {
        qint64 sentnow = sock->write(block);
        if(sentnow >= 0)
            sent += sentnow;
        else
            return;
    }
}
