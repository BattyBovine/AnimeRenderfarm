#ifndef RENDERPROGRESS_H
#define RENDERPROGRESS_H

#include <QDialog>

namespace Ui {
    class RenderProgress;
}

class RenderProgress : public QDialog
{
    Q_OBJECT

public:
    explicit RenderProgress(QWidget *parent = 0);
    ~RenderProgress();

private:
    Ui::RenderProgress *ui;
};

#endif // RENDERPROGRESS_H
