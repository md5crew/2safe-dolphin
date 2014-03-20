/***************************************************************************
 *   Copyright (C) 2012 by Sergei Stolyarov <sergei.stolyarov@regolit.com> *
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

#ifndef DolphinDropboxPlugin_H
#define DolphinDropboxPlugin_H

#include <kabstractfileitemactionplugin.h>
#include <QDateTime>
#include <QHash>
#include <QVariantList>
#include <QLocalSocket>
#include <QFileInfo>
#include <QTimer>
#include <QVariant>
#include "QJson/serializer.h"
#include "QJson/parser.h"



//The dropbox protocol info can be found in the dropboxd-protocol file
//it can be found here http://dl.dropbox.com/u/4577542/dropbox/dropboxd-protocol.txt
//Thanks Steffen Schuldenzucker!

/**
 * @brief Dropbox implementation for the KVersionControlPlugin interface.
 */
class DolphinDropboxPlugin : public KAbstractFileItemActionPlugin
{
    Q_OBJECT

public:
    DolphinDropboxPlugin(QObject* parent = 0, const QVariantList & args = QVariantList());
    virtual ~DolphinDropboxPlugin();
    virtual QList<QAction*> actions(const KFileItemListProperties & fileItemInfos, QWidget * parentWidget);

private slots:
//    void shareInBrowseAction();
//    void browseFolderInBrowserAction();
//    void shareFileLinkAction();
//    void browseRevisionsAction();
//    void copyPublicUrlAction();
//    void copyGalleryLinkAction();
    void testHashAction();

private:
    struct Private;
    Private * p;
    QString sendCommand(QVariant string, QLocalSocket *socket, bool inGuiThread = true);
//    QString sendCommand2(QVariant string, QLocalSocket *socket, bool inGuiThread = true);
    QVariant parseJson(QByteArray inp);
    bool testJson(QByteArray inp);
};
#endif // DolphinDropboxPlugin_H

