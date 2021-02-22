cmake_minimum_required (VERSION 3.1)
project(zziptest VERSION "0.13.71" LANGUAGES C)

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
find_package ( PythonInterp 3.5 REQUIRED )
find_package ( UnixCommands REQUIRED ) # bash cp mv rm gzip tar
find_program ( MKZIP zip)

set(EXEEXT ${CMAKE_EXECUTABLE_SUFFIX})
set(README ${CMAKE_SOURCE_DIR}/README)
set(PY ${PYTHON_EXECUTABLE})
set(srcdir ${CMAKE_CURRENT_SOURCE_DIR})
set(topdir ${CMAKE_SOURCE_DIR})

# targets ########################################################
add_executable(zzipself    zzipself.c)
add_executable(zzipsetstub zzipsetstub.c)

target_link_libraries(zzipself     libzzip        ZLIB::ZLIB)
target_link_libraries(zzipsetstub  libzzip        ZLIB::ZLIB)

target_include_directories(zzipself     PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(zzipsetstub  PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

if(CMAKE_GENERATOR MATCHES "Makefiles")
add_custom_command(OUTPUT zzipshowme${EXEEXT}
   COMMAND rm -f test
   COMMAND ln -s . test
   COMMAND ${MKZIP} -0 -j $@ zzipself${EXEEXT}
   COMMAND ${MKZIP} -9 -j $@ ${README}
   COMMAND test -s $@ || cp test2.zip $@ || true
   COMMAND test ! -f $@.zip || mv $@.zip $@
   COMMAND ./zzipsetstub${EXEEXT} $@ zzipself${EXEEXT}
   COMMAND chmod +x $@
   DEPENDS zzipself zzipsetstub)
add_custom_target(sfx ALL DEPENDS zzipshowme${EXEEXT})
endif()

add_custom_target(tests
   COMMAND ${PY} ${srcdir}/zziptests.py --exeext=${EXEEXT} --topsrcdir=${topdir} -v ${TESTFLAGS}
   DEPENDS ../bins/zzcat ../bins/zzdir ../bins/zziptest ../bins/zzxorcat ../bins/zzxorcopy)
add_custom_command(OUTPUT ../bins/zzcat COMMMAND cd ../bins && $(MAKE) zzcat)
add_custom_command(OUTPUT ../bins/zzdir COMMMAND cd ../bins && $(MAKE) zzdir)
add_custom_command(OUTPUT ../bins/zziptest COMMMAND cd ../bins && $(MAKE) zziptest)
add_custom_command(OUTPUT ../bins/zzxorcat COMMMAND cd ../bins && $(MAKE) zzxorcat)
add_custom_command(OUTPUT ../bins/zzxorcopy COMMMAND cd ../bins && $(MAKE) zzxorcopy)

add_custom_command(OUTPUT test0.zip
    COMMAND ${BASH} -c "test -d tmp || mkdir tmp"
    COMMAND ${CP} ${README} tmp/README
    COMMAND ${BASH} -c "cd tmp && ${MKZIP} ../test0.zip README"
    COMMAND ${CP} test0.zip test.zip
    BYPRODUCTS test.zip
    VERBATIM)
add_custom_target(testzips ALL DEPENDS test0.zip)

add_custom_target(checks
    COMMAND ${BASH} -c "../bins/zzcat test/README > test.out"
    COMMAND ${BASH} -c "if diff test.out ${README} >/dev/null \
       ; then rm test.out ; echo check OK; true \
       ; else rm test.out ; echo check FAIL ; false; fi"
    DEPENDS ../bins/zzcat test0.zip
    VERBATIM)
add_custom_target(check DEPENDS checks tests)

# install ########################################################
# - nothing -
