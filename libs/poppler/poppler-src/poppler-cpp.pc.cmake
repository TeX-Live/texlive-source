prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@

Name: poppler-cpp
Description: cpp backend for Poppler PDF rendering library
Version: @POPPLER_VERSION@
Requires: @PC_REQUIRES@
@PC_REQUIRES_PRIVATE@

Libs: -L${libdir} -lpoppler-cpp
Cflags: -I${includedir}/poppler/cpp
