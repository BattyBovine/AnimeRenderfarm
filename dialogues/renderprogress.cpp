#include "renderprogress.h"
#include "ui_renderprogress.h"

RenderProgress::RenderProgress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenderProgress)
{
    ui->setupUi(this);
}

RenderProgress::~RenderProgress()
{
    delete ui;
}
