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

#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkSession>

#include <QDesktopServices>
#include <QFile>
#include <QDir>

#include <QCryptographicHash>

#include "objects/renderthread.h"
#include "dialogues/preferences.h"
#include "objects/tcpcommunicator.h"

class ServerThread : public QThread
{
    Q_OBJECT

public:
    explicit ServerThread(QObject *parent = 0);
    ~ServerThread();

    void setBindIP(QString in="");
    void setBindPort(quint16);
    
protected:
    void run();

private:
    QTcpServer *server;
    QTcpSocket *client;

    RenderThread *renderproc;

    TcpCommunicator comm;

    QString bindip;
    quint16 bindport;
    QString temppath;

    QString projectname;
    QList< QPair<QString,QString> > embedlist;

signals:
    void initServer();
    void serverStatus(QString);

    void renderStart();

public slots:

private slots:
    void startServer();

    void beginTransfer();
    void getProjectName();
    void getProjectFile();
    void getEmbeddedFile();
    void getCommandLine();

    void startRenderThread();

    QList< QPair<QString,QString> > processEmbeddedFiles(QString);

    void cleanup();

};

#endif // SERVERTHREAD_H
