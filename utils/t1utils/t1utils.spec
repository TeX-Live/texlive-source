Summary:	Programs for manipulating PostScript Type 1 fonts

Name:		t1utils
Version:	1.33
Release:	1
Source:		http://www.lcdf.org/type/t1utils-1.33.tar.gz

URL:		http://www.lcdf.org/type/

Group:		Utilities/Printing
Vendor:		Little Cambridgeport Design Factory
Packager:	Eddie Kohler <kohler@icir.org>
License:	freely modifiable and distributable

BuildRoot:	/tmp/t1utils-build

%description
The t1utils package is a set of programs for
manipulating PostScript Type 1 fonts. It contains
programs to change between binary PFB format (for
storage), ASCII PFA format (for printing), a
human-readable and -editable ASCII format, and
Macintosh resource forks.

%prep
%setup

%build
%configure
make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT

%files
%attr(-,root,root) %doc NEWS README
%attr(0755,root,root) %{_bindir}/t1ascii
%attr(0755,root,root) %{_bindir}/t1binary
%attr(0755,root,root) %{_bindir}/t1asm
%attr(0755,root,root) %{_bindir}/t1disasm
%attr(0755,root,root) %{_bindir}/t1unmac
%attr(0755,root,root) %{_bindir}/t1mac
%attr(0644,root,root) %{_mandir}/man1/t1ascii.1*
%attr(0644,root,root) %{_mandir}/man1/t1binary.1*
%attr(0644,root,root) %{_mandir}/man1/t1asm.1*
%attr(0644,root,root) %{_mandir}/man1/t1disasm.1*
%attr(0644,root,root) %{_mandir}/man1/t1unmac.1*
%attr(0644,root,root) %{_mandir}/man1/t1mac.1*
