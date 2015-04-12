#!/bin/sh

ACLOCAL="aclocal -I m4"
AUTOHEADER="autoheader"
AUTOMAKE="automake --add-missing --copy --foreign"
AUTOCONF="autoconf"

# Delete old cache directories.
# automake will stop if their contents was created by an earlier version.
rm -rf autom4te.cache

# Generate the Makefiles and configure files
echo "Building macros..."
if ( $ACLOCAL --version ) < /dev/null > /dev/null 2>&1; then
	$ACLOCAL
else
	echo "aclocal not found -- aborting"
	exit 1
fi

echo "Building config header template..."
if ( $AUTOHEADER --version ) < /dev/null > /dev/null 2>&1; then
	$AUTOHEADER
	echo timestamp > stamp-h.in
else
	echo "autoheader not found -- aborting"
	exit 1
fi

# We do not really need automake, but want to install programs like install-sh.
echo "Installing some useful programs..."
if ( $AUTOMAKE --version ) < /dev/null > /dev/null 2>&1; then
	$AUTOMAKE 2>/dev/null
else
	echo "automake not found -- aborting"
	exit 1
fi

echo "Building configure..."
if ( $AUTOCONF --version ) < /dev/null > /dev/null 2>&1; then
	$AUTOCONF
else
	echo "autoconf not found -- aborting"
	exit 1
fi

echo
echo 'run "./configure ; make"'
echo
