#######################################################################################################################
# source files
set(${PROJECT_NAME}_SOURCE
	main.cpp
)

set(${PROJECT_NAME}_HEADERS
	main.hpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_SOURCE})
qt5_use_modules(${PROJECT_NAME} Core Network)
target_link_libraries(${PROJECT_NAME} ${TUFAO_LIBRARIES} ${Test})
