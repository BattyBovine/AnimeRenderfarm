#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QTcpServer>

class ServerThread : public QTcpServer
{
    Q_OBJECT
public:
    explicit ServerThread(QObject *parent = 0);
    ~ServerThread();
    
protected:
    void incomingConnection(int);

private:

signals:

public slots:

};

#endif // SERVERTHREAD_H
