#!/usr/local/bin/bash

set -e

# All 3 RE options
./configure --disable-pcre --disable-posixre
make check

./configure --disable-pcre --enable-posixre
make check

./configure --enable-pcre
make check

# Out of directory build
mkdir -p build
cd build
../configure --enable-pcre
make check
