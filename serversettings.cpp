#include "serversettings.h"
#include "ui_serversettings.h"

ServerSettings::ServerSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerSettings)
{
    ui->setupUi(this);

    this->loadSettings();

    this->renderServerStatus();
}

ServerSettings::~ServerSettings()
{
    delete ui;
}


void ServerSettings::renderServerStatus()
{
    bool serverstatus = (ui->comboRenderServer->currentIndex()<=0?false:true);

    ui->labelServer->setVisible(serverstatus);
    ui->labelPort->setVisible(serverstatus);
    ui->editServer->setVisible(serverstatus);
    ui->spinnerPort->setVisible(serverstatus);
}



bool ServerSettings::loadSettings()
{
    ui->comboRenderServer->setCurrentIndex(settings.value("RenderServer", 0).toInt());

    ui->editServer->setText(settings.value("Server","127.0.0.1").toString());
    ui->spinnerPort->setValue(settings.value("Port", "26463").toInt());

    return true;
}

bool ServerSettings::saveSettings()
{
    settings.setValue("RenderServer", ui->comboRenderServer->currentIndex());

    settings.setValue("Server", ui->editServer->text());
    settings.setValue("Port", ui->spinnerPort->value());

    return true;
}
