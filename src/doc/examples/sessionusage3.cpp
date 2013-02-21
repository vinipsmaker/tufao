bool RequestHandler::handleRequest(Tufao::HttpServerRequest &request,
                                   Tufao::HttpServerResponse &response,
                                   const QStringList &)
{
    response->writeHead(200);

    Tufao::Session::apply(store, request, response,
                          [&response](QVariant &properties) {
        properties["access"] = properties["access"].toInt() + 1;

        response << "You visited this page "
                 << QByteArray::number(properties["access"].toInt())
                 << " times";
    });

    response->end();
    return true;
}
