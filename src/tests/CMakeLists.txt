find_package(Qt5Test REQUIRED)
enable_testing()

# Tests
set(tests
    ibytearray
)

foreach(test ${tests})
    add_executable("${test}" "${test}.cpp")
    qt5_use_modules("${test}" Core Network Test)
    set_target_properties(
        "${test}"
        PROPERTIES
        AUTOMOC ON
    )
    target_link_libraries("${test}" "${TUFAO_LIBRARY}")

    add_test("${test}" "${CMAKE_CURRENT_BINARY_DIR}/${test}")
endforeach()

add_custom_target(tests ALL
    ctest --output-on-failure
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Running tests"
    DEPENDS ${tests}
)
