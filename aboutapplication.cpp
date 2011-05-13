#include "aboutapplication.h"
#include "ui_aboutapplication.h"

AboutApplication::AboutApplication(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutApplication)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("About ")+qApp->applicationName());
    ui->labelApplicationName->setText(qApp->applicationName());
    ui->labelApplicationVersion->setText(tr("Version ")+qApp->applicationVersion());
    ui->labelApplicationCopyright->setText(tr("©2011 ")+qApp->organizationName()+tr(". All Rights Reserved."));
}

AboutApplication::~AboutApplication()
{
    delete ui;
}
