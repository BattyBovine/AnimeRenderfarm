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

#ifndef SERVERSETTINGS_H
#define SERVERSETTINGS_H

#include <QDialog>

#include <QSettings>

namespace Ui {
    class ServerSettings;
}

class ServerSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ServerSettings(QWidget *parent = 0);
    ~ServerSettings();

private:
    Ui::ServerSettings *ui;

    void closeEvent(QCloseEvent *);

    QSettings settings;

signals:

public slots:

private slots:
    void renderServerStatus();
    int validateServer(QString);

    bool loadSettings();
    bool saveSettings();



public:
    enum IPStatus {
        OK,
        EMPTY,
        INVALID_LENGTH,
        EXTERNAL_IP
    };

};

#endif // SERVERSETTINGS_H
