# Source files
set(httppluginserver_SRC
    httppluginserver.cpp
)

# Build info
qt5_add_resources(qrc_SRC "config.qrc")

add_executable(httppluginserver ${httppluginserver_SRC} ${qrc_SRC})
qt5_use_modules(httppluginserver Core Network Test)
set_target_properties(
    "${test}"
    PROPERTIES
    AUTOMOC ON
)
target_link_libraries(httppluginserver "${TUFAO_LIBRARY}")

# Test
add_test(httppluginserver "${CMAKE_CURRENT_BINARY_DIR}/httppluginserver")
