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

#include "preferences.h"
#include "ui_preferences.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent,Qt::WindowCloseButtonHint),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);

    // Load the saved settings if they exist
    this->loadSettings();
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::closeEvent(QCloseEvent *)
{
    // Save settings when the window is closed
    this->saveSettings();
}



void Preferences::showOpenAnimeStudioDialogue()
{
    QString supported = "";
#if defined Q_WS_WIN
    // On Win32 systems, we want to locate an executable with the .exe extension
    supported += "Executable Files (*.exe);;";
#elif defined Q_WS_MACX
    // On Mac OS X, we want to locate a .app bundle and search it for the executable
    supported += "Application Bundles (*.app);;";
#endif
    // On any other system, we can't guarantee that the executable has a valid extension
    supported += "All files (*.*);;";

    // Use the current executable path value as the starting point, or root if not defined
    QString startpath = QDir::toNativeSeparators(ui->editAnimeStudioPath->text());
    if(startpath.isEmpty())
        startpath = QDir::rootPath();
    else
        startpath = startpath.mid(0,startpath.lastIndexOf(QDir::separator())+1);

    // Load the result of the file browser activity into a QString
    QString exe = QFileDialog::getOpenFileName(
        this, tr("Open Anime Studio Executable"),
        startpath, supported
    );
    // If empty, the user probably canceled; pretend nothing happened
    if(exe.isEmpty())
        return;

    // Check to see if the file is executable; if not, throw an error and pretend nothing happened
    if(!QFileInfo(exe).isExecutable()) {
        QMessageBox::critical(this, tr("Not An Executable File"),
                              tr("This is not an executable file. Please select the Anime "
                                 "Studio executable file, and make sure you have the "
                                 "necessary permissions to run it."));
        return;
    }

    // We probably don't need to check length now, but whatever, do it and set the value
    if(exe.length()>0)
        ui->editAnimeStudioPath->setText(QDir::toNativeSeparators(exe));
}

void Preferences::showOpenOutputDirectoryDialogue()
{
    // Start from the currently set directory or, if empty, the default "Movies" directory
    QString startpath = QDir::toNativeSeparators(ui->editOutputDirectory->text());
    if(startpath.isEmpty())
        startpath = QDesktopServices::storageLocation(QDesktopServices::MoviesLocation);

    // Get the selected folder
    QString folder = QFileDialog::getExistingDirectory(
        this, tr("Open Output Folder"),
        startpath, QFileDialog::ShowDirsOnly
    );
    // Set the path if not empty
    if(!folder.isEmpty())
        ui->editOutputDirectory->setText(QDir::toNativeSeparators(folder));
    else
        return;
}



bool Preferences::loadSettings()
{
    ui->editAnimeStudioPath->setText(settings.value("AnimeStudioPath", "").toString());
    ui->editOutputDirectory->setText(settings.value("OutputDirectory", "").toString());
    ui->comboOutputFormat->setCurrentIndex(settings.value("OutputFormat", 3).toInt());

    ui->groupSetFrameRange->setChecked(settings.value("SetFrameRange",false).toBool());
    ui->spinnerStartFrame->setValue(settings.value("StartFrame",0).toInt());
    ui->spinnerEndFrame->setValue(settings.value("EndFrame",0).toInt());

    ui->checkAntialiasedEdges->setChecked(settings.value("AntialiasedEdges",true).toBool());
    ui->checkApplyShapeEffects->setChecked(settings.value("ApplyShapeEffects",true).toBool());
    ui->checkApplyLayerEffects->setChecked(settings.value("ApplyLayerEffects",true).toBool());
    ui->checkRenderAtHalfDimensions->setChecked(settings.value("RenderAtHalfDimensions",false).toBool());
    ui->checkRenderAtHalfFramerate->setChecked(settings.value("RenderAtHalfFramerate",false).toBool());
    ui->checkReducedParticles->setChecked(settings.value("ReducedParticles",false).toBool());
    ui->checkExtraSmoothImages->setChecked(settings.value("ExtraSmoothImages",true).toBool());
    ui->checkUseNTSCSafeColours->setChecked(settings.value("UseNTSCSafeColours",false).toBool());
    ui->checkDoNotPremultiplyAlpha->setChecked(settings.value("DoNotPremultiplyAlpha",false).toBool());
    ui->checkVariableLineWidths->setChecked(settings.value("VariableLineWidths",true).toBool());

    ui->comboRenderServer->setCurrentIndex(settings.value("RenderServer",0).toInt());
    ui->editServer->setText(settings.value("ServerIP","127.0.0.1").toString());
    ui->spinnerPort->setValue(settings.value("ServerPort","26463").toInt());

    return true;
}

bool Preferences::saveSettings()
{
    saveAnimeStudioPath();
    saveOutputDirectory();
    saveOutputFormat();

    saveSetFrameRange();
    saveStartFrame();
    saveEndFrame();

    saveAntialiasedEdges();
    saveApplyShapeEffects();
    saveApplyLayerEffects();
    saveRenderAtHalfDimensions();
    saveRenderAtHalfFramerate();
    saveReducedParticles();
    saveExtraSmoothImages();
    saveUseNTSCSafeColours();
    saveDoNotPremultiplyAlpha();
    saveVariableLineWidths();

    saveRenderServer();
    saveServerIP();
    saveServerPort();

    return true;
}

void Preferences::saveAnimeStudioPath() {
    settings.setValue("AnimeStudioPath", ui->editAnimeStudioPath->text()); }
void Preferences::saveOutputDirectory() {
    settings.setValue("OutputDirectory", ui->editOutputDirectory->text()); }
void Preferences::saveOutputFormat() {
    settings.setValue("OutputFormat", ui->comboOutputFormat->currentIndex()); }

void Preferences::saveSetFrameRange() {
    settings.setValue("SetFrameRange", ui->groupSetFrameRange->isChecked()); }
void Preferences::saveStartFrame() {
    settings.setValue("StartFrame", ui->spinnerStartFrame->value()); }
void Preferences::saveEndFrame() {
    settings.setValue("EndFrame", ui->spinnerEndFrame->value()); }

void Preferences::saveAntialiasedEdges() {
    settings.setValue("AntialiasedEdges", ui->checkAntialiasedEdges->isChecked()); }
void Preferences::saveApplyShapeEffects() {
    settings.setValue("ApplyShapeEffects", ui->checkApplyShapeEffects->isChecked()); }
void Preferences::saveApplyLayerEffects() {
    settings.setValue("ApplyLayerEffects", ui->checkApplyLayerEffects->isChecked()); }
void Preferences::saveRenderAtHalfDimensions() {
    settings.setValue("RenderAtHalfDimensions", ui->checkRenderAtHalfDimensions->isChecked()); }
void Preferences::saveRenderAtHalfFramerate() {
    settings.setValue("RenderAtHalfFramerate", ui->checkRenderAtHalfFramerate->isChecked()); }
void Preferences::saveReducedParticles() {
    settings.setValue("ReducedParticles", ui->checkReducedParticles->isChecked()); }
void Preferences::saveExtraSmoothImages() {
    settings.setValue("ExtraSmoothImages", ui->checkExtraSmoothImages->isChecked()); }
void Preferences::saveUseNTSCSafeColours() {
    settings.setValue("UseNTSCSafeColours", ui->checkUseNTSCSafeColours->isChecked()); }
void Preferences::saveDoNotPremultiplyAlpha() {
    settings.setValue("DoNotPremultiplyAlpha", ui->checkDoNotPremultiplyAlpha->isChecked()); }
void Preferences::saveVariableLineWidths() {
    settings.setValue("VariableLineWidths", ui->checkVariableLineWidths->isChecked()); }

void Preferences::saveRenderServer() {
    settings.setValue("RenderServer", ui->comboRenderServer->currentIndex()); }
void Preferences::saveServerIP() {
    settings.setValue("ServerIP", ui->editServer->text()); }
void Preferences::saveServerPort() {
    settings.setValue("ServerPort", ui->spinnerPort->value()); }
