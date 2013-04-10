bool RequestHandler::handleUpgrade(Tufao::HttpServerRequest &request,
                                   const QByteArray &head)
{
    Tufao::WebSocket *socket = new Tufao::WebSocket(this);
    socket->startServerHandshake(request, head);
    socket->setMessagesType(Tufao::WebSocket::TEXT_MESSAGE);

    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    connect(socket, SIGNAL(newMessage(QByteArray)),
            this, SIGNAL(newMessage(QByteArray)));
    connect(this, SIGNAL(newMessage(QByteArray)),
            socket, SLOT(sendMessage(QByteArray)));

    return true;
}
