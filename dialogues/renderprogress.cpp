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
#ifdef Q_WS_WIN
    if(taskbarInterface)
        taskbarInterface->Release();
#endif

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

    emit renderCanceled();

    timer.stop();
#ifdef Q_WS_WIN
    updateTaskbarProgress(ui->progressRender->minimum());
    updateTaskbarState(TBPF_NOPROGRESS);
#endif
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
