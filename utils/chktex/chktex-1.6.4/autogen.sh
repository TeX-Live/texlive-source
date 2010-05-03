#!/bin/sh
#
# autogen.sh glue
#
# Requires: automake, autoconf
set -e

echo "Update aclocal"
aclocal -I m4

echo "Update autoheader"
( [ `which autoheader2.50` ] && autoheader2.50 ) || ( [ `which autoheader` ] && autoheader )

echo "Update automake"
set +e
automake --foreign --add-missing 2>/dev/null
set -e

echo "Update autoconf"
( [ `which autoconf2.50` ] && autoconf2.50 ) || ( [ `which autoconf` ] && autoconf )

echo timestamp > stamp-h.in
exit 0
