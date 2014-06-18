## Makefile.am for the TeX Live subdirectory utils/texdoctk/
## (In a separate directory because we hope that some day a new
##  volunteer will appear to take over this program.)
##
## Copyright (C) 2012-2014 Karl Berry <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
EXTRA_DIST =

perl_scripts = texdoctk
scriptsdir = texmf-dist/scripts/texdoctk
nodist_bin_SCRIPTS =
#
include $(srcdir)/../../am/script_links.am
#
texmfdir = $(datarootdir)/$(scriptsdir)
dist_texmf_SCRIPTS = texdoctk.pl

install-data-hook: install-perl-links
uninstall-hook: uninstall-links

dist_man1_MANS = texdoctk.1 

CLEANFILES = $(nodist_bin_SCRIPTS)

## No tests since it's unlikely builders will have Perl Tk installed.
