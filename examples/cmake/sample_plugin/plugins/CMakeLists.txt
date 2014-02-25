#######################################################################################################################
# source files
set(PLUGIN_SOURCE
	Test.cpp
)

set(PLUGIN_HEADERS
	Test.hpp
)

add_library(Test SHARED ${PLUGIN_HEADERS} ${PLUGIN_SOURCE})
qt5_use_modules(Test Core Network)
target_link_libraries(Test ${TUFAO_LIBRARIES})
