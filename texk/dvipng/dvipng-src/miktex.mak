## miktex.mak: dvipng
##
## Copyright (C) 2004 Christian Schenk <cs@miktex.org>
## 
## This file is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation; either version 2, or (at your
## option) any later version.
## 
## This file is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this file; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
## 02110-1301 USA.

miktex_cc_no_warnings = 1
miktex_cc_disable_optimization = 1

!include ..\miktex.inc

objects = \
	$(outdir)\color.obj \
	$(outdir)\draw.obj \
	$(outdir)\dvi.obj \
	$(outdir)\dvipng.obj \
	$(outdir)\font.obj \
	$(outdir)\misc.obj \
	$(outdir)\papersiz.obj \
	$(outdir)\pk.obj \
	$(outdir)\ppagelist.obj \
	$(outdir)\set.obj \
	$(outdir)\special.obj \
	$(outdir)\vf.obj \
	$(wrapper_obj) \

sources = \
	color.c \
	draw.c \
	dvi.c \
	dvipng.c \
	font.c \
	misc.c \
	papersiz.c \
	pk.c \
	ppagelist.c \
	set.c \
	special.c \
	vf.c \

extra_cdefines = \
	-Dalloca=_alloca \
	-DUSE_MIKTEX_EXIT \

extra_cincludes = \
	-I$(gdlibdir) \
	-I$(kpslibdir) \

.c{$(outdir)\}.obj:
	$(cc) $(cstandard) \
		$(extra_cdefines) \
		$(extra_cincludes) \
		$(ccopt_output_directory)$(outdir)\ $<

binaries = $(outdir)\dvipng.exe

all: common-all $(binaries)

install: common-install install-binaries

qrt: common-qrt

# -----------------------------------------------------------------------------
# dvipng
# -----------------------------------------------------------------------------

libs = \
	$(miktex_lib) \
	$(gd_lib) \
	$(kps_lib) \
	$(gnu_lib) \
	$(texmf_lib) \

$(outdir)\dvipng.exe: \
			$(outdir) \
			$(objects) \
			$(libs) \
			$(outdir)\dvipng.res \

	$(link) $(lstandard) \
		-out:$@ \
		$(objects) \
		$(libs) \
		$(outdir)\dvipng.res \

$(outdir)\dvipng.res: \
		$(libdir)\miktex-version.h \
		$(outdir) \
		dvipng-version.h \
		dvipng.rc \

	$(rc) $(rcstandard) $(rcopt_output_file)$@ dvipng.rc

# -----------------------------------------------------------------------------
# clean-up
# -----------------------------------------------------------------------------

mostlyclean: common-mostlyclean

clean: common-clean mostlyclean

distclean: common-distclean clean

realclean: common-realclean distclean

# -----------------------------------------------------------------------------
# dependencies
# -----------------------------------------------------------------------------

!include ..\common-dependencies.inc

depend: $(sources)
	$(MAKEDEPEND1) $(extra_cincludes) $(extra_cdefines) $(sources)

# DO NOT DELETE

$(outdir)\color.obj: dvipng.h config.h .\inttypes.h $(gdlibdir)\gd.h
$(outdir)\color.obj: $(gdlibdir)\gd_io.h $(gdlibdir)\gdfx.h
$(outdir)\color.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\color.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\color.obj: ..\lib\miktex-features.h commands.h
$(outdir)\draw.obj: dvipng.h config.h .\inttypes.h $(gdlibdir)\gd.h
$(outdir)\draw.obj: $(gdlibdir)\gd_io.h $(gdlibdir)\gdfx.h
$(outdir)\draw.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\draw.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\draw.obj: ..\lib\miktex-features.h commands.h
$(outdir)\dvi.obj: dvipng.h config.h .\inttypes.h $(gdlibdir)\gd.h
$(outdir)\dvi.obj: $(gdlibdir)\gd_io.h $(gdlibdir)\gdfx.h
$(outdir)\dvi.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\dvi.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\dvi.obj: ..\lib\miktex-features.h commands.h ..\libgnu\gnu-miktex.h
$(outdir)\dvipng.obj: dvipng.h config.h .\inttypes.h
$(outdir)\dvipng.obj: $(gdlibdir)\gd.h $(gdlibdir)\gd_io.h
$(outdir)\dvipng.obj: $(gdlibdir)\gdfx.h
$(outdir)\dvipng.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\dvipng.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\dvipng.obj: ..\lib\miktex-features.h commands.h
$(outdir)\font.obj: dvipng.h config.h .\inttypes.h $(gdlibdir)\gd.h
$(outdir)\font.obj: $(gdlibdir)\gd_io.h $(gdlibdir)\gdfx.h
$(outdir)\font.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\font.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\font.obj: ..\lib\miktex-features.h commands.h
$(outdir)\misc.obj: dvipng.h config.h .\inttypes.h $(gdlibdir)\gd.h
$(outdir)\misc.obj: $(gdlibdir)\gd_io.h $(gdlibdir)\gdfx.h
$(outdir)\misc.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\misc.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\misc.obj: ..\lib\miktex-features.h commands.h
$(outdir)\misc.obj: ..\libgnu\gnu-miktex.h
$(outdir)\papersiz.obj: dvipng.h config.h .\inttypes.h
$(outdir)\papersiz.obj: $(gdlibdir)\gd.h
$(outdir)\papersiz.obj: $(gdlibdir)\gd_io.h
$(outdir)\papersiz.obj: $(gdlibdir)\gdfx.h
$(outdir)\papersiz.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\papersiz.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\papersiz.obj: ..\lib\miktex-features.h commands.h
$(outdir)\pk.obj: dvipng.h config.h .\inttypes.h $(gdlibdir)\gd.h
$(outdir)\pk.obj: $(gdlibdir)\gd_io.h $(gdlibdir)\gdfx.h
$(outdir)\pk.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\pk.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\pk.obj: ..\lib\miktex-features.h commands.h
$(outdir)\ppagelist.obj: dvipng.h config.h .\inttypes.h
$(outdir)\ppagelist.obj: $(gdlibdir)\gd.h
$(outdir)\ppagelist.obj: $(gdlibdir)\gd_io.h
$(outdir)\ppagelist.obj: $(gdlibdir)\gdfx.h
$(outdir)\ppagelist.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\ppagelist.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\ppagelist.obj: ..\lib\miktex-features.h commands.h
$(outdir)\set.obj: dvipng.h config.h .\inttypes.h $(gdlibdir)\gd.h
$(outdir)\set.obj: $(gdlibdir)\gd_io.h $(gdlibdir)\gdfx.h
$(outdir)\set.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\set.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\set.obj: ..\lib\miktex-features.h commands.h
$(outdir)\special.obj: dvipng.h config.h .\inttypes.h
$(outdir)\special.obj: $(gdlibdir)\gd.h $(gdlibdir)\gd_io.h
$(outdir)\special.obj: $(gdlibdir)\gdfx.h
$(outdir)\special.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\special.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\special.obj: ..\lib\miktex-features.h commands.h
$(outdir)\vf.obj: dvipng.h config.h .\inttypes.h $(gdlibdir)\gd.h
$(outdir)\vf.obj: $(gdlibdir)\gd_io.h $(gdlibdir)\gdfx.h
$(outdir)\vf.obj: $(kpslibdir)\kpathsea\kpathsea.h
$(outdir)\vf.obj: $(kpslibdir)\web2c-miktex.h ..\lib\miktex.h
$(outdir)\vf.obj: ..\lib\miktex-features.h commands.h
