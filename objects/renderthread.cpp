#include "renderthread.h"

RenderThread::RenderThread(QObject *parent) :
    QThread(parent)
{
    renderprocess = NULL;

    format = indexToFormat(3);
    frameStart = frameEnd = -1;
    switchAA = "yes";
    switchShapeFX = "yes";
    switchLayerFX = "yes";
    switchHalfSize = "no";
    switchHalfFPS = "no";
    switchFewParticles = "no";
    switchExtraSmooth = "yes";
    switchNTSCSafe = "no";
    switchPremultiply = "yes";  // This option is weird.
    switchVariableWidths = "yes";

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
    if(in.isEmpty())
        emit renderError("Anime Studio executable not configured");
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
    QString filename = project.second.mid(0,project.second.lastIndexOf("."));
    QString outdir = (outputDirectory.isEmpty()?(project.first):(outputDirectory))+
            QDir::separator()+(isImageSequence()?filename+QDir::separator():"");
    QDir dirpath;dirpath.mkpath(outdir);

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
    renderprocess->setStandardOutputFile(QDir::toNativeSeparators(
        QDesktopServices::storageLocation(QDesktopServices::TempLocation))+
        QDir::separator()+project.second+".log");

    renderprocess->start(program, arguments);
    connect(renderprocess, SIGNAL(finished(int)), this, SLOT(renderFinished()));
    emit renderStarted(project);
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
