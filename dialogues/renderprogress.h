#ifndef RENDERPROGRESS_H
#define RENDERPROGRESS_H

#include <QDialog>
#include <QMessageBox>

#include <QCloseEvent>
#include <QTimer>

#ifdef Q_WS_WIN
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

    void start();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::RenderProgress *ui;

    QTimer timer;

signals:
    void stateChanged(TBPFLAG);
    void progressChanged(int,int);
    void renderFinished();
    void renderCanceled();

public slots:

private slots:
    void updateProgressBarValue();
};

#endif // RENDERPROGRESS_H
