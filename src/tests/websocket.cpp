#include "websocket.h"
#include <QtTest/QTest>
#include "../websocket.h"

using namespace Tufao;

void WebSocketTest::properties()
{
    WebSocket websocket;

    QCOMPARE(websocket.messagesType(), WebSocketMessageType::BINARY_MESSAGE);

    websocket.setMessagesType(WebSocketMessageType::TEXT_MESSAGE);
    QCOMPARE(websocket.messagesType(), WebSocketMessageType::TEXT_MESSAGE);

    websocket.setMessagesType(WebSocketMessageType::BINARY_MESSAGE);
    QCOMPARE(websocket.messagesType(), WebSocketMessageType::BINARY_MESSAGE);
}

QTEST_APPLESS_MAIN(WebSocketTest)
