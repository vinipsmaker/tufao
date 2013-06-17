#include "websocket.h"
#include <QtTest/QTest>
#include "../websocket.h"

using namespace Tufao;

void WebSocketTest::properties()
{
    WebSocket websocket;

    QCOMPARE(websocket.messagesType(), WebSocket::BINARY_MESSAGE);

    websocket.setMessagesType(WebSocket::TEXT_MESSAGE);
    QCOMPARE(websocket.messagesType(), WebSocket::TEXT_MESSAGE);

    websocket.setMessagesType(WebSocket::BINARY_MESSAGE);
    QCOMPARE(websocket.messagesType(), WebSocket::BINARY_MESSAGE);
}

QTEST_APPLESS_MAIN(WebSocketTest)
