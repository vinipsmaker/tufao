find_package(PkgConfig)

# Build info
configure_file(tufao0.prf.in tufao0.prf @ONLY)
if(PKG_CONFIG_FOUND)
    configure_file(tufao0.pc.in tufao0.pc @ONLY)
endif(PKG_CONFIG_FOUND)

# Install info
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tufao0.prf
    DESTINATION ${QT_MKSPECS_DIR}/features
)
if(PKG_CONFIG_FOUND)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tufao0.pc
            DESTINATION ${libdir}/pkgconfig)
endif(PKG_CONFIG_FOUND)
