#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkSession>

#include <QDesktopServices>
#include <QFile>
#include <QDir>

#include <QCryptographicHash>

class ServerThread : public QThread
{
    Q_OBJECT

public:
    explicit ServerThread(QObject *parent = 0);
    ~ServerThread();

    void setPort(quint16);
    
protected:
    void run();

private:
    void writeData(QTcpSocket*,QString);
    void writeString(QTcpSocket*,QString);
    QByteArray ServerThread::readData(QTcpSocket*);

    QTcpServer *server;
    QTcpSocket *client;

    quint16 port;
    QString temppath;

    QString filename;
    qint64 filelen;
    QString filehash;

signals:
    void initServer();
    void serverStatus(QString);

public slots:

private slots:
    void beginTransfer();
    void getProjectFile();
    void cleanClientSocket();

    void startServer();
    void trackStatus();

};

#endif // SERVERTHREAD_H
