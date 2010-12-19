## Makefile.am for the TeX Live subdirectory texk/kpathsea/
##
## Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all

SUBDIRS = . doc man

EXTRA_DIST = BUGS PROJECTS

INCLUDES = -I$(top_builddir)/.. -I$(top_srcdir)/..
AM_CFLAGS = $(WARNING_CFLAGS)

lib_LTLIBRARIES = libkpathsea.la

libkpathsea_la_CPPFLAGS = -DMAKE_KPSE_DLL

libkpathsea_la_LDFLAGS = -no-undefined -version-info $(KPSE_LT_VERSINFO)

libkpathsea_la_LIBADD = $(LTLIBOBJS)

BUILT_SOURCES = paths.h

## Put tex-file.c first, because it's what depends on the paths, and may
## reduce frustration if the paths are wrong by doing it first.
libkpathsea_la_SOURCES = \
	tex-file.c \
	absolute.c \
	atou.c \
	cnf.c \
	concat.c \
	concat3.c \
	concatn.c \
	db.c \
	debug.c \
	dir.c \
	elt-dirs.c \
	expand.c \
	extend-fname.c \
	file-p.c \
	find-suffix.c \
	fn.c \
	fontmap.c \
	getopt.c \
	getopt1.c \
	hash.c \
	kdefault.c \
	kpathsea.c \
	line.c \
	magstep.c \
	make-suffix.c \
	path-elt.c \
	pathsearch.c \
	proginit.c \
	progname.c \
	readable.c \
	rm-suffix.c \
	str-list.c \
	str-llist.c \
	tex-glyph.c \
	tex-hush.c \
	tex-make.c \
	tilde.c \
	truncate.c \
	uppercasify.c \
	variable.c \
	version.c \
	xbasename.c \
	xcalloc.c \
	xdirname.c \
	xfopen.c \
	xfseek.c \
	xfseeko.c \
	xftell.c \
	xftello.c \
	xgetcwd.c \
	xmalloc.c \
	xopendir.c \
	xputenv.c \
	xrealloc.c \
	xstat.c \
	xstrdup.c

if MINGW32
libkpathsea_la_SOURCES += mingw32.c
endif MINGW32

$(libkpathsea_la_OBJECTS): paths.h kpathsea.h

EXTRA_DIST += \
	putenv.c \
	strcasecmp.c \
	strtol.c \
	strstr.c

kpseincludedir = ${includedir}/kpathsea

nodist_kpseinclude_HEADERS = \
	c-auto.h \
	kpathsea.h \
	paths.h

kpseinclude_HEADERS = \
	absolute.h \
	c-ctype.h \
	c-dir.h \
	c-errno.h \
	c-fopen.h \
	c-limits.h \
	c-memstr.h \
	c-minmax.h \
	c-namemx.h \
	c-pathch.h \
	c-pathmx.h \
	c-proto.h \
	c-stat.h \
	c-std.h \
	c-unistd.h \
	cnf.h \
	concatn.h \
	config.h \
	db.h \
	debug.h \
	default.h \
	expand.h \
	fn.h \
	fontmap.h \
	getopt.h \
	hash.h \
	lib.h \
	line.h \
	magstep.h \
	mingw32.h \
	pathsearch.h \
	proginit.h \
	progname.h \
	readable.h \
	recorder.h \
	str-list.h \
	str-llist.h \
	systypes.h \
	tex-file.h \
	tex-glyph.h \
	tex-hush.h \
	tex-make.h \
	tilde.h \
	truncate.h \
	types.h \
	variable.h \
	version.h \
	win32lib.h \
	xopendir.h \
	xstat.h

## First null out comments and leading/trailing whitespace, then remove
## lines that define invalid C identifiers, then remove blank lines and
## lines that define lowercase values (those are never path values).
## Each line remaining looks like
## <name> = <value>
## (but = and surrounding spaces are optional, hence can't remove all spaces).
## Change this to #ifndef DEFAULT_name@#define DEFAULT_name "value"@#endif,
## then change the @'s to newlines (sed isn't good at multiline replacements).
##
## No backslash-newline escapes in the long sed replacement because that
## will turn into a space in the output.
##
## We replace the $TEXMF* references with nonexisting directories.  These
## are the compile-time defaults and are not useful in the native TL
## builds; we rely completely on the self-location.  (Previously, the
## random build-time paths would get included in the binaries, to
## consequent confusion.)
##
## We preserve the $SELFAUTO* references, since the selfauto-using
## compile-time path for cnf files is how texmf.cnf gets found, which
## determines everything else.
##
## The definition of DEFAULT_TEXMF (and other variables)
## that winds up in the final paths.h will not be used.
##
## We don't want to rewrite paths.h when we have only changed comments
## in texmf.cnf that have no effect on paths.h, since that would cause
## almost everything to be rebuilt.
##
## We also replace all semicolons with colons in the paths.
## (The path-splitting code should be changed to understand both.)
##
paths.h: stamp-paths
stamp-paths: texmf.cnf bsnl.awk
	echo "/* paths.h: Generated from texmf.cnf. */" >paths.tmp
	awk -f $(srcdir)/bsnl.awk $(srcdir)/texmf.cnf \
	| sed -e 's/%.*//' -e 's/^[ 	]*//' -e 's/[ 	]*$$//' \
	| grep '^[ 	]*[A-Z0-9_]*[ 	=]' \
	| sed '/^$$/d' \
	| sed 's/^\([^ 	=]*\)[ 	]*=*[ 	]*\(.*\)/#ifndef DEFAULT_\1%#define DEFAULT_\1 "\2"%#endif/' \
	| tr '%' '\012' \
	| sed -e 's%\$$TEXMFCONFIG%/nonesuch/texmfconfig%g' \
	      -e 's%\$$TEXMFDIST%/nonesuch/texmfdist%g' \
	      -e 's%\$$TEXMFDOCDIR%/nonesuch/texmfdocdir%g' \
	      -e 's%\$$TEXMFLOCAL%/nonesuch/texmflocal%g' \
	      -e 's%\$$TEXMFMAIN%/nonesuch/texmfmain%g' \
	      -e 's%\$$TEXMFSYSCONFIG%/nonesuch/texmfsysconfig%g' \
	      -e 's%\$$TEXMFSYSVAR%/nonesuch/texmfsysvar%g' \
	      -e 's%\$$TEXMFVAR%/nonesuch/texmfvar%g' \
	      -e 's%\$$TEXMF%/nonesuch/texmf%g' \
	      -e 's%\$$VARTEXFONTS%/nonesuch/vartexfonts%g' \
	| sed -e 's%;%:%g' \
	>>paths.tmp
	@if cmp -s paths.h paths.tmp 2>/dev/null; then \
	  echo "paths.h is unchanged"; \
	else \
	  echo "cp paths.tmp paths.h"; \
	  cp paths.tmp paths.h; \
	fi
	rm -f paths.tmp
	date >$@

kpathsea.h: Makefile paths.h
	rm -f $@
	echo '/* This is a generated file */' >$@
	echo '/* collecting all kpathsea headers. */' >>$@
	echo '#include <kpathsea/config.h>' >>$@
	echo '#include <kpathsea/c-auto.h>' >>$@
	echo '#include <kpathsea/paths.h>' >>$@
	(cd $(srcdir) && ls -1 *.h) \
	| sed -e '/^config\.h/d' \
	      -e '/^paths\.h/d' \
	      -e '/^kpathsea\.h/d' \
	      -e '/^mingw32\.h/d' \
	      -e '/^win32lib\.h/d' \
	      -e 's,^,#include <kpathsea/,' -e s',$$,>,' >>$@

EXTRA_DIST += bsnl.awk

DISTCLEANFILES = paths.h stamp-paths kpathsea.h

## The programs
bin_PROGRAMS = kpseaccess kpsereadlink kpsestat kpsewhich

kpseaccess_SOURCES = access.c

kpsereadlink_SOURCES = readlink.c

kpsewhich_LDADD = libkpathsea.la

## The scripts and their data
web2cdir = ${prefix}/texmf/web2c

dist_web2c_SCRIPTS = mktexdir mktexnam mktexupd

dist_web2c_DATA = mktex.opt mktexdir.opt mktexnam.opt

dist_noinst_SCRIPTS = mktexlsr mktexmf mktexpk mktextfm

dist_noinst_DATA = texmf.cnf

# Symlinks within $(bindir): FILE:LINK indicates LINK->FILE
bin_links = \
	mktexlsr:texhash

install-exec-hook:
if !WIN32
	@for f in $(dist_noinst_SCRIPTS); do \
	  if grep "original $$f --" "$(DESTDIR)$(bindir)/$$f" >/dev/null 2>&1 \
	      || test ! -r "$(DESTDIR)$(bindir)/$$f"; then \
	    echo " $(INSTALL_SCRIPT) '$(srcdir)/$$f' '$(DESTDIR)$(bindir)/$$f'"; \
	    $(INSTALL_SCRIPT) "$(srcdir)/$$f" "$(DESTDIR)$(bindir)/$$f"; \
	  else :; fi; \
	done
	@cd $(DESTDIR)$(bindir) && \
	  for s in $(bin_links); do \
	    link=`echo $$s | sed 's,.*:,,'`; \
	    file=`echo $$s | sed 's,:.*,,'`; \
	    rm -f $$link; \
	    echo "creating link '$$link' -> '$$file'"; \
	    $(LN_S) $$file $$link; \
	  done
endif !WIN32

install-data-hook:
	@for f in $(dist_noinst_DATA); do \
	  if grep "original $$f --" "$(DESTDIR)$(web2cdir)/$$f" >/dev/null 2>&1 \
	      || test ! -r "$(DESTDIR)$(web2cdir)/$$f"; then \
	    echo " $(INSTALL_DATA) '$(srcdir)/$$f' '$(DESTDIR)$(web2cdir)/$$f'"; \
	    $(INSTALL_DATA) "$(srcdir)/$$f" "$(DESTDIR)$(web2cdir)/$$f"; \
	  else :; fi; \
	done

uninstall-hook:
if !WIN32
	@for f in $(dist_noinst_SCRIPTS); do \
	  if grep "original $$f --" "$(DESTDIR)$(bindir)/$$f" >/dev/null 2>&1; then \
	    echo " rm -f '$(DESTDIR)$(bindir)/$$f'"; \
	    rm -f "$(DESTDIR)$(bindir)/$$f"; \
	  else :; fi; \
	done
	@for s in $(bin_links); do \
	  link=`echo $$s | sed 's,.*:,,'`; \
	  rm -f $(DESTDIR)$(bindir)/$$link; \
	done
endif !WIN32
	@for f in $(dist_noinst_DATA); do \
	  if grep "original $$f --" "$(DESTDIR)$(web2cdir)/$$f" >/dev/null 2>&1; then \
	    echo " rm -f '$(DESTDIR)$(web2cdir)/$$f'"; \
	    rm -f "$(DESTDIR)$(web2cdir)/$$f"; \
	  else :; fi; \
	done

## The tests
TESTS = kpseaccess.test kpsereadlink.test kpsestat.test kpsewhich.test
TESTS_ENVIRONMENT = LN_S='$(LN_S)' LT_OBJDIR='$(LT_OBJDIR)'
EXTRA_DIST += $(TESTS)

## Not used
##
EXTRA_DIST += mktex.cnf

## Eventually delete these files
##
EXTRA_DIST += Makefile.in.orig common.ac.orig withenable.ac

