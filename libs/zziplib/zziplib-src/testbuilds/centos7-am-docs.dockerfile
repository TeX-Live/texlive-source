FROM centos:7.7.1908
ARG no_build=false

ARG _libdir=/usr/local/lib64
ARG _docdir=/usr/share/doc

RUN yum install -y epel-release
RUN yum install -y gcc zlib-devel python3 make unzip zip gzip tar

RUN mkdir src
COPY CMakeLists.txt README COPYING.LIB ChangeLog src/
COPY Makefile.am Makefile.in configure.ac configure config.h.in zziplib.spec src/
COPY uses src/uses
COPY bins src/bins
COPY docs src/docs
COPY test src/test
COPY SDL src/SDL
COPY zzipwrap src/zzipwrap
COPY zzip src/zzip

RUN mkdir src/build
RUN cd src/build && sh ../configure --libdir=$_libdir --with-docdir=$_docdir --disable-static
RUN $no_build || (cd src/build && make)
RUN cd src/build && make docs
RUN cd src/build && make install-docs install-mans
