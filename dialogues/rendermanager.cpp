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

#include "rendermanager.h"
#include "ui_rendermanager.h"

RenderManager::RenderManager(QWidget *parent) :
    QDialog(parent,Qt::WindowCloseButtonHint),
    ui(new Ui::RenderManager)
{
    ui->setupUi(this);

    cClientThread = NULL;
    cRenderThread = NULL;

    // Create a new render thread, and set the necessary options
    Preferences *prefsman = new Preferences(this);

    if(prefsman->getRenderServer()>0) {
        // If the render server is set to "Remote", start a new client thread and connect to it
        cClientThread = new ClientThread(this);
        cClientThread->setOutputDirectory(prefsman->getOutputDirectory());
        cClientThread->setFormat(prefsman->getOutputFormat());
        cClientThread->setSwitches(prefsman->getAntialiasedEdges(),
                                   prefsman->getApplyShapeEffects(),
                                   prefsman->getApplyLayerEffects(),
                                   prefsman->getRenderAtHalfDimensions(),
                                   prefsman->getRenderAtHalfFramerate(),
                                   prefsman->getReducedParticles(),
                                   prefsman->getExtraSmoothImages(),
                                   prefsman->getUseNTSCSafeColours(),
                                   prefsman->getDoNotPremultiplyAlpha(),
                                   prefsman->getVariableLineWidths());
        connect(cClientThread, SIGNAL(renderComplete(QPair<QString,QString>)),
                this, SLOT(renderEnd(QPair<QString,QString>)));
        connect(cClientThread, SIGNAL(renderProgress(QString,int)),
                this, SLOT(progressUpdate(QString,int)));
    } else {
        // Otherwise, start a local render thread
        cRenderThread = new RenderThread(this);
        cRenderThread->setExe(prefsman->getAnimeStudioPath());
        cRenderThread->setOutputDirectory(prefsman->getOutputDirectory());
        cRenderThread->setFormat(prefsman->getOutputFormat());
        cRenderThread->setSwitches(prefsman->getAntialiasedEdges(),
                                   prefsman->getApplyShapeEffects(),
                                   prefsman->getApplyLayerEffects(),
                                   prefsman->getRenderAtHalfDimensions(),
                                   prefsman->getRenderAtHalfFramerate(),
                                   prefsman->getReducedParticles(),
                                   prefsman->getExtraSmoothImages(),
                                   prefsman->getUseNTSCSafeColours(),
                                   prefsman->getDoNotPremultiplyAlpha(),
                                   prefsman->getVariableLineWidths());
        connect(cRenderThread, SIGNAL(renderComplete(QPair<QString,QString>)),
                this, SLOT(renderEnd(QPair<QString,QString>)));
        connect(cRenderThread, SIGNAL(renderProgress(QString,int)),
                this, SLOT(progressUpdate(QString,int)));
    }

    // After this, we no longer need the preferences
    prefsman->deleteLater();

    // Default to a busy indicator
    ui->labelProgressInfo->setText(tr("Waiting for projects to render..."));
    ui->progressRender->setMinimum(0);
    ui->progressRender->setMaximum(0);
    ui->progressRender->reset();
#ifdef Q_WS_WIN
    taskbarInterface = NULL;
#endif
    // Reset the completed projects counter
    completecount = 0;
}

RenderManager::~RenderManager()
{
    if(cClientThread)
        cClientThread->deleteLater();
    if(cRenderThread)
        cRenderThread->deleteLater();

    delete ui;
}

void RenderManager::closeEvent(QCloseEvent *e)
{
    // If we're not done rendering, ask for confirmation before closing the window
    if(ui->progressRender->value() < ui->progressRender->maximum()) {
        QMessageBox::StandardButton response;
        response = QMessageBox::question(this, tr("Are you sure?"),
            "<p>"+tr("Are you sure you wish to cancel the render job?")+"</p>",
            QMessageBox::Yes | QMessageBox::No);

        // If we don't want to cancel after all, ignore the event and continue as usual
        if(response==QMessageBox::No) {
            e->ignore();
            return;
        }
    }
    // Otherwise, let other objects know we've been canceled, and accept the event
    emit renderCanceled(listProjects);

    // Reset the taskbar indicator before destroying the window
#ifdef Q_WS_WIN
    updateTaskbarState(TBPF_NOPROGRESS);
#endif

    // Emit a signal to let other objects know we're closing
    emit closing();

    e->accept();
}



#ifdef Q_WS_WIN
bool RenderManager::initTaskbarInterface(WId win, ITaskbarList3 *tb)
{
    // Get the taskbar and window ID from the main window
    winMain = win;
    taskbarInterface = tb;
    return (winMain && taskbarInterface);
}
#endif

void RenderManager::setProjects(QList< QPair<QString,QString> > input)
{
    // Get the list of projects from the main window
    listProjects = input;
}

void RenderManager::start()
{
    // If the projects list is empty, we shouldn't even bother trying
    if(listProjects.isEmpty()) {
        emit renderFailed("Project list is empty");
        this->close();
    }
    // Set the taskbar icon to indeterminate while we wait for results
#ifdef Q_WS_WIN
    updateTaskbarState(TBPF_INDETERMINATE);
#endif

    // Give us 100 progress units for all projects
    ui->progressRender->setMinimum(0);
    ui->progressRender->setMaximum(listProjects.count()*100);

    //Finally, begin the first render, and open the window
    renderStartNext();
#ifdef Q_WS_MACX
    // Open as a modal dialogue on Mac OS X, like a proper progress meter
    this->open();
#else
    // On other systems, show the usual dialogue, allowing the user to minimise the main window
    this->show();
#endif
}



void RenderManager::renderStartNext()
{
    // Set taskbar state to a standard progress indicator
#ifdef Q_WS_WIN
    updateTaskbarState(TBPF_NORMAL);
#endif

    // Set the first project in the list as the next render, then begin the render process on
    // whatever interface we're using
    if(cClientThread) {
        cClientThread->setProject(listProjects.first());
        cClientThread->start();
    } else if(cRenderThread) {
        cRenderThread->setProject(listProjects.first());
        cRenderThread->start();
    }
}

void RenderManager::renderEnd(QPair<QString,QString>)
{
    // Increase the progress counter
    completecount++;

    // Remove the first project in preparation for the next loop
    listProjects.removeFirst();

    // Tell other classes we're done if there's nothing left, otherwise start the next render
    if(listProjects.isEmpty())
        emit renderFinished(listProjects);
    else
        renderStartNext();
}



void RenderManager::progressUpdate(QString info, int current)
{
    // Set the progress label to show which file we're about to render
    ui->labelProgressInfo->setText(info);

    // Set the current progress to all completed projects, plus the current project's progress
    updateTaskbarProgress((completecount*100)+current);
}



#ifdef Q_WS_WIN
void RenderManager::updateTaskbarState(TBPFLAG state)
{
    if(taskbarInterface)
        taskbarInterface->SetProgressState(winMain, state);
}
#endif

void RenderManager::updateTaskbarProgress(int value)
{
    ui->progressRender->setValue(value);
#ifdef Q_WS_WIN
    if(taskbarInterface)
        taskbarInterface->SetProgressValue(winMain, value, ui->progressRender->maximum());
#endif
}



void RenderManager::updateThreadPriority(int priority)
{
    // Set the priority of the thread, though I'm not sure it makes a difference
    if(cRenderThread && cRenderThread->isRunning())
        cRenderThread->setPriority((QThread::Priority)priority);
}
