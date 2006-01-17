################################################################################
#
# Makefile  : Web2C / man
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/02/19 17:37:40 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

!include <msvc/common.mak>

localmodes = modes

# If you like, delete the man pages for programs you aren't installing.
# The programs are listed in alphabetical order.
# 
manfiles=		\
	amstex.1	\
	bibtex.1	\
	dmp.1		\
	dvicopy.1	\
	dvitype.1	\
	eplain.1	\
	etex.1		\
	gftodvi.1	\
	gftopk.1	\
	gftype.1	\
	latex.1		\
	makempx.1	\
	mf.1		\
	mft.1		\
	mpost.1		\
	mpto.1		\
	newer.1		\
	omega.1		\
	patgen.1	\
	pdftex.1	\
	pktogf.1	\
	pktype.1	\
	pltotf.1	\
	pooltype.1	\
	tangle.1	\
	tex.1		\
	tftopl.1	\
	vftovp.1	\
	vptovf.1	\
	weave.1		\
	tie.1

default all: man

tie.1: ..\tiedir\tie.1
	if not exist tie.1 @$(copy) ..\tiedir\tie.1 tie.1

cweb.1: ..\cwebdir\cweb.1
	if not exist cweb.1 @$(copy) ..\cwebdir\cweb.1 cweb.1

!include <msvc/install.mak>

!include <msvc/clean.mak>

#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
