## miktex.mak: dvipos
## Copyright (C) 2003 Jin-Hwan Cho <chofchof@ktug.or.kr>
## 
## This file is part of dvipos.
## 
## dvipos is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
## 
## dvipos is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with dvipdfmx; if not, write to the Free Software Foundation,
## 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

miktex_cc_no_runtime_checks = 1
miktex_cc_no_warnings = 1

!include ..\miktex.inc

objects =	\
		$(outdir)\dvicore.obj \
		$(outdir)\dvipos.obj \
		$(outdir)\tfm.obj \
		$(outdir)\utils.obj \

sources =	\
		dvicore.c \
		dvipos.c \
		tfm.c \
		utils.c \

cc_options = $(cflags) $(cdebug) $(cvarsdll) $(cflagspackfunc) \
		 -I. -I$(libdir) -I$(gnulibdir) -I$(kpslibdir) \
		$(miktexvars)

.c{$(outdir)\}.obj:
!ifdef xxx
	$(sed) -e "s/^static \(.*\)(/\1(/" $< > _xyz.c
	$(cc) $(cc_options) -Fo$@ _xyz.c
	del _xyz.c
!else
	$(cc) $(cc_options) -Fo$(outdir)\ $<
!endif

.cpp{$(outdir)\}.obj:
	$(cc) $(cc_options) $(cflagseh) -Fo$(outdir)\ $<

binaries = $(outdir)\dvipos.exe

all: common-all $(binaries)

install: common-install install-binaries

qrt: common-qrt

# -----------------------------------------------------------------------------
# dvipos
# -----------------------------------------------------------------------------

$(outdir)\dvipos.exe: $(outdir) $(objects) \
		$(miktex_lib) $(kps_lib) $(gnu_lib)
	$(link) $(conlflags) $(ldebug) \
		-out:$@ \
		$(objects) \
		$(miktex_lib) $(kps_lib) $(gnu_lib) $(conlibsdll)

#$(outdir)\dvipos.res: $(outdir) dvipos.rc
#	$(rc) $(rcflags) $(rcvars) -I$(libdir) -Fo$@ dvipos.rc

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
	$(MAKEDEPEND1) -I..\libkps $**

