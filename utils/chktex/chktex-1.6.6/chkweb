#! /bin/sh

#  ChkWEB v1.0, runs the LaTeX parts of a CWEB file through ChkTeX.
#  Copyright (C) 1996 Jens T. Berger Thielemann
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
#  Contact the author at:
#		Jens Berger
#		Spektrumvn. 4
#		N-0666 Oslo
#		Norway
#		E-mail: <jensthi@ifi.uio.no>


params=

for p
do
    case $p in
    --) break
	;;
    -*) params="$params $p"
	shift
	;;
    *) break
	;;
    esac
done


if test "$1" = ""; then
    deweb | chktex -v3 $params
else
    for file
    do
	if test -f $file; then
	    deweb $file | chktex -p $file -v3 $params
	else
	    deweb $file.w | chktex -p $file.w -v3 $params
	fi
    done
fi

