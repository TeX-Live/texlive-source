#! /bin/sh

# Copyright (C) 2001-2019 Peter Selinger.
# This file is free software and it is covered by the GNU general
# public license. See the file COPYING for details.

# provide (dumb) replacements for missing functions

# first, because the "which" command on Solaris is totally useless,
# we need to implement our own. This code is adapted from autoconf.

my_which () {
    if test "$#" -ne 1; then
	echo "my_which: wrong number of arguments" >&2
	return 255
    fi
    cmd="$1"
    IFS="${IFS=   }"; save_ifs="$IFS"; IFS=":"
    path="$PATH"
    for dir in $path; do
	test -z "$dir" && dir=.
	if test -f "$dir/$cmd"; then
	    echo "$dir/$cmd"
	    IFS="$save_ifs"
	    return 0
	fi
    done
    IFS="$save_ifs"
    return 1
}

# "mktemp" replacement: note that this creates the same filename each time. 
# Thus, when creating more than one tempfile, must give different templates.

MKTEMP=`my_which mktemp`
if test -z "$MKTEMP"; then
    echo "Warning: the mktemp program is missing. Using a dummy replacement." >&2
    mktemp () {
	echo "$1" | sed -e "s/XXXXXX/$$/"
    }
fi

	
