cmake_minimum_required (VERSION 3.1)
project(zzipsdl VERSION "0.13.71" LANGUAGES C)

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
pkg_search_module ( SDL sdl2)

# targets ########################################################
set(ZZIPSDL_SRCS 
    SDL_rwops_zzip.c)

set(ZZIPSDL_HDRS
    SDL_rwops_zzip.h)

set(ZZCATSDL_SRCS
    ${ZZIPSDL_SRCS}
    SDL_rwops_zzcat.c)

if(SDL_FOUND)
add_executable(zzcatsdl ${ZZCATSDL_SRCS} )
target_link_libraries(zzcatsdl libzzip "${SDL_LIBRARIES}" ZLIB::ZLIB)
target_include_directories(zzcatsdl PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" "${SDL_INCLUDE_DIRS}")
endif()

# install ########################################################
if(SDL_FOUND)
set(zzipsdldir "${CMAKE_INSTALL_INCLUDEDIR}/SDL_rwops_zzip")
install(FILES ${ZZIPSDL_SRCS} ${ZZIPSDL_HDRS}
        DESTINATION ${zzipsdldir})
endif()

# pkgconfig ######################################################

if(SDL_FOUND)
if(UNIX)
join_paths(libdir "\${prefix}" "${CMAKE_INSTALL_LIBDIR}")
join_paths(includedir "\${prefix}" "${CMAKE_INSTALL_INCLUDEDIR}")
add_custom_command(OUTPUT SDL_rwops_zzip.pc
   COMMAND ${BASH} -c "echo 'prefix=${CMAKE_INSTALL_PREFIX}' > SDL_rwops_zzip.pc"
   COMMAND ${BASH} -c "echo 'zzipsdldir=\${prefix}/${zzipsdldir}' >> SDL_rwops_zzip.pc"
   COMMAND ${BASH} -c "echo '' >> SDL_rwops_zzip.pc"
   COMMAND ${BASH} -c "echo 'Name: SDL_rwops_zzip' >> SDL_rwops_zzip.pc"
   COMMAND ${BASH} -c "echo 'Version: ${PROJECT_VERSION}' >> SDL_rwops_zzip.pc"
   COMMAND ${BASH} -c "echo 'Description: SDL_rwops for ZZipLib' >> SDL_rwops_zzip.pc"
   COMMAND ${BASH} -c "echo 'Requires: sdl, zziplib' >> SDL_rwops_zzip.pc"
   COMMAND ${BASH} -c "echo 'Cflags: -I\${zzipsdldir}' >> SDL_rwops_zzip.pc"
   VERBATIM)
add_custom_target(pkgconfig-sdl ALL DEPENDS SDL_rwops_zzip.pc)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SDL_rwops_zzip.pc
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig" )
endif()
endif()
