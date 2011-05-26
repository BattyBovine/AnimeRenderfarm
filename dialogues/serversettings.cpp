/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Anime Renderfarm - A remote batch renderer for Anime Studio            *
 *  Copyright (C) 2011 Batty Bovine Productions                            *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "serversettings.h"
#include "ui_serversettings.h"

ServerSettings::ServerSettings(QWidget *parent) :
    QDialog(parent,Qt::WindowCloseButtonHint),
    ui(new Ui::ServerSettings)
{
    ui->setupUi(this);

    // Force the size of the window to remain at its created size
    this->setFixedSize(this->sizeHint());

    // Load settings
    this->loadSettings();

    // Check whether to hide server settings based on combo selection
    this->renderServerStatus(ui->comboRenderServer->currentIndex());
}

ServerSettings::~ServerSettings()
{
    delete ui;
}

void ServerSettings::closeEvent(QCloseEvent *)
{
    // Save settings when the window is closed
    this->saveSettings();
}


void ServerSettings::renderServerStatus(int i)
{
    // Check whether local or remote rendering is set
    bool serverstatus = (i<=0?false:true);

    // If remote, enable remote server settings
    ui->labelServer->setEnabled(serverstatus);
    ui->labelPort->setEnabled(serverstatus);
    ui->editServer->setEnabled(serverstatus);
    ui->spinnerPort->setEnabled(serverstatus);
}

int ServerSettings::validateServer(QString ip)
{
    // Check if the input is empty, and if so, set to default values
    if(ip.trimmed().isEmpty()) {
        ui->comboRenderServer->setCurrentIndex(0);
        ui->editServer->setText("127.0.0.1");
        return EMPTY;
    }

    // Check the number of octets, and fail if not equal to four
    QStringList ipocts = ip.trimmed().split(".", QString::SkipEmptyParts);
    if(ipocts.size() != 4)
        return INVALID_LENGTH;

    // Convert octets to unsigned char to ensure they are within valid ranges
    uchar octs[4];
    for(short i=0; i<ipocts.size(); i++)
        octs[i] = ipocts.at(i).toInt();

    // Check whether this is an external IP
    if((octs[0]!=192 && octs[1]!=168) &&
       (octs[0]!=172 && octs[1]!=(octs[1]&0x1F)) &&
        octs[0]!=10)
        return EXTERNAL_IP;

    return OK;
}



bool ServerSettings::loadSettings()
{
    ui->comboRenderServer->setCurrentIndex(settings.value("RenderServer",0).toInt());

    ui->editServer->setText(settings.value("Server","127.0.0.1").toString());
    ui->spinnerPort->setValue(settings.value("Port","26463").toInt());

    return true;
}

bool ServerSettings::saveSettings()
{
    settings.setValue("RenderServer", ui->comboRenderServer->currentIndex());

    settings.setValue("Server",ui->editServer->text());
    settings.setValue("Port",ui->spinnerPort->value());

    return true;
}
