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

#ifndef TCPCOMMUNICATOR_H
#define TCPCOMMUNICATOR_H

#include <QObject>

#include <QTcpSocket>
#include <QString>
#include <QFile>

class TcpCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit TcpCommunicator(QObject *parent = 0);

    void reset();

    QByteArray readData(QTcpSocket*);
    void writeFile(QTcpSocket*,QString);
    void writeData(QTcpSocket*,QByteArray);

private:
    QByteArray data;
    qint64 size;

signals:
    void beginWriteRaw(QTcpSocket*,QByteArray);

public slots:
    void writeRaw(QTcpSocket*,QByteArray);

};

#endif // TCPCOMMUNICATOR_H
