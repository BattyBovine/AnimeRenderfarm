#include "renderthread.h"

RenderThread::RenderThread(QObject *parent) :
    QThread(parent)
{
    renderprocess = NULL;

    // Set options to defaults in case they are somehow not set earlier
    setFormat();
    setFrameRange();
    setSwitches();
    // This is so that the render process actually starts in a new thread instead of 'run()'
    connect(this, SIGNAL(cli(QString,QStringList)),
            this, SLOT(executeRenderCommand(QString,QStringList)));
}

RenderThread::~RenderThread()
{
    if(renderprocess)
        renderprocess->deleteLater();
}



void RenderThread::setExe(QString in)
{
    // On Mac OS X, we need to do a bit of extra work to find the executable in the bundle
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
    format = RenderThread::indexToFormat(in);
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
    // Check for any possible conditions, and emit errors for each one
    if(exe.isEmpty()) {
        emit renderError("Anime Studio executable not configured"); return; }
    if(project.first.isEmpty()||project.second.isEmpty()) {
        emit renderError("Project file not set properly"); return; }
    if(outputDirectory.isEmpty())
        emit renderWarning("Output directory not configured; defaulting to project location");

    // Get our separator now, to avoid multiple function calls
    QString sep = QDir::separator();

    // Separate the filename from the extension
    QString filename = project.second.mid(0,project.second.lastIndexOf("."));

    // Compile the final output directory, and create it if it doesn't exist
    QString outdir = (outputDirectory.isEmpty()?(project.first):(outputDirectory+sep))+
            (RenderThread::isImageSequence(format)?filename+sep:"");
    QDir dirpath; dirpath.mkpath(outdir);   // This is just silly.

    // Set all of the arguments based on the configuration settings we now have
    QStringList args;
    args << "-r" << project.first+project.second << "-v" <<
            "-f" << format << "-o" << outdir+filename+RenderThread::extension(format);
    if(frameStart>=0 && frameEnd>=0)
        args << "-start" << QString::number(frameStart) << "-end" << QString::number(frameEnd);
    args << "-aa" << switchAA << "-shapefx" << switchShapeFX << "-layerfx" << switchLayerFX <<
            "-halfsize" << switchHalfSize <<"-halffps" << switchHalfFPS <<
            "-fewparticles" << switchFewParticles << "-extrasmooth" << switchExtraSmooth <<
            "-ntscsafe" << switchNTSCSafe << "-premultiply" << switchPremultiply <<
            "-variablewidths" << switchVariableWidths;

    // Finally, emit the signal to begin the render
    emit cli(exe, args);
}

void RenderThread::executeRenderCommand(QString program, QStringList arguments)
{
    // Create a new process for the render job
    if(!renderprocess)
        renderprocess = new QProcess(this);
//    renderprocess->setStandardOutputFile(QDir::toNativeSeparators(
//        QDesktopServices::storageLocation(QDesktopServices::TempLocation))+
//        QDir::separator()+project.second+".log");

    // Get regular progress updates from the process, and have it emit a signal when finished
    connect(renderprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(calculateProgress()));
    connect(renderprocess, SIGNAL(finished(int)), this, SLOT(renderFinished()));
//    renderprocess->setProcessChannelMode(QProcess::MergedChannels);

    // Now start the process, and emit a signal saying we've done so
    renderprocess->start(program, arguments);
    emit renderProgress(tr("Currently rendering ")+project.second, 0);
}



void RenderThread::calculateProgress()
{
    // Calculate the render progress based on the program output
    if(renderprocess) {
        QString out;
        // Loop through as long as there is data to process
        while(!(out=renderprocess->readLine()).isEmpty()) {
            // If it says "Done!", we should emit a 100% completion
            if(out.contains("Done!")) {
                emit renderProgress(tr("Finished rendering ")+project.second, 100);
                break;
            } else {
                // Extract the current and total frames from the output
                QRegExp regex("^Frame \\d+ \\((\\d+)/(\\d+)\\)");
                if(out.contains(regex)) {
                    // Convert to float, then get a percentage from that
                    float current = regex.cap(1).toFloat();
                    float total = regex.cap(2).toFloat();
                    emit renderProgress(tr("Currently rendering ")+project.second,
                                        int((current/total)*100.0f));
                }
            }
        }
    }
}

void RenderThread::renderCancel()
{
    // If the user cancels the render, kill the process
    if(renderprocess) {
        renderprocess->kill();
        renderprocess->deleteLater();
        renderprocess = NULL;
    }
}

void RenderThread::renderFinished()
{
    // If we're finished, delete the process and emit a completion signal
    if(renderprocess) {
        renderprocess->deleteLater();
        renderprocess = NULL;
    }
    emit renderComplete(project);
}



bool RenderThread::isImageSequence(QString fmt)
{
    return (fmt=="JPEG" || fmt=="TGA" || fmt=="BMP" || fmt=="PNG");
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
QString RenderThread::extension(QString fmt)
{
    if(fmt=="JPEG")
        return ".jpg";
    if(fmt=="BMP")
        return ".bmp";
    if(fmt=="TGA")
        return ".tga";
    if(fmt=="PNG")
        return ".png";
    if(fmt=="QT")
        return ".mov";
    if(fmt=="SWF")
        return ".swf";

    return "";
}
