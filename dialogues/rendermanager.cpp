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

    this->setFixedSize(this->sizeHint());

    cRenderThread = new RenderThread(this);
    cRenderThread->setExe(settings.value("AnimeStudioPath").toString());
    cRenderThread->setOutputDirectory(settings.value("OutputDirectory").toString());
    cRenderThread->setSwitches(settings.value("AntialiasedEdges").toBool(),
                               settings.value("ApplyShapeEffects").toBool(),
                               settings.value("ApplyLayerEffects").toBool(),
                               settings.value("RenderAtHalfDimensions").toBool(),
                               settings.value("RenderAtHalfFramerate").toBool(),
                               settings.value("ReducedParticles").toBool(),
                               settings.value("ExtraSmoothImages").toBool(),
                               settings.value("UseNTSCSafeColours").toBool(),
                               settings.value("DoNotPremultiplyAlpha").toBool(),
                               settings.value("VariableLineWidths").toBool());

    connect(cRenderThread, SIGNAL(renderComplete(QPair<QString,QString>)),
            this, SLOT(renderEnd(QPair<QString,QString>)));

    ui->labelProgressInfo->setText(tr("Waiting for projects to render..."));

#ifdef Q_WS_WIN
    taskbarInterface = NULL;
#endif
}

RenderManager::~RenderManager()
{
#ifdef Q_WS_WIN
    updateTaskbarState(TBPF_NOPROGRESS);
#endif

    if(cRenderThread)
        cRenderThread->deleteLater();

    delete ui;
}

void RenderManager::closeEvent(QCloseEvent *e)
{
    if(ui->progressRender->value() < ui->progressRender->maximum()) {
        QMessageBox::StandardButton response;
        response = QMessageBox::question(this, tr("Are you sure?"),
            "<p>"+tr("Are you sure you wish to cancel the render job?")+"</p>",
            QMessageBox::Yes | QMessageBox::No);

        if(response==QMessageBox::No) {
            e->ignore();
            return;
        }
    }

    emit renderCanceled();

    e->accept();
}



#ifdef Q_WS_WIN
bool RenderManager::initTaskbarInterface(WId win, ITaskbarList3 *tb)
{
    winMain = win;
    taskbarInterface = tb;
    return (winMain && taskbarInterface);
}
#endif

void RenderManager::setProjects(QList<QPair<QString, QString> > input)
{
    listProjects = input;
}

void RenderManager::start()
{
    if(listProjects.isEmpty()) {
        emit renderFailed("Project list is empty");
        this->close();
    }

    ui->labelProgressInfo->setText(tr("Preparing projects for render..."));
    ui->progressRender->setMinimum(0);
    ui->progressRender->setMaximum(0);
    ui->progressRender->reset();
#ifdef Q_WS_WIN
    updateTaskbarState(TBPF_INDETERMINATE);
#endif

//    ui->progressRender->setMaximum(listProjects.count());

    renderStartNext();

    this->show();
}



void RenderManager::renderStartNext()
{
    cRenderThread->setProject(listProjects.first());
    listProjects.removeFirst();

    cRenderThread->start();
}
void RenderManager::renderEnd(QPair<QString,QString>)
{
    if(listProjects.isEmpty()) {
        emit renderFinished();
    } else {
        renderStartNext();
    }
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



void RenderManager::updateProgressBarValue()
{
    int value = ui->progressRender->value()+1;
    updateTaskbarProgress(value);
#ifdef Q_WS_WIN
    updateTaskbarState(TBPF_NORMAL);
#endif
}

void RenderManager::updateThreadPriority(int priority)
{
    if(cRenderThread && cRenderThread->isRunning())
        cRenderThread->setPriority((QThread::Priority)priority);
}
