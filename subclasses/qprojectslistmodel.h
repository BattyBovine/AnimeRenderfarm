#ifndef QPROJECTSLISTMODEL_H
#define QPROJECTSLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QPair>

class qProjectsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit qProjectsListModel(QObject *parent = 0);

    int rowCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    bool setData(const QModelIndex &, const QVariant &, int);
    bool insertRows(int, int, const QModelIndex &);
    bool removeRows(int, int, const QModelIndex &);
    bool clearall();

    bool addRowAtEnd(QString);

    QStringList getList();
    QList< QPair<QString,QString> > getListPairs();
    bool contains(QString);

private:
    QList< QPair<QString,QString> > listProjects;

    QPair<QString,QString> pairFromPath(QString);

signals:

public slots:

};

#endif // QPROJECTSLISTMODEL_H
