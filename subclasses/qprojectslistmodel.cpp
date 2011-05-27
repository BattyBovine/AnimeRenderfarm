/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Anime Renderfarm - A remote batch renderer for Anime Studio            *
 *  Copyright (C) 2011 Batty Bovine Productions                            *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

bool qProjectsListModel::clearall()
{
    listProjects.clear();

    return (listProjects.isEmpty());
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
bool qProjectsListModel::setListPairs(QList< QPair<QString,QString> > newlist)
{
    listProjects = newlist;
    return true;
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
