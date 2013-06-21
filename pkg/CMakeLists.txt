find_package(PkgConfig)

# General info
if (USE_QT5)
    set(PKG_SUFFIX "-qt5")
else()
    set(PKG_SUFFIX "")
endif()

# Build (preprocess) info

#  qmake
configure_file(tufao.prf.in tufao${TUFAO_VERSION_MAJOR}.prf @ONLY)

#  PKG-CONFIG
if(PKG_CONFIG_FOUND)
    configure_file(tufao.pc.in tufao${TUFAO_VERSION_MAJOR}${PKG_SUFFIX}.pc @ONLY)
endif(PKG_CONFIG_FOUND)

# Install info (for built/processed files)

#  qmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tufao${TUFAO_VERSION_MAJOR}.prf"
    DESTINATION "${QT_MKSPECS_DIR}/features"
    COMPONENT development
)

#  PKG-CONFIG
if(PKG_CONFIG_FOUND)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tufao${TUFAO_VERSION_MAJOR}${PKG_SUFFIX}.pc"
            DESTINATION "${libdir}/pkgconfig"
            COMPONENT development
    )
endif(PKG_CONFIG_FOUND)

# Instal info (plain version)
#  CMake
if (USE_QT5)
    install(FILES FindTufao0-qt5.cmake
        DESTINATION "${CMAKE_ROOT}/Modules"
        COMPONENT development
    )
else (USE_QT5)
    install(FILES FindTufao.cmake
        DESTINATION "${CMAKE_ROOT}/Modules"
        COMPONENT development
        RENAME FindTufao0.cmake
    )
endif (USE_QT5)

# Legacy support
if (NOT USE_QT5)
    # qmake
    install(FILES tufao0.prf
        DESTINATION "${QT_MKSPECS_DIR}/features"
        COMPONENT development
        RENAME tufao.prf
    )
    # CMake
    install(FILES FindTufao.cmake
        DESTINATION "${CMAKE_ROOT}/Modules"
        COMPONENT development
    )
endif (NOT USE_QT5)
