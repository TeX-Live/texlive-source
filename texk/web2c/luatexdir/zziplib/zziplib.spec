%define lib   lib010
Summary:      ZZipLib - libZ-based ZIP-access Library
Name:         zziplib
Version:      0.13.47
Release:      1.suse100
Serial:       1
Copyright:    LGPL
Group:        Development/Libraries
URL:          http://zziplib.sf.net
Vendor:       Guido Draheim <guidod@gmx.de>
Source0:      http://prdownloads.sf.net/%{name}/%{name}-%{version}.tar.bz2
BuildRoot:    /var/tmp/%{name}-%{version}-%{release}

Distribution: Original
Packager:     Guido Draheim <guidod@gmx.de>
Requires:      zlib
BuildRequires: zlib-devel
BuildRequires: SDL-devel

#Begin3
# Author1:        too@iki.fi (Tomi Ollila)
# Author2:        guidod@gmx.de (Guido Draheim)
# Maintained-by:  guidod@gmx.de (Guido Draheim)
# Primary-Site:   zziplib.sf.net
# Keywords:       zip zlib inflate archive gamedata
# Platforms:      zlib posix
# Copying-Policy: Lesser GPL Version 2
#End

%package %lib
Summary:      ZZipLib - Documentation Files
Group:        Development/Libraries
Provides:     zziplib
Provides:     libzzip0
Provides:     libzzip-0.so.10

%package doc
Summary:      ZZipLib - Documentation Files
Group:        Development/Libraries

%package devel
Summary:      ZZipLib - Development Files
Group:        Development/Libraries
Requires:     zziplib-%lib = %version
# Requires: pkgconfig (not yet)

%description
 : zziplib provides read access to zipped files in a zip-archive,
 : using compression based solely on free algorithms provided by zlib.

%description %lib
 : zziplib provides read access to zipped files in a zip-archive,
 : using compression based solely on free algorithms provided by zlib.
 zziplib provides an additional API to transparently access files
 being either real files or zipped files with the same filepath argument.
 This is handy to package many files being shared data into a single
 zip file - as it is sometimes used with gamedata or script repositories.
 The library itself is fully multithreaded, and it is namespace clean
 using the zzip_ prefix for its exports and declarations.
 
%description doc
 : zziplib provides read access to zipped files in a zip-archive,
 : using compression based solely on free algorithms provided by zlib.
 these are the (html) docs, mostly generated actually.

%description devel
 : zziplib provides read access to zipped files in a zip-archive,
 : using compression based solely on free algorithms provided by zlib.
 these are the header files needed to develop programs using zziplib.
 there are test binaries to hint usage of the library in user programs.

%prep
#'
%setup
# fixing relink problems during install too
LDFLAGS="-L%buildroot%_libdir" \
CFLAGS="$RPM_OPT_FLAGS" \
sh configure --prefix=%{_prefix} --enable-sdl --disable-builddir \
  --with-docdir=%{_docdir} --mandir=%{_mandir} TIMEOUT=9
cp -a zzip zzip64

%build
%define _FILE_OFFSET64 -D_ZZIP_LARGEFILE -D_FILE_OFFSET_BITS=64
%define _RELEASEINFO64 "RELEASE_INFO=-release 0-64"
%define _CFLAGS_OFFSET64 "AM_CFLAGS=%_FILE_OFFSET64"
make
(cd zzip64 && make %_CFLAGS_OFFSET64 %_RELEASEINFO64)
make doc

%install
rm -rf %{buildroot}
(cd zzip64 && make install %_RELEASEINFO64 DESTDIR=%{buildroot})
(cd %buildroot/%_libdir && mv    libzzip.so   libzzip64.so)
(cd %buildroot/%_libdir && mv    libzzip.a    libzzip64.a)
(cd %buildroot/%_libdir && \
sed -e 's/zzip.so/zzip64.so/' -e 's/zzip.a/zzip64.a/' libzzip.la >libzzip64.la)
(cd %buildroot/%_libdir/pkgconfig && \
sed -e 's/largefile=/largefile= %_FILE_OFFSET64/' \
    -e 's/-lzzip/-lzzip64/' -e 's/zziplib/zziplib64/' zziplib.pc >zziplib64.pc)

make install DESTDIR=%{buildroot}
(cd %buildroot/%_libdir && mv    libzzip.so   libzzip32.so)
(cd %buildroot/%_libdir && mv    libzzip.a    libzzip32.a)
(cd %buildroot/%_libdir && ln -s libzzip32.so libzzip.so)
(cd %buildroot/%_libdir && ln -s libzzip32.a  libzzip.a)
(cd %buildroot/%_libdir && \
sed -e 's/zzip.so/zzip32.so/' -e 's/zzip.a/zzip32.a/' libzzip.la >libzzip32.la)
(cd %buildroot/%_libdir/pkgconfig && \
sed -e 's/-lzzip/-lzzip32/' -e 's/zziplib/zziplib32/' zziplib.pc >zziplib32.pc)

# the 12.8x and 11.8x and 10.8x packages are all the same actually
(cd %buildroot/%_libdir && \
(for i in libzzip*.so.1? ; do : \
; v10=`echo $i | sed -e "s/.so.../.so.10/"` \
; v11=`echo $i | sed -e "s/.so.../.so.11/"` \
; v12=`echo $i | sed -e "s/.so.../.so.12/"` \
; test ! -e $v10 && test -e $v12 && ln -s $v12 $v10 \
; test ! -e $v12 && test -e $v10 && ln -s $v10 $v12 \
; ln -s $v10 $v11 || true; done))

make install-doc DESTDIR=%{buildroot}
make install-man3 DESTDIR=%{buildroot}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%clean
rm -rf %{buildroot}

%files %lib
      %defattr(-,root,root)
      %{_libdir}/lib*.so.*

%files doc
      %defattr(-,root,root)
      %{_datadir}/groups/*
%dir  %{_datadir}/omf/%{name}
      %{_datadir}/omf/%{name}/*
%post doc
test ! -f %_bindir/scrollkeeper-update || %_bindir/scrollkeeper-update
%postun doc
test ! -f %_bindir/scrollkeeper-update || %_bindir/scrollkeeper-update

%files devel
      %defattr(-,root,root)
      %{_bindir}/*
%dir  %{_includedir}/zzip
      %{_includedir}/zzip/*
      %{_includedir}/*.h
      %{_libdir}/lib*.so
      %{_libdir}/lib*.a
      %{_libdir}/lib*.la
      %{_libdir}/pkgconfig/*
%dir  %{_datadir}/%{name}
      %{_datadir}/%{name}/*
      %{_datadir}/aclocal/%{name}*.m4
      %{_mandir}/man3/*	
