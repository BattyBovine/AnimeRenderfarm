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


void Preferences::closeEvent(QCloseEvent *e)
{
    // Save settings when the window is closed
    this->saveSettings();

    // Emit a signal to let other objects know we're closing
    emit closing();

    e->accept();
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

void Preferences::toggleServerSettings(int index)
{
    // If the render server is set to "This Computer", disable server settings
    if(index<=0) {
        ui->labelServer->setEnabled(false);
        ui->editServer->setEnabled(false);
        ui->labelPort->setEnabled(false);
        ui->spinnerPort->setEnabled(false);
    } else {
        ui->labelServer->setEnabled(true);
        ui->editServer->setEnabled(true);
        ui->labelPort->setEnabled(true);
        ui->spinnerPort->setEnabled(true);
    }
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

    ui->editBindToAddress->setText(settings.value("BindIP","").toString());
    ui->spinnerBindToPort->setValue(settings.value("BindPort","26463").toInt());

    return true;
}

bool Preferences::saveSettings()
{
    saveAnimeStudioPath(ui->editAnimeStudioPath->text());
    saveOutputDirectory(ui->editOutputDirectory->text());
    saveOutputFormat(ui->comboOutputFormat->currentIndex());

    saveSetFrameRange(ui->groupSetFrameRange->isChecked());
    saveStartFrame(ui->spinnerStartFrame->value());
    saveEndFrame(ui->spinnerEndFrame->value());

    saveAntialiasedEdges(ui->checkAntialiasedEdges->isChecked());
    saveApplyShapeEffects(ui->checkApplyShapeEffects->isChecked());
    saveApplyLayerEffects(ui->checkApplyLayerEffects->isChecked());
    saveRenderAtHalfDimensions(ui->checkRenderAtHalfDimensions->isChecked());
    saveRenderAtHalfFramerate(ui->checkRenderAtHalfFramerate->isChecked());
    saveReducedParticles(ui->checkReducedParticles->isChecked());
    saveExtraSmoothImages(ui->checkExtraSmoothImages->isChecked());
    saveUseNTSCSafeColours(ui->checkUseNTSCSafeColours->isChecked());
    saveDoNotPremultiplyAlpha(ui->checkDoNotPremultiplyAlpha->isChecked());
    saveVariableLineWidths(ui->checkVariableLineWidths->isChecked());

    saveRenderServer(ui->comboRenderServer->currentIndex());
    saveServerIP(ui->editServer->text());
    saveServerPort(ui->spinnerPort->value());

    saveBindIP(ui->editBindToAddress->text());
    saveBindPort(ui->spinnerBindToPort->value());

    return true;
}



void Preferences::saveAnimeStudioPath(QString in) {
    settings.setValue("AnimeStudioPath", in); }
void Preferences::saveOutputDirectory(QString in) {
    settings.setValue("OutputDirectory", in); }
void Preferences::saveOutputFormat(int in) {
    settings.setValue("OutputFormat", in); }

void Preferences::saveSetFrameRange(bool in) {
    settings.setValue("SetFrameRange", in); }
void Preferences::saveStartFrame(int in) {
    settings.setValue("StartFrame", in); }
void Preferences::saveEndFrame(int in) {
    settings.setValue("EndFrame", in); }

void Preferences::saveAntialiasedEdges(bool in) {
    settings.setValue("AntialiasedEdges", in); }
void Preferences::saveApplyShapeEffects(bool in) {
    settings.setValue("ApplyShapeEffects", in); }
void Preferences::saveApplyLayerEffects(bool in) {
    settings.setValue("ApplyLayerEffects", in); }
void Preferences::saveRenderAtHalfDimensions(bool in) {
    settings.setValue("RenderAtHalfDimensions", in); }
void Preferences::saveRenderAtHalfFramerate(bool in) {
    settings.setValue("RenderAtHalfFramerate", in); }
void Preferences::saveReducedParticles(bool in) {
    settings.setValue("ReducedParticles", in); }
void Preferences::saveExtraSmoothImages(bool in) {
    settings.setValue("ExtraSmoothImages", in); }
void Preferences::saveUseNTSCSafeColours(bool in) {
    settings.setValue("UseNTSCSafeColours", in); }
void Preferences::saveDoNotPremultiplyAlpha(bool in) {
    settings.setValue("DoNotPremultiplyAlpha", in); }
void Preferences::saveVariableLineWidths(bool in) {
    settings.setValue("VariableLineWidths", in); }

void Preferences::saveRenderServer(int in) {
    settings.setValue("RenderServer", in); }
void Preferences::saveServerIP(QString in) {
    settings.setValue("ServerIP", in); }
void Preferences::saveServerPort(int in) {
    settings.setValue("ServerPort", in); }

void Preferences::saveBindIP(QString in) {
    settings.setValue("BindIP", in); }
void Preferences::saveBindPort(int in) {
    settings.setValue("BindPort", in); }



QString Preferences::getAnimeStudioPath() {
    return settings.value("AnimeStudioPath", "").toString(); }
QString Preferences::getOutputDirectory() {
    return settings.value("OutputDirectory", "").toString(); }
int Preferences::getOutputFormat() {
    return settings.value("OutputFormat", 3).toInt(); }

bool Preferences::getSetFrameRange() {
    return settings.value("SetFrameRange",false).toBool(); }
int Preferences::getStartFrame() {
    return settings.value("StartFrame",0).toInt(); }
int Preferences::getEndFrame() {
    return settings.value("EndFrame",0).toInt(); }

bool Preferences::getAntialiasedEdges() {
    return settings.value("AntialiasedEdges",true).toBool(); }
bool Preferences::getApplyShapeEffects() {
    return settings.value("ApplyShapeEffects",true).toBool(); }
bool Preferences::getApplyLayerEffects() {
    return settings.value("ApplyLayerEffects",true).toBool(); }
bool Preferences::getRenderAtHalfDimensions() {
    return settings.value("RenderAtHalfDimensions",false).toBool(); }
bool Preferences::getRenderAtHalfFramerate() {
    return settings.value("RenderAtHalfFramerate",false).toBool(); }
bool Preferences::getReducedParticles() {
    return settings.value("ReducedParticles",false).toBool(); }
bool Preferences::getExtraSmoothImages() {
    return settings.value("ExtraSmoothImages",true).toBool(); }
bool Preferences::getUseNTSCSafeColours() {
    return settings.value("UseNTSCSafeColours",false).toBool(); }
bool Preferences::getDoNotPremultiplyAlpha() {
    return settings.value("DoNotPremultiplyAlpha",false).toBool(); }
bool Preferences::getVariableLineWidths() {
    return settings.value("VariableLineWidths",true).toBool(); }

int Preferences::getRenderServer() {
    return settings.value("RenderServer",0).toInt(); }
QString Preferences::getServerIP() {
    return settings.value("ServerIP","127.0.0.1").toString(); }
int Preferences::getServerPort() {
    return settings.value("ServerPort","26463").toInt(); }

QString Preferences::getBindIP() {
    return settings.value("BindIP","").toString(); }
int Preferences::getBindPort() {
    return settings.value("BindPort","26463").toInt(); }
