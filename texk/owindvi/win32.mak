################################################################################
#
# Makefile  : OWindvi
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/06/11 00:40:30 popineau>
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

c_auto_h_dir = ../windvi

!include <msvc/common.mak>

{../windvi}.c{$(objdir)}.obj:
	$(compile) $<

{../windvi}.rc.res:
	$(RC) $(rcflags) $(rcvars) -DOmega /fo$@ $<

# Compilation options.
DEFS = $(DEFS) -DKPATHSEA -DUSE_POPEN                                     \
	-DPS_GS                                                           \
	-DA4                                                              \
	-DHTEX                                                            \
	-DBOOK_MODE                                                       \
	-DTRANSFORM=1                                                     \
	-DGREY                                                            \
	-DBUTTONS				                          \
	-DDO_BANDING=1                                                    \
	-DSRC_SPECIALS							\
	-DNeedFunctionPrototypes                                        \
	-DOmega=1                                                         \
	-I../windvi \
#	-I$(WWWLIBDIR)                                                    \
	-I$(ZLIBDIR) -I$(REGEXLIBDIR)                                     \
	-I$(T1LIBDIR)                                                     \
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
	$(objdir)\winhtex.obj                                           \
	$(objdir)\winimg.obj                                              \
	$(objdir)\winprint.obj $(objdir)\winver.obj $(objdir)\wincfg.obj  \
	$(objdir)\winlog.obj $(objdir)\winopt.obj $(objdir)\winxutl.obj   \
	$(objdir)\winevt.obj $(objdir)\dvi-draw.obj                       \
	$(objdir)\dvi-init.obj $(objdir)\font-open.obj $(objdir)\gf.obj   \
	$(objdir)\pk.obj $(objdir)\psgsw32.obj                            \
#	$(objdir)\psheader.obj     \
	$(objdir)\special.obj $(objdir)\util.obj $(objdir)\vf.obj         \
	$(objdir)\hypertex.obj \
#	$(objdir)\mime.obj \
	$(objdir)\wwwfetch.obj  \
	$(objdir)\src-spec.obj                                            \
	$(ALLOCA)                                                         \
	$(objdir)\t1.obj $(objdir)\dvips.obj $(objdir)\tfmload.obj

program = $(objdir)\owindvi.exe
programs = $(program)

manfiles = oxdvi.1

default: all

all: $(objdir) $(programs)

$(program): $(objects) $(objdir)\windvi.res $(kpathsealib) $(zliblib) $(bzip2lib) $(gsw32lib)
	$(link_gui) $(**) $(guilibs) shell32.lib

$(objdir)\windvi.res: ..\windvi\windvi.rc
	$(RC) $(rcvars) $(rcflags) /fo$@ ..\windvi\windvi.rc

..\windvi\c-auto.h: ..\windvi\c-auto.in
	$(perl) $(win32perldir)\conf-cauto.pl ..\windvi\c-auto.in $@

!include <msvc/config.mak>

oxdvi.1: ../windvi/xdvi.man sedscript
	$(sed) -f sedscript <../windvi/xdvi.man >$@

sedscript: ../windvi/mksedscript.perl
	$(perl) ../windvi/mksedscript.perl kpathsea ps grid ghost pkpath sizes vfpath figpath \
	headerpath $(defs) >$@

!include <msvc/install.mak>

install:: install-exec install-man

!include <msvc/clean.mak>

extraclean::

distclean::
	-@$(del) psheader.c sedscript xdvi.1 # gsftopk.1

depend:	
#	pushd ../dvipsk & $(MAKE) -$(MAKEFLAGS) $@ & popd
# FIXME: this does not work !
	$(sed) -e "s@\([ 	]\|\./\)\([A-z0-9_-][A-z0-9_-]*\)\.\([ch]\)\>@\1../windvi/\2.\3@g" \
		< ../windvi/depend.mak > ./depend.mak

!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: