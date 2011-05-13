#include "serversettings.h"
#include "ui_serversettings.h"

ServerSettings::ServerSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerSettings)
{
    ui->setupUi(this);

    this->loadSettings();
}

ServerSettings::~ServerSettings()
{
    delete ui;
}

void ServerSettings::closeEvent(QCloseEvent *)
{
    this->saveSettings();
}



bool ServerSettings::loadSettings()
{
    ui->editServer->setText(settings.value("Server","127.0.0.1").toString());
    ui->spinnerPort->setValue(settings.value("Port", "26463").toInt());

    return true;
}

bool ServerSettings::saveSettings()
{
    settings.setValue("Server", ui->editServer->text());
    settings.setValue("Port", ui->spinnerPort->value());

    return true;
}
