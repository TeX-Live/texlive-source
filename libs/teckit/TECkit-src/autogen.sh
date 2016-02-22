#!/bin/sh
echo "*** Recreating libtool files"
if test -z $LTIZE; then
LTIZE="$AUTODIR""libtoolize"
fi
echo "$LTIZE"
	$LTIZE -f -c;

echo "*** Recreating aclocal.m4"
ACLOCAL="$AUTODIR""aclocal"
echo "$ACLOCAL"
	$ACLOCAL -I .;

echo "*** Recreating configure"
AUTOCONF="$AUTODIR""autoconf"
AUTOHEAD="$AUTODIR""autoheader"
	$AUTOHEAD ;
	$AUTOCONF;
	
echo "*** Recreating the Makefile.in files"
AUTOMAKE="$AUTODIR""automake"
	$AUTOMAKE --foreign -a -c;

