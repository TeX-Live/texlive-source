#
#   Copyright (C) 2013  Kevin W. Hamlen
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#
# AC_PROG_VIM
#
# Test for vim and set $vim to the correct value.
#
#
dnl @synopsis AC_PROG_VIM
dnl
dnl This macro tests if vim is installed. If vim
dnl is installed, it sets $vim to the right value
dnl
dnl @version 1.0
dnl @author Kevin W. Hamlen
dnl
AC_DEFUN([AC_PROG_VIM],[
AC_CHECK_PROG(vim,[vim],vim)
export $vim;
])
