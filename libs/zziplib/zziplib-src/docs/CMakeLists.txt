cmake_minimum_required (VERSION 3.1)
project(zzipbins VERSION "0.13.71" LANGUAGES C)

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
find_package(PythonInterp 3.5 REQUIRED)
find_package(UnixCommands REQUIRED) # bash cp mv rm gzip tar
find_program(XMLTO xmlto)

set(EXEEXT ${CMAKE_EXECUTABLE_SUFFIX})
set(README ${CMAKE_SOURCE_DIR}/README)

set(topdir ${CMAKE_SOURCE_DIR})
set(srcdir ${CMAKE_CURRENT_SOURCE_DIR})
set(outdir ${CMAKE_CURRENT_BINARY_DIR})
set(PY ${PYTHON_EXECUTABLE})

set(mandir ${CMAKE_INSTALL_FULL_MANDIR})
set(docdir ${CMAKE_INSTALL_FULL_DOCDIR})

set(bins ${CMAKE_SOURCE_DIR}/bins)
set(SDLs ${CMAKE_SOURCE_DIR}/SDL)

# targets ########################################################
set(doc_FILES README.MSVC6 README.SDL COPYING.MPL COPYING.LIB COPYING.ZLIB
            zziplib.html zzipmmapped.html zzipfseeko.html)
set(htm_FILES zzip-index.htm zzip-zip.htm zzip-file.htm zzip-sdl-rwops.htm
            zzip-extio.htm zzip-xor.htm zzip-crypt.htm zzip-cryptoid.htm
            zzip-api.htm  zzip-basics.htm zzip-extras.htm zzip-parse.htm
            64on32.htm future.htm fseeko.htm mmapped.htm memdisk.htm
            configs.htm sfx-make.htm   developer.htm download.htm
            history.htm referentials.htm  faq.htm copying.htm notes.htm
            functions.htm zip-php.htm)
set(bins_FILES ${bins}/zzdir.c ${bins}/zzcat.c ${bins}/zzobfuscated.c ${bins}/zziptest.c ${bins}/zzxordir.c ${bins}/zzxorcat.c ${bins}/zzxorcopy.c )
set(SDLs_FILES ${SDLs}/SDL_rwops_zzcat.c ${SDLs}/SDL_rwops_zzip.c ${SDLs}/SDL_rwops_zzip.h )
set(zzipdoc_FILES makedocs.py          zzipdoc/__init__.py
    zzipdoc/commentmarkup.py          zzipdoc/match.py
    zzipdoc/dbk2htm.py                zzipdoc/htm2dbk.py
    zzipdoc/functionheader.py         zzipdoc/options.py
    zzipdoc/functionlisthtmlpage.py   zzipdoc/textfileheader.py
    zzipdoc/functionlistreference.py  zzipdoc/textfile.py
    zzipdoc/functionprototype.py      zzipdoc/htmldocument.py
                                      zzipdoc/docbookdocument.py)

add_custom_command(OUTPUT changes.htm
    COMMAND ${BASH} -c "E=changes.htm \
    ; echo '<pre>' > $E ; cat ${topdir}/ChangeLog \
    | sed -e 's,\\&,\\&amp\\;,g' \
    -e 's,<,\\&lt\\;,g' -e 's,>,\\&gt\\;,g' \
    -e '/^[A-Z].*[12][09][09][09]/s,\\(.*\\),<b>&</b>,' \
    -e '/^[0-9]/s,\\(.*\\),<b>&</b>,' >> $E ; echo '</pre>' >>$E"
    DEPENDS ${topdir}/ChangeLog
    VERBATIM)

set(mksite_sh_args --VERSION=${PROJECT_VERSION} --xml --src-dir=${srcdir} --print)
add_custom_command(OUTPUT site.html
    COMMAND ${CP} ${srcdir}/body.htm site.htm
    COMMAND ${BASH} ${srcdir}/mksite.sh ${mksite_sh_args} site.htm
    COMMAND ${BASH} -c "for i in *.html; do sed -i -e s:${srcdir}:../../docs: $i; done; true"
    DEPENDS ${htm_FILES} changes.htm
    VERBATIM)
add_custom_target(site DEPENDS site.html)

add_custom_command(OUTPUT zzip.xml
    COMMAND ${PY} ${srcdir}/zzipdoc/htm2dbk.py ${htm_FILES} zziplib.xml -o ${outdir}/zzip.xml
    DEPENDS zziplib.xml ${htm_FILES}
    WORKING_DIRECTORY ${srcdir}
    VERBATIM)
add_custom_target(dbk DEPENDS zzip.xml)

if(XMLTO)
add_custom_command(OUTPUT zzip.html
    COMMAND ${XMLTO} html-nochunks zzip.xml
    DEPENDS zzip.xml
    VERBATIM)
add_custom_command(OUTPUT zzip.pdf
    COMMAND ${XMLTO} pdf zzip.xml
    DEPENDS zzip.xml
    VERBATIM)
add_custom_target(pdf DEPENDS zzip.pdf)
endif()

set(docinfo --package=zziplib --version=${PROJECT_VERSION})
file(GLOB zzip_sources "${topdir}/zzip/*.c")
add_custom_command(OUTPUT zziplib.xml
    COMMAND ${PY} ${srcdir}/makedocs.py ${zzip_sources} ${docinfo} --onlymainheader=zzip/lib.h --output=zziplib
    COMMAND ${MV} zziplib.docbook zziplib.xml
    DEPENDS libzzip
    BYPRODUCTS zziplib.html
    VERBATIM)
add_custom_command(OUTPUT zzipmmapped.xml
    COMMAND ${PY} ${srcdir}/makedocs.py ${zzip_sources} ${docinfo} --onlymainheader=zzip/mmapped.h --output=zzipmmapped
    COMMAND ${MV} zzipmmapped.docbook zzipmmapped.xml
    DEPENDS libzzipmmapped
    BYPRODUCTS zzipmmapped.html
    VERBATIM)
add_custom_command(OUTPUT zzipfseeko.xml
    COMMAND ${PY} ${srcdir}/makedocs.py ${zzip_sources} ${docinfo} --onlymainheader=zzip/fseeko.h --output=zzipfseeko
    COMMAND ${MV} zzipfseeko.docbook zzipfseeko.xml
    DEPENDS libzzipfseeko
    BYPRODUCTS zzipfseeko.html
    VERBATIM)
add_custom_command(OUTPUT manpages.tar
    COMMAND ${BASH} -c "test -d man3 && rm -rf man3; mkdir man3"
    COMMAND ${PY} ${srcdir}/dbk2man.py -o man3 man zziplib.xml
    COMMAND ${PY} ${srcdir}/dbk2man.py -o man3 man zzipmmapped.xml
    COMMAND ${PY} ${srcdir}/dbk2man.py -o man3 man zzipfseeko.xml
    COMMAND ${BASH} -c "test -d man3/man3 && mv man3 man3_; test -d man3_/man3 && mv man3_/man3 .; rm -rf man3_"
    COMMAND ${BASH} -c "chmod 664 man3/*.3"
    COMMAND ${BASH} -c "tar cf manpages.tar man3"
    COMMAND ${BASH} -c "ls -l `pwd`/manpages.tar `pwd`/htmpages.tar >&2 || true"
    DEPENDS zziplib.xml zzipmmapped.xml zzipfseeko.xml
    VERBATIM)
add_custom_command(OUTPUT htmpages.tar
    COMMAND ${BASH} -c "test -d html && rm -rf html; mkdir html"
    COMMAND ${PY} ${srcdir}/dbk2man.py -o html html zziplib.xml
    COMMAND ${PY} ${srcdir}/dbk2man.py -o html html zzipmmapped.xml
    COMMAND ${PY} ${srcdir}/dbk2man.py -o html html zzipfseeko.xml
    COMMAND ${PY} ${srcdir}/dir2index.py -o html html
    COMMAND ${BASH} -c "tar cf htmpages.tar html/*.*"
    COMMAND ${BASH} -c "ls -l `pwd`/htmpages.tar `pwd`/manpages.tar >&2 || true"
    DEPENDS zziplib.xml zzipmmapped.xml zzipfseeko.xml
    VERBATIM)

add_custom_target(mans DEPENDS manpages)
add_custom_target(man DEPENDS manpages)
add_custom_target(htm DEPENDS htmpages)
add_custom_target(manpages DEPENDS manpages.tar)
add_custom_target(htmpages DEPENDS htmpages.tar)
add_custom_target(doc DEPENDS zziplib.xml zzipmmapped.xml zzipfseeko.xml site.html)
add_custom_target(docs DEPENDS doc manpages htmpages)


string(REPLACE ";" " " all_bins_FILES "${bins_FILES}")
string(REPLACE ";" " " all_SDLs_FILES "${SDLs_FILES}")
add_custom_target(install-site
   COMMAND ${BASH} -c "ls -l ${CMAKE_CURRENT_BINARY_DIR}/site.html"
   COMMAND ${BASH} -c "mkdir -vp $(DESTDIR)${docdir}  >&2 || ls -ld $(DESTDIR)${docdir} >&2"
   COMMAND ${BASH} -c "cd ${outdir} && cp zziplib.html zzipmmapped.html zzipfseeko.html $(DESTDIR)${docdir}/"
   COMMAND ${BASH} -c "cd ${srcdir} && cp README.* COPYING.* $(DESTDIR)${docdir}/"
   COMMAND ${BASH} -c "cd ${topdir} && cp ChangeLog          $(DESTDIR)${docdir}/"
   COMMAND ${BASH} -c "cd ${topdir} && cp ${all_bins_FILES}  $(DESTDIR)${docdir}/"
   COMMAND ${BASH} -c "cd ${topdir} && cp ${all_SDLs_FILES}  $(DESTDIR)${docdir}/"
   COMMAND ${BASH} -c "cd $(DESTDIR)${docdir} && cp ${outdir}/*.html ."
   COMMAND ${BASH} -c "cd $(DESTDIR)${docdir} && ln -s zzip-index.html index.html"
   DEPENDS site.html zziplib.html zzipmmapped.html zzipfseeko.html
   VERBATIM)
add_custom_target(install-htmpages
   COMMAND ${BASH} -c "ls -l ${CMAKE_CURRENT_BINARY_DIR}/*pages.tar"
   COMMAND ${BASH} -c "mkdir -vp $(DESTDIR)${docdir}  >&2 || ls -ld $(DESTDIR)${docdir} >&2"
   COMMAND ${BASH} -c "cd $(DESTDIR)${docdir} && tar x -f ${outdir}/htmpages.tar"
   COMMAND ${BASH} -c "cd $(DESTDIR)${docdir} && mv html man"
   DEPENDS htmpages.tar
   VERBATIM)
add_custom_target(install-manpages
   COMMAND ${BASH} -c "ls -l ${CMAKE_CURRENT_BINARY_DIR}/*pages.tar"
   COMMAND ${BASH} -c "mkdir -vp $(DESTDIR)${mandir}  >&2 || ls -ld $(DESTDIR)${mandir} >&2"
   COMMAND ${BASH} -c "cd $(DESTDIR)${mandir} && tar xf ${outdir}/manpages.tar"
   DEPENDS manpages.tar
   VERBATIM)
add_custom_target(install-docs DEPENDS install-manpages install-htmpages install-site)
add_custom_target(install-mans DEPENDS install-manpages)
add_custom_target(install-man  DEPENDS install-manpages)

# install ########################################################
set(mandir ${CMAKE_INSTALL_FULL_MANDIR})
install(CODE "execute_process(COMMAND ${BASH} -c \"set -e
   mkdir -vp $DESTDIR/${mandir} || ls -ld $DESTDIR/${mandir}
   cd $DESTDIR/${mandir} && tar xf ${CMAKE_CURRENT_BINARY_DIR}/manpages.tar
   \")")
