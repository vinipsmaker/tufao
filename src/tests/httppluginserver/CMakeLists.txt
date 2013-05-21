# Source files
set(httppluginserver_SRC
    httppluginserver.cpp
)

# Build info
qt5_add_resources(qrc_SRC "config.qrc")

add_executable(httppluginserver ${httppluginserver_SRC} ${qrc_SRC})
setup_test_target(httppluginserver)
