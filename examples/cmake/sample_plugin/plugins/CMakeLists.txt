#######################################################################################################################
# source files
set(PLUGIN_SOURCE
	Test.cpp
)

set(PLUGIN_HEADERS
	Test.hpp
)

add_library(Test SHARED ${PLUGIN_HEADERS} ${PLUGIN_SOURCE})
target_link_libraries(Test ${TUFAO_LIBRARIES} Qt5::Test Qt5::Core Qt5::Network)
