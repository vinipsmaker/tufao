bool RequestHandler::handleRequest(Tufao::HttpServerRequest &request,
                                   Tufao::HttpServerResponse &response)
{
    response.writeHead(HttpResponseStatusCode::OK);
    response.end("Hello World\n");
    return true;
}
