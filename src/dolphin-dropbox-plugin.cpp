/***************************************************************************
 *   Copyright (C) 2012 by Sergei Stolyarov <sergei@regolit.com>           *
 *   Copyright (C) 2010 by Thomas Richard <thomas.richard@proan.be>        *
 *   Copyright (C) 2009-2010 by Peter Penz <peter.penz@gmx.at>             *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/
 

#include <kaction.h>
#include <kactionmenu.h>
#include <kdemacros.h>
#include <kfileitem.h>
#include <klocalizedstring.h>
#include <kfileitemlistproperties.h>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QLocalSocket>
#include <QApplication>
#include <QClipboard>
#include <QtDBus>

#include <KDE/KPluginFactory>
#include <KDE/KPluginLoader>

#include <QtDebug>

#include "dolphin-dropbox-plugin.h"

//Сокет демона
#define SOCKET_FILE "example.sock"

K_PLUGIN_FACTORY(DolphinDropboxPluginFactory, registerPlugin<DolphinDropboxPlugin>();)
K_EXPORT_PLUGIN(DolphinDropboxPluginFactory("dolphin-dropbox-plugin"))

struct DolphinDropboxPlugin::Private
{
    QString contextFilePath;
    QLocalSocket controlSocket;
    QString controlSocketPath;
};


DolphinDropboxPlugin::DolphinDropboxPlugin(QObject* parent, const QVariantList & args):
    KAbstractFileItemActionPlugin(parent)
{
    Q_UNUSED(args);
    p = new Private;

	p->controlSocketPath =  QDir::tempPath(); 
    p->controlSocketPath.append(QDir::separator()).append(SOCKET_FILE);
    p->controlSocketPath = QDir::toNativeSeparators(p->controlSocketPath);
    p->controlSocket.connectToServer(p->controlSocketPath);
}

DolphinDropboxPlugin::~DolphinDropboxPlugin()
{
    delete p;
}

QList<QAction*> DolphinDropboxPlugin::actions(const KFileItemListProperties & fileItemInfos, QWidget * parentWidget)
{
    Q_UNUSED(parentWidget);
    QList<QAction*> actions;

    if (fileItemInfos.items().count() != 1) {
        // do not display menu on empty or multiple selection
        return actions;
    }

    KFileItem item = fileItemInfos.items().first();
    QString itemPath = item.localPath();
    QFileInfo itemFileInfo = QFileInfo(itemPath);

    if (!item.isDir() && !item.isFile() && !item.isLocalFile()) {
        // file is not dir and local file
        return actions;
    }


    // everything looks fine so populate the menu
    KActionMenu * menuAction = new KActionMenu(this);
    menuAction->setText("MyPlugin");
    actions << menuAction;

    p->contextFilePath = itemPath;
 
    //-----------------------
    //Тестовый метод
    {
        QAction * testAction = new KAction(this);
        testAction->setIcon(KIcon("klipper"));
        testAction->setText(i18nc("@item:inmenu", "TEST"));
        menuAction->addAction(testAction);
        connect(testAction, SIGNAL(triggered()),
                this, SLOT(testHashAction()));
    }
    //-------------------------
    return actions;
}


/*
*/

//Re-entrant
QString DolphinDropboxPlugin::sendCommand(QVariant command, QLocalSocket *socket, bool inGuiThread)
{
    //If we're in the GuiThread we can't afford to wait long
    //in the worker thread we have bigger margins
    int waitTime = 500;
    if(inGuiThread)
        waitTime = 100;

    if(!socket->isOpen()) {
        socket->connectToServer(p->controlSocketPath);
        if(!socket->waitForConnected(waitTime))
            return QString();
    }

    //Формирование JSON.
    QJson::Serializer serializer;

    QByteArray _jcommand = serializer.serialize(command);
    socket->write(_jcommand);
    //Отправка в сокет
    socket->flush();

    bool ready = false;
    QString reply;

    while(!ready)  {
        if(!socket->waitForReadyRead(waitTime))    {
            //If we have to wait this long, the socket probably isn't open anymore (dropbox died or closed)
            socket->close();
            return QString();
        }

        reply.append(socket->readAll());

        if(reply.endsWith("}"))
        {
            if (testJson(reply.toUtf8()))
                ready = true;
        }
    }

    return reply;
}

QVariant DolphinDropboxPlugin::parseJson(QByteArray inp){
    QJson::Parser parser;
    bool ok;
    QVariant out =parser.parse(inp,&ok);
    if (!ok)
        return QVariant();
    return out;
}

//Проверка корректности JSON
bool DolphinDropboxPlugin::testJson(QByteArray inp){
    QJson::Parser parser;
    bool ok;
    parser.parse(inp,&ok);
    return ok;
}

void DolphinDropboxPlugin::testHashAction(){
    QVariantMap command;
    command.insert("verb","hash");
    command.insert("path",p->contextFilePath);
    sendCommand(command, &(p->controlSocket));
}


