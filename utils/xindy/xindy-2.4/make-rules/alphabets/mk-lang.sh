#! /bin/sh

# Copyright (C) 2010 Peter Breitenlohner <tex-live@tug.org>.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program in the file COPYING; if not, write to the
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301 USA.

# Generate the Makefile fragments lang/Makefile.am from language
# definitions in lang.defs containing lines:
#   lang [[prefix] UTF8_DOC [DOC]...]...
# where
#   prefix is an optional filename prefix, without final '-'
#   UTF8_DOC is a DOCS_UTF8_* variable, without leading 'DOCS_'
#   DOC is an other DOCS_* variable, without leading 'DOCS_'

CP_DOCS='CP1250 CP1252 CP850'
LATIN_DOCS='LATIN1 LATIN2 LATIN3 LATIN4 LATIN5 LATIN9'
CYR_DOCS='CYR_1251 CYR_ISO88595 CYR_ISOIR111 CYR_KOI8_R CYR_KOI8_U'
ISO_DOCS='ISO88597 ISO88598'
UTF_DOCS='UTF8_LAT UTF8_CYR UTF8_OTHER'

ALL_DOCS="$CP_DOCS $LATIN_DOCS $CYR_DOCS $ISO_DOCS $UTF_DOCS"

err_exit () {
	echo "bad argument \"$arg\""
	exit 1
}

do_XDY () {
	XDY="$XDY $name"
	eval DOCS_$arg="\"\$DOCS_$arg $name\""
}

do_UTF8 () {
	PLIN="$PLIN $name"
	do_XDY
}

do_CODE () {
	PL="$PL $name"
	do_XDY
}

parse_lang () {
	for DOC in $ALL_DOCS; do
		eval DOCS_$DOC=
	done
	PLIN= PL= XDY=	
	lang=$1
	shift
	pref=
	for arg
	do
		case $arg in
			[a-z]*) pref="$arg-";;
			UTF8_*) name=${pref}utf8; do_UTF8;;
			[A-Z]*)	case $arg in
					CP*) val="cp${arg:2}";;
					ISO*) val="iso${arg:3}";;
					LATIN*) val="latin${arg:5}";;
					CYR_1251) val=cp1251;;
					CYR_ISO88595) val=iso88595;;
					CYR_ISOIR111) val=isoir111;;
					CYR_KOI8_R) val=koi8-r;;
					CYR_KOI8_U) val=koi8-u;;
					*) err_exit;;
				esac
				name="$pref$val"
				do_CODE
			;;
			*) err_exit;;
		esac
	done
}

print_header () {
	cat <<-_EOF
	## Makefile fragment for $lang.

	## Copyright (C) 2009, 2010 by Peter Breitenlohner.
	## Copyright (C) 2004-2005 by Gour.
	##
	## This program is free software; you can redistribute it and/or
	## modify it under the terms of the GNU General Public License as
	## published by the Free Software Foundation; either version 2 of the
	## License, or (at your option) any later version.
	##
	## This program is distributed in the hope that it will be useful, but
	## WITHOUT ANY WARRANTY; without even the implied warranty of
	## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	## General Public License for more details.
	##
	## You should have received a copy of the GNU General Public License
	## along with this program in the file COPYING; if not, write to the
	## Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
	## Boston, MA 02110-1301 USA.

	subdirs += $lang
	_EOF
}

print_xdy () {
	test $# -eq 0 && return
	printf '\nnobase_nodist_lang_DATA +='
	for xdy
	do
		printf ' \\\n\t'$lang'/%s-lang.xdy' $xdy
		printf ' \\\n\t'$lang'/%s-test.xdy' $xdy
		printf ' \\\n\t'$lang'/%s.xdy' $xdy
	done
	printf "\n"
}

print_docs () {
	test $# -eq 0 && return
	printf '\nDOCS_%s +=' $DOC
	printf ' \\\n\t'$lang'/%s-doc.tex' "$@"
	printf "\n"
}

print_extra_hdr () {
	test -n "$print_extra_hdr_done" && return
	printf '\nEXTRA_DIST +='
	print_extra_hdr_done=yes
}

print_extra () {
	test $# -eq 0 && return
	print_extra_hdr
	printf ' \\\n\t%s' "$@"
}

print_extra_plin () {
	test $# -eq 0 && return
	print_extra_hdr
	printf ' \\\n\t'$lang'/%s.pl.in' "$@"
}

print_pl () {
	test $# -eq 0 && return
	printf '\ndist_noinst_DATA +='
	printf ' \\\n\t'$lang'/%s.pl' "$@"
	printf "\n"
}

print_clean () {
	test $# -eq 0 && return
	printf '\nCLEANFILES +='
	for clean
	do
		case $clean in
			*utf8) printf ' \\\n\t'$lang'/%s.pl' $clean;;
		esac
		printf ' \\\n\t'$lang'/%s.raw' $clean
	done
	printf "\n"
}

make_lang () {
	print_header
	print_xdy $XDY
	for DOC in $ALL_DOCS; do
		eval print_docs \$DOCS_$DOC
	done
	print_extra_hdr_done=
	case $lang in
		general) PL="$PL template";;
		klingon) PL="$PL translit-ascii";;
		test1) PL="$PL test1";;
		romanian)
			cat <<\_EOF

romanian/latin2.pl : romanian/utf8.pl.in
	sed -e 's/@u{\([^}]*\)}//g;' -f $(srcdir)/rom-comma-kludge.sed <$< | \
	$(CONVERT_FROM_UTF8)ISO8859-2 >$@
_EOF
			print_extra rom-comma-kludge.sed
		;;
	esac
	print_extra_plin $PLIN
	test -n "$print_extra_hdr_done" && printf '\n'
	print_pl $PL
	print_clean $XDY
	echo
}

all_langs () {
	while read line
	do
		parse_lang $line
		mkdir -p $lang
		make_lang >$lang/Makefile.tmp
		if diff $lang/Makefile.tmp $lang/Makefile.am >/dev/null 2>&1; then
			rm -f $lang/Makefile.tmp
		else
			mv -f $lang/Makefile.tmp $lang/Makefile.am
		fi
	done
}

all_langs < lang.defs
