## Makefile.am for the TeX Live subdirectory texk/web2c/otps/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
INCLUDES = -I$(top_builddir)/.. -I$(top_srcdir) $(KPATHSEA_INCLUDES)
AM_YFLAGS = -d -v

proglib = ../lib/lib.a

## Automake automatically uses $(proglib)=../lib/lib.a as dependency.
## This does not work for the configure substitution  $(KPATHSEA_LIBS),
## thus we let $(proglib) artificially depend on $(KPATHSEA_LIBS).

## Rebuild $(proglib)
$(proglib): $(KPATHSEA_DEPEND) ${top_srcdir}/lib/*.c
	cd ../lib && $(MAKE) $(AM_MAKEFLAGS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

otps_programs = otp2ocp outocp
otps_scripts = mkocp mkofm

noinst_HEADERS = otp.h routines.h yystype.h

if OTANGLE
bin_PROGRAMS = $(otps_programs)
bin_SCRIPTS = $(otps_scripts)
endif OTANGLE
EXTRA_PROGRAMS = $(otps_programs)

EXTRA_DIST = $(otps_scripts)

otp2ocp_SOURCES = \
	otp-parser.y \
	otp-lexer.l \
	otp2ocp.c \
	routines.c

otp2ocp_LDADD = $(LEXLIB) $(proglib) $(KPATHSEA_LIBS)

DISTCLEANFILES = otp-parser.output

outocp_SOURCES = \
	outocp.c

outocp_LDADD = $(proglib) $(KPATHSEA_LIBS)

## Not used
##
EXTRA_DIST += bugs.txt

