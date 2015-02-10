# Private macros for the TeX Live (TL) tree.
# Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

## ------------------------------ ##
## Define lists of sub-packages.  ##
## ------------------------------ ##

# Hopefully these lists are defined here and nowhere else.
# Note: directories in these lists need not exist.

# KPSE_LIBS_PKGS()
# ----------------
# Define two lists of library sub-packages:
#   generic libraries, i.e., subdirs 'libs/*', that can be used by
#   utility and TeXk sub-packages
# and
#   TeX specific libraries, i.e., subdirs 'texk/*', that can only
#   be used by TeXk sub-packages and can't use generic libraries
# Each library must precede required other libraries (if any).
AC_DEFUN([KPSE_LIBS_PKGS], [dnl
m4_define([kpse_libs_pkgs], [dnl
harfbuzz
icu
teckit
graphite2
zziplib
xpdf
poppler
mpfr
gmp
cairo
pixman
potrace
gd
freetype2
libpng
libpaper
luajit
lua52
zlib
])[]dnl
m4_define([kpse_texlibs_pkgs], [dnl
ptexenc
kpathsea
])]) # KPSE_LIBS_PKGS

# KPSE_ALL_SYSTEM_FLAGS()
# -----------------------
# Generate flags for all potential system libraries available,
# only used at top-level.
AC_DEFUN([KPSE_ALL_SYSTEM_FLAGS], [dnl
AC_REQUIRE([KPSE_LIBS_PREPARE])[]dnl
AC_REQUIRE([KPSE_KPATHSEA_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_PTEXENC_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_ZLIB_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_LIBPAPER_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_LIBPNG_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_FREETYPE2_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_GD_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_POTRACE_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_PIXMAN_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_CAIRO_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_GMP_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_MPFR_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_POPPLER_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_XPDF_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_ZZIPLIB_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_GRAPHITE2_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_TECKIT_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_ICU_SYSTEM_FLAGS])[]dnl
AC_REQUIRE([KPSE_HARFBUZZ_SYSTEM_FLAGS])[]dnl
]) # KPSE_ALL_SYSTEM_FLAGS

# KPSE_UTILS_PKGS()
# -----------------
# Define the list of utility sub-packages, i.e., subdirs 'utils/*'.
AC_DEFUN([KPSE_UTILS_PKGS], [dnl
m4_define([kpse_utils_pkgs], [dnl
devnag
lacheck
m-tx
pmx
ps2eps
t1utils
texdoctk
tpic2pdftex
vlna
xindy
xpdfopen
])]) # KPSE_UTILS_PKGS

# KPSE_TEXK_PKGS()
# ----------------
# Define the list of TeXk sub-packages, i.e., subdirs 'texk/*',
# excluding 'texk/kpathsea'.
AC_DEFUN([KPSE_TEXK_PKGS], [dnl
m4_define([kpse_texk_pkgs], [dnl
web2c
afm2pl
bibtex-x
chktex
cjkutils
detex
dtl
dvi2tty
dvidvi
dviljk
dvipdfm-x
dvipng
dvipos
dvipsk
dvisvgm
gsftopk
lcdf-typetools
makeindexk
makejvf
mendexk
musixtnt
ps2pk
psutils
seetexk
tex4htk
ttf2pk2
ttfdump
xdvik
texlive
])]) # KPSE_TEXK_PKGS



## ------------------------------- ##
##  Loop over one of these lists.  ##
## ------------------------------- ##

# KPSE_FOR_PKGS(LIST, ACTION)
# ---------------------------
# Run the shell code ACTION for each element Kpse_Pkg in kpse_LIST_pkgs,
# with Kpse_pkg a sanitized version of Kpse_Pkg for shell variables and
# Kpse_PKG a sanitized (upper case) version for make variables.
AC_DEFUN([KPSE_FOR_PKGS],
[AC_REQUIRE([KPSE_LIBS_PKGS])AC_REQUIRE([KPSE_UTILS_PKGS])AC_REQUIRE([KPSE_TEXK_PKGS])[]dnl
AC_FOREACH([Kpse_Pkg], kpse_$1_pkgs,
[m4_pushdef([Kpse_pkg], AS_TR_SH(Kpse_Pkg))[]dnl
m4_pushdef([Kpse_PKG], AS_TR_CPP(Kpse_Pkg))[]dnl
$2[]dnl
m4_popdef([Kpse_pkg])m4_popdef([Kpse_PKG])[]dnl
])]) # KPSE_FOR_PKGS
