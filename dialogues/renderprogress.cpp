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
    connect(this, SIGNAL(finished(int)), this, SLOT(close()));
}

RenderProgress::~RenderProgress()
{
    delete ui;
}

void RenderProgress::closeEvent(QCloseEvent *)
{
    timer.stop();

#ifdef Q_WS_WIN
    emit stateChanged(TBPF_NOPROGRESS);
#endif
}



void RenderProgress::start() {
    timer.start();
    emit stateChanged(TBPF_INDETERMINATE);
}



void RenderProgress::updateProgressBarValue() {
    int value = ui->progressRender->value()+1;
    ui->progressRender->setValue(value);

    if(value >= ui->progressRender->maximum()) {
        emit finished(value);
        return;
    }

#ifdef Q_WS_WIN
    emit stateChanged(TBPF_NORMAL);
    emit progressChanged(value, ui->progressRender->maximum());
#endif
}
