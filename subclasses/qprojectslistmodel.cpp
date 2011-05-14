#include "qprojectslistmodel.h"
#include <QApplication>
#include <QPalette>
#include <QBrush>
#include <QDir>

qProjectsListModel::qProjectsListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int qProjectsListModel::rowCount(const QModelIndex & /* parent */) const
{
    return listProjects.count();
}

QVariant qProjectsListModel::data(const QModelIndex &index, int role ) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= listProjects.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        QPair<QString,QString> value = listProjects.at(index.row());
        return value.second;
    }

    return QVariant();
}

bool qProjectsListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    beginInsertRows(QModelIndex(), row, row+count-1);

    for(int i=0; i < count; i++)
        listProjects.insert(row, QPair<QString,QString>("",""));

    endInsertRows();
    return true;
}

bool qProjectsListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    beginRemoveRows(QModelIndex(), row, row+count-1);

    for(int i=0; i<count; ++i)
        listProjects.removeAt(row);

    endRemoveRows();
    return true;
}

bool qProjectsListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid() && role == Qt::EditRole) {
        int row = index.row();
        listProjects.replace(row, this->pairFromPath(value.toString()));
        emit(dataChanged(index, index));
        return true;
    }

    return false;
}

Qt::ItemFlags qProjectsListModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractListModel::flags(index);
}



QStringList qProjectsListModel::getList()
{
    QStringList projects;
    QPair<QString,QString> projectitem;
    foreach(projectitem, listProjects) {
        projects << (projectitem.first+projectitem.second);
    }

    return projects;
}

QList< QPair<QString,QString> > qProjectsListModel::getListPairs()
{
    return listProjects;
}

QPair<QString,QString> qProjectsListModel::pairFromPath(QString path)
{
    int i = path.lastIndexOf(QDir::separator())+1;
    QPair<QString,QString> p;
    p.first = path.mid(0,i);
    p.second = path.mid(i,path.length());
    return p;
}

bool qProjectsListModel::contains(QString match) {
    QPair<QString,QString> p1 = this->pairFromPath(match);
    QPair<QString,QString> p2;
    foreach(p2, listProjects) {
        if((p1.first == p2.first) && (p1.second == p2.second))
            return true;
    }
    return false;
}

bool qProjectsListModel::addRowAtEnd(QString row)
{
    this->insertRow(listProjects.size()+1);
    QModelIndex index = this->index(listProjects.size()-1, 0);
    this->setData(index, row, Qt::EditRole);
    return true;
}
