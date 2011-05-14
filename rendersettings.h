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
