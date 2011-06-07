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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QMessageBox>

#include <QSettings>
#include <QFileDialog>

namespace Ui {
    class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

    QString getAnimeStudioPath();
    QString getOutputDirectory();
    int getOutputFormat();

    bool getSetFrameRange();
    int getStartFrame();
    int getEndFrame();

    bool getAntialiasedEdges();
    bool getApplyShapeEffects();
    bool getApplyLayerEffects();
    bool getRenderAtHalfDimensions();
    bool getRenderAtHalfFramerate();
    bool getReducedParticles();
    bool getExtraSmoothImages();
    bool getUseNTSCSafeColours();
    bool getDoNotPremultiplyAlpha();
    bool getVariableLineWidths();

    int getRenderServer();
    QString getServerIP();
    int getServerPort();

private:
    Ui::Preferences *ui;

    void closeEvent(QCloseEvent *);

    QSettings settings;

signals:
    void closing();

public slots:
    void saveAnimeStudioPath(QString);
    void saveOutputDirectory(QString);
    void saveOutputFormat(int);

    void saveSetFrameRange(bool);
    void saveStartFrame(int);
    void saveEndFrame(int);

    void saveAntialiasedEdges(bool);
    void saveApplyShapeEffects(bool);
    void saveApplyLayerEffects(bool);
    void saveRenderAtHalfDimensions(bool);
    void saveRenderAtHalfFramerate(bool);
    void saveReducedParticles(bool);
    void saveExtraSmoothImages(bool);
    void saveUseNTSCSafeColours(bool);
    void saveDoNotPremultiplyAlpha(bool);
    void saveVariableLineWidths(bool);

    void saveRenderServer(int);
    void saveServerIP(QString);
    void saveServerPort(int);

private slots:
    bool loadSettings();
    bool saveSettings();
    void showOpenAnimeStudioDialogue();
    void showOpenOutputDirectoryDialogue();
    void toggleServerSettings(int);

};

#endif // PREFERENCES_H
