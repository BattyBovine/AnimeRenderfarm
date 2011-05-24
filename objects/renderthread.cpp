#include "renderthread.h"

RenderThread::RenderThread(QObject *parent) :
    QThread(parent)
{
    renderprocess = NULL;
    stdouttimer.setInterval(100);

    setFormat();
    setFrameRange();
    setSwitches();

    connect(this, SIGNAL(cli(QString,QStringList)),
            this, SLOT(executeRenderCommand(QString,QStringList)));
}

RenderThread::~RenderThread()
{
    if(renderprocess)
        delete renderprocess;
}



void RenderThread::setExe(QString in)
{
#ifdef Q_WS_MACX
    in += "/Contents/MacOS/Anime Studio";
    if(!QFileInfo(in).isExecutable()) {
        in += " Pro";
        if(!QFileInfo(in).isExecutable())
            emit renderError("No Mac OS X executable found in the application bundle");
    }
#endif
    exe = in;
}

void RenderThread::setProject(QPair<QString,QString> in)
{
    project = in;
}

void RenderThread::setOutputDirectory(QString in)
{
    outputDirectory = in;
}

void RenderThread::setFormat(int in)
{
    format = indexToFormat(in);
}

void RenderThread::setFrameRange(int start, int end)
{
    if(end>start)
        emit renderError("Start frame must be smaller than end frame");
    frameStart = start;
    frameEnd = end;
}

void RenderThread::setSwitches(bool aa, bool sfx, bool lfx, bool hsize, bool hfps,
                               bool fewpart, bool xsmooth, bool ntsc, bool nopmult, bool varw)
{
    switchAA=(aa?"yes":"no");
    switchShapeFX=(sfx?"yes":"no");
    switchLayerFX=(lfx?"yes":"no");
    switchHalfSize=(hsize?"yes":"no");
    switchHalfFPS=(hfps?"yes":"no");
    switchFewParticles=(fewpart?"yes":"no");
    switchExtraSmooth=(xsmooth?"yes":"no");
    switchNTSCSafe=(ntsc?"yes":"no");
    switchPremultiply=(!nopmult?"yes":"no");    // This option is weird.
    switchVariableWidths=(varw?"yes":"no");
}



void RenderThread::run()
{
    if(exe.isEmpty())
        emit renderError("Anime Studio executable not configured");
    if(project.first.isEmpty()||project.second.isEmpty())
        emit renderError("Project file not set properly");
    if(outputDirectory.isEmpty())
        emit renderWarning("Output directory not configured; defaulting to project location");

    QString filename = project.second.mid(0,project.second.lastIndexOf("."));
    QString outdir = (outputDirectory.isEmpty()?(project.first):(outputDirectory))+
            QDir::separator()+(isImageSequence()?filename+QDir::separator():"");
    QDir dirpath; dirpath.mkpath(outdir);   // This is just silly.

    QStringList args;
    args << "-r" << project.first+project.second << "-v" <<
            "-f" << format << "-o" << outdir+filename+extension();
    if(frameStart>=0 && frameEnd>=0)
        args << "-start" << QString::number(frameStart) << "-end" << QString::number(frameEnd);
    args << "-aa" << switchAA << "-shapefx" << switchShapeFX << "-layerfx" << switchLayerFX <<
            "-halfsize" << switchHalfSize <<"-halffps" << switchHalfFPS <<
            "-fewparticles" << switchFewParticles << "-extrasmooth" << switchExtraSmooth <<
            "-ntscsafe" << switchNTSCSafe << "-premultiply" << switchPremultiply <<
            "-variablewidths" << switchVariableWidths;

    emit cli(exe, args);
}



void RenderThread::executeRenderCommand(QString program, QStringList arguments)
{
    if(!renderprocess)
        renderprocess = new QProcess(this);
//    renderprocess->setStandardOutputFile(QDir::toNativeSeparators(
//        QDesktopServices::storageLocation(QDesktopServices::TempLocation))+
//        QDir::separator()+project.second+".log");

    connect(renderprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(calculateProgress()));
    connect(renderprocess, SIGNAL(finished(int)), this, SLOT(renderFinished()));
    renderprocess->setProcessChannelMode(QProcess::MergedChannels);
    renderprocess->start(program, arguments);

    stdouttimer.start();
    connect(&stdouttimer, SIGNAL(timeout()), this, SLOT(calculateProgress()));

    emit renderStarted(project);
}

void RenderThread::calculateProgress()
{
    if(renderprocess) {
        QString out;
        while(!(out=renderprocess->readLine()).isEmpty()) {
            if(out.contains("Done!")) {
                emit renderProgress(100);
                break;
            } else {
                QRegExp regex("^Frame \\d+ \\((\\d+)/(\\d+)\\)");
                if(out.contains(regex)) {
                    float current = regex.cap(1).toFloat();
                    float total = regex.cap(2).toFloat();
                    emit renderProgress(int((current/total)*100.0f));
                }
            }
        }
    }
}

void RenderThread::renderCancel()
{
    if(renderprocess)
        renderprocess->kill();
}

void RenderThread::renderFinished()
{
    if(renderprocess) {
        renderprocess->deleteLater();
        renderprocess = NULL;
    }
    emit renderComplete(project);
}



bool RenderThread::isImageSequence()
{
    return (format=="JPEG" || format=="TGA" || format=="BMP" || format=="PNG");
}
QString RenderThread::indexToFormat(int index)
{
    switch(index) {
    case 0:
        return "JPEG";
    case 1:
        return "BMP";
    case 2:
        return "TGA";
    case 3:
    default:
        return "PNG";
    case 4:
        return "QT";
    case 5:
        return "SWF";
    }
}
QString RenderThread::extension()
{
    if(format=="JPEG")
        return ".jpg";
    if(format=="BMP")
        return ".bmp";
    if(format=="TGA")
        return ".tga";
    if(format=="PNG")
        return ".png";
    if(format=="QT")
        return ".mov";
    if(format=="SWF")
        return ".swf";

    return "";
}
