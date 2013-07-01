find_package(Qt5Test REQUIRED)
enable_testing()

# Tests
set(tests
    ibytearray
    headers
    httpfileserver
    httppluginserver
    websocket
    sessionsettings
    httpserverrequestrouter
    httpupgraderouter
    asctime
    rfc1036
    rfc1123
    cryptography
    httpserverresponse
    dependencytree
)

macro(setup_test_target target)
    qt5_use_modules("${target}" Core Network Test)
    set_target_properties(
        "${target}"
        PROPERTIES
        AUTOMOC ON
    )
    target_link_libraries("${target}" "${TUFAO_LIBRARY}")

    add_test(NAME "${target}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/src"
        COMMAND $<TARGET_FILE:${target}>
    )
endmacro()

foreach(test ${tests})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${test}")
        add_subdirectory("${test}")
    else()
        add_executable("${test}" "${test}.cpp")
        setup_test_target("${test}")
    endif()
endforeach()

add_custom_target(tests ALL
    ctest --output-on-failure
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Running tests"
    DEPENDS ${tests}
)
