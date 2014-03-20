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
#include <QDesktopServices>

#include <KDE/KPluginFactory>
#include <KDE/KPluginLoader>

#include <QtDebug>

#include "dolphin-dropbox-plugin.h"

#define SOCKET_FILE ".2safe/control.sock"

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
    p->controlSocketPath = (QDir::homePath() + QDir::separator() + QString(SOCKET_FILE));
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

    if (!itemFileInfo.filePath().contains("/home/xlab/2safe/", Qt::CaseInsensitive) &&
         !itemFileInfo.filePath().contains("/home/xlab/Blog/", Qt::CaseInsensitive)) {
        // file is not in supported dirs
        return actions;
    }

    // everything looks fine so populate the menu
    KActionMenu * menuAction = new KActionMenu(this);
    menuAction->setText("2Safe Plugin");
    menuAction->setIcon(QIcon("/home/xlab/Coding/2safe-dolphin/2safe.png"));
    actions << menuAction;

    p->contextFilePath = itemPath;

    QAction * publicLinkAction = new KAction(this);
    publicLinkAction->setIcon(KIcon("klipper"));
    publicLinkAction->setText(i18nc("@item:inmenu", "Get public link"));
    connect(publicLinkAction, SIGNAL(triggered()),
            this, SLOT(publicLinkActionHandler()));

    QAction * openRemoteFolderAction = new KAction(this);
    openRemoteFolderAction->setIcon(KIcon("konqueror"));
    openRemoteFolderAction->setText(i18nc("@item:inmenu", "Open in remote folder"));
    connect(openRemoteFolderAction, SIGNAL(triggered()),
            this, SLOT(openRemoteFolderActionHandler()));

    menuAction->addAction(publicLinkAction);
    menuAction->addAction(openRemoteFolderAction);

    return actions;
}

//Re-entrant
QString DolphinDropboxPlugin::sendCommand(QVariant command, QLocalSocket *socket, bool inGuiThread)
{
    //If we're in the GuiThread we can't afford to wait long
    //in the worker thread we have bigger margins
    int waitTime = 2000;
    if(inGuiThread)
        waitTime = 1000;

    if(!socket->isOpen()) {
        socket->connectToServer(p->controlSocketPath);
        if(!socket->waitForConnected(waitTime))
            return QString();
    }

    QJson::Serializer serializer;

    QByteArray _jcommand = serializer.serialize(command);
    socket->write(_jcommand);
    socket->flush();

    bool ready = false;
    QString reply;

    if(!socket->waitForReadyRead(waitTime))    {
        socket->close();
        return QString();
    }

    reply.append(socket->readAll());
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

void DolphinDropboxPlugin::publicLinkActionHandler(){
    QVariantMap command;
    QVariantMap args;
    args.insert("file", p->contextFilePath);
    command.insert("type","action");
    command.insert("verb","get_public_link");
    command.insert("args", args);
    QString link = sendCommand(command, &(p->controlSocket));
    if(!link.isEmpty()) {
        //QDesktopServices::openUrl(QUrl(link));
        QApplication::clipboard()->setText(link);
    }
}

void DolphinDropboxPlugin::openRemoteFolderActionHandler(){
    QVariantMap command;
    QVariantMap args;
    args.insert("file", p->contextFilePath);
    command.insert("type","action");
    command.insert("verb","open_in_browser");
    command.insert("args", args);
    QString link = sendCommand(command, &(p->controlSocket));
    if(!link.isEmpty()){
        QDesktopServices::openUrl(QUrl(link));
    }
}