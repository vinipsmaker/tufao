bool RequestHandler::handleRequest(Tufao::HttpServerRequest &request,
                                   Tufao::HttpServerResponse &response)
{
    response.writeHead(200, "OK");

    Tufao::Session::apply(store, request, response,
                          [&response](QMap<QByteArray, QVariant> &properties) {
        properties["access"] = properties["access"].toInt() + 1;

        response << "You visited this page "
                 << QByteArray::number(properties["access"].toInt())
                 << " times";
    });

    response.end();
    return true;
}
