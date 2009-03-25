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

bin_PROGRAMS = otp2ocp outocp

noinst_HEADERS = otp.h routines.h yystype.h

dist_bin_SCRIPTS = mkocp mkofm

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
EXTRA_DIST = bugs.txt

## Eventually delete these files
##
EXTRA_DIST += \
	Makefile.in.orig
