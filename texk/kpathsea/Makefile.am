## Makefile.am for the TeX Live subdirectory texk/kpathsea/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all-am
	touch libkpathsea.la

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

if WIN32
if MINGW32
libkpathsea_la_SOURCES += mingw32.c
else !MINGW32
libkpathsea_la_SOURCES += win32lib.c
endif !MINGW32
libkpathsea_la_SOURCES += knj.c
SUBDIRS += win32
endif WIN32

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

kpseinclude_HEADERS = $(special_headers) $(normal_headers)

# Headers treated specially when building kpathsea.h
#
special_headers = \
	config.h \
	knj.h \
	mingw32.h \
	win32lib.h

normal_headers = \
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
	debug.h \
	expand.h \
	getopt.h \
	hash.h \
	lib.h \
	line.h \
	magstep.h \
	pathsearch.h \
	proginit.h \
	progname.h \
	readable.h \
	str-list.h \
	str-llist.h \
	systypes.h \
	tex-file.h \
	tex-glyph.h \
	tex-hush.h \
	tex-make.h \
	types.h \
	variable.h \
	version.h

noinst_HEADERS = \
	db.h \
	default.h \
	fn.h \
	fontmap.h \
	tilde.h \
	truncate.h \
	xopendir.h \
	xstat.h

## Handle backslash continuations, then null out comments and
## leading/trailing whitespace.  An awk script does the rest.
## 
## We don't want to rewrite paths.h when we have only changed comments
## in texmf.cnf that have no effect on paths.h, since that would cause
## almost everything to be rebuilt.
##
paths.h: stamp-paths
stamp-paths: texmf.cnf bsnl.awk cnf-to-paths.awk
# ensure grep is not completely broken, e.g., from GREP_OPTIONS.
	echo a | grep -v b >/dev/null || { echo "*** grep broken, goodbye."; exit 1; }
#
# generate paths.h without build machine directories.
	echo "/* paths.h: Generated from texmf.cnf. */" >paths.tmp
	$(AWK) -f $(srcdir)/bsnl.awk $(srcdir)/texmf.cnf \
	| sed -e 's/%.*//' -e 's/^[ 	]*//' -e 's/[ 	]*$$//' \
	| $(AWK) -f $(srcdir)/cnf-to-paths.awk \
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
	echo '/* collecting all public kpathsea headers. */' >>$@
	for f in config.h c-auto.h paths.h $(normal_headers); do \
	  echo "#include <kpathsea/$$f>" >>$@; \
	done

EXTRA_DIST += bsnl.awk cnf-to-paths.awk

DISTCLEANFILES = paths.h stamp-paths kpathsea.h

## The programs
bin_PROGRAMS = kpseaccess kpsereadlink kpsestat kpsewhich

kpseaccess_SOURCES = access.c

kpsereadlink_SOURCES = readlink.c

kpsewhich_LDADD = libkpathsea.la

EXTRA_PROGRAMS = progname-test xdirtest

progname_test_CPPFLAGS = -DMAKE_KPSE_DLL -DTEST
progname_test_SOURCES = progname.c
progname_test_LDADD = libkpathsea.la

xdirtest_LDADD = libkpathsea.la

## The scripts and their data
web2cdir = $(datarootdir)/texmf/web2c

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
	    $(LN_S) $$file $$link || exit 1; \
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
EXTRA_DIST += mktex.cnf win32lib.h.mugica

