Summary: FreeType library
Name: freetype
Version: 1.5
Release: 1
Source: ftp://ftp.freetype.org/freetype/freetype1-%{version}-%{release}.tar.gz
URL: http://www.freetype.org/
Copyright: BSD-Like + GPL
Group: Libraries
BuildRoot: /var/tmp/freetype

%description
The FreeType engine is a free and portable TrueType font rendering
engine.  It has been developed to provide TrueType support to a
great variety of platforms and environments.

Note that FreeType is a *library*.  It is not a font server for your
favorite platform, even though it was designed to be used in many of
them.  Note also that it is *not* a complete text-rendering library.
Its purpose is simply to open and manage font files, as well as
load, hint and render individual glyphs efficiently.  You can also
see it as a `TrueType driver' for a higher-level library, though
rendering text with it is extremely easy, as demo-ed by the test
programs.

This package contains the files needed to run programs that use the
FreeType engine.

%package devel
Summary: FreeType development headers and libraries
Group: Development/Libraries
Requires: %{name} = %{version}

%description devel
The FreeType engine is a free and portable TrueType font rendering
engine.  It has been developed to provide TrueType support to a
great variety of platforms and environments.

Note that FreeType is a *library*.  It is not a font server for your
favorite platform, even though it was designed to be used in many of
them.  Note also that it is *not* a complete text-rendering library.
Its purpose is simply to open and manage font files, as well as
load, hint and render individual glyphs efficiently.  You can also
see it as a `TrueType driver' for a higher-level library, though
rendering text with it is extremely easy, as demo-ed by the test
programs.

This package contains all supplementary files you need to develop
your own programs using the FreeType engine.

%package demo
Summary: FreeType test and demo programs
Group: Applications/Graphics
Requires: %{name} = %{version}

%description demo
The FreeType engine is a free and portable TrueType font rendering engine.
It has been developed to provide TT support to a great variety of platforms
and environments.

Note that FreeType is a *library*. It is not a font server for your favorite
platform, even though it was designed to be used in many of them. Note also 
that it is *not* a complete text-rendering library. Its purpose is simply to
open and manage font files, as well as load, hint and render individual 
glyphs efficiently. You can also see it as a "TrueType driver" for a 
higher-level library, though rendering text with it is extremely easy, as 
demo-ed by the test programs.

This package contains several programs bundled with the FreeType engine for
testing and demonstration purposes.

%changelog
* Tue Jan 08 2002  Werner Lemberg <werner.lemberg@freetype.org>
- Updated to version 1.5.

* Mon Oct 29 2001  Antoine Leca <Antoine-Freetype@Leca-Marti.org>
- Updated to version 1.4.9 (pre 1.5); NOTE: 1.4 was never released officially.

* Wed Feb 23 2000  Werner Lemberg <werner.lemberg@freetype.org>
- Updated to version 1.4.

* Thu Sep  9 1999  Pavel Kankovsky <peak@kerberos.troja.mff.cuni.cz>
- Simplified (and fixed) file list.

* Wed Jun 16 1999  Werner Lemberg <werner.lemberg@freetype.org>
- Updated to version 1.3.

* Sun Oct 25 1998  Pavel Kankovsky <peak@kerberos.troja.mff.cuni.cz>
- libttf.so version number updated again.
- Default localedir based on prefix.
- File list adjustments (howto/).

* Sun Oct 16 1998  Pavel Kankovsky <peak@kerberos.troja.mff.cuni.cz>
- Source filename fixed.
- HOWTO removed.
- libttf.so version number updated.

* Tue Sep 29 1998  Robert Wilhelm <robert@freetype.org>
- Updated to version 1.2.

* Tue Jul 14 1998  Alexander Zimmermann <Alexander.Zimmermann@fmi.uni-passau.de>
- Added missing files.
- Added %defattr tags.

* Thu Jun 18 1998  Robert Wilhelm <robert@freetype.org>
- Added lots of attr(-,root,root).

* Wed May 27 1998  Pavel Kankovsky <peak@kerberos.troja.mff.cuni.cz>
- Changed group attr of freetype and freetype-devel package.
- Fixed misc glitches.

* Sun May 24 1998  Pavel Kankovsky <peak@kerberos.troja.mff.cuni.cz>
- Split the package into three parts (runtime library, development
  tools, and demo programs).
- Added missing files (headers, NLS).
- Added ldconfing upon (de)installation.

* Thu Mar 12 1998  Bruno Lopes F. Cabral <bruno@openline.com.br>
- NLS for Portuguese language is missing, sorry (may be in a near future)
  (please note the workaround using --with-locale-dir and gnulocaledir.
  NLS Makefile needs a bit more rework but again I'll not patch it here).

%prep 
%setup

find . -name CVS -type d | xargs rm -rf

%build
./configure --prefix=/usr --enable-static
make all

%install
make install prefix=$RPM_BUILD_ROOT/usr

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(644, root, root, 755)
%doc announce license.txt docs/FTL.txt docs/GPL.txt
/usr/lib/libttf.so.*
/usr/share/locale/

%files devel
%defattr(644, root, root, 755)
%doc INSTALL README PATENTS announce license.txt readme.1st
%doc docs/ howto/
/usr/lib/libttf.so
/usr/lib/libttf.la
/usr/lib/libttf.a
/usr/include/freetype/

%files demo
%defattr(755, root, root, 755)
%doc announce license.txt
/usr/bin/ftdump
/usr/bin/fterror
/usr/bin/ftlint
/usr/bin/ftmetric
/usr/bin/ftsbit
/usr/bin/ftstring
/usr/bin/ftstrpnm
/usr/bin/ftstrtto
/usr/bin/fttimer
/usr/bin/ftview
/usr/bin/ftzoom
