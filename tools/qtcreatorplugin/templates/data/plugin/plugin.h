#ifndef PLUGIN_H
#define PLUGIN_H

#include <Tufao/AbstractHttpServerRequestHandlerFactory>

class Plugin: public QObject, Tufao::AbstractHttpServerRequestHandlerFactory
{
    Q_OBJECT
    Q_INTERFACES(Tufao::AbstractHttpServerRequestHandlerFactory)
public:
    explicit Plugin(QObject *parent = 0);

    Tufao::AbstractHttpServerRequestHandler *createHandler(QObject *parent);
};

#endif // PLUGIN_H
