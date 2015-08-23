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
    httpserverrequestrouter.cpp
    httppluginserver.cpp
    headers.cpp
    priv/rfc1123.cpp
    priv/rfc1036.cpp
    priv/asctime.cpp
    sessionstore.cpp
    simplesessionstore.cpp
    notfoundhandler.cpp
    urlrewriterhandler.cpp
    httpupgraderouter.cpp
	 classhandlermanager.cpp
    classhandler.cpp
)

add_definitions(-DTUFAO_LIBRARY)
if(NOT MSVC)
    add_definitions(
        -Wall
        -Wextra
#        -Wpedantic
#        -Weffc++
    )
endif()

add_library("${TUFAO_LIBRARY}" SHARED ${tufao_SRC})

qt5_use_modules("${TUFAO_LIBRARY}" Core Network)

set_target_properties(
    "${TUFAO_LIBRARY}"
    PROPERTIES
    SOVERSION ${TUFAO_VERSION_MAJOR}
    VERSION ${TUFAO_VERSION}
    VS_KEYWORD Qt4VSv1.0
    AUTOMOC ON
    INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${libdir}"
)

# CXX_STANDARD was introduced in CMake 3.1
if("${CMAKE_VERSION}" VERSION_GREATER "3.0")
    set_target_properties(
        "${TUFAO_LIBRARY}"
        PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
    )
endif()

if("${CMAKE_VERSION}" VERSION_LESS "2.8.12")
  target_include_directories("${TUFAO_LIBRARY}" INTERFACE "${CMAKE_INSTALL_PREFIX}/${includedir}")
  install(TARGETS "${TUFAO_LIBRARY}" EXPORT "${TUFAO_LIBRARY}-export"
          LIBRARY DESTINATION "${libdir}"
          ARCHIVE DESTINATION "${libdir}"
          RUNTIME DESTINATION bin
          PUBLIC_HEADER DESTINATION "${includedir}"
  )
else()
  # Support relocatable packages. It can be usefull on Windows
  install(TARGETS "${TUFAO_LIBRARY}" EXPORT "${TUFAO_LIBRARY}-export"
          LIBRARY DESTINATION "${libdir}"
          ARCHIVE DESTINATION "${libdir}"
          RUNTIME DESTINATION bin
          PUBLIC_HEADER DESTINATION "${includedir}" INCLUDES DESTINATION "${includedir}"
  )
endif()

# Tests
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install info
install(TARGETS "${TUFAO_LIBRARY}"
    DESTINATION "${libdir}"
    COMPONENT runtime
)
install(DIRECTORY .
    DESTINATION "${includedir}/Tufao"
    COMPONENT development
    FILES_MATCHING PATTERN *.h
    PATTERN doc EXCLUDE
    PATTERN priv EXCLUDE
    PATTERN tests EXCLUDE
)
