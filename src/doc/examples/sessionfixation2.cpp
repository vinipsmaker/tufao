bool RequestHandler::handleRequest(Tufao::HttpServerRequest &request,
                                   Tufao::HttpServerResponse &response)
{
    // To make sense, this line must be inserted before call any session-related
    // code
    store.resetSession(request);

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
