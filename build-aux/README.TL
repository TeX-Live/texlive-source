Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
You may freely use, modify and/or distribute this file.

This directory is the central repository for auxiliary files, mostly
shell scripts, needed to build the TeX Live (TL) tree.  They are used by
all packages `owned' by the TL tree, i.e., maintained as part of TL.

compile, missing, test-driver, and ylwrap come from automake; ltmain.sh
comes from libtool.  Those must be manually updated when new versions of the
packages are installed.  The Master/tlpkg/dev/srclist.txt file has checks
for these, but updates are not automatic.  (We generally try to use the
latest official release, as it is available directly from GNU, not any
distro version.)

relpath is locally written.

The rest of these files are maintained elsewhere and are available from
the GNU gnulib project (http://savannah.gnu.org/projects/gnulib).  The
script Master/tlpkg/bin/tl-update-auto attempts to keep those up to date
(except when the TL sources are frozen).
