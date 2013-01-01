# Source files
set(tufao_SRC
    httpserver.cpp
    httpserverrequest.cpp
    httpserverresponse.cpp
    httpsserver.cpp
    priv/tcpserverwrapper.cpp
    priv/reasonphrase.cpp
    priv/http_parser.c
    websocket.cpp
    abstractmessagesocket.cpp
    httpfileserver.cpp
    abstracthttpserverrequesthandler.cpp
    httpserverrequestrouter.cpp
    httppluginserver.cpp
    headers.cpp
    priv/rfc1123.cpp
    priv/rfc1036.cpp
    priv/asctime.cpp
    sessionstore.cpp
    simplesessionstore.cpp
)

add_definitions(-DTUFAO_LIBRARY
    -Wall
)

add_library(tufao SHARED ${tufao_SRC})

qt5_use_modules(tufao Core Network)

set_target_properties(
    tufao
    PROPERTIES
    SOVERSION ${TUFAO_VERSION_MAJOR}
    VERSION ${TUFAO_VERSION}
    VS_KEYWORD Qt4VSv1.0
    AUTOMOC ON
)

# Install info
install(TARGETS tufao
    DESTINATION "${libdir}"
    COMPONENT runtime
)
install(DIRECTORY .
    DESTINATION "${includedir}/Tufao"
    COMPONENT development
    FILES_MATCHING PATTERN *.h
    PATTERN doc EXCLUDE
    PATTERN priv EXCLUDE
)
