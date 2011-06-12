#ifndef TCPCOMMUNICATOR_H
#define TCPCOMMUNICATOR_H

#include <QTcpSocket>
#include <QString>
#include <QFile>

class TcpCommunicator
{
public:
    TcpCommunicator();
    ~TcpCommunicator();

    void reset();

    QByteArray readData(QTcpSocket*);
    void writeFile(QTcpSocket*,QString);
    void writeData(QTcpSocket*,QByteArray);

private:
    QByteArray data;
    qint64 size;
};

#endif // TCPCOMMUNICATOR_H
