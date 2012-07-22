# Extra include files

install(DIRECTORY .
    DESTINATION "${includedir}/Tufao"
    COMPONENT development
    PATTERN CMakeLists.txt EXCLUDE
)
