#include "rendersettings.h"
#include "ui_rendersettings.h"

RenderSettings::RenderSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenderSettings)
{
    ui->setupUi(this);

    this->loadSettings();
}

RenderSettings::~RenderSettings()
{
    delete ui;
}



void RenderSettings::showOpenOutputDirectoryDialogue()
{
    QString folder = QFileDialog::getExistingDirectory(
        this, tr("Open Output Folder"),
        QDir::toNativeSeparators(QDir::homePath()),
        QFileDialog::ShowDirsOnly
    );

    if(!folder.isEmpty())
        folderOpened(folder);
}

void RenderSettings::folderOpened(QString folder)
{
    ui->editOutputDirectory->setText(folder);
}



bool RenderSettings::loadSettings()
{
    settings.beginGroup("RenderSettings");

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

    settings.endGroup();

    return true;
}

bool RenderSettings::saveSettings()
{
    settings.beginGroup("RenderSettings");

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

    settings.endGroup();

    return true;
}
