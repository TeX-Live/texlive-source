################################################################################
#
# Makefile  : paths, paths definitions for web2c/win32
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/08 10:11:56 popineau>
#
################################################################################
#
# The compile-time paths are defined in kpathsea/paths.h, which is built
# from kpathsea/texmf.in and these definitions.  See kpathsea/INSTALL
# for how the various path-related files are used and created.

# Do not change prefix and exec_prefix in Makefile.in!
# configure doesn't propagate the change to the other Makefiles.
# Instead, give the -prefix/-exec-prefix options to configure.
# (See kpathsea/INSTALL for more details.) This is arguably
# a bug, but it's not likely to change soon.
!ifndef prefix
prefix = c:\Program Files\TeXLive
!endif
exec_prefix = $(prefix)

arch = win32

# !ifdef DEVELOPMENT
# arch = $(arch)-development
# !endif

!ifdef STATIC
arch = $(arch)-static
!endif

# Architecture-dependent executables.
bindir = $(exec_prefix)\bin\$(arch)

# Architecture-independent executables.
scriptdir = $(bindir)

# Architecture-dependent files, such as lib*.a files.
libdir = $(exec_prefix)\lib\$(arch)

# Architecture-independent files.
datadir = $(prefix)

# Header files.
includedir = $(prefix)\include\$(arch)

# GNU .info* files.
infodir = $(texmf)\doc\info

# Unix man pages.
manext = 1
mandir = $(texmf)\doc\man
man1dir = $(texmf)\doc\man\man$(manext)
man3dir = $(texmf)\doc\man\man3
man5dir = $(texmf)\doc\man\man5

# TeX system-specific directories. Not all of the following are relevant
# for all programs, but it seems cleaner to collect everything in one place.

# The default paths are now in kpathsea/texmf.in. Passing all the
# paths to sub-makes can make the arg list too long on system V.
# Note that if you make changes below, you will have to make the
# corresponding changes to texmf.in or texmf.cnf yourself.

# The root of the main tree.
texmf = $(datadir)\texmf
texmfdist = $(datadir)\texmf-dist

# !ifdef DEVELOPMENT
# texmf = $(texmf)-development
# !endif

# The directory used by varfonts.
vartexfonts = $$(TEMP)/texfonts

# Regular input files.
texinputdir = $(texmf)\tex
mfinputdir = $(texmf)\metafont
mpinputdir = $(texmf)\metapost
mftinputdir = $(texmf)\mft

# dvips's epsf.tex, rotate.tex, etc. get installed here;
# ditto for dvilj's fonts support.
dvips_plain_macrodir = $(texinputdir)\plain\dvips
dvilj_latex2e_macrodir = $(texinputdir)\latex\dvilj

# mktex.cnf, texmf.cnf, etc.
web2cdir = $(texmf)\web2c

# The top-level font directory.
fontdir = $(texmf)\fonts

# Memory dumps (.fmt/.base/.mem).
fmtdir = $(web2cdir)
basedir = $(fmtdir)
memdir = $(fmtdir)

# Pool files.
texpooldir = $(web2cdir)
mfpooldir = $(texpooldir)
mppooldir = $(texpooldir)

# Where the .map files from fontname are installed.
fontnamedir = $(texmf)\fontname

# For dvips configuration files, psfonts.map, etc.
dvipsdir = $(texmf)\dvips

# For dvips .pro files, gsftopk's render.ps, etc.
psheaderdir = $(dvipsdir)

# If a font can't be found close enough to its stated size, we look for
# each of these sizes in the order given.  This colon-separated list is
# overridden by the envvar TEXSIZES, and by a program-specific variable
# (e.g., XDVISIZES), and perhaps by a config file (e.g., in dvips).
# This list must be sorted in ascending order.
default_texsizes = 300:600

# End of paths.mak.
#
# Local Variables:
# mode: Makefile
# End: