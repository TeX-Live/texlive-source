## Makefile.am for the TeX Live subdirectory texk/web2c/otps/win32/
##
## Copyright (C) 2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CFLAGS = $(WARNING_CFLAGS)

# Build WIN32 replacements for Unix shell scripts.
bin_PROGRAMS = mkocp mkofm

mkocp_CPPFLAGS = $(KPATHSEA_INCLUDES)

mkocp_LDADD = $(KPATHSEA_LIBS)
mkocp_DEPENDENCIES = $(KPATHSEA_DEPEND)

## Rebuild libkpathsea
@KPATHSEA_RULE@

EXTRA_DIST = ChangeLog

