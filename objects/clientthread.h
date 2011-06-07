#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QTcpSocket>

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

private:
    bool isImageSequence();
    QString indexToFormat(int);
    QString extension();

    QTcpSocket *socket;

    QPair<QString,QString> project;
    QString outputDirectory;
    QString format;
    int frameStart, frameEnd;
    QString switchAA,switchShapeFX,switchLayerFX,switchHalfSize,switchHalfFPS,switchFewParticles,
        switchExtraSmooth,switchNTSCSafe,switchPremultiply,switchVariableWidths;

signals:
    void initClient(QString,int);
    void renderProgress(QString,int);
    void renderComplete(QPair<QString,QString>);

public slots:

private slots:
    void startClient(QString,int);
    void readServerData();
    void connectionError(QAbstractSocket::SocketError);

};

#endif // CLIENTTHREAD_H
