#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QTcpSocket>

#include <QFile>

#include <QMessageBox>

class ClientThread : public QThread
{
    Q_OBJECT
public:
    explicit ClientThread(QObject *parent = 0);
    ~ClientThread();

    void start();

    void setProject(QPair<QString,QString>);
    void setOutputDirectory(QString in);
    void setFormat(int in=3);
    void setFrameRange(int start=-1,int end=-1);
    void setSwitches(bool aa=true, bool sfx=true, bool lfx=true, bool hsize=false,
                     bool hfps=false, bool fewpart=false, bool xsmooth=true,
                     bool ntsc=false, bool nopmult=false, bool varw=true);
    void setServerIP(QString in="127.0.0.1");
    void setServerPort(quint16 in=26463);

private:
    bool isImageSequence();
    QString indexToFormat(int);
    QString extension();

    QString readString();
    void sendData(QString filepath);

    QTcpSocket *socket;

    QPair<QString,QString> project;
    QString outputDirectory;
    QString format;
    int frameStart, frameEnd;
    QString switchAA,switchShapeFX,switchLayerFX,switchHalfSize,switchHalfFPS,switchFewParticles,
        switchExtraSmooth,switchNTSCSafe,switchPremultiply,switchVariableWidths;
    QString serverIP;
    quint16 serverPort;

signals:
    void initClient(QString,int);
    void renderProgress(QString,int);
    void renderComplete(QPair<QString,QString>);
    void renderError(QString);
    void renderCancel();

public slots:

private slots:
    void handleServerResponse();
    void connectionError(QAbstractSocket::SocketError);

};

#endif // CLIENTTHREAD_H
