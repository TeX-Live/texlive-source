## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `automake Makefile' to create only Makefile.in.

## Copyright (C) 2004-2005 by Gour.
## Copyright (C) 2014 by Joachim Schrod.
## Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
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

modulesdir = $(pkglibdir)/modules

nobase_modules_DATA =

EXTRA_DIST = $(nobase_modules_DATA)

## Include all Makefile.am fragments.
##
include base/Makefile.am
include class/Makefile.am
include lang/german/Makefile.am
include lang/korean/Makefile.am
include lang/latin/Makefile.am
include ord/Makefile.am
include rules/Makefile.am
include styles/Makefile.am
