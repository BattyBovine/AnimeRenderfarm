#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QDialog>
#include <QCloseEvent>

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

protected:
    void closeEvent(QCloseEvent*);

signals:
    void closing();

private:
    Ui::ServerManager *ui;

    ServerThread *serverproc;

public slots:
    void getStatus(QString);
};

#endif // SERVERMANAGER_H
