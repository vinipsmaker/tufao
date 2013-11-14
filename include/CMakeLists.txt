# Extra include files

install(DIRECTORY .
    DESTINATION "${includedir}"
    COMPONENT development
    PATTERN CMakeLists.txt EXCLUDE
)
