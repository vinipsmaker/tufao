# Source files
set(tufao_SRC
    httpserver.cpp
    httpserverrequest.cpp
    httpserverresponse.cpp
    httpsserver.cpp
    url.cpp
    querystring.cpp
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

# Build info
include_directories(${Qt5Core_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
)

add_definitions(-DTUFAO_LIBRARY
    ${Qt5Core_DEFINITIONS}
    ${Qt5Network_DEFINITIONS}
    -Wall
)

add_library(tufao SHARED ${tufao_SRC})

target_link_libraries(tufao
    ${Qt5Core_LIBRARIES}
    ${Qt5Network_LIBRARIES}
)

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
