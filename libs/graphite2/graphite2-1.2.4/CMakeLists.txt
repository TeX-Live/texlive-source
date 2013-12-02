CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0 FATAL_ERROR)
cmake_policy(SET CMP0012 NEW)
include(TestBigEndian)

project(graphite2)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
if (NOT CMAKE_BUILD_TYPE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE Release)
endif (NOT CMAKE_BUILD_TYPE)
#if (CMAKE_BUILD_TYPE STREQUAL ClangASN)
#    add_definitions(-fsanitize=address -fno-omit-frame-pointer)
#    set_property(CACHE CMAKE_C_COMPILER PROPERTY VALUE clang)
#    set_property(CACHE CMAKE_CXX_COMPILER PROPERTY VALUE clang)
#endif (CMAKE_BUILD_TYPE STREQUAL ClangASN)

enable_language(CXX C)

include_directories(${PROJECT_SOURCE_DIR}/include)

enable_testing()

set(GRAPHITE2_VM_TYPE auto CACHE STRING "Choose the type of vm machine: Auto, Direct or Call.")
option(GRAPHITE2_NSEGCACHE "Compile out the gr_*_with_seg_cache APIs")
option(GRAPHITE2_NFILEFACE "Compile out the gr_make_file_face* APIs")
option(GRAPHITE2_NTRACING "Compile out log segment tracing capability")
option(GRAPHITE2_TELEMETRY "Add memory usage telemetry")
option(GRAPHITE2_ASAN "Enable Address Sanitizing")


message(STATUS "Build: " ${CMAKE_BUILD_TYPE})
string(REPLACE "ON" "disabled" _SEGCACHE_SUPPORT ${GRAPHITE2_NSEGCACHE})
string(REPLACE "OFF" "enabled" _SEGCACHE_SUPPORT ${_SEGCACHE_SUPPORT})
string(REPLACE "ON" "disabled" _FILEFACE_SUPPORT ${GRAPHITE2_NFILEFACE})
string(REPLACE "OFF" "enabled" _FILEFACE_SUPPORT ${_FILEFACE_SUPPORT})
string(REPLACE "ON" "disabled" _TRACING_SUPPORT ${GRAPHITE2_NTRACING})
string(REPLACE "OFF" "enabled" _TRACING_SUPPORT ${_TRACING_SUPPORT})
message(STATUS "Segment Cache support: " ${_SEGCACHE_SUPPORT})
message(STATUS "File Face support: " ${_FILEFACE_SUPPORT})
message(STATUS "Tracing support: " ${_TRACING_SUPPORT})

if (GRAPHITE2_ASAN)
    add_definitions(-fsanitize=address -fno-omit-frame-pointer -g)
    find_program(ASAN_SYMBOLIZER llvm-symbolizer
            PATHS "/usr/lib" "/usr/local/lib"
            PATH_SUFFIXES "llvm-3.3/bin" "llvm-3.4/bin" "llvm-3.5/bin")
endif (GRAPHITE2_ASAN)

string(TOLOWER ${GRAPHITE2_VM_TYPE} GRAPHITE2_VM_TYPE)
if (NOT GRAPHITE2_VM_TYPE MATCHES "auto|direct|call")
    message(SEND_ERROR "unrecognised vm machine type: ${GRAPHITE2_VM_TYPE}. Only Auto, Direct or Call are available")
endif (NOT GRAPHITE2_VM_TYPE MATCHES "auto|direct|call")
if (GRAPHITE2_VM_TYPE STREQUAL "auto")
    if (CMAKE_BUILD_TYPE MATCHES "[Rr]el(ease|[Ww]ith[Dd]eb[Ii]nfo)")
        set(GRAPHITE2_VM_TYPE "direct")
    else(CMAKE_BUILD_TYPE MATCHES "[Rr]el(ease|[Ww]ith[Dd]eb[Ii]nfo)")
        set(GRAPHITE2_VM_TYPE "call")
    endif(CMAKE_BUILD_TYPE MATCHES "[Rr]el(ease|[Ww]ith[Dd]eb[Ii]nfo)")
endif (GRAPHITE2_VM_TYPE STREQUAL "auto")
if (GRAPHITE2_VM_TYPE STREQUAL "direct" AND NOT (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    message(WARNING "vm machine type direct can only be built using GCC")
    set(GRAPHITE2_VM_TYPE "call")
endif (GRAPHITE2_VM_TYPE STREQUAL "direct" AND NOT (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
message(STATUS "Using vm machine type: ${GRAPHITE2_VM_TYPE}")

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(doc)
if (NOT (GRAPHITE2_NSEGCACHE OR GRAPHITE2_NFILEFACE))
    add_subdirectory(gr2fonttest)
endif (NOT (GRAPHITE2_NSEGCACHE OR GRAPHITE2_NFILEFACE))

set(version 3.0.1)
set(libdir ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(includedir ${CMAKE_INSTALL_PREFIX}/include)

configure_file(graphite2.pc.in graphite2.pc)

install(FILES ${PROJECT_BINARY_DIR}/graphite2.pc DESTINATION lib${LIB_SUFFIX}/pkgconfig)

