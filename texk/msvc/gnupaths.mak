################################################################################
#
# Makefile  : paths definitions for Gnu tools
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/08/22 12:09:48 popineau>
#
################################################################################

prefix = $(gnuroot)
bindir = $(gnubin)
libdir = $(gnulib)
sharedir = $(gnushare)
includedir = $(gnuinclude)
docdir = $(gnuroot)\doc
mandir = $(gnuroot)\man
infodir = $(gnuinfo)
#htmldocdir = $(docdir)\html
gnuroot_srcdir = $(root_srcdir)\gnu

# End of gnupaths.mak
# 
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
