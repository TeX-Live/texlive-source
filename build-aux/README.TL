$Id$
Copyright 2016      Karl Berry <tex-live@tug.org>
Copyright 2009-2013 Peter Breitenlohner <tex-live@tug.org>
You may freely use, modify and/or distribute this file.

This directory is the central repository for auxiliary files, mostly
shell scripts, needed to build the TeX Live (TL) tree.  They are used by
all packages included in the TL tree, whether or not TL is the actual
owner/maintainer of the package

The files fall into three sets:

0) relpath is locally written.

1) Those in GNU Gnulib (http://savannah.gnu.org/projects/gnulib).  The
script Master/tlpkg/bin/tl-update-auto attempts to keep these up to date
nightly (except when the TL sources are frozen).

2) Those not in Gnulib, that must come directly from an automake or
libtool installation.  These must be manually updated when new versions
of those GNU packages are installed.  The Master/tlpkg/dev/srclist.txt
file checks for these relative to installation on the local machine, but
updates are not automatic.

There should be no other files in this directory.

We generally try to use the latest official release, as it is available
directly from GNU, rather than any distro or other patched version.
