#include "plugin.h"
#include "requesthandler.h"
#include <QtCore/QtPlugin>

Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
}

Tufao::AbstractHttpServerRequestHandler *Plugin::createHandler(QObject *parent)
{
    return new RequestHandler(parent);
}

Q_EXPORT_PLUGIN2(PLUGIN, Plugin)
