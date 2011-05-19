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

#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <QDialog>
#include <QMessageBox>

#include <QCloseEvent>
#include <QTimer>

#ifdef Q_WS_WIN
#include <Windows.h>
#include <ShObjIdl.h>
#endif

namespace Ui {
    class RenderManager;
}

class RenderManager : public QDialog
{
    Q_OBJECT

public:
    explicit RenderManager(QWidget *parent = 0);
    ~RenderManager();

    void setProjects(QList< QPair<QString,QString> >);
#ifdef Q_WS_WIN
    bool initTaskbarInterface(WId,ITaskbarList3*);
#endif

    void start();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::RenderManager *ui;

    QList< QPair<QString,QString> > listProjects;

#ifdef Q_WS_WIN
    WId winMain;
    ITaskbarList3 *taskbarInterface;

    void updateTaskbarState(TBPFLAG state);
#endif
    void updateTaskbarProgress(int);

signals:
    void renderFinished();
    void renderCanceled();

public slots:

private slots:
    void updateProgressBarValue();
};

#endif // RENDERMANAGER_H
