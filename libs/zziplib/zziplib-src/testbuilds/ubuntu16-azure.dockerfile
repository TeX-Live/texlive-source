FROM ubuntu:16.04
ARG no_check=false
ARG no_install=false

RUN apt-get update
RUN apt-get install -y gcc zlib1g-dev python3 cmake zip gzip tar pkg-config
RUN apt-get install -y unzip
# RUN apt-get install -y libsdl2-dev
RUN apt-get install -y --fix-broken --ignore-missing python3-wheel || true
RUN apt-get install -y --fix-broken --ignore-missing python3-pip || true
RUN pip3 install unittest-xml-reporting

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
RUN cd src/build && cmake .. -DTESTFLAGS=--xmlresults=zziptests.tmp
RUN cd src/build && make all
RUN $no_check   || (cd src/build && make check VERBOSE=1) || true
RUN $no_install || (cd src/build && make install)

