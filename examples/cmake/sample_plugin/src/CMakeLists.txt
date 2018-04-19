#######################################################################################################################
# source files
set(${PROJECT_NAME}_SOURCE
	main.cpp
)

set(${PROJECT_NAME}_HEADERS
	main.hpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_SOURCE})
target_link_libraries(${PROJECT_NAME} ${TUFAO_LIBRARIES} ${Test} Qt5::Core Qt5::Network)
