#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QThread>
#include <QProcess>
#include <QTimer>
#include <QDir>
#include <QDesktopServices>

#include <QPair>

class RenderThread : public QThread
{
    Q_OBJECT
public:
    explicit RenderThread(QObject *parent = 0);
    ~RenderThread();

    void setExe(QString);
    void setProject(QPair<QString,QString>);
    void setOutputDirectory(QString in);
    void setFormat(int in=3);
    void setFrameRange(int start=-1,int end=-1);
    void setSwitches(bool aa=true, bool sfx=true, bool lfx=true, bool hsize=false,
                     bool hfps=false, bool fewpart=false, bool xsmooth=true,
                     bool ntsc=false, bool nopmult=false, bool varw=true);

    static QStringList generateCommand(QString,QString,QString,int,int,QString,QString,QString,
                                       QString,QString,QString,QString,QString,QString,QString);

    static bool isImageSequence(QString);
    static QString indexToFormat(int);
    static QString extension(QString);
    static QString boolToString(bool);

protected:
    void run();

private:
    QProcess *renderprocess;
    QString exe;
    QStringList cmd;
    QPair<QString,QString> project;
    QString outputPath;
    int format;
    int frameStart, frameEnd;
    QString switchAA,switchShapeFX,switchLayerFX,switchHalfSize,switchHalfFPS,switchFewParticles,
        switchExtraSmooth,switchNTSCSafe,switchPremultiply,switchVariableWidths;

    QTimer stdouttimer;

signals:
    void cli(QString,QStringList);

    void renderStarted(QPair<QString,QString>);
    void renderProgress(QString,int);
    void renderComplete(QPair<QString,QString>);

    void renderWarning(QString);
    void renderError(QString);

public slots:
    void renderCancel();

private slots:
    void executeRenderCommand(QString,QStringList);
    void calculateProgress();
    void renderFinished();

};

#endif // RENDERTHREAD_H
