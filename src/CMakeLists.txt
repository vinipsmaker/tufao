include(${QT_USE_FILE})

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

# MOC header files
set(tufao_MOC_HDRS
    httpserver.h
    httpserverrequest.h
    httpserverresponse.h
    httpsserver.h
    priv/tcpserverwrapper.h
    websocket.h
    abstractmessagesocket.h
    httpfileserver.h
    abstracthttpserverrequesthandler.h
    httpserverrequestrouter.h
    httppluginserver.h
    sessionstore.h
    simplesessionstore.h
)

# Build info
add_definitions(-DTUFAO_LIBRARY
    -Wall
)

qt4_wrap_cpp(tufao_MOC ${tufao_MOC_HDRS}
    OPTIONS
    "-DQT_VERSION_MAJOR=${QT_VERSION_MAJOR}"
    "-DQT_VERSION_MINOR=${QT_VERSION_MINOR}"
    "-DQT_VERSION_PATCH=${QT_VERSION_PATCH}"
)
add_library(tufao SHARED ${tufao_SRC} ${tufao_MOC})
target_link_libraries(tufao ${QT_LIBRARIES})

set_target_properties(
    tufao
    PROPERTIES
    SOVERSION ${TUFAO_VERSION_MAJOR}
    VERSION ${TUFAO_VERSION}
    VS_KEYWORD Qt4VSv1.0
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
