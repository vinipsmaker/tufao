bool RequestHandler::handleRequest(Tufao::HttpServerRequest &request,
                                   Tufao::HttpServerResponse &response)
{
    Tufao::Session s(store, request, response);

    s["access"] = s["access"]().toInt() + 1;

    response.writeHead(200, "OK");

    response << "You have "
             << QByteArray::number(s["access"]().toInt())
             << " access";

    response.end();
    return true;
}
