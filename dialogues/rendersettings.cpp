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

#include "rendersettings.h"
#include "ui_rendersettings.h"

RenderSettings::RenderSettings(QWidget *parent) :
    QDialog(parent,Qt::WindowCloseButtonHint),
    ui(new Ui::RenderSettings)
{
    ui->setupUi(this);

    this->loadSettings();
}

RenderSettings::~RenderSettings()
{
    delete ui;
}

void RenderSettings::closeEvent(QCloseEvent *)
{
    this->saveSettings();
}



void RenderSettings::showOpenAnimeStudioDialogue()
{
    QString exe = QFileDialog::getOpenFileName(
        this, tr("Open Anime Studio Executable"),
        QDir::toNativeSeparators(QDir::rootPath())
    );

    if(!QFileInfo(exe).isExecutable()) {
        QMessageBox::critical(this, tr("Not An Executable"),
                              tr("This is not an executable file. Please make sure to "
                                 "select the Anime Studio executable file."));
        return;
    }

    if(exe.length()>0)
        ui->editAnimeStudioPath->setText(QDir::toNativeSeparators(exe));
}

void RenderSettings::showOpenOutputDirectoryDialogue()
{
    QString folder = QFileDialog::getExistingDirectory(
        this, tr("Open Output Folder"),
        QDir::toNativeSeparators(QDir::homePath()),
        QFileDialog::ShowDirsOnly
    );

    if(!folder.isEmpty())
        ui->editOutputDirectory->setText(QDir::toNativeSeparators(folder));
}



bool RenderSettings::loadSettings()
{
    ui->editAnimeStudioPath->setText(settings.value("AnimeStudioPath", "").toString());
    ui->editOutputDirectory->setText(settings.value("OutputDirectory", "").toString());
    ui->comboOutputFormat->setCurrentIndex(settings.value("OutputFormat", 0).toInt());

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

    return true;
}

bool RenderSettings::saveSettings()
{
    settings.setValue("AnimeStudioPath", ui->editAnimeStudioPath->text());
    settings.setValue("OutputDirectory", ui->editOutputDirectory->text());
    settings.setValue("OutputFormat", ui->comboOutputFormat->currentIndex());

    settings.setValue("SetFrameRange", ui->groupSetFrameRange->isChecked());
    settings.setValue("StartFrame", ui->spinnerStartFrame->value());
    settings.setValue("EndFrame", ui->spinnerEndFrame->value());

    settings.setValue("AntialiasedEdges", ui->checkAntialiasedEdges->isChecked());
    settings.setValue("ApplyShapeEffects", ui->checkApplyShapeEffects->isChecked());
    settings.setValue("ApplyLayerEffects", ui->checkApplyLayerEffects->isChecked());
    settings.setValue("RenderAtHalfDimensions", ui->checkRenderAtHalfDimensions->isChecked());
    settings.setValue("RenderAtHalfFramerate", ui->checkRenderAtHalfFramerate->isChecked());
    settings.setValue("ReducedParticles", ui->checkReducedParticles->isChecked());
    settings.setValue("ExtraSmoothImages", ui->checkExtraSmoothImages->isChecked());
    settings.setValue("UseNTSCSafeColours", ui->checkUseNTSCSafeColours->isChecked());
    settings.setValue("DoNotPremultiplyAlpha", ui->checkDoNotPremultiplyAlpha->isChecked());
    settings.setValue("VariableLineWidths", ui->checkVariableLineWidths->isChecked());

    return true;
}
