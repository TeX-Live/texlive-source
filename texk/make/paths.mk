# paths.mk -- installation directories.
# $Id$

# Do not change prefix and exec_prefix in Makefile.in!
# configure doesn't propagate the change to the other Makefiles.
# Instead, give the -prefix/-exec-prefix options to configure.
# (See kpathsea/INSTALL for more details.) This is arguably
# a bug, but it's not likely to change soon.
prefix = @prefix@
exec_prefix = @exec_prefix@

# Architecture-dependent executables.
bindir = @bindir@

# Architecture-independent executables.
scriptdir = $(bindir)

# Architecture-dependent files, such as lib*.a files.
libdir = @libdir@

# Architecture-independent files.
datarootdir = @datarootdir@
datadir = @datadir@

# Header files.
includedir = @includedir@

# GNU .info* files.
infodir = @infodir@

# Unix man pages.
manext = 1
mandir = @mandir@
man1dir = $(mandir)/man$(manext)
man5dir = $(mandir)/man5

# modifiable single-machine data.
localstatedir = @localstatedir@

# read-only single-machine data (configuration).
sysconfdir = @sysconfdir@

# executables that other programs run.
libexecdir = @libexecdir@


# TeX system-specific directories. Not all of the following are relevant
# for all programs, but it seems cleaner to collect everything in one place.

# The default paths are in kpathsea/texmf.cnf. Passing all the paths to
# sub-makes can make the arg list too long.  If you make
# changes below, you will have to make the corresponding changes to
# texmf.cnf yourself.

# The root of the main tree.
texmf = @texmfmain@

# The directory used by varfonts.
vartexfonts = /var/tmp/texfonts

# Regular input files.
texinputdir = $(texmf)/tex
mfinputdir = $(texmf)/metafont
mpinputdir = $(texmf)/metapost
mftinputdir = $(texmf)/mft

# dvips's epsf.tex, rotate.tex, etc. get installed here;
# ditto for dvilj's fonts support.
dvips_plain_macrodir = $(texinputdir)/plain/dvips
dvilj_latex2e_macrodir = $(texinputdir)/latex/dvilj

# mktex.cnf, texmf.cnf, etc.
web2cdir = $(texmf)/web2c

# The top-level font directory.
fontdir = $(texmf)/fonts

# Memory dumps (.fmt/.base/.mem).
fmtdir = $(web2cdir)/tex
basedir = $(web2cdir)/metafont
memdir = $(web2cdir)/metapost

# Pool files.
texpooldir = $(web2cdir)
mfpooldir = $(texpooldir)
mppooldir = $(texpooldir)

# Where the .map files from fontname are installed.
fontnamedir = $(texmf)/fontname

# For dvips configuration files, psfonts.map, etc.
dvipsdir = $(texmf)/dvips

# For dvips .pro files, gsftopk's render.ps, etc.
psheaderdir = $(dvipsdir)

# If a font can't be found close enough to its stated size, we look for
# each of these sizes in the order given.  This colon-separated list is
# overridden by the envvar TEXSIZES, and by a program-specific variable
# (e.g., XDVISIZES), and perhaps by a config file (e.g., in dvips).
# This list must be sorted in ascending order.
default_texsizes = 600

# End of paths.mk.
