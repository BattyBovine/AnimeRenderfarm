/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Anime Renderfarm - A remote batch renderer for Anime Studio            *
 *  Copyright (C) 2011 Batty Bovine Productions                            *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

    winRenderManager = NULL;
    winPreferences = NULL;

    // Recover window size and position if the data exists, otherwise center the window
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

    // Initialise the taskbar pointers for later use
#ifdef Q_WS_WIN
    taskbarInterface = NULL;
    taskbarID = RegisterWindowMessage(L"TaskbarButtonCreated");
#endif

    // Create and set a new list model based on our custom class
    listProjectsModel = new qProjectsListModel(ui->listProjects);
    ui->listProjects->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listProjects->setModel(listProjectsModel);
}

AnimeRenderfarm::~AnimeRenderfarm()
{
    if(winRenderManager)
        winRenderManager->deleteLater();
    if(winPreferences)
        winPreferences->deleteLater();

#ifdef Q_WS_WIN
    if(taskbarInterface)
        taskbarInterface->Release();
#endif

    delete ui;
}

#ifdef Q_WS_WIN
bool AnimeRenderfarm::winEvent(MSG *message, long *result)
{
    if(message->message == taskbarID) {
        HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL,
                                      CLSCTX_INPROC_SERVER,
                                      IID_ITaskbarList3,
                                      reinterpret_cast<void**>(&taskbarInterface));
        if(SUCCEEDED(hr)) {
            hr = taskbarInterface->HrInit();

            if(FAILED(hr)) {
                taskbarInterface->Release();
                taskbarInterface = NULL;
            }
        }
        *result = hr;
        return true;
    }
    return false;
}
#endif

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

// Delete and Backspace should remove the selected projects from the list
void AnimeRenderfarm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Delete ||
       e->key() == Qt::Key_Backspace) {
        if(messageRemoveProjectsConfirm()) {
            QModelIndexList indices;
            while((indices = ui->listProjects->selectionModel()->selectedIndexes()).size()) {
                listProjectsModel->removeRow(indices.first().row());
            }
        }
    }
}

void AnimeRenderfarm::dragEnterEvent(QDragEnterEvent *e)
{
    // If the drag event contains a list of URIs, accept the drag action
    if(e->mimeData()->hasFormat("text/uri-list"))
        e->acceptProposedAction();
}
void AnimeRenderfarm::dropEvent(QDropEvent *e)
{
    // When the dragged items are dropped, compile the list of URIs into a list of paths
    QStringList slItems;
    QList<QUrl> lText = e->mimeData()->urls();
    foreach(QUrl url, lText)
        slItems << QDir::toNativeSeparators(url.toLocalFile());
    // Open the files
    filesOpened(slItems);
}

void AnimeRenderfarm::closeEvent(QCloseEvent *)
{
    // Save the state of the window on exit
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState(0));
}



void AnimeRenderfarm::showOpenProjectsDialogue()
{
    // Compile a list of supported formats based on the defined macros
    QString supported;
    supported += tr("All supported formats") + " ("+SUPPORTED_FORMATS_PROJECTS+");;";
    supported += tr("Anime Studio Projects") + " ("+SUPPORTED_FORMATS_PROJECTS_ANME+");;";
    supported += tr("Moho Projects") + " ("+SUPPORTED_FORMATS_PROJECTS_MOHO+");;";
    supported.replace(".", "*.");
    supported += tr("All files") += " (*.*);;";

    // Open a file browser dialogue
    QStringList files = QFileDialog::getOpenFileNames(
        this, tr("Open Anime Studio Projects"),
        QDir::toNativeSeparators(QDir::homePath()), supported
    );

    // Open the files that were selected by the user
    if(files.length()>0)
        filesOpened(files);
}



void AnimeRenderfarm::openPreferences()
{
    // Create a new settings window if the user tries to open a second one
    if(winPreferences) {
        if(winPreferences->isVisible())
            return;
        winPreferences->deleteLater();
    }
    winPreferences = new Preferences(this);
    winPreferences->show();
}

void AnimeRenderfarm::openAboutApplication()
{
    // Show an "About..." box at the user's request
    QMessageBox::about(this, tr("About ")+qApp->applicationName(),
        "<center><h2>"+qApp->applicationName()+"</h2>" +
        "<p>"+tr("Version ")+qApp->applicationVersion()+"</p>" +
        "<p>&copy; 2011 "+qApp->organizationName()+". "+tr("All Rights Reserved.")+
        "</p></center>");
}

void AnimeRenderfarm::openAboutQt()
{
    // Show an "About Qt..." box at the user's request
    qApp->aboutQt();
}



void AnimeRenderfarm::renderCompleted(QList< QPair<QString,QString> > newlist)
{
    // When the render is completed successfully, send an alert and clean up
    qApp->alert(this, 0);
//    listProjectsModel->clearall();
    renderEnd(newlist);
}
void AnimeRenderfarm::renderEnd(QList< QPair<QString,QString> > newlist)
{
    // Whether completed or canceled, set the new list and re-enable the projects list
    listProjectsModel->setListPairs(newlist);
    ui->listProjects->setEnabled(true);
    if(winRenderManager) {
        winRenderManager->deleteLater();
        winRenderManager = NULL;
    }
}



bool AnimeRenderfarm::messageRemoveProjectsConfirm() {
    // Make sure the user is certain they want to delete projects from the list
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
    // Sort the received files alphabetically
    files.sort();
    // For each one, if it's a valid project file, add it to the projects list
    for(int i=0; i<files.count(); i++) {
        QString file = QDir::toNativeSeparators(files.at(i));
        if(fileIsProject(file) && !listProjectsModel->contains(file))
            listProjectsModel->addRowAtEnd(file);
    }
}

bool AnimeRenderfarm::fileIsProject(QString file) {
    // Return true if the given file has a valid project extension
    QStringList slSupportedList = QString(SUPPORTED_FORMATS_PROJECTS).split(' ');
    foreach(QString ext, slSupportedList) {
        if(file.endsWith(ext)) return true; }
    return false;
}



void AnimeRenderfarm::renderProjects() {
    // If the executable path is not set, throw a warning and open the settings dialogue
    if((!settings.contains("AnimeStudioPath")||
            settings.value("AnimeStudioPath").toString().isEmpty())) {
        QMessageBox::information(this, tr("Set Default Options"),
            "<p>"+tr("You must first locate your Anime Studio executable.")+"</p>");
        this->openPreferences();
        return;
    }

    // If the projects list is empty, warn the user
    QStringList slProjects = listProjectsModel->getList();
    if(slProjects.size()<=0) {
        QMessageBox::information(this, tr("Eh?"),
            "<p>"+tr("The project list is empty. I don't know what you thought would "
            "happen, but...it's this. This is what happens.")+"</p>");
        return;
    }

    // Disable the projects list while the render is in progress, mainly as a formality
    ui->listProjects->setEnabled(false);
    if(!winRenderManager) {
        // Create a new Render Manager
        winRenderManager = new RenderManager(this);
        // Give the render manager the ID for this window, and the taskbar interface pointer
#ifdef Q_WS_WIN
        winRenderManager->initTaskbarInterface(this->winId(), taskbarInterface);
#endif
        // Connect our requisite signals and slots
        connect(winRenderManager, SIGNAL(renderFinished(QList< QPair<QString,QString> >)),
                this, SLOT(renderCompleted(QList< QPair<QString,QString> >)));
        connect(winRenderManager, SIGNAL(renderCanceled(QList< QPair<QString,QString> >)),
                this, SLOT(renderEnd(QList< QPair<QString,QString> >)));
    }
    // Set our projects list, and begin
    winRenderManager->setProjects(listProjectsModel->getListPairs());
    winRenderManager->start();
}
