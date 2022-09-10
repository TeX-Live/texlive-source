dnl Process this file with autoconf to produce a configure script.

AC_INIT([libpaper], [1.1.27])
AC_CONFIG_SRCDIR([configure.ac])
AM_INIT_AUTOMAKE([no-define])
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

dnl Check for standard build environment
dnl ====================================
AC_PROG_CC
AC_GNU_SOURCE
AC_PROG_INSTALL
AM_PROG_LIBTOOL
AC_PROG_MAKE_SET

dnl Complier features
dnl =================
AC_C_CONST

dnl set up some substitutions
dnl =========================
PAPERCONF="${sysconfdir}/papersize"
AC_SUBST(PAPERCONF)
AH_TEMPLATE(PAPERCONF, [Default configuration file location])
AC_DEFINE_UNQUOTED(PAPERCONF, "$PAPERCONF")

PAPERCONFVAR=PAPERCONF
AC_SUBST(PAPERCONFVAR)
AH_TEMPLATE(PAPERCONFVAR, [Environment variable that overrides the default configuration file location])
AC_DEFINE_UNQUOTED(PAPERCONFVAR, "$PAPERCONFVAR")

PAPERSIZE=letter
AC_SUBST(PAPERSIZE)
AH_TEMPLATE(PAPERSIZE, [Default paper size])
AC_DEFINE_UNQUOTED(PAPERSIZE, "$PAPERSIZE")

RUNPARTSDIR="${sysconfdir}/libpaper.d"
AC_SUBST(RUNPARTSDIR)
AH_TEMPLATE(RUNPARTSDIR, [Default directory for run-parts])
AC_DEFINE_UNQUOTED(RUNPARTSDIR, "$RUNPARTSDIR")

RUNPARTSDIRVAR=PAPERDIR
AC_SUBST(RUNPARTSDIRVAR)
AH_TEMPLATE(RUNPARTSDIRVAR, [Environment variable that overrides the default directory for run-parts])
AC_DEFINE_UNQUOTED(RUNPARTSDIRVAR, "$RUNPARTSDIRVAR")

PAPERSIZEVAR=PAPERSIZE
AC_SUBST(PAPERSIZEVAR)
AH_TEMPLATE(PAPERSIZEVAR, [Environment variable that overrides the default paper size])
AC_DEFINE_UNQUOTED(PAPERSIZEVAR, "$PAPERSIZEVAR")

dnl Create files
dnl ============
AC_OUTPUT([
	Makefile
	lib/Makefile
	src/Makefile
	src/paperconfig
	man/Makefile
	man/paperconf.1
	man/paperconfig.8
	man/papersize.5
	man/systempapername.3
])

