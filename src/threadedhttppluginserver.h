#ifndef TUFAO_THREADEDHTTPPLUGINSERVER_H
#define TUFAO_THREADEDHTTPPLUGINSERVER_H

#include <functional>
#include <initializer_list>

#include <QtCore/QRegularExpression>
#include <QtCore/QObject>
#include <QtCore/QDebug>

#include "tufao_global.h"
#include "threadedhttprequestdispatcher.h"


namespace Tufao {

class ThreadedHttpPluginServer : public ThreadedHttpRequestDispatcher
{
    Q_OBJECT
public:
    /*!
     It's a simple typedef for the type of factory accepted by the
     ThreadedHttpPluginServer.
     */
    typedef std::function<AbstractHttpServerRequestHandler* (AbstractHttpServerRequestHandler*,void**)>
    Factory;

    static AbstractHttpServerRequestHandler* defaultFactoy (AbstractHttpServerRequestHandler*handler, void**);

    explicit ThreadedHttpPluginServer(Factory fact = defaultFactoy, QObject *parent = 0);


    /*!
     * \brief setConfig
     * \param file
     * \return
     * \sa HttpPluginServer::setConfig
     *
     * Loads the config file and starts the internal threads.
     */
    bool setConfig(const QString &file);

    /*!
      Returns the path of the last configuration file used. This file is used to
      handle requests, loading the appropriate plugins, generating actual
      handlers and mapping them to the rules described in this file.

      \sa
      setConfig
      */
    QString config() const;

    // ThreadedHttpRequestDispatcher interface
protected slots:
    virtual void initializeThreads() Q_DECL_OVERRIDE;

private:
    void changeConfig(const QString &file);
    void onConfigFileChanged();
    void clear();
    void loadConfig();

    struct Priv;
};

} // namespace Tufao

#endif // TUFAO_THREADEDHTTPPLUGINSERVER_H
