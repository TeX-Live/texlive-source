## $Id$
## Makefile.am for the TeX Live subdirectory texk/web2c/omegafonts/
##
## Copyright 2015-2021 Karl Berry <tex-live@tug.org>
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS =  -I$(top_builddir)/.. -I$(top_srcdir) $(KPATHSEA_INCLUDES) -DNOT_WEB2C
AM_CFLAGS = $(WARNING_CFLAGS)
AM_YFLAGS = -d -v

proglib = ../lib/lib.a

# Rebuild $(proglib)
$(proglib): ${top_srcdir}/lib/*.c
	cd ../lib && $(MAKE) $(AM_MAKEFLAGS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

bin_PROGRAMS =
if OTANGLE
bin_PROGRAMS += omfonts
if WIN32
noinst_PROGRAMS = call_omfonts
endif WIN32
endif OTANGLE
EXTRA_PROGRAMS = omfonts

omfonts_SOURCES = \
	char_routines.c \
	char_routines.h \
	dimen_routines.c \
	dimen_routines.h \
	dvi.h \
	error_routines.c \
	error_routines.h \
	extra_routines.c \
	extra_routines.h \
	font_routines.c \
	font_routines.h \
	header_routines.c \
	header_routines.h \
	ligkern_routines.c \
	ligkern_routines.h \
	list_routines.c \
	list_routines.h \
	manifests.h \
	omfonts.c \
	omfonts.h \
	out_ofm.c \
	out_ofm.h \
	out_routines.c \
	out_routines.h \
	param_routines.c \
	param_routines.h \
	parse_ofm.c \
	parse_ofm.h \
	parser.h \
	pl-parser.y \
	pl-lexer.l \
	print_routines.c \
	print_routines.h

LDADD = $(proglib) $(KPATHSEA_LIBS)
omfonts_DEPENDENCIES = $(proglib) $(KPATHSEA_DEPEND)

call_omfonts_CPPFLAGS = -DEXEPROG=\"omfonts.exe\"
nodist_call_omfonts_SOURCES = ../callexe.c
call_omfonts_LDADD =

DISTCLEANFILES = pl-parser.output

linked_programs = ofm2opl opl2ofm ovf2ovp ovp2ovf

bin_links = \
	omfonts$(EXEEXT):ofm2opl \
	omfonts$(EXEEXT):opl2ofm \
	omfonts$(EXEEXT):ovf2ovp \
	omfonts$(EXEEXT):ovp2ovf

include $(top_srcdir)/../../am/bin_links.am

if OTANGLE
install-exec-hook: install-bin-links
if WIN32
# same install happens in dvipdfmx.
	for f in $(linked_programs); do \
	  $(INSTALL_PROGRAM_ENV) $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=install $(INSTALL_PROGRAM) call_omfonts$(EXEEXT) "$(DESTDIR)$(bindir)/$$f$(EXEEXT)"
	done
endif WIN32
uninstall-hook: uninstall-bin-links
if WIN32
	for f in $(linked_programs); do \
	  rm -f $(DESTDIR)$(bindir)/$$f$(EXEEXT); \
	done
endif WIN32
endif OTANGLE

## Tests
##
OMFONTS_tests = bad.test charwd.test check.test help.test level1.test ligkern.test \
	ofonts.test omfonts.test overbmp.test realnum.test repeat.test \
	selectfont.test shorten.test specialhex.test version.test yannis.test
if OTANGLE
TESTS = $(OMFONTS_tests)
endif OTANGLE
bad.log charwd.log check.log help.log level1.log ligkern.log \
	ofonts.log overbmp.log realnum.log repeat.log \
	selectfont.log specialhex.log version.log: omfonts$(EXEEXT)
omfonts.log: omfonts$(EXEEXT) ../pltotf$(EXEEXT) ../tftopl$(EXEEXT)
shorten.log yannis.log: omfonts$(EXEEXT) ../pltotf$(EXEEXT)
../pltotf$(EXEEXT):
	cd .. && $(MAKE) $(AM_MAKEFLAGS) pltotf$(EXEEXT)
../tftopl$(EXEEXT):
	cd .. && $(MAKE) $(AM_MAKEFLAGS) tftopl$(EXEEXT)

EXTRA_DIST = $(OMFONTS_tests)

## bad.test
EXTRA_DIST += \
	tests/badofm.ofm tests/badopl.opl \
	tests/badovf.ofm tests/badovf.ovf tests/badovp.ovp
DISTCLEANFILES += bad*.*
## charwd.test
EXTRA_DIST += tests/charwd-r.pl tests/charwd-v.vpl
DISTCLEANFILES += tests/charwdr.* tests/charwdv.*
## check.test
EXTRA_DIST += tests/check.ofm tests/check.opl tests/checked.opl
DISTCLEANFILES += tests/xcheck*
## level1.test
EXTRA_DIST += tests/level1.opl
DISTCLEANFILES += tests/xlevel1.*
## ligkern.test
EXTRA_DIST += \
	tests/ligall.opl tests/ligbch.opl tests/ligbdy.opl \
	tests/ligblb.opl tests/ligblv.opl tests/ligblv.ovp \
	tests/ligloop1.ofm tests/ligloop1.opl tests/ligloop2.opl \
	tests/liguse.opl tests/liguse1.opl tests/liguse2.opl
DISTCLEANFILES += tests/xlig*.*
## ofonts.test
EXTRA_DIST += tests/ofontr0.pl \
	tests/ofontr1.pl tests/ofontr1.vpl \
	tests/ofontr2.opl tests/ofontr2.ovp \
	tests/ofontr3.opl tests/ofontr3.ovp \
	tests/ofontv4.pl tests/ofontv4.vpl \
	tests/ofontv5.opl tests/ofontv5.ovp \
	tests/ofontv6.opl tests/ofontv6.ovp
DISTCLEANFILES += tests/xofont*
## omfonts.test
EXTRA_DIST += tests/Cherokee.pl tests/OCherokee.ovp
DISTCLEANFILES += tests/Cherokee.tfm tests/OCherokee.ofm \
	tests/OCherokee.opl tests/OCherokee.ovf \
	tests/xCherokee.* tests/xOCherokee.*
## overbmp.test
EXTRA_DIST += tests/inbmp.opl tests/overbmp.opl tests/overbmp.ovp
DISTCLEANFILES += tests/xinbmp* tests/xoverbmp*
## realnum.test
EXTRA_DIST += tests/realnum.opl tests/realnum.out
DISTCLEANFILES += tests/realnum.*
## repeat.test
EXTRA_DIST += tests/repeat.opl tests/repeated.opl
DISTCLEANFILES += tests/xrepeated.*
## selectfont.test
EXTRA_DIST += tests/sample.out tests/sample.ovp tests/sample0-h.opl
DISTCLEANFILES += tests/sample*.ofm tests/sample*.ovf tests/sample1-h.opl tests/xsample*.out
## shorten.test
EXTRA_DIST += tests/shorten.opl
DISTCLEANFILES += tests/shortend.*
## specialhex.test
EXTRA_DIST += tests/specialhex.ovp
DISTCLEANFILES += tests/specialhex.ofm tests/specialhex.opl \
	tests/specialhex.ovf tests/xspecialhex.*
## yannis.test
EXTRA_DIST += tests/arabicr.pl tests/arabic.ovp
DISTCLEANFILES += tests/*yarabic*

