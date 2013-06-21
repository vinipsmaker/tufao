find_package(Qt4 4.7 REQUIRED QtCore QtNetwork QtTest)
include(${QT_USE_FILE})

enable_testing()

# Tests
set(tests
    ibytearray
    headers
    httpfileserver
    websocket
    sessionsettings
    asctime
    rfc1036
    rfc1123
    cryptography
    httpserverresponse
)

macro(setup_test_target target source)
    qt4_wrap_cpp(${target}_MOC "${target}.h"
        OPTIONS
        "-DQT_VERSION_MAJOR=${QT_VERSION_MAJOR}"
        "-DQT_VERSION_MINOR=${QT_VERSION_MINOR}"
        "-DQT_VERSION_PATCH=${QT_VERSION_PATCH}"
    )
    add_executable("${test}" "${source}" "${${target}_MOC}")
    target_link_libraries("${target}" "${TUFAO_LIBRARY}" ${QT_LIBRARIES})

    add_test(NAME "${target}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/src"
        COMMAND $<TARGET_FILE:${target}>
    )
endmacro()

foreach(test ${tests})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${test}")
        add_subdirectory("${test}")
    else()
        setup_test_target("${test}" "${test}.cpp")
    endif()
endforeach()

add_custom_target(tests ALL
    ctest --output-on-failure
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Running tests"
    DEPENDS ${tests}
)
