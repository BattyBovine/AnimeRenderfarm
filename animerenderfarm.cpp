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

    if(!settings.contains("geometry") ||
       !settings.contains("state")) {
        this->setGeometry((qApp->desktop()->screenGeometry().width()/2)-(this->geometry().width()/2),
                          (qApp->desktop()->screenGeometry().height()/2)-(this->geometry().height()/2),
                           NULL, NULL);
    } else {
        restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("state").toByteArray(), 0);
    }

    listProjectsModel = new qProjectsListModel(ui->listProjects);
    ui->listProjects->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listProjects->setModel(listProjectsModel);

    winRenderSettings = NULL;
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
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState(0));
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

void AnimeRenderfarm::openAboutApplication()
{
    QMessageBox::about(this, tr("About ")+qApp->applicationName(),
        qApp->applicationName().prepend("<center><h2>").append("</h2>") +
        qApp->applicationVersion().prepend(tr("Version ").prepend("<p>")).append("</p>") +
        qApp->organizationName().prepend("<p>© 2011 ").append("<br/>") +
        tr("All Rights Reserved").append("</p></center>"));
}

void AnimeRenderfarm::openAboutQt()
{
    qApp->aboutQt();
}



bool AnimeRenderfarm::messageRemoveProjectsConfirm() {
    QMessageBox::StandardButton response;
    response = QMessageBox::question(this, tr("Are you sure?"),
        tr("Are you sure you wish to remove the selected items from the render queue?")
            .prepend("<p>").append("</p>") +
        tr("Note that this will not delete your project files from your hard drive.")
            .prepend("<p>").append("</p>"),
        QMessageBox::Yes | QMessageBox::No);
    return (response==QMessageBox::Yes);
}



void AnimeRenderfarm::filesOpened(QStringList files)
{
    files.sort();
    QList< QPair<QString,QString> > lCurrent = listProjectsModel->getList();
    for(int i=0; i<files.count(); i++) {
        QString file = files.at(i);

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
    settings.beginGroup("RenderSettings");
    if(!settings.contains("OutputDirectory") ||
       settings.value("OutputDirectory","").toString().isEmpty()) {
        QMessageBox::information(this,
            tr("Set An Output Directory"),
            tr("<p>You must first set your default options, particularly the"
               "folder where you want your rendered files to be saved.</p>"));
        this->openRenderSettings();
        settings.endGroup();
        return;
    }
    settings.endGroup();

    QList< QPair<QString,QString> > lProjects = listProjectsModel->getList();
    if(lProjects.size()<=0) {
        QMessageBox::information(this, tr("?"),
            tr("<p>The project list is empty. I don't know what you thought would happen, but...it's this. This is what happens.</p>"));
        return;
    }
    QString sFilesToRender;
    for(int i=0; i<lProjects.count(); i++) {
        QPair<QString,QString> project = lProjects.at(i);
        sFilesToRender.append(project.second+(i<lProjects.count()-1?", ":""));
    }

    QMessageBox::information(this,
        tr("Rendering..."),
        tr("<p>The following files will be rendered:</p>")+
        QString("<p>")+sFilesToRender+QString("</p>"));
}
