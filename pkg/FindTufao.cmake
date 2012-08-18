# - Config file for Tufão package
# It defines the following variables
#  TUFAO_LIBRARIES
#  TUFAO_INCLUDE_DIR
#  TUFAO_VERSION_STRING

find_library(TUFAO_LIBRARIES
    NAMES tufao
)

foreach(path ${CMAKE_SYSTEM_PREFIX_PATH})
    set(tufao_search_path ${tufao_search_path} "${path}/include/tufao-0")
endforeach()

foreach(path ${CMAKE_SYSTEM_INCLUDE_PATH})
    set(tufao_search_path ${tufao_search_path} "${path}/tufao-0")
endforeach()

find_path(TUFAO_INCLUDE_DIR
    NAMES Tufao/tufao_global.h
    PATHS ${tufao_search_path}
)

unset(tufao_search_path)

if(TUFAO_INCLUDE_DIR)
    file(STRINGS "${TUFAO_INCLUDE_DIR}/Tufao/tufao_global.h" tufao_version_major REGEX "^# +define TUFAO_VERSION_MAJOR [0-9]*")
    string(REGEX MATCH "[0-9]+" tufao_version_major ${tufao_version_major})

    file(STRINGS "${TUFAO_INCLUDE_DIR}/Tufao/tufao_global.h" tufao_version_minor REGEX "^# +define TUFAO_VERSION_MINOR [0-9]*")
    string(REGEX MATCH "[0-9]+" tufao_version_minor ${tufao_version_minor})

    set(TUFAO_VERSION_STRING "${tufao_version_major}.${tufao_version_minor}")
    unset(tufao_version_major)
    unset(tufao_version_minor)
endif(TUFAO_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TUFAO
    REQUIRED_VARS TUFAO_LIBRARIES TUFAO_INCLUDE_DIR
    VERSION_VAR TUFAO_VERSION_STRING
)
