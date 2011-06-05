#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QDialog>

#include "objects/serverthread.h"

namespace Ui {
    class ServerManager;
}

class ServerManager : public QDialog
{
    Q_OBJECT

public:
    explicit ServerManager(QWidget *parent = 0);
    ~ServerManager();

private:
    Ui::ServerManager *ui;

    ServerThread *serverproc;

public slots:
    void getStatus(QString);
};

#endif // SERVERMANAGER_H
