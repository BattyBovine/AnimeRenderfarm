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

#include "renderprogress.h"
#include "ui_renderprogress.h"

RenderProgress::RenderProgress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenderProgress)
{
    ui->setupUi(this);

    ui->progressRender->setValue(0);
    ui->progressRender->setMinimum(0);
    ui->progressRender->setMaximum(1000);

    timer.setInterval(10);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateProgressBarValue()));
    connect(this, SIGNAL(renderFinished()), this, SLOT(close()));

#ifdef Q_WS_WIN
    taskbarInterface = NULL;
#endif
}

RenderProgress::~RenderProgress()
{
    delete ui;
}

void RenderProgress::closeEvent(QCloseEvent *e)
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

#ifdef Q_WS_WIN
    updateTaskbarState(TBPF_NOPROGRESS);
#endif
    timer.stop();
    emit renderCanceled();

    e->accept();
}



#ifdef Q_WS_WIN
bool RenderProgress::initTaskbarInterface(WId win, ITaskbarList3 *tb)
{
    winMain = win;
    taskbarInterface = tb;
    return (winMain && taskbarInterface);
}
#endif

void RenderProgress::start()
{
    timer.start();
#ifdef Q_WS_WIN
    updateTaskbarState(TBPF_INDETERMINATE);
#endif
    this->show();
}



#ifdef Q_WS_WIN
void RenderProgress::updateTaskbarState(TBPFLAG state)
{
    if(taskbarInterface)
        taskbarInterface->SetProgressState(winMain, state);
}
#endif

void RenderProgress::updateTaskbarProgress(int value)
{
    ui->progressRender->setValue(value);
#ifdef Q_WS_WIN
    if(taskbarInterface)
        taskbarInterface->SetProgressValue(winMain, value, ui->progressRender->maximum());
#endif
}



void RenderProgress::updateProgressBarValue()
{
    int value = ui->progressRender->value()+1;
    updateTaskbarProgress(value);
#ifdef Q_WS_WIN
    updateTaskbarState(TBPF_NORMAL);
#endif

    if(value >= ui->progressRender->maximum()) {
        emit renderFinished();
        return;
    }
}
