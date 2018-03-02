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

# Generate internal headers & install them
set(internal_header_file "${CMAKE_SOURCE_DIR}/src/include/rawrtcc_internal.h")
set(rawrtcc_HEADERS_INCLUDE "")
foreach (header IN LISTS rawrtcc_HEADERS)
    string(CONCAT rawrtcc_HEADERS_INCLUDE
            ${rawrtcc_HEADERS_INCLUDE}
            "#include <rawrtcc/internal/"
            ${header} ">\n")
endforeach ()
configure_file(${CMAKE_SOURCE_DIR}/src/include/rawrtcc_internal.h.cmakein
        ${internal_header_file} @ONLY)
install(FILES ${rawrtcc_HEADERS} DESTINATION include/rawrtcc/internal)
install(FILES ${internal_header_file} DESTINATION include/rawrtcc/internal)

# Generate pkg-config file & install it
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pkg-config.pc.cmakein
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION lib/pkgconfig)
