# Building Harfbuzz and _luaharfbuzz_ on Windows

## Building Harfbuzz

### Prerequisites
* [Visual Studio Community Edition](https://www.visualstudio.com/vs/community/)
* C++ Support for Visual Studio (See [here](http://stackoverflow.com/questions/31953769/visual-studio-doesnt-have-cl-exe))

### Building
* Download the Harfbuzz source for win32 from the [releases pages](https://github.com/behdad/harfbuzz/releases) (look for a zip file that ends with _-win32_, for e.g. _harfbuzz-1.4.2-win32.zip_), and unzip it.
* Launch the Visual Studio developer prompt for the platform you want to target. This is very important. For e.g, on my 64-bit system, I am using _VS2015 x64 Native Tools Command Prompt_
* Navigate to the _win32_ folder in the Harfbuzz source tree and do `nmake /f Makefile.vc CFG=release`
* This should generate _harfbuzz.lib_ and _harfbuzz-vs14.dll_.

## Copying Harfbuzz headers and DLL to the right locations
The Harfbuzz headers and DLLs must be copied to the right locations, for Lua and LuaRocks to be able to find them during installation and running:

* Copy all the Harfbuzz header (_src/*.h_) files to _C:\external\include\harfbuzz_. This is where LuaRocks will look for them by default. Make sure the header files are in the _harfbuzz_ sub-directory under _C:\external\include_, otherwise it will not work.
* Copy _harfbuzz.lib_ and _harfbuzz-vs14.dll_ to the folder _C:\external\lib_. This is where LuaRocks will look for them by default.
* Copy  _harfbuzz-vs14.dll_ to _C:\Windows\System32_. This is where running programs can locate the Harfbuzz DLL.

## Installing Lua and LuaRocks
It is highly recommended that you install Lua 5.2 and LuaRocks in a sandboxed environment on your machine. [Hererocks] makes it dead simple to do on Windows. Make sure you have installed Python first. On my system, I just did `chocolatey install python`, but you can use an alternate method and skip running that command below.

[Hererocks]:https://github.com/mpeterv/hererocks

```
chocolatey install python
wget https://raw.githubusercontent.com/mpeterv/hererocks/latest/hererocks.py
python hererocks.py lua52 -l5.2 -rlatest
source lua52/bin/activate
```
## Install _luaharfbuzz_
If you followed the steps in the previous sections, and have a sandboxed Lua installation, which the Lua program on %PATH%, then doing the following should just work:

```
C:\> luarocks install luaharfbuzz
```

This will download the _luaharfbuzz_ source, compile the C modules, link them against Lua and Harfbuzz, and install it in the right location.

## Test
Test whether the installation was successful by loading _luaharfbuzz_ in the Lua REPL

```
C:\> lua
Lua 5.2.4  Copyright (C) 1994-2015 Lua.org, PUC-Rio
> hb = require("harfbuzz")
> print(hb.version())
1.4.1
>
```

