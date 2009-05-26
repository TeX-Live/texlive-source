## Makefile.am for the TeX Live subdirectory texk/web2c/omegafonts/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
INCLUDES =  -I$(top_builddir)/.. -I$(top_srcdir) $(KPATHSEA_INCLUDES)
AM_YFLAGS = -d -v

proglib = ../lib/lib.a

# Automake automatically uses $(proglib)=../lib/lib.a as dependency.
# This does not work for the configure substitution  $(KPATHSEA_LIBS),
# thus we let $(proglib) artificially depend on $(KPATHSEA_LIBS).
#
# Rebuild $(proglib)
$(proglib): $(KPATHSEA_DEPEND) ${top_srcdir}/lib/*.c
	cd ../lib && $(MAKE) $(AM_MAKEFLAGS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

bin_PROGRAMS = omfonts

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

LDADD = $(LEXLIB) $(proglib) $(KPATHSEA_LIBS)

DISTCLEANFILES = pl-parser.output

linked_programs = ofm2opl opl2ofm ovf2ovp ovp2ovf

install-exec-hook:
	cd $(DESTDIR)$(bindir) && \
	  for f in $(linked_programs); do \
	    rm -f $$f$(EXEEXT) && \
	    $(LN_S) omfonts$(EXEEXT) $$f$(EXEEXT) || exit 1; \
	  done

unistall-hook:
	for f in $(linked_programs); do \
	  rm -f $(DESTDIR)$(bindir)/$$f$(EXEEXT); \
	done

