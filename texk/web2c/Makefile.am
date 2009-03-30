##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = -I$(top_builddir)/.. $(KPATHSEA_INCLUDES)

version = $(WEB2CVERSION)

SUBDIRS = doc lib man mpware web2c . window
if OMFONTS
SUBDIRS += omegafonts
endif OMFONTS
SUBDIRS += otps

proglib = lib/lib.a

## Below we will add to these
EXTRA_PROGRAMS =
bin_PROGRAMS =
noinst_PROGRAMS =
dist_man_MANS =
nodist_man_MANS =
check_PROGRAMS =
dist_check_SCRIPTS =
TESTS =
EXTRA_LIBRARIES =
EXTRA_DIST = PROJECTS cftests config.h cpascal.h help.h
DISTCLEANFILES = $(EXTRA_LIBRARIES)
CLEANFILES =

## in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

## The sh script that does the conversion.
web2c = srcdir=$(srcdir) $(SHELL) $(srcdir)/web2c/convert

## We put this in the web2c directory for lack of a better place, but it
## is separate from (and simpler than) the actual web-to-C conversion programs.
makecpool = ./web2c/makecpool

## Additional dependencies:
web2c_common = web2c/convert web2c/common.defines
web2c_programs = web2c/fixwrites web2c/splitup web2c/web2c $(makecpool)
web2c_texmf = $(web2c_common) $(web2c_programs) \
              web2c/texmf.defines web2c/coerce.h

## This is right for most Web2C programs
LDADD = $(proglib) $(KPATHSEA_LIBS)

## Automake automatically uses $(proglib)=lib/lib.a as dependency.
## This does not work for the configure substitution  $(KPATHSEA_LIBS),
## thus we let $(proglib) artificially depend on $(KPATHSEA_LIBS).

## Rebuild $(proglib)
$(proglib): $(KPATHSEA_DEPEND) ${srcdir}/lib/*.c
	cd lib && $(MAKE) $(AM_MAKEFLAGS)

## Rebuild $(web2c_programs)
$(web2c_programs): $(web2c_common) ${srcdir}/web2c/*.[chly]
	cd web2c && $(MAKE) $(AM_MAKEFLAGS)

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libz
@ZLIB_RULE@
## Rebuild libpng
@LIBPNG_RULE@
## Rebuild libxpdf & Co
@XPDF_RULE@
## Rebuild libopenbsd-compat
@OBSDCOMPAT_RULE@

## The environment for building.
buildenv = TEXMFCNF=$(srcdir)/../kpathsea

## Calling tangle & Co.
tangle = WEBINPUTS=.:$(srcdir) $(buildenv) $(TANGLE)
tangleboot = WEBINPUTS=.:$(srcdir) $(buildenv) $(TANGLEBOOT)
otangle = WEBINPUTS=.:$(srcdir) $(buildenv) $(OTANGLE)
ctangle = $(buildenv) $(CTANGLE)
ctangleboot = $(buildenv) $(CTANGLEBOOT)
tie = WEBINPUTS=.:$(srcdir) $(buildenv) $(TIE)

## The environment for making dumps.
## No need to specify TEXMF, as ../kpathsea/texmf.cnf should contain
## suitable absolute paths at this point.  Those are converted to
## SELFAUTO* paths on installation.
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

## old MetaPost
include $(srcdir)/mpdir/am/mp.am

## new MetaPost, also used for by luaTeX
include $(srcdir)/mplibdir/am/mplib.am

## e-TeX
include $(srcdir)/etexdir/am/etex.am

## pdfTeX
include $(srcdir)/pdftexdir/am/pdftex.am

## luaTeX
include $(srcdir)/luatexdir/am/luatex.am

## XeTeX
include $(srcdir)/xetexdir/am/xetex.am

## omegaware web programs
include $(srcdir)/omegaware/am/omegaware.am

## Aleph
include $(srcdir)/alephdir/am/aleph.am

## SyncTeX
include $(srcdir)/synctexdir/am/synctex.am

## libmd5
include $(srcdir)/libmd5/am/md5.am

## Not used
##
EXTRA_DIST += fmtutil.in

## Eventually delete these files
##
EXTRA_DIST += Makefile.in.orig configure.in.orig

