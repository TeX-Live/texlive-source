prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@

Name: poppler
Description: PDF rendering library
Version: @POPPLER_VERSION@

Libs: -L${libdir} -lpoppler
Cflags: -I${includedir}/poppler
