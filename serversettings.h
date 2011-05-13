#ifndef SERVERSETTINGS_H
#define SERVERSETTINGS_H

#include <QDialog>

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
};

#endif // SERVERSETTINGS_H
