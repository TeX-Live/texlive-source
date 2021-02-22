cmake_minimum_required (VERSION 3.1)
project(zzipbins VERSION "0.13.71" LANGUAGES C)

if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

include ( GNUInstallDirs )
include ( FindPkgConfig )

# options ########################################################
option(BUILD_SHARED_LIBS "Build a shared library" ON)
option(BUILD_STATIC_LIBS "Build the static library" OFF)
option(BUILD_TESTS "Build test programs" OFF)
option(MSVC_STATIC_RUNTIME "Build with static runtime libs (/MT)" ON)

# Zlib library needed
find_package ( ZLIB REQUIRED )
# pkg_search_module ( ZZIP zzip )
# pkg_search_module ( ZZIPMAPPED zzipmmapped )
# pkg_search_module ( ZZIPFSEEKO zzipfseeko )

# targets ########################################################
add_executable(zzcat      zzcat.c)
add_executable(zzdir      zzdir.c)
add_executable(zzxorcat   zzxorcat.c)
add_executable(zzxordir   zzxordir.c)
add_executable(zzxorcopy   zzxorcopy.c)
add_executable(zzobfuscated zzobfuscated.c)
add_executable(zziptest   zziptest.c)
add_executable(zzip       zzip.c zzipmake-zip.c)
add_executable(unzzip     unzzip.c unzzipcat-zip.c unzzipdir-zip.c)
add_executable(unzzip-mix unzzip.c unzzipcat-mix.c unzzipdir-mix.c)
add_executable(unzzip-big unzzip.c unzzipcat-big.c unzzipdir-big.c)
add_executable(unzzip-mem unzzip.c unzzipcat-mem.c unzzipdir-mem.c)
add_executable(unzip-mem  unzip-mem.c)

target_link_libraries(zzcat        libzzip        ZLIB::ZLIB)
target_link_libraries(zzdir        libzzip        ZLIB::ZLIB)
target_link_libraries(zzxorcat     libzzip        ZLIB::ZLIB)
target_link_libraries(zzxordir     libzzip        ZLIB::ZLIB)
target_link_libraries(zzxorcopy    libzzip        ZLIB::ZLIB)
target_link_libraries(zzobfuscated libzzip        ZLIB::ZLIB)
target_link_libraries(zziptest     libzzip        ZLIB::ZLIB)
target_link_libraries(zzip         libzzip        ZLIB::ZLIB)
target_link_libraries(unzzip       libzzip        ZLIB::ZLIB)
target_link_libraries(unzzip-mix   libzzip        ZLIB::ZLIB)
target_link_libraries(unzzip-big   libzzipfseeko  ZLIB::ZLIB)
target_link_libraries(unzzip-mem   libzzipmmapped ZLIB::ZLIB)
target_link_libraries(unzip-mem    libzzipmmapped ZLIB::ZLIB)

target_include_directories(zzcat        PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(zzdir        PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(zzxorcat     PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(zzxordir     PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(zzxorcopy    PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(zzobfuscated PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(zziptest     PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(zzip         PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(unzzip       PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(unzzip-mix   PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(unzzip-big   PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(unzzip-mem   PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(unzip-mem    PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

# install ########################################################
install(TARGETS zzcat zzdir zzxorcat zzxordir zzxorcopy
                unzzip unzzip-mix unzzip-big unzzip-mem unzip-mem
        DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(FILES zziplib.m4
        DESTINATION "${CMAKE_INSTALL_DATADIR}/aclocal")
