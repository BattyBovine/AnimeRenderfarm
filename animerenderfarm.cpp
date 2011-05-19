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

#ifdef Q_WS_WIN
    taskbarInterface = NULL;
    taskbarID = RegisterWindowMessage(L"TaskbarButtonCreated");
#endif

    listProjectsModel = new qProjectsListModel(ui->listProjects);
    ui->listProjects->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listProjects->setModel(listProjectsModel);

    winRenderProgress = NULL;
    winRenderSettings = NULL;
    winServerSettings = NULL;
}

AnimeRenderfarm::~AnimeRenderfarm()
{
    if(winRenderProgress)
        winRenderProgress->deleteLater();
    if(winRenderSettings)
        winRenderSettings->deleteLater();
    if(winServerSettings)
        winServerSettings->deleteLater();

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
    if(e->mimeData()->hasFormat("text/uri-list"))
        e->acceptProposedAction();
}

void AnimeRenderfarm::dropEvent(QDropEvent *e)
{
    QStringList slItems;
    QList<QUrl> lText = e->mimeData()->urls();
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
    QString supported;

    supported += tr("All supported formats") + " (*.anme *.moho);;";

    supported += tr("Anime Studio Projects") + " (*.anme);;";
    supported += tr("Moho Projects") + " (*.moho);;";

    supported.replace(".", "*.");

    supported += tr("All files") += " (*.*);;";

    QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Open Anime Studio Projects"),
        QDir::toNativeSeparators(QDir::homePath()),
        supported
    );

    if(files.length()>0)
        filesOpened(files);
}



void AnimeRenderfarm::openRenderSettings()
{
    if(winRenderSettings) {
        if(winRenderSettings->isVisible())
            return;
        winRenderSettings->deleteLater();
    }
    winRenderSettings = new RenderSettings(this);
    winRenderSettings->show();
}

void AnimeRenderfarm::openServerSettings()
{
    if(winServerSettings) {
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



void AnimeRenderfarm::renderCompleted()
{
    qApp->alert(this, 0);

    listProjectsModel->clearall();

    if(this->isMinimized())
        QMessageBox::information(this, tr("Render Complete!"),
            "<p>"+tr("All projects have successfully finished rendering.")+"</p>");

    renderEnd();
}
void AnimeRenderfarm::renderEnd()
{
    ui->listProjects->setEnabled(true);

    if(winRenderProgress) {
        winRenderProgress->deleteLater();
        winRenderProgress = NULL;
    }
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

    ui->listProjects->setEnabled(false);
    if(!winRenderProgress) {
        winRenderProgress = new RenderProgress(this);
#ifdef Q_WS_WIN
        winRenderProgress->initTaskbarInterface(this->winId(), taskbarInterface);
#endif
        connect(winRenderProgress, SIGNAL(renderFinished()),
                this, SLOT(renderCompleted()));
        connect(winRenderProgress, SIGNAL(renderCanceled()),
                this, SLOT(renderEnd()));
    }
    winRenderProgress->setProjects(listProjectsModel->getListPairs());
    winRenderProgress->start();
}
