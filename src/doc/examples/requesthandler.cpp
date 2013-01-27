bool RequestHandler::handleRequest(Tufao::HttpServerRequest &request,
                                   Tufao::HttpServerResponse &response)
{
    response.writeHead(200);
    response.end("Hello World\n");
    return true;
}
