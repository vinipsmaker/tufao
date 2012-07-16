# Project Tufão
project(tufao)
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
    abstracthttpserverrequesthandlerfactory.h
)

# Build info
add_definitions(-DTUFAO_LIBRARY
    -Wall
)

QT4_WRAP_CPP(tufao_MOC ${tufao_MOC_HDRS})
add_library(tufao SHARED ${tufao_SRC} ${tufao_MOC})
target_link_libraries(tufao ${QT_LIBRARIES})

SET_TARGET_PROPERTIES(
    tufao
    PROPERTIES
    SOVERSION ${version_major}
    VERSION ${version}
)

# Install info
install(TARGETS tufao
    DESTINATION ${libdir}
)
install(DIRECTORY .
    DESTINATION ${includedir}/Tufao
    PATTERN *.h
    PATTERN doc EXCLUDE
    PATTERN priv EXCLUDE
)
