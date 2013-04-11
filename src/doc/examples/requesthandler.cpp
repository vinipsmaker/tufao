bool RequestHandler::handleRequest(Tufao::HttpServerRequest &request,
                                   Tufao::HttpServerResponse &response)
{
    response.writeHead(HttpResponseStatus::OK);
    response.end("Hello World\n");
    return true;
}
