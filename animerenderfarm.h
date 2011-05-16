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

#include "dialogues/rendersettings.h"
#include "dialogues/serversettings.h"
#include "dialogues/renderprogress.h"

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
    void changeEvent(QEvent *e);
    void keyPressEvent(QKeyEvent *);
    void closeEvent(QCloseEvent *);

#ifdef Q_WS_WIN
    bool winEvent(MSG *message, long *result);
#endif

private:
    Ui::AnimeRenderfarm *ui;

    QSettings settings;

    qProjectsListModel *listProjectsModel;

    RenderSettings *winRenderSettings;
    ServerSettings *winServerSettings;

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

    void openRenderSettings();
    void openServerSettings();
    void openAboutApplication();
    void openAboutQt();

    void renderFinished();

#ifdef Q_WS_WIN
    void updateTaskbarState(TBPFLAG state);
    void updateTaskbarProgress(int,int);
#endif

    void renderProjects();
};

#endif // ANIMERENDERFARM_H
