#ifndef PLUGIN_H
#define PLUGIN_H

#include <Tufao/AbstractHttpServerRequestHandlerLoader>

class Plugin: public QObject, Tufao::AbstractHttpServerRequestHandlerLoader
{
    Q_OBJECT
    Q_INTERFACES(Tufao::AbstractHttpServerRequestHandlerLoader)
public:
    explicit Plugin(QObject *parent = 0);

    Tufao::AbstractHttpServerRequestHandler *createHandler(QObject *parent);
};

#endif // PLUGIN_H
