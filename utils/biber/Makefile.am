## See ./README for our basic strategy here.
##
##   Copyright 2011 Karl Berry <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
ACLOCAL_AMFLAGS = -I ../../m4 -I m4

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

nodist_bin_SCRIPTS = $(BIBER_BINARY)

# this EXTRA_ variable must be defined because nodist_bin_SCRIPTS
# refers to a configure-substituted value.
EXTRA_SCRIPTS =

EXTRA_DIST = TeXLive bin

