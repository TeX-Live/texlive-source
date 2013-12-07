## Makefile.am for the TeX Live subdirectory texk/web2c/omegafonts/
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
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
if OMFONTS
bin_PROGRAMS += omfonts
if WIN32
noinst_PROGRAMS = call_omfonts
endif WIN32
endif OMFONTS
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
if OMFONTS
install-exec-hook: install-bin-links
if WIN32
	for f in $(linked_programs); do \
	  $(INSTALL_PROGRAM) call_omfonts$(EXEEXT) $(DESTDIR)$(bindir)/$$f$(EXEEXT) || exit 1; \
	done
endif WIN32
uninstall-hook: uninstall-bin-links
if WIN32
	for f in $(linked_programs); do \
	  rm -f $(DESTDIR)$(bindir)/$$f$(EXEEXT); \
	done
endif WIN32
endif OMFONTS
endif OTANGLE

## Tests
##
OMFONTS_tests = charwd.test level1.test omfonts.test realnum.test \
	repeat.test selectfont.test specialhex.test
if OTANGLE
if OMFONTS
TESTS = $(OMFONTS_tests)
endif OMFONTS
endif OTANGLE

EXTRA_DIST = $(OMFONTS_tests)

## charwd.test
EXTRA_DIST += tests/charwd-r.pl tests/charwd-v.vpl
DISTCLEANFILES += tests/charwdr.* tests/charwdv.*
## level1.test
EXTRA_DIST += tests/level1.opl
DISTCLEANFILES += tests/xlevel1.ofm tests/xlevel1.opl
## omfonts.test
EXTRA_DIST += tests/Cherokee.pl tests/OCherokee.ovp
DISTCLEANFILES += tests/Cherokee.tfm tests/OCherokee.ofm \
	tests/OCherokee.opl tests/OCherokee.ovf \
	tests/xCherokee.* tests/xOCherokee.*
## realnum.test
EXTRA_DIST += tests/realnum.opl tests/realnum.out
DISTCLEANFILES += tests/realnum.*
## repeat.test
EXTRA_DIST += tests/repeat.opl tests/repeated.opl
DISTCLEANFILES += tests/xrepeated.*
## selectfont.test
EXTRA_DIST += tests/sample.out tests/sample.ovp tests/sample0-h.opl
DISTCLEANFILES += tests/sample*.ofm tests/sample*.ovf tests/sample1-h.opl tests/xsample*.out
## specialhex.test
EXTRA_DIST += tests/specialhex.ovp
DISTCLEANFILES += tests/specialhex.ofm tests/specialhex.opl \
	tests/specialhex.ovf tests/xspecialhex.*

