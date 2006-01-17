################################################################################
#
# Makefile  : paths definitions for Xemacs tools
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/10 16:59:40 popineau>
#
################################################################################

prefix = $(xemacstools)
bindir = $(prefix)\$(xemacsversion)\i586-pc-win32
#libdir = $(prefix)\lib
#sharedir = $(prefix)\share
#includedir = $(prefix)\include
docdir = $(prefix)\doc
#mandir = $(prefix)\man
infodir = $(prefix)\info
htmldocdir = $(docdir)\html

# End of xemacspaths.mak
# 
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
