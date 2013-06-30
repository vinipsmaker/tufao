/*  This file is part of the Tufão project
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TUFAO_PRIV_HTTPPLUGINSERVER_H
#define TUFAO_PRIV_HTTPPLUGINSERVER_H

#include "../httppluginserver.h"
#include "../httpserverrequestrouter.h"

#include <QtCore/QPluginLoader>
#include <QtCore/QFileInfo>
#include <QtCore/QFileSystemWatcher>

namespace Tufao {

class ConfigFile
{
public:
    ConfigFile() :observingFile_(false), autoreload_(false) {}

    /*!
      Whether the observed path is file() or  QFileInfo(file()).path().

      \note
      You need to call setFile again after the file begins to exist, then the
      watcher() will begin to watch the wanted file.

      \retval true
      If the observed path is file().

      \retval false
      If the observed path if QFileInfo(file()).path().
    */
    bool observingFile() const
    {
        return observingFile_;
    }

    const QFileSystemWatcher &watcher() const
    {
        return watcher_;
    }

    void setFile(const QString &file, bool autoreload)
    {
        // clean old resources
        if (file_.size()) {
            if (observingFile_)
                // normal flow
                watcher_.removePath(file_);
            else
                // watches directory
                watcher_.removePath(QFileInfo(file_).path());
        }

        // use new resources
        file_ = file;
        autoreload_ = autoreload;
        observingFile_ = true;

        if (!autoreload || !file.size())
            return;

        QFileInfo info(file);
        if (info.exists()) {
            // normal flow

            watcher_.addPath(file);
        } else {
            // watches directory

            watcher_.addPath(info.path());
            observingFile_ = false;
        }
    }

    QString file() const
    {
        return file_;
    }

    bool autoreload() const
    {
        return autoreload_;
    }

    void clear()
    {
        watcher_.removePath(file_);
        file_.clear();
    }

private:
    bool observingFile_;
    QString file_;

    // Kind of optional<QFileSystemWatcher>
    bool autoreload_;
    QFileSystemWatcher watcher_;
};

struct HttpPluginServer::Priv
{
    Tufao::HttpServerRequestRouter router;
    ConfigFile configFile;
    QList<QPluginLoader*> plugins;
    QObjectList handlers;
};

} // namespace Tufao

#endif // TUFAO_PRIV_HTTPPLUGINSERVER_H










