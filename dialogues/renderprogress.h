#ifndef RENDERPROGRESS_H
#define RENDERPROGRESS_H

#include <QDialog>
#include <QMessageBox>

#include <QCloseEvent>
#include <QTimer>

#ifdef Q_WS_WIN
#include <Windows.h>
#include <ShObjIdl.h>
#endif

namespace Ui {
    class RenderProgress;
}

class RenderProgress : public QDialog
{
    Q_OBJECT

public:
    explicit RenderProgress(QWidget *parent = 0);
    ~RenderProgress();

#ifdef Q_WS_WIN
    bool initTaskbarInterface(WId,ITaskbarList3*);
#endif

    void start();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::RenderProgress *ui;

    QTimer timer;

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

#endif // RENDERPROGRESS_H
