################################################################################
#
# Makefile  : Windvi
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/07 16:04:45 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
USE_GSW32 = 1
USE_KPATHSEA = 1
USE_ZLIB = 1
USE_BZIP2 = 1
USE_T1 = 1
USE_NETWORK2=1

# This is a gui application
GUI = 1
# MEMDBG=1
XDVIK_MAKE = 1

windvimajver = 0
windviminver = 66
xrcvars = -DWINDVI_MAJVER=\""$(windvimajver)"\" -DWINDVI_MINVER=\""$(windviminver)"\"

subdirs = doc

!include <msvc/common.mak>

# Compilation options.
DEFS = $(DEFS) -DKPATHSEA -DUSE_POPEN					\
	-DPS_GS                                                         \
	-DA4                                                            \
	-DHTEX                                                          \
	-DBOOK_MODE                                                     \
	-DTRANSFORM=1                                                   \
	-DGREY                                                          \
	-DBUTTONS				                        \
	-DDO_BANDING=1                                                  \
	-DSRC_SPECIALS							\
	-DNeedFunctionPrototypes                                        \
#	-I$(WWWLIBDIR)                                                  \
	-I$(ZLIBDIR) 							\
	-I$(REGEXLIBDIR)                                     		\
	-I$(T1LIBDIR)                                                   \
	-I$(BZ2LIBDIR)                                                  \
	-I$(root_srcdir)                         			\
	$(ZLIB_FLAGS) $(WWWLIB_FLAGS)

!ifdef WIN32
XDVI_H = winutil.h
!else
XDVI_H = xdvi.h
!endif

# Define these appropriately if configure guesses incorrectly.
x_link =

# Extra xdvi-specific compiler options.
ps_def = 
prog_cflags = $(ps_def) $(x_cppflags)

# We don't use alloca ourselves, but the X library might, and this way
# we avoid buggy versions in -lPW or -lucb.
ALLOCA = # $(objdir)\alloca.obj

objects = $(objdir)\windvi.obj $(objdir)\windraw.obj                      \
	$(objdir)\winabout.obj $(objdir)\wingoto.obj                      \
	$(objdir)\wincolor.obj                                            \
	$(objdir)\wintransf.obj                                           \
	$(objdir)\winhtex.obj                                             \
	$(objdir)\winimg.obj                                              \
	$(objdir)\winprint.obj $(objdir)\winver.obj $(objdir)\wincfg.obj  \
	$(objdir)\winlog.obj $(objdir)\winopt.obj $(objdir)\winxutl.obj   \
	$(objdir)\winevt.obj $(objdir)\dvi-draw.obj                       \
	$(objdir)\dvi-init.obj $(objdir)\font-open.obj $(objdir)\gf.obj   \
	$(objdir)\pk.obj $(objdir)\psgsw32.obj                            \
#	$(objdir)\psheader.obj                                            \
	$(objdir)\special.obj $(objdir)\util.obj $(objdir)\vf.obj         \
	$(objdir)\hypertex.obj                                            \
#	$(objdir)\mime.obj                                                \
	$(objdir)\wwwfetch.obj                                            \
	$(objdir)\src-spec.obj                                            \
	$(ALLOCA)                                                         \
	$(objdir)\t1.obj $(objdir)\dvips.obj $(objdir)\tfmload.obj

program = $(objdir)\windvi.exe
programs = $(program)

manfiles = xdvi.1

default all:: $(objdir) $(programs)

$(program): $(objects) $(objdir)\windvi.res $(kpathsealib) $(zliblib) $(bzip2lib) $(gsw32lib)
	$(link_gui) $(**) $(guilibs) shell32.lib

$(objdir)\windvi.res: ..\windvi\windvi.rc
	$(RC) $(rcvars) $(rcflags) /fo$@ windvi.rc

xdvi.1: xdvi.man sedscript
	sed -f sedscript <$(srcdir)/xdvi.man >$@

sedscript: mksedscript.perl
	$(perl) $(srcdir)/mksedscript.perl kpathsea ps grid ghost pkpath sizes vfpath figpath \
	headerpath selfile $(DEFS) >$@

!include <msvc/subdirs.mak>

!include <msvc/config.mak>

installdirs = $(docdir)\windvi\examples

!include <msvc/install.mak>

# install: install-exec install-data
install:: install-exec install-data install-man

install-data::
	@echo $(verbose) & ( \
		for /f %i in (samples.lst) do \
		$(copy) .\Examples\%i $(docdir)\windvi\examples $(redir_stdout) \
	)
!include <msvc/clean.mak>

distclean::
	-@$(del) psheader.c sedscript xdvi.1 # gsftopk.1

mfmodes.h: $(win32perldir)/modes-to-cfg.pl
	$(perl) $(win32perldir)/modes-to-cfg.pl

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
