FROM dockcross/windows-static-x64:latest
ARG no_check=false
ARG no_install=false

RUN mkdir src
COPY CMakeLists.txt README COPYING.LIB ChangeLog src/
COPY CMakeScripts src/CMakeScripts
COPY bins src/bins
COPY docs src/docs
COPY test src/test
COPY SDL src/SDL
COPY zzipwrap src/zzipwrap
COPY zzip src/zzip

RUN { echo "[requires]" \
    ; echo "zlib/1.2.11" \
    ; echo "" \
    ; echo "[generators]" \
    ; echo "cmake_paths" \
    ; } > src/conanfile.txt

RUN mkdir src/build
RUN cd src/build && conan install ..
RUN cd src/build && cmake .. -DCMAKE_TOOLCHAIN_FILE=./conan_paths.cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_SYSTEM_NAME=Windows
RUN cd src/build && cmake --build .
# RUN $no_check || (cd src/build && make check)
# RUN $no_install || (cd src/build && make install)
RUN cd src/build && cmake --build . --target install

