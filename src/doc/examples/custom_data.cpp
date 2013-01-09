bool Handler::handleRequest(HttpServerRequest &request,
                            HttpServerResponse &response)
{
    // ...

    QStringList args = request.customData().toMap()["args"].toStringList();

    // ...
}
