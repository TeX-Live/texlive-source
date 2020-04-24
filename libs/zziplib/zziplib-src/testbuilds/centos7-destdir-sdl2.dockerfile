FROM centos:7.7.1908
ARG no_check=false
ARG no_install=false

RUN yum install -y epel-release
RUN yum install -y gcc zlib-devel python3 cmake3 make unzip zip gzip tar  SDL2-devel

RUN mkdir src
COPY CMakeLists.txt README COPYING.LIB ChangeLog src/
COPY CMakeScripts src/CMakeScripts
COPY bins src/bins
COPY docs src/docs
COPY test src/test
COPY SDL src/SDL
COPY zzipwrap src/zzipwrap
COPY zzip src/zzip

RUN mkdir src/build
RUN cd src/build && cmake3 ..
RUN cd src/build && make
RUN $no_check || (cd src/build && make check)
RUN $no_install || (cd src/build && make install DESTDIR=/new)
