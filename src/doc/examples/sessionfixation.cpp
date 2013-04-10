bool RequestHandler::handleRequest(Tufao::HttpServerRequest &request,
                                   Tufao::HttpServerResponse &response)
{
    QByteArray username(getUsername(request));
    QByteArray userpassword(getUserpassword(request));

    if (!performLogin(username, userpassword)) {
        loginFail(response);
        return true;
    }

    store.setProperty(request, response, "user", username);

    loginSuccess(response);
    return true;
}
