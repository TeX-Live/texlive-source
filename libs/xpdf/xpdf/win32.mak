################################################################################
#
# Makefile  : XPdf/XPdf subdirectory
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/11/13 15:37:26 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif


USE_XPDF = 1
MAKE_XPDF = 1
USE_ZLIB = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

goodir = ..\goo
goolib = $(goodir)\$(objdir)\libGoo.lib

DEFS = -I. -I.. $(DEFS) -I$(goodir)	              \
	-DUSE_ZLIB		                      \
	-DHAVE_POPEN -DUSE_GZIP			      \
	-DA4_PAPER -DJAPANESE_SUPPORT		      \
	-DCHINESE_GB_SUPPORT -DCHINESE_CNS_SUPPORT    \
	-DOPI_SUPPORT 

!ifdef XPDF_DLL
DEFS = $(DEFS) -DMAKE_XPDF_DLL
!endif

xpdflib_objs = \
	$(objdir)\Annot.obj		\
	$(objdir)\Array.obj		\
	$(objdir)\BuiltinFont.obj	\
	$(objdir)\BuiltinFontTables.obj \
	$(objdir)\Catalog.obj		\
	$(objdir)\CMap.obj		\
	$(objdir)\CharCodeToUnicode.obj \
	$(objdir)\Decrypt.obj		\
	$(objdir)\Dict.obj 		\
	$(objdir)\Error.obj 		\
	$(objdir)\FontEncodingTables.obj	\
	$(objdir)\FontFile.obj		\
#	$(objdir)\FormWidget.obj  	\
	$(objdir)\Function.obj		\
	$(objdir)\Gfx.obj		\
	$(objdir)\GfxFont.obj		\
	$(objdir)\GfxState.obj		\
	$(objdir)\JBIG2Stream.obj	\
	$(objdir)\Lexer.obj		\
	$(objdir)\Link.obj		\
	$(objdir)\NameToCharCode.obj	\
	$(objdir)\Object.obj		\
	$(objdir)\Outline.obj		\
	$(objdir)\OutputDev.obj		\
	$(objdir)\PDFDocEncoding.obj	\
	$(objdir)\PDFDoc.obj		\
	$(objdir)\Page.obj		\
	$(objdir)\PSTokenizer.obj	\
	$(objdir)\GlobalParams.obj	\
	$(objdir)\Parser.obj		\
	$(objdir)\Stream.obj		\
	$(objdir)\UnicodeMap.obj	\
	$(objdir)\XRef.obj

pdftotext_objs =  $(objdir)\TextOutputDev.obj \
	$(objdir)\pdftotext.obj

pdftops_objs = $(objdir)\PSOutputDev.obj   \
	 $(objdir)\pdftops.obj 

pdfinfo_objs = $(objdir)\pdfinfo.obj 

pdffonts_objs = $(objdir)\pdffonts.obj 

pdfimages_objs = $(objdir)\ImageOutputDev.obj $(objdir)\pdfimages.obj 

pdftopbm_objs = $(objdir)\PBMOutputDev.obj $(objdir)\pdftopbm.obj 

programs = $(xpdfdll) $(objdir)\pdftops.exe $(objdir)\pdftotext.exe         \
	$(objdir)\pdfinfo.exe $(objdir)\pdfimages.exe                       \
	$(objdir)\pdffonts.exe						    \
#	$(objdir)\pdftopbm.exe

libfiles = $(xpdflib)

default: all

all: $(objdir) $(xpdf) $(programs)

lib: $(objdir) $(xpdflib)

!ifdef XPDF_DLL
$(xpdflib): $(objdir) $(xpdflib_objs) $(goolib) libxpdf.def
	$(archive) /DEF:libxpdf.def $(xpdflib_objs) $(goolib)

$(xpdfdll): $(xpdflib_objs) $(xpdflib) $(goolib) $(objdir)\libxpdf.res $(gnuw32lib)
	$(link_dll) $(**)  $(xpdflib:.lib=.exp) $(conlibs)

!else
$(xpdflib): $(objdir) $(xpdflib_objs) $(goolib) $(zliblib)
	$(archive) $(xpdflib_objs) $(goolib) $(zliblib)
!endif

$(goolib):
	pushd $(goodir) & $(make) lib & popd

$(objdir)\pdftops.exe: $(pdftops_objs) $(xpdflib) $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\pdftotext.exe: $(pdftotext_objs) $(xpdflib) $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\pdfinfo.exe: $(pdfinfo_objs) $(xpdflib) $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\pdffonts.exe: $(pdffonts_objs) $(xpdflib) $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\pdftopbm.exe: $(pdftopbm_objs) $(xpdflib) $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\pdfimages.exe: $(pdfimages_objs) $(xpdflib) $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\PSOutputDev.obj: PSOutputdev.cc
	$(compile) /TpPSOutputdev.cc

$(objdir)\pdftops.obj: pdftops.cc
	$(compile) /Tppdftops.cc

$(objdir)\TextOutputDev.obj: TextOutputDev.cc
	$(compile) /TpTextOutputDev.cc

$(objdir)\pdftotext.obj: pdftotext.cc
	$(compile) /Tppdftotext.cc

..\aconf.h: ..\aconf.h.in
	$(perl) $(win32perldir)\conf-cauto.pl ..\aconf.h.in ..\aconf.h

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec # install-lib install-include

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>

!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
