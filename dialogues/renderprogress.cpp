#include "renderprogress.h"
#include "ui_renderprogress.h"

RenderProgress::RenderProgress(QWidget *parent) :
    QDialog(parent,Qt::CustomizeWindowHint|
            Qt::WindowMinimizeButtonHint|
            Qt::WindowCloseButtonHint),
    ui(new Ui::RenderProgress)
{
    ui->setupUi(this);

//    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);
}

RenderProgress::~RenderProgress()
{
    delete ui;
}
