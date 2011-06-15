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

#include "tcpcommunicator.h"

TcpCommunicator::TcpCommunicator(QObject *parent) :
    QObject(parent),
    data(NULL), size(0)
{
    connect(this, SIGNAL(beginWriteRaw(QTcpSocket*,QByteArray)),
            this, SLOT(writeRaw(QTcpSocket*,QByteArray)));
}



void TcpCommunicator::reset()
{
    data = NULL;
    size = 0;
}



QByteArray TcpCommunicator::readData(QTcpSocket *sock)
{
    if(!sock || !sock->isValid())
        return NULL;

    QDataStream in(sock);
    in.setVersion(QDataStream::Qt_4_0);

    if(size==0) {
        if(sock->bytesAvailable() < (int)sizeof(qint64))
            return NULL;
        in >> size;
    }

    if(sock->bytesAvailable() < size)
        return NULL;

    QByteArray data;
    in >> data;

    size = 0;

    return data;
}

void TcpCommunicator::writeFile(QTcpSocket *sock, QString filepath)
{
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly))
        return;

    writeData(sock, file.readAll());
    file.close();
}

void TcpCommunicator::writeData(QTcpSocket *sock, QByteArray data)
{
    if(!sock || !sock->isValid())
        return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << (qint64)data.size();
    out << data;

    emit beginWriteRaw(sock, block);
}



void TcpCommunicator::writeRaw(QTcpSocket *sock, QByteArray raw)
{
    qint64 sent = 0;
    while(sent < sizeof(raw)) {
        qint64 sentnow = sock->write(raw);
        if(sentnow >= 0)
            sent += sentnow;
        else
            return;
    }
}
