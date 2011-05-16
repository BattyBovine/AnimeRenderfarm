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

#include "dialogues/rendersettings.h"
#include "dialogues/serversettings.h"
#include "dialogues/renderprogress.h"
#include "subclasses/qprojectslistmodel.h"

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

private:
    Ui::AnimeRenderfarm *ui;

    QSettings settings;

    qProjectsListModel *listProjectsModel;

    RenderSettings *winRenderSettings;
    ServerSettings *winServerSettings;

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

    void renderProjects();
};

#endif // ANIMERENDERFARM_H
