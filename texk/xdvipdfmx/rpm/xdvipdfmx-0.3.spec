# # # # # # # # # #
# PREAMBLE
# # # # # # # # # #

Summary: An extended version of DVIPDFMx with support for XeTeX output
Name: xdvipdfmx
Version: 0.3
Release: 1
Copyright: GPL
Group: Productivity/Publishing/TeX/Base
Source: http://scripts.sil.org/svn-view/xdvipdfmx/TAGS/xdvipdfmx-%{version}.tar.gz
URL: http://scripts.sil.org/xetex_linux
Vendor: SIL International
Packager: Jonathan Kew <jonathan_kew@sil.org>

Requires: tetex, fontconfig, freetype2, libpng, zlib

# to build, we need various -devel packages...
BuildRequires: fontconfig-devel, freetype2-devel, libpng-devel, zlib-devel, te_kpath

%description
xdvipdfmx is an output driver for the XeTeX typesetting system.
It is an extended version of DVIPDFMx by Jin-Hwan Cho and Shunsaku Hirata,
which is itself an extended version of dvipdfm by Mark A. Wicks.
This driver converts XDV (extended DVI) output from the xetex program
into standard PDF that can be viewed or printed.

# # # # # # # # # #
# PREP
# # # # # # # # # #

%prep

# setup macro does standard clean-and-unpack
%setup

# # # # # # # # # #
# BUILD
# # # # # # # # # #

%build
sh ./configure --with-freetype2=`freetype-config --prefix`
make

# # # # # # # # # #
# INSTALL
# # # # # # # # # #

%install
# trying to copy the teTeX installation of dvipdfm, instead of using make install
program=xdvipdfmx
oldbin=`which dvipdfm`
realoldbin=`readlink -f ${oldbin}`
bindir=`dirname ${realoldbin}`
cp src/${program} ${bindir}/${program}
echo ${bindir}/${program} > installed-files
if [ "${oldbin}" != "${realoldbin}" ]; then
	linkdir=`dirname ${oldbin}`
	ln -fs ${bindir}/${program} ${linkdir}/${program}
	echo ${linkdir}/${program} >> installed-files
fi

# # # # # # # # # #
# FILE LIST
# # # # # # # # # #

%files -f installed-files
