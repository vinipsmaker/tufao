bool RequestHandler::handleRequest(Tufao::HttpServerRequest &request,
                                   Tufao::HttpServerResponse &response)
{
    response.writeHead(200, "OK");

    Tufao::Session::apply(store, "access", request, response,
                          [&response](QVariant &access) {
        access = access.toInt() + 1;

        response << "You visited this page "
                 << QByteArray::number(access.toInt())
                 << " times";
    });

    response.end();
    return true;
}
