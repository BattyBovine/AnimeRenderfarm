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
