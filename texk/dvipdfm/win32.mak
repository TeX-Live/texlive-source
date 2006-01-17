################################################################################
#
# Makefile  : Dvipdfm
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/02/16 17:48:30 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

version = 0.13.2c

USE_GNUW32 = 1
USE_KPATHSEA = 1
USE_PNG = 1
USE_ZLIB = 1

!include <msvc/common.mak>

DEFS = $(DEFS) -DHAVE_CONFIG_H

dvipdfmdir=$(texmf)\dvipdfm

programs=$(objdir)\dvipdfm.exe $(objdir)\ebb.exe

headers = dvi.h pdflimits.h error.h mfileio.h jpeg.h mem.h numbers.h   \
	pdfdev.h pdfdoc.h pdfparse.h pdfspecial.h tfm.h type1.h epdf.h \
	vf.h dvicodes.h t1crypt.h pkfont.h tpic.h thumbnail.h htex.h \
	twiddle.h mpost.h psimage.h psspecial.h colors.h colorsp.h \
	dvipdfm.h encodings.h ttf.h $(TTF_OPT) macglyphs.h

src = pdfdev.c pdfdoc.c dvi.c mfileio.c mem.c jpeg.c numbers.c pdfobj.c \
	pdfparse.c pdfspecial.c tfm.c type1.c dvipdfm.c epdf.c vf.c     \
	t1crypt.c pkfont.c tpic.c thumbnail.c pngimage.c ebb.c htex.c   \
	mpost.c psimage.c psspecial.c colorsp.c encodings.c $(TTF_OPT) ttf.c

objects = $(objdir)\pdfdev.obj $(objdir)\pdfdoc.obj $(objdir)\dvi.obj      \
	$(objdir)\mfileio.obj $(objdir)\mem.obj $(objdir)\jpeg.obj         \
	$(objdir)\numbers.obj $(objdir)\pdfobj.obj $(objdir)\pdfparse.obj  \
	$(objdir)\pdfspecial.obj $(objdir)\tfm.obj $(objdir)\type1.obj     \
	$(objdir)\dvipdfm.obj $(objdir)\epdf.obj $(objdir)\vf.obj 	   \
	$(objdir)\t1crypt.obj $(objdir)\pkfont.obj $(objdir)\tpic.obj      \
	$(objdir)\thumbnail.obj $(objdir)\pngimage.obj $(objdir)\htex.obj  \
	$(objdir)\mpost.obj $(objdir)\psimage.obj $(objdir)\psspecial.obj  \
	$(objdir)\colorsp.obj $(objdir)\encodings.obj $(TTF_OPT)           \
	$(objdir)\ttf.obj
manfiles = dvipdfm.1
eobjects=$(objdir)\ebb.obj $(objdir)\jpeg.obj $(objdir)\pdfobj.obj         \
	$(objdir)\mem.obj $(objdir)\mfileio.obj $(objdir)\numbers.obj      \
	$(objdir)\pdfparse.obj $(objdir)\pngimage.obj
encodings=ot1.enc ot1alt.enc ot1r.enc
mapfiles=cmr.map psbase14.map lw35urw.map lw35urwa.map

default: all

all: $(objdir) $(programs)

$(objdir)\dvipdfm.exe: $(objects) $(kpathsealib) $(zliblib) $(pnglib)
	$(link) $(**) $(conlibs)

$(objdir)\ebb.exe: $(eobjects) $(kpathsealib) $(zliblib) $(pnglib)
	$(link) $(**) $(conlibs)

!include <msvc/config.mak>

acconfig.h: acconfig.h.in
	$(perl) -I$(win32perldir) $(win32perldir)/conf-cauto.pl acconfig.h.in

installdirs = $(dvipdfmdir)\base $(dvipdfmdir)\config $(docdir)\dvipdfm $(prefix)\setupw32

!include <msvc/install.mak>

install:: install-exec install-data install-doc

install-data::
#	-@echo $(verbose) & ( \
#		$(copy) .\t1fonts.map $(dvipdfmdir)\config\t1fonts.map $(redir_stdout) \
#		& $(copy) .\config.dvipdfm $(dvipdfmdir)\config\config $(redir_stdout) \
#		& $(copy) .\config.dvipdfm $(prefix)\setupw32 $(redir_stdout) \
#		& ( for %%f in ($(encodings)) do $(copy) %%f $(dvipdfmdir)\base $(redir_stdout) ) \
#		& ( for %%f in ($(mapfiles)) do $(copy) %%f $(dvipdfmdir)\config $(redir_stdout) ) \
#	)

install-doc:: $(programs)
	-@echo $(verbose) & ( \
	pushd doc \
	& tex dvipdfm $(redir_stdout) \
	& tex dvipdfm $(redir_stdout) \
	& ..\$(objdir)\dvipdfm dvipdfm $(redir_stdout) \
	& $(copydir) /d * $(docdir)\dvipdfm $(redir_stdout) \
	& $(del) *.log dvipdfm.dvi dvipdfm.pdf *.aux $(redir_stderr) \
	& popd \
	)

distclean::
#	@$(copy) config.dvipdfm config-dvipdfm $(redir_stdout)

!include <msvc/clean.mak>

distclean::
#	@$(copy) config-dvipdfm config.dvipdfm $(redir_stdout)
	-@$(del) config-dvipdfm $(redir_stderr)

!include <msvc/rdepend.mak>
!include "./depend.mak"

#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
