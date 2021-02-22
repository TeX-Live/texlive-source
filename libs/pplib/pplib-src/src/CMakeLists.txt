cmake_minimum_required(VERSION 3.5)

project(pplib
    LANGUAGES C)
set(VERSION 1.0.5)

set(CMAKE_C_STANDARD 99)

add_library(pplib_utils STATIC
    util/utilplat.h
    util/utilcryptdef.h
    util/utildecl.h
    #
    util/utilcrypt.c
    util/utilcrypt.h
    util/utiliof.c
    util/utiliof.h
    util/utillog.c
    util/utillog.h
    util/utilmd5.c
    util/utilmd5.h
    util/utilmem.c
    util/utilmem.h
    #
    util/utilmemheap.c
    util/utilmemheap.h
    util/utilmemheapiof.c
    util/utilmemheapiof.h
    util/utilmeminfo.c
    util/utilmeminfo.h
    #
    util/utilnumber.c
    util/utilnumber.h
    util/utilsha.c
    util/utilsha.h
)

target_include_directories(pplib_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/util>
)

target_link_libraries(pplib_utils
    PUBLIC
        m
        zlib
)

add_library(pplib_filters STATIC
    util/utilbasexx.c
    util/utilbasexx.h
    util/utilflate.c
    util/utilflate.h
    util/utilfpred.c
    util/utilfpred.h
    util/utillzw.c
    util/utillzw.h
)

target_include_directories(pplib_filters PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/util>
)

target_link_libraries(pplib_filters
    PUBLIC
        m
        zlib
)

add_library(pplib
    ppapi.h
    ppconf.h
    pplib.h
    #
    pparray.c
    pparray.h
    ppcrypt.c
    ppcrypt.h
    ppdict.c
    ppdict.h
    ppfilter.h
    ppheap.c
    ppheap.h
    ppload.c
    ppload.h
    ppstream.c
    ppstream.h
    ppxref.c
    ppxref.h
)

target_include_directories(pplib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/util>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(pplib
    PRIVATE
        pplib_filters
        pplib_utils
)

target_include_directories(pplib
    PRIVATE
        util
)

if (MSVC)
    #set(warning_flags '') # /W4 /WX
else (MSVC)
    set(warning_flags
        -Wall
        -Wextra
        -Wcast-align
        -Wcast-qual
        -Wdeclaration-after-statement
        -Wnested-externs
        -Wno-unknown-pragmas
        -Wpointer-arith
        -Wwrite-strings
    )
    
    foreach(target pplib_utils pplib_filters pplib)
        target_compile_options(${target} PRIVATE ${warning_flags})
    endforeach()
endif (MSVC)

set_target_properties(pplib
    PROPERTIES
        PUBLIC_HEADER "ppapi.h;ppconf.h;pplib.h"
)

add_library(pplib::pplib ALIAS pplib)

install(TARGETS pplib
    EXPORT pplib
    DESTINATION lib
)

enable_testing()

list(APPEND tests pptest1 pptest2 pptest3)

foreach(_test IN LISTS tests)
    add_executable(${_test} ${_test}.c)
    target_link_libraries(${_test} PRIVATE pplib)

    add_test(
        NAME pplib.${_test}
        COMMAND $<TARGET_FILE:${_test}>
    )
endforeach()
