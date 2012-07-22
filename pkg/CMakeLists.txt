find_package(PkgConfig)

# Build info
configure_file(tufao0.prf.in tufao0.prf @ONLY)
if(PKG_CONFIG_FOUND)
    configure_file(tufao.pc.in tufao${TUFAO_VERSION_MAJOR}.pc @ONLY)
endif(PKG_CONFIG_FOUND)

# Install info
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tufao0.prf"
    DESTINATION "${QT_MKSPECS_DIR}/features"
    COMPONENT development
)
if(PKG_CONFIG_FOUND)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tufao${TUFAO_VERSION_MAJOR}.pc"
            DESTINATION "${libdir}/pkgconfig"
            COMPONENT development
    )
endif(PKG_CONFIG_FOUND)

# Legacy support
install(FILES tufao.prf
    DESTINATION "${QT_MKSPECS_DIR}/features"
    COMPONENT development
)
