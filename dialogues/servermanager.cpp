#include "servermanager.h"
#include "ui_servermanager.h"

ServerManager::ServerManager(QWidget *parent) :
    QDialog(parent,Qt::WindowCloseButtonHint),
    ui(new Ui::ServerManager)
{
    ui->setupUi(this);

    ui->editServerStatus->setText("Preparing server...");
    serverproc = new ServerThread(this);
    serverproc->start();
}

ServerManager::~ServerManager()
{
    if(serverproc)
        serverproc->deleteLater();

    delete ui;
}



void ServerManager::getStatus(QString status)
{
    ui->editServerStatus->setText(status);
}
