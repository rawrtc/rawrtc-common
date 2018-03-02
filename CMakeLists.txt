# Project
cmake_minimum_required(VERSION 3.2)
# TODO: Find a way to keep this in sync with the one in rawrtcc.h
project(librawrtcc
        VERSION 0.0.1)
set(PROJECT_DESCRIPTION
        "Common code used in RAWRTC projects.")
set(PROJECT_URL
        "https://github.com/rawrtc/rawrtc-common")

# Dependency versions
set(OPENSSL_VERSION "openssl >= 1.0.2")
set(LIB_RE_VERSION "libre >= 0.5.7")
# Note: This MUST be here as cmake 3.5.0 overwrites LIB_RE_VERSION for some reason...
set(DEPENDENCY_VERSIONS "${OPENSSL_VERSION}, ${LIB_RE_VERSION}")

# Debug build type as default
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, using DEBUG")
    set(CMAKE_BUILD_TYPE "DEBUG")
endif ()

# Enable verbose output in DEBUG mode
if (${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
    message(STATUS "enabling verbose outout")
    set(CMAKE_VERBOSE_MAKEFILE on)
endif ()

# Use pkg-config
find_package(PkgConfig REQUIRED)

# Dependency list
set(rawrtcc_DEP_LIBRARIES)

# Dependency: OpenSSL
pkg_check_modules(LIB_OPENSSL REQUIRED ${OPENSSL_VERSION})
include_directories(${LIB_OPENSSL_INCLUDE_DIRS})
link_directories(${LIB_OPENSSL_LIBRARY_DIRS})
list(APPEND rawrtcc_DEP_LIBRARIES ${LIB_OPENSSL_LIBRARIES})

# Dependency: libre
pkg_check_modules(LIB_RE REQUIRED ${LIB_RE_VERSION})
include_directories(${LIB_RE_STATIC_INCLUDE_DIRS})
link_directories(${LIB_RE_STATIC_LIBRARY_DIRS})
list(APPEND rawrtcc_DEP_LIBRARIES ${LIB_RE_STATIC_LIBRARIES})

# Dependency versions
set(PKG_CONFIG_REQUIRES "")
set(PKG_CONFIG_REQUIRES_PRIVATE "${DEPENDENCY_VERSIONS}")

# Linker flags
set(PKG_CONFIG_LIB_DIRS "-L\${libdir}")
set(PKG_CONFIG_LIBRARIES "-lrawrtcc")
set(PKG_CONFIG_LIBS "${PKG_CONFIG_LIB_DIRS} ${PKG_CONFIG_LIBRARIES}")
set(PKG_CONFIG_LIB_DIRS_PRIVATE "${PKG_CONFIG_LIB_DIRS}")
set(PKG_CONFIG_LIBRARIES_PRIVATE "${PKG_CONFIG_LIBRARIES}")
set(PKG_CONFIG_LIBS_PRIVATE "${PKG_CONFIG_LIB_DIRS_PRIVATE} ${PKG_CONFIG_LIBRARIES_PRIVATE}")

# Add custom target to install the library
add_custom_target(install-${PROJECT_NAME}
        $(MAKE) install
        COMMENT "Installing ${PROJECT_NAME}")

# Walk through subdirectories
add_subdirectory(src)
