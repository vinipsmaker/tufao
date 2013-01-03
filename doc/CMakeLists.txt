if (GENERATE_DOC)
    find_package(Doxygen REQUIRED)
endif()

# Configure info
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY
)

if (GENERATE_DOC)
    # Build info
    add_custom_target(doc ALL
        "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Generating API documentation with Doxygen"
    )
    add_custom_target(pdfdoc ALL
        make
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/latex"
        COMMENT "Running make to generate PDF output"
        DEPENDS doc
    )

    # Install info
    #  html an qch
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html"
        "${CMAKE_CURRENT_BINARY_DIR}/qch"
        DESTINATION "share/doc/tufao"
        COMPONENT doc
        PATTERN index.qhp EXCLUDE
    )
    #  pdf
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/latex/refman.pdf"
        DESTINATION "share/doc/tufao/pdf"
        COMPONENT doc
    )
    #  man
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/man"
        DESTINATION "share"
        COMPONENT doc
    )
endif(GENERATE_DOC)
