#include "servermanager.h"
#include "ui_servermanager.h"

ServerManager::ServerManager(QWidget *parent) :
    QDialog(parent,Qt::WindowCloseButtonHint),
    ui(new Ui::ServerManager)
{
    ui->setupUi(this);

    Preferences *prefsman = new Preferences(this);
    ui->editServerStatus->setText("Preparing server...");

    serverproc = new ServerThread(this);
    serverproc->setBindIP(prefsman->getBindIP());
    serverproc->setBindPort(prefsman->getBindPort());
    serverproc->start();
    prefsman->deleteLater();
}

ServerManager::~ServerManager()
{
    if(serverproc) {
        serverproc->terminate();
        serverproc->deleteLater();
    }

    delete ui;
}

void ServerManager::closeEvent(QCloseEvent *e)
{
    // Emit a signal to let other objects know we're closing
    emit closing();

    e->accept();
}



void ServerManager::getStatus(QString status)
{
    ui->editServerStatus->setText(status);
}
