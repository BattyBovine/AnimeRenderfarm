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

    QSettings settings;

signals:

public slots:

private slots:
    void renderServerStatus();

    bool loadSettings();
    bool saveSettings();

};

#endif // SERVERSETTINGS_H
