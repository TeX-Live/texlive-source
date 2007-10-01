%define name lzma
%define major 4
%define minor 32.0beta5
%define _tmppath /tmp

Name: lzma
Version: %{major}.%{minor}
Release: 1
Group: Applications/Archiving
Vendor: Tukaani.org
URL: http://www.tukaani.org/lzma/
Packager: Michael J. Baysek <mike@bayseksolutions.com>
License: GPL
Summary: LZMA Compression Binaries

Buildroot: %{_tmppath}/%{name}-%{major}.%{minor}
Source: %{name}-%{major}.%{minor}.tar.gz

%description
 LZMA is a data compression algorithm that provides compression
 equal or greather than bzip2, with faster compression, and much
 faster decompression.


%prep

%setup 

%build

%configure

make 

%install

%makeinstall

#Strip binaries
strip $RPM_BUILD_ROOT/usr/bin/lzmainfo
strip $RPM_BUILD_ROOT/usr/bin/lzma
strip $RPM_BUILD_ROOT/usr/bin/lzmadec

strip $RPM_BUILD_ROOT/usr/lib/liblzmadec.so.0.0.0
strip $RPM_BUILD_ROOT/usr/lib/liblzmadec.a

%clean

rm -rf $RPM_BUILD_ROOT

%files

%docdir /usr/share/man1

/usr/bin/lzcat
/usr/bin/lzcmp
/usr/bin/lzdiff
/usr/bin/lzegrep
/usr/bin/lzfgrep
/usr/bin/lzgrep
/usr/bin/lzless
/usr/bin/lzma
/usr/bin/lzmadec
/usr/bin/lzmainfo
/usr/bin/lzmore
/usr/bin/unlzma
/usr/include/lzmadec.h
/usr/lib/liblzmadec.a
/usr/lib/liblzmadec.la
/usr/lib/liblzmadec.so.0.0.0
/usr/share/man/man1/lzcat.1.gz
/usr/share/man/man1/lzcmp.1.gz
/usr/share/man/man1/lzdiff.1.gz
/usr/share/man/man1/lzegrep.1.gz
/usr/share/man/man1/lzfgrep.1.gz
/usr/share/man/man1/lzgrep.1.gz
/usr/share/man/man1/lzless.1.gz
/usr/share/man/man1/lzma.1.gz
/usr/share/man/man1/lzmadec.1.gz
/usr/share/man/man1/lzmainfo.1.gz
/usr/share/man/man1/lzmore.1.gz
/usr/share/man/man1/unlzma.1.gz

