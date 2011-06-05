#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

#include <QTimer>

class ServerThread : public QThread
{
    Q_OBJECT

public:
    explicit ServerThread(QObject *parent = 0);
    ~ServerThread();
    
protected:
    void run();

private:
    QTcpServer *server;

    QTimer *statustimer;

signals:
    void initServer();
    void serverStatus(QString);

public slots:

private slots:
    void startServer();
    void trackStatus();

};

#endif // SERVERTHREAD_H
