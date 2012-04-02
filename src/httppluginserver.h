/*  This file is part of the Tufão project
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TUFAO_HTTPPLUGINSERVER_H
#define TUFAO_HTTPPLUGINSERVER_H

#include "abstracthttpserverrequesthandler.h"

namespace Tufao {

namespace Priv {

struct HttpPluginServer;

} // namespace Priv

/*!
  Your plugins need to be a subclass of AbstractHttpServerRequestHandlerFactory.
  */
class TUFAO_EXPORT HttpPluginServer : public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    explicit HttpPluginServer(const QString &configFile = QString(),
                              QObject *parent = 0);
    ~HttpPluginServer();

    void setConfig(const QString &file);
    QString config() const;

public slots:
    bool handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response,
                       const QStringList &args = QStringList());

    void reloadConfig();

private:
    void clear();

    Priv::HttpPluginServer *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPPLUGINSERVER_H
