#!/bin/bash

set -e

if [ -z "$PREFIX" ]; then
    echo "Missing PREFIX"
    exit 1
fi

if [ -z "$TMPSRC" ]; then
    echo "Missing TMPSRC"
    exit 1
fi

export PATH="$PREFIX/bin:$PATH"

mkdir -p "$PREFIX"
mkdir -p "$TMPSRC"

if [ "$LUA_VERSION" == "LUA51" ]; then
    URL="http://www.lua.org/ftp/lua-5.1.5.tar.gz"
elif [ "$LUA_VERSION" == "LUA52" ]; then
    URL="http://www.lua.org/ftp/lua-5.2.4.tar.gz"
elif [ "$LUA_VERSION" == "LUA53" ]; then
    URL="http://www.lua.org/ftp/lua-5.3.1.tar.gz"
else
    echo "Invalid Lua version: LUA_VERSION=$LUA_VERSION"
    exit 1
fi

# Install lua to $PREFIX
mkdir -p "$TMPSRC/lua-src" && pushd "$TMPSRC/lua-src"
curl "$URL" | tar xz --strip-components 1
sed -i -e 's~#define LUA_ROOT.*$~#define LUA_ROOT "'"$PREFIX/"'"~g' src/luaconf.h
make PLAT=$PLAT
make PLAT=$PLAT INSTALL_TOP="$PREFIX" install
popd

lua -v

# Install luarocks to $PREFIX
mkdir -p "$TMPSRC/luarocks-src" && pushd "$TMPSRC/luarocks-src"
curl --location "http://luarocks.org/releases/luarocks-2.2.2.tar.gz" | tar xz --strip-components 1
./configure --prefix="$PREFIX"
make install
popd

luarocks --version

