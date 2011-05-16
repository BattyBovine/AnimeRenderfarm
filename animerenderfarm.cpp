#include "animerenderfarm.h"
#include "ui_animerenderfarm.h"

#define SUPPORTED_FORMATS_PROJECTS_ANME ".anme"
#define SUPPORTED_FORMATS_PROJECTS_MOHO ".moho"
#define SUPPORTED_FORMATS_PROJECTS      ".anme .moho"



AnimeRenderfarm::AnimeRenderfarm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AnimeRenderfarm)
{
    ui->setupUi(this);

    if(!settings.contains("Geometry") ||
       !settings.contains("State")) {
        this->setGeometry(
            (qApp->desktop()->screenGeometry().width()/2)-(this->geometry().width()/2),
            (qApp->desktop()->screenGeometry().height()/2)-(this->geometry().height()/2),
            NULL, NULL);
    } else {
        restoreGeometry(settings.value("Geometry").toByteArray());
        restoreState(settings.value("State").toByteArray(), 0);
    }

    listProjectsModel = new qProjectsListModel(ui->listProjects);
    ui->listProjects->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listProjects->setModel(listProjectsModel);

    winRenderSettings = NULL;
    winServerSettings = NULL;
}

AnimeRenderfarm::~AnimeRenderfarm()
{
    delete ui;
}

void AnimeRenderfarm::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AnimeRenderfarm::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete ||
       event->key() == Qt::Key_Backspace) {
        if(this->messageRemoveProjectsConfirm()) {
            QModelIndexList indices;
            while((indices = ui->listProjects->selectionModel()->selectedIndexes()).size()) {
                listProjectsModel->removeRow(indices.first().row());
            }
        }
    }
}

void AnimeRenderfarm::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void AnimeRenderfarm::dropEvent(QDropEvent *event)
{
    QStringList slItems;
    QList<QUrl> lText = event->mimeData()->urls();
    foreach(QUrl url, lText)
        slItems << QDir::toNativeSeparators(url.toLocalFile());

    filesOpened(slItems);
}

void AnimeRenderfarm::closeEvent(QCloseEvent *)
{
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState(0));
}



void AnimeRenderfarm::showOpenProjectsDialogue()
{
    QString sSupported;

    sSupported += tr("All supported formats"); sSupported += " (";
    sSupported += SUPPORTED_FORMATS_PROJECTS; sSupported += ");;";

    sSupported += tr("Anime Studio Projects"); sSupported += " (";
    sSupported += SUPPORTED_FORMATS_PROJECTS_ANME; sSupported += ");;";
    sSupported += tr("Moho Projects"); sSupported += " (";
    sSupported += SUPPORTED_FORMATS_PROJECTS_MOHO; sSupported += ");;";

    sSupported.replace(".", "*.");

    sSupported += tr("All files");sSupported += " (*.*);;";

    QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Open Anime Studio Projects"),
        QDir::toNativeSeparators(QDir::homePath()),
        sSupported
    );

    if(files.length()>0)
        filesOpened(files);
}



void AnimeRenderfarm::openRenderSettings()
{
    if(winRenderSettings != NULL) {
        if(winRenderSettings->isVisible())
            return;
        winRenderSettings->deleteLater();
    }
    winRenderSettings = new RenderSettings(this);
    winRenderSettings->show();
}

void AnimeRenderfarm::openServerSettings()
{
    if(winServerSettings != NULL) {
        if(winServerSettings->isVisible())
            return;
        winServerSettings->deleteLater();
    }
    winServerSettings = new ServerSettings(this);
    winServerSettings->show();
}

void AnimeRenderfarm::openAboutApplication()
{
    QMessageBox::about(this, tr("About ")+qApp->applicationName(),
        "<center><h2>"+qApp->applicationName()+"</h2>" +
        "<p>"+tr("Version ")+qApp->applicationVersion()+"</p>" +
        "<p>© 2011 "+qApp->organizationName()+". "+tr("All Rights Reserved.")+"</p></center>");
}

void AnimeRenderfarm::openAboutQt()
{
    qApp->aboutQt();
}



bool AnimeRenderfarm::messageRemoveProjectsConfirm() {
    QMessageBox::StandardButton response;
    response = QMessageBox::question(this, tr("Are you sure?"),
        "<p>"+tr("Are you sure you wish to remove the "
                 "selected items from the render queue?")+"</p>" +
        "<p>"+tr("Note that this will not delete your "
                 "project files from your hard drive.")+"</p>",
        QMessageBox::Yes | QMessageBox::No);
    return (response==QMessageBox::Yes);
}



void AnimeRenderfarm::filesOpened(QStringList files)
{
    files.sort();
    for(int i=0; i<files.count(); i++) {
        QString file = QDir::toNativeSeparators(files.at(i));
        if(fileIsProject(file) && !listProjectsModel->contains(file))
            listProjectsModel->addRowAtEnd(file);
    }
}

bool AnimeRenderfarm::fileIsProject(QString file) {
    QStringList slSupportedList = QString(SUPPORTED_FORMATS_PROJECTS).split(' ');
    foreach(QString ext, slSupportedList) {
        if(file.endsWith(ext))
            return true;
    }
    return false;
}



void AnimeRenderfarm::renderProjects() {
    if((!settings.contains("AnimeStudioPath")||
            settings.value("AnimeStudioPath").toString().isEmpty()) ||
        (!settings.contains("OutputDirectory")||
            settings.value("OutputDirectory").toString().isEmpty())) {
        QMessageBox::information(this, tr("Set Default Options"),
            "<p>"+tr("You must first set your default options, particularly "
            "the location of the Anime Studio executable and the folder "
            "where you want your rendered files to be saved.")+"</p>");
        this->openRenderSettings();
        return;
    }

    QStringList slProjects = listProjectsModel->getList();
    if(slProjects.size()<=0) {
        QMessageBox::information(this, tr("Eh?"),
            "<p>"+tr("The project list is empty. I don't know what you thought would "
            "happen, but...it's this. This is what happens.")+"</p>");
        return;
    }

//    if(winRenderSettings==NULL)
//        winRenderSettings = new RenderSettings(this);
//    foreach(QString proj, slProjects) {
//        QStringList args;
//        args << "-r" << proj << "-v" << "-f" << winRenderSettings->getOutputFormat();
//        if(winRenderSettings->getFrameRange()) {
//            args << "-start" << winRenderSettings->getStartFrame() <<
//                    "-end" << winRenderSettings->getEndFrame();
//        }
//        args << "-aa" << winRenderSettings->getAntialiasedEdges() <<
//                "-shapefx" << winRenderSettings->getApplyShapeEffects() <<
//                "-layerfx" << winRenderSettings->getApplyLayerEffects() <<
//                "-halfsize" << winRenderSettings->getRenderAtHalfDimensions() <<
//                "-halffps" << winRenderSettings->getRenderAtHalfFramerate() <<
//                "-fewparticles" << winRenderSettings->getReducedParticles() <<
//                "-extrasmooth" << winRenderSettings->getExtraSmoothImages() <<
//                "-ntscsafe" << winRenderSettings->getUseNTSCSafeColours() <<
//                "-premultiply" << winRenderSettings->getDoNotPremultiplyAlpha() <<
//                "-variablewidths" << winRenderSettings->getVariableLineWidths();

//        QProcess *renderjob = new QProcess(this);
//        renderjob->start(winRenderSettings->getAnimeStudioPath(),args);
//        renderjob->waitForFinished(-1);

//        QString r_stdout = renderjob->readAllStandardOutput();
//        QString r_stderr = renderjob->readAllStandardError();
//        if(!r_stdout.isEmpty() || !r_stderr.isEmpty()) {
//            if(!r_stdout.isEmpty())
//                QMessageBox::information(this, tr("Output for ")+proj, r_stdout);
//            if(!r_stderr.isEmpty())
//                QMessageBox::critical(this, tr("Errors for ")+proj, r_stderr);
//        }
//    }

//    QMessageBox::information(this, tr("Complete!"), tr("Finished with all files."));

    RenderProgress *winRenderProgress = new RenderProgress(this);
    winRenderProgress->show();
}
