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

#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include <QDialog>
#include <QDesktopWidget>
#include <QMessageBox>

#include <QSettings>
#include <QFileDialog>

namespace Ui {
    class RenderSettings;
}

class RenderSettings : public QDialog
{
    Q_OBJECT

public:
    explicit RenderSettings(QWidget *parent = 0);
    ~RenderSettings();

private:
    Ui::RenderSettings *ui;

    void closeEvent(QCloseEvent *);

    QSettings settings;

signals:

public slots:

private slots:
    bool loadSettings();
    bool saveSettings();
    void showOpenAnimeStudioDialogue();
    void showOpenOutputDirectoryDialogue();

};

#endif // RENDERSETTINGS_H
