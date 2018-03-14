# Library sources
set(rawrtcc_SOURCES
        certificate.c
        message_buffer.c
        utils.c)

# Library internal headers
set(rawrtcc_HEADERS
        certificate.h
        message_buffer.h
        utils.h
        debug.h)

# Setup library (link & install)
add_library(rawrtcc SHARED ${rawrtcc_SOURCES})
target_link_libraries(rawrtcc
        ${rawrtcc_DEP_LIBRARIES})
add_library(rawrtcc-static STATIC ${rawrtcc_SOURCES})
target_link_libraries(rawrtcc-static
        ${rawrtcc_DEP_LIBRARIES})
install(TARGETS rawrtcc rawrtcc-static
        DESTINATION lib)

# Generate configuration header
configure_file(${CMAKE_SOURCE_DIR}/src/include/rawrtcc_configuration.h.cmakein
        ${CMAKE_SOURCE_DIR}/src/include/rawrtcc_configuration.h @ONLY)

# Install internal headers
install(FILES ${rawrtcc_HEADERS} DESTINATION include/rawrtcc/internal)

# Generate pkg-config file & install it
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pkg-config.pc.cmakein
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION lib/pkgconfig)
