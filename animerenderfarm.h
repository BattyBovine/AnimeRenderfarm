#ifndef ANIMERENDERFARM_H
#define ANIMERENDERFARM_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFileDialog>

#include <QDragEnterEvent>
#include <QDropEvent>

#include <QSettings>
#include <QUrl>

#include "rendersettings.h"
#include "aboutapplication.h"
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

    RenderSettings *winRenderSettings;
    AboutApplication *winAboutApplication;

    qProjectsListModel *listProjectsModel;

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
    void openAboutApplication();

    void renderProjects();
};

#endif // ANIMERENDERFARM_H
