## Makefile.am for the TeX Live subdirectory texk/web2c/.
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES)
## Not yet for C, C++, and ObjC++ (too many warnings or not yet implemented)
AM_CFLAGS = $(WARNING_CFLAGS)
AM_CXXFLAGS = ## $(WARNING_CXXFLAGS)
AM_OBJCXXFLAGS = ## $(WARNING_OBJCXXFLAGS)

version = $(WEB2CVERSION)

SUBDIRS = doc lib man web2c . window omegafonts otps

proglib = lib/lib.a

## Below we will add to these
EXTRA_PROGRAMS =
EXTRA_LIBRARIES =
EXTRA_LTLIBRARIES =
bin_PROGRAMS =
lib_LIBRARIES =
lib_LTLIBRARIES =
noinst_PROGRAMS =
dist_man_MANS =
nodist_man_MANS =
check_PROGRAMS =
TESTS =
EXTRA_DIST = PROJECTS cftests cpascal.h help.h w2c/config.h
DISTCLEANFILES = CXXLD.sh
CLEANFILES =
TRIPTRAP_CLEAN =
TRIPTRAP =
bin_links =
install_exe_links =
uninstall_exe_links =

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am

## The sh script that does the conversion.
web2c = AM_V_P=$(AM_V_P) $(SHELL) ./web2c-sh $@

## We put this in the web2c directory because it runs on the build system, it
## is separate from (and simpler than) the actual web-to-C conversion programs.
makecpool = $(AM_V_GEN)web2c/makecpool

# Additional dependencies:
web2c_common = web2c/convert web2c/common.defines
# When cross-compiling, EXEEXT for the build and hosts system may differ.
# Thus we depend on the auxiliary files web2c/stamp-*.
web2c_stamps = web2c/stamp-fixwrites web2c/stamp-splitup web2c/stamp-web2c
makecpool_stamp = web2c/stamp-makecpool
web2c_depend = $(web2c_common) $(web2c_stamps) web2c-sh
web2c_texmf = $(web2c_depend) web2c/texmf.defines web2c/coerce.h

# This is right for most Web2C programs
LDADD = $(proglib) $(KPATHSEA_LIBS)
# Automake automatically uses $(proglib)=lib/lib.a as dependency.
# This does not work for the configure substitution $(KPATHSEA_LIBS),
# thus we must explicitly use these $(default_dependencies) for
# programs that do not depend on tangle or ctangle.
default_dependencies = $(proglib) $(KPATHSEA_DEPEND)

# Rebuild Web2C programs
web2c/stamp-fixwrites: $(web2c_common) ${srcdir}/web2c/*.[chly]
	cd web2c && $(MAKE) $(AM_MAKEFLAGS) stamp-fixwrites
	touch $@
web2c/stamp-splitup: $(web2c_common) ${srcdir}/web2c/*.[chly]
	cd web2c && $(MAKE) $(AM_MAKEFLAGS) stamp-splitup
	touch $@
web2c/stamp-web2c: $(web2c_common) ${srcdir}/web2c/*.[chly]
	cd web2c && $(MAKE) $(AM_MAKEFLAGS) stamp-web2c
	touch $@
web2c/stamp-makecpool: $(web2c_common) ${srcdir}/web2c/makecpool.c
	cd web2c && $(MAKE) $(AM_MAKEFLAGS) stamp-makecpool

# Rebuild $(proglib)
$(proglib): ${srcdir}/lib/*.c
	cd lib && $(MAKE) $(AM_MAKEFLAGS) lib.a
## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libptexenc
@PTEXENC_RULE@
## Rebuild libz
@ZLIB_RULE@
## Rebuild liblua52
@LUA52_RULE@
## Rebuild libluajit
@LUAJIT_RULE@
## Rebuild libpng
@LIBPNG_RULE@
## Rebuild libfreetype
@FREETYPE2_RULE@
## Rebuild libpixman
@PIXMAN_RULE@
## Rebuild libcairo
@CAIRO_RULE@
## Rebuild libgmp
@GMP_RULE@
## Rebuild libmpfr
@MPFR_RULE@
## Rebuild libxpdf
@XPDF_RULE@
## Rebuild poppler
@POPPLER_RULE@
## Rebuild libzzip
@ZZIPLIB_RULE@
## Rebuild libTEXkit
@TECKIT_RULE@
## Rebuild libsicuxxx
@ICU_RULE@
## Rebuild libharfbuzz
@HARFBUZZ_RULE@
## Rebuild libgraphite2
@GRAPHITE2_RULE@

# The environment for building.
buildenv = TEXMFCNF=$(srcdir)/../kpathsea

# Calling tangle & Co.
tangle = $(tangle_silent)WEBINPUTS=.:$(srcdir) $(buildenv) $(TANGLE)
tangleboot = $(tangle_silent)WEBINPUTS=.:$(srcdir) $(buildenv) $(TANGLEBOOT)
tangle_silent = $(tangle_silent_@AM_V@)
tangle_silent_ = $(tangle_silent_@AM_DEFAULT_V@)
tangle_silent_0 = @echo "  TANGLE  " $@; $(SHELL) ./silent-sh $@ #
tangle_silent_1 =
#
ctangle = $(buildenv) $(CTANGLE)
ctangle_silent = $(ctangle_silent_@AM_V@)
ctangle_silent_ = $(ctangle_silent_@AM_DEFAULT_V@)
ctangle_silent_0 = @echo "  CTANGLE " $@; $(SHELL) ./silent-sh $@ #
ctangle_silent_1 =
#
tie = $(tie_silent)WEBINPUTS=.:$(srcdir) $(buildenv) $(TIE)
tie_silent = $(tie_silent_@AM_V@)
tie_silent_ = $(tie_silent_@AM_DEFAULT_V@)
tie_silent_0 = @echo "  TIE     " $@; $(SHELL) ./silent-sh $@ #
tie_silent_1 =

# Calling tangle & Co. via tangle-sh (several output files)
texmf_tangle = WEBINPUTS=.:$(srcdir) AM_V_P=$(AM_V_P) $(SHELL) ./tangle-sh $@ $(TANGLE)

# For trip, trap, and other tests
DIFF = diff
DIFFFLAGS =
TESTS_ENVIRONMENT = abs_srcdir=$(abs_srcdir) DIFF='$(DIFF)' DIFFFLAGS='$(DIFFFLAGS)' LN_S='$(LN_S)' KPSEWHICH='$(KPSEWHICH)'
triptrap_diffs = $(AM_V_GEN)srcdir=$(srcdir) abs_srcdir=$(abs_srcdir) MAKE=$(MAKE) $(SHELL) $(srcdir)/triptrap-sh

.PHONY: triptrap
triptrap:
	$(triptrap_diffs) $(TRIPTRAP)
EXTRA_DIST += triptrap-sh

clean-local: triptrap-clean
.PHONY: triptrap-clean
triptrap-clean: $(TRIPTRAP_CLEAN)

# The environment for making dumps.
# No need to specify TEXMF, as ../kpathsea/texmf.cnf should contain
# suitable absolute paths at this point.  Those are converted to
# SELFAUTO* paths on installation.
dumpenv = TEXMFCNF=../kpathsea

## ################################

## Prevent Make from deleting the intermediate forms.
.PRECIOUS: %.ch %.p %.c

## ################################

## tangle and ctangle
include $(srcdir)/am/bootstrap.am

## normal web programs (except tangle)
include $(srcdir)/am/web.am

## normal cweb programs (except ctangle)
include $(srcdir)/am/cweb.am

## TeX and MF
include $(srcdir)/am/texmf.am

## MetaPost
include $(srcdir)/mplibdir/am/mplib.am

## pMetaPost and upMetaPost
include $(srcdir)/pmpostdir/am/pmpost.am

## libmputil, libmplib, used by MetaPost and luaTeX
include $(srcdir)/mplibdir/am/libmputil.am
include $(srcdir)/mplibdir/am/libmplib.am

## e-TeX
include $(srcdir)/etexdir/am/etex.am

## pTeX
include $(srcdir)/ptexdir/am/ptex.am

## e-pTeX
include $(srcdir)/eptexdir/am/eptex.am

## upTeX
include $(srcdir)/uptexdir/am/uptex.am

## e-upTeX
include $(srcdir)/euptexdir/am/euptex.am

## pdfTeX
include $(srcdir)/pdftexdir/am/libpdftex.am
include $(srcdir)/pdftexdir/am/pdftex.am
include $(srcdir)/pdftexdir/am/ttf2afm.am
include $(srcdir)/pdftexdir/am/pdftosrc.am

## luaTeX
include $(srcdir)/luatexdir/am/luasocket.am
include $(srcdir)/luatexdir/am/luamisc.am
include $(srcdir)/luatexdir/am/libunilib.am
include $(srcdir)/luatexdir/am/luafontforge.am
include $(srcdir)/luatexdir/am/libluatex.am
include $(srcdir)/luatexdir/am/luatex.am
include $(srcdir)/luatexdir/am/txt2zlib.am

## XeTeX
include $(srcdir)/xetexdir/am/xetex.am

## omegaware web programs
include $(srcdir)/omegaware/am/omegaware.am

## programs required for 'make check' in omegafonts/ and omegaware/
if OTANGLE
check_PROGRAMS += pltotf tftopl
endif OTANGLE

## Aleph
include $(srcdir)/alephdir/am/aleph.am

## SyncTeX, must come after all TeX-like engines
include $(srcdir)/synctexdir/am/synctex.am

## libmd5
include $(srcdir)/libmd5/am/md5.am

## Symlinks within $(bindir)
include $(srcdir)/../../am/bin_links.am
install-exec-hook: install-bin-links $(install_exe_links)
uninstall-hook: uninstall-bin-links $(uninstall_exe_links)

