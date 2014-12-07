find_package(PkgConfig)

# Build info
configure_file(tufao.prf.in tufao${TUFAO_VERSION_MAJOR}.prf @ONLY)
if(PKG_CONFIG_FOUND)
    configure_file(tufao.pc.in tufao${TUFAO_VERSION_MAJOR}.pc @ONLY)
endif(PKG_CONFIG_FOUND)

# Install info
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tufao${TUFAO_VERSION_MAJOR}.prf"
    DESTINATION "${QT_MKSPECS_DIR}/features"
    COMPONENT development
)
if(PKG_CONFIG_FOUND)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tufao${TUFAO_VERSION_MAJOR}.pc"
            DESTINATION "${libdir}/pkgconfig"
            COMPONENT development
    )
endif(PKG_CONFIG_FOUND)
