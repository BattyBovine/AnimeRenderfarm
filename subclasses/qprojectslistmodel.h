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
