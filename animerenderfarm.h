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

#ifndef ANIMERENDERFARM_H
#define ANIMERENDERFARM_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>

#include <QDragEnterEvent>
#include <QDropEvent>

#include <QSettings>
#include <QUrl>

#include "subclasses/qprojectslistmodel.h"

#include "dialogues/preferences.h"
#include "dialogues/rendermanager.h"
#include "dialogues/servermanager.h"

#ifdef Q_WS_WIN
#include <Windows.h>
#include <ShObjIdl.h>
#endif

namespace Ui {
    class AnimeRenderfarm;
}

class AnimeRenderfarm : public QMainWindow {
    Q_OBJECT
public:
    AnimeRenderfarm(QWidget *parent = 0);
    ~AnimeRenderfarm();

protected:
    void changeEvent(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void closeEvent(QCloseEvent *);
#ifdef Q_WS_WIN
    bool winEvent(MSG *message, long *result);
#endif

private:
    Ui::AnimeRenderfarm *ui;

    QSettings settings;

    qProjectsListModel *listProjectsModel;

    Preferences *winPreferences;
    RenderManager *winRenderManager;
    ServerManager *winServerManager;

#ifdef Q_WS_WIN
    unsigned int taskbarID;
    ITaskbarList3 *taskbarInterface;
#endif

    bool fileIsProject(QString);

    bool messageRemoveProjectsConfirm();

signals:

public slots:
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);

private slots:
    void filesOpened(QStringList);

    void showOpenProjectsDialogue();

    void openPreferences();
    void openServerManager();
    void openAboutApplication();
    void openAboutQt();

    void renderCompleted(QList< QPair<QString,QString> >);
    void renderEnd(QList< QPair<QString,QString> >);

    void renderProjects();
};

#endif // ANIMERENDERFARM_H
