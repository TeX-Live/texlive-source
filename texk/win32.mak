################################################################################
#
# Makefile  : Top-Level Makefile for programs using kpathsea 
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/09 22:05:17 popineau>
#
################################################################################
root_srcdir=..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

# Package subdirectories, the library, and all subdirectories.
# Make the library before the programs.
subdirs= kpathsea	\
	 contrib	\
	 web2c		\
	 afm2pl		\
	 bibtex8	\
	 chktex		\
	 cjkutils	\
	 devnag         \
	 detex		\
	 dtl		\
	 dvi2tty	\
	 dvidvi		\
	 dviljk 	\
	 dvipos 	\
	 dvipdfm	\
	 dvipdfmx	\
	 dvipsk		\
	 gsftopk	\
	 lacheck	\
	 makeindexk 	\
	 musixflx	\
	 odvipsk	\
	 owindvi	\
	 ps2pkm		\
	 seetexk	\
	 tetex		\
	 tex4htk	\
	 texlive	\
	 ttf2pt1	\
	 ttf2pk		\
	 ttfdump	\
	 windvi
# tth oxdvik xdvik

# Not everything from common.mk is relevant to this top-level
# Makefile, but most of it is, and it doesn't seem worth separating the
# compilation-specific stuff.
!include <msvc/common.mak>

!include <msvc/subdirs.mak>

# Targets that only apply to web2c.
triptrap trip trap mptrap \
formats fmts bases mems \
install-formats install-fmts install-bases install-mems \
install-links c-sources: do-kpathsea
	-@echo off & echo Entering web2c for $@
	-@pushd web2c & $(make) $@ & popd

!include <msvc/config.mak>
!include <msvc/clean.mak>

# Local Variables:
# mode: Makefile
# End:
