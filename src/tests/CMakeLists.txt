find_package(Qt5Test REQUIRED)

# Source files
set(tests_SRC
    main.cpp
    ibytearraytest.cpp
)

add_executable(tests ${tests_SRC})

qt5_use_modules(tests Core Network Test)

set_target_properties(
    "tests"
    PROPERTIES
    AUTOMOC ON
)

target_link_libraries(tests "${TUFAO_LIBRARY}")

enable_testing()
add_test(tests "${CMAKE_CURRENT_BINARY_DIR}/tests")
add_custom_target(test ALL
    ctest --output-on-failure
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Running tests"
    DEPENDS tests
)
