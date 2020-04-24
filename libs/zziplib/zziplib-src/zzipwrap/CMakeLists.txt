cmake_minimum_required (VERSION 3.1)
project(zzipwrap VERSION "0.13.71" LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeScripts")

include ( CheckIncludeFiles )
include ( GNUInstallDirs )
include ( FindPkgConfig )
include ( JoinPaths )

# options ########################################################
option(BUILD_SHARED_LIBS "Build a shared library" ON)
option(BUILD_STATIC_LIBS "Build the static library" OFF)
option(BUILD_TESTS "Build test programs" OFF)
option(MSVC_STATIC_RUNTIME "Build with static runtime libs (/MT)" ON)
option(ZZIPLIBTOOL "Ensure binary compatibility with libtool" ON)

check_include_files ( unistd.h ZZIP_HAVE_UNISTD_H )
# set ( ZZIP_HAVE_ZLIB_H 1 )

# Zlib library needed
find_package ( ZLIB REQUIRED )
pkg_search_module ( ZZIP zzip )

set(RELNUM "${PROJECT_VERSION_MAJOR}")
set(VERNUM "${PROJECT_VERSION_MINOR}")
set(FIXNUM "0.${PROJECT_VERSION_PATCH}")
# the "0." on FIXNUM is just for compatibility with automake's libtool.

# targets ########################################################
set(zzipwrap_SRCS
    zzipwrap.c)

set(libzzipwrap_SRCS 
    wrap.c)

set(libzzipwrap_HDRS
    wrap.h)

add_library(libzzipwrap ${libzzipwrap_SRCS} )
target_link_libraries(libzzipwrap libzzip ZLIB::ZLIB )
target_include_directories(libzzipwrap PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(libzzipwrap PROPERTIES OUTPUT_NAME "zzipwrap" RELEASE_POSTFIX "-${RELNUM}")
SET_TARGET_PROPERTIES(libzzipwrap PROPERTIES VERSION ${VERNUM}.${FIXNUM} SOVERSION ${VERNUM})

add_executable(zzipwrap ${zzipwrap_SRCS} )
target_link_libraries(zzipwrap libzzipwrap )
target_include_directories(zzipwrap PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

if(UNIX)
join_paths(libdir "\${prefix}" "${CMAKE_INSTALL_LIBDIR}")
join_paths(includedir "\${prefix}" "${CMAKE_INSTALL_INCLUDEDIR}")
add_custom_command(OUTPUT zzipwrap.pc
   COMMAND ${BASH} -c "echo 'prefix=${CMAKE_INSTALL_PREFIX}' > zzipwrap.pc"
   COMMAND ${BASH} -c "echo 'libdir=${libdir}' >> zzipwrap.pc"
   COMMAND ${BASH} -c "echo 'includedir=${includedir}' >> zzipwrap.pc"
   COMMAND ${BASH} -c "echo '' >> zzipwrap.pc"
   COMMAND ${BASH} -c "echo 'Name: ${PROJECT_NAME}' >> zzipwrap.pc"
   COMMAND ${BASH} -c "echo 'Version: ${PROJECT_VERSION}' >> zzipwrap.pc"
   COMMAND ${BASH} -c "echo 'Description: Callback Wrappers for ZZipLib' >> zzipwrap.pc"
   COMMAND ${BASH} -c "echo 'Requires: zziplib' >> zzipwrap.pc"
   COMMAND ${BASH} -c "echo 'Libs: -L\${libdir} -lzzipwrap' >> zzipwrap.pc"
   COMMAND ${BASH} -c "echo 'Cflags: -I\${includedir}' >> zzipwrap.pc"
   VERBATIM)
add_custom_target(pkgconfigs ALL DEPENDS zzipwrap.pc)
endif()

# install ########################################################
set(outdir ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX)
install(FILES ${outdir}/zzipwrap.pc 
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig" )
endif()

install(FILES ${libzzipwrap_HDRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zzip )
install(TARGETS libzzipwrap
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(ZZIPLIBTOOL)
  if(CMAKE_SHARED_LIBRARY_SONAME_C_FLAG)
    set(lib ${CMAKE_SHARED_LIBRARY_PREFIX})
    set(dll ${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(libdir ${CMAKE_INSTALL_FULL_LIBDIR})
    get_target_property(libname libzzipwrap OUTPUT_NAME)
    get_target_property(librelease libzzipwrap RELEASE_POSTFIX)
    get_target_property(libversion libzzipwrap VERSION)
    install(CODE "execute_process(COMMAND bash -c \"set -e
        cd $DESTDIR/${libdir}
        [ -f ${lib}${libname}${librelease}${dll}.${libversion} ] || exit 0
        echo -n .. Installing: `pwd`
        ln -svf ${lib}${libname}${librelease}${dll}.${libversion} ${lib}${libname}${dll}
        \")")
  endif()
endif()
