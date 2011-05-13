#ifndef ABOUTAPPLICATION_H
#define ABOUTAPPLICATION_H

#include <QDialog>
#include <QDesktopWidget>

#include <QSettings>

namespace Ui {
    class AboutApplication;
}

class AboutApplication : public QDialog
{
    Q_OBJECT

public:
    explicit AboutApplication(QWidget *parent = 0);
    ~AboutApplication();

private:
    Ui::AboutApplication *ui;

    QSettings settings;

};

#endif // ABOUTAPPLICATION_H
