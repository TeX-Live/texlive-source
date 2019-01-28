Summary: Programs to manipulate OpenType and multiple-master fonts
Name: lcdf-typetools
Version: 2.108
Copyright: GPL
Vendor: Little Cambridgeport Design Factory <http://www.lcdf.org/>
Group: Utilities/Printing
Source: %{name}-%{version}.tar.gz
Buildroot: /var/tmp/%{name}-%{version}-buildroot
Requires: tetex > 2.0

%description
This package contains four tools for working with OpenType fonts:

 cfftot1    allows you to translate Compact Font Format (CFF) or
            PostScript-flavored OpenType fonts into PostScript
            Type 1 font format

 otfinfo    reports information about OpenType fonts, such as the
            features they support and the contents of their ``size''
            optical size option

 otftotfm   allows you to create TeX font metrics and encodings for
            using OpenType fonts

 t1dotlessj creates a Type 1 font with a single character --
            the dotless j corresponding to the specified design

 t1lint     checks a Type 1 font for correctness (preliminary)

 t1reencode reencodes a Type 1 font, replacing its internal encoding with
            one you specify

 t1testpage creates a PostScript test page for a specified
            font file (preliminary)

 ttftotype42 creates a Type 42 wrapper for a TrueType or TrueType-flavored 
            OpenType font.

The package now includes programs for working with multiple-master
fonts formerly distributed as mminstance.  These tools allow you to
use multiple-master fonts with programs that require single-master
fonts (afm2tfm, ps2pk, fontinst, etc.).  Both programs work fine with
fonts that contain intermediate masters (e.g., Adobe Jenson MM and
Adobe Kepler MM).

mmafm        creates an AFM (Adobe font metric) file corresponding to
             a single instance of a multiple-master font.  It reads
             (and therefore requires) the AMFM and AFM files
             distributed with the font.

mmpfb        creates a normal, single-master font program that looks
             like an instance of a multiple-master font.  It reads
             the multiple-master font program in PFA or PFB format.

## PREP
%prep 
%setup -q

## BUILD
%build
%configure
make

## PRE
%pre

## POST
%post

## INSTALL
%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install
#make install all DESTDIR=%{buildroot}

## FILES
%files  
%defattr(-,root,root)
%doc NEWS ONEWS README
%{_bindir}/cfftot1
%{_bindir}/mmafm 
%{_bindir}/mmpfb
%{_bindir}/otfinfo
%{_bindir}/otftotfm
%{_bindir}/t1dotlessj
%{_bindir}/t1lint
%{_bindir}/t1reencode
%{_bindir}/t1testpage
%{_bindir}/ttftotype42
%{_mandir}/man*/*
%{_datadir}/lcdf-typetools/*

%changelog

* Fri Aug  3 2007 Eddie Kohler <kohler@cs.ucla.edu>
- Updates.

* Wed Sep  8 2004 Claire Connelly <cmc@math.hmc.edu> - 2.12-hmcmath.1
- Initial packaging.
