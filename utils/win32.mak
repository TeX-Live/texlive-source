################################################################################
#
# Makefile  : fpTeX utils subdirectory
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/08/21 11:49:39 popineau>
#
################################################################################
root_srcdir = ..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

!include <msvc/common.mak>

# Package subdirectories, the library, and all subdirectories.
subdirs = \
	bzip2		\
	gzip		\
#	dvipng		\
	jpeg2ps 	\
	lcdf-typetools	\
	psutils		\
	t1utils		\
	texinfo		\
	vlna		\
	TeXSetup	\
	TeXLive

!include <msvc/subdirs.mak>
!include <msvc/clean.mak>

#
# Local Variables:
# mode: Makefile
# End:
# DO NOT DELETE
