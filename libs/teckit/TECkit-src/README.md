TECkit - A Text Encoding Conversion Toolkit
===========================================

Overview
--------

TECkit is a low-level toolkit intended to be used by applications for
conversions between text encodings. For example, it can be used when importing
legacy text into a Unicode-based application.

The primary component of TECkit is a library: the TECkit engine. The engine
relies on mapping tables in a specific, documented binary format. The TECkit
compiler creates these tables from plain-text, human-readable descriptions.

## Project status
[![TECKit](https://github.com/silnrsi/teckit/workflows/TECKit/badge.svg)](https://github.com/silnrsi/teckit/actions?query=workflow%3ATECKit)
[![TECKit non-x86](https://github.com/silnrsi/teckit/workflows/TECKit%20non-x86/badge.svg)](https://github.com/silnrsi/teckit/actions?query=workflow%3A%22TECKit+non-x86%22)

The TECkit libraries supports the Unicode 17.0.0 character repertoire.

TECkit also includes some command line applications:

  - `teckit_compile`: TECkit compiler
  - `txtconv`: a simple text encoding conversion tool
  - `sfconv`: convert legacy encoded SFM files to Unicode

**Note:**

More powerful tools to handle text encodings using the TECkit library are

- SILConverters <https://software.sil.org/silconverters>
- LibreOffice Linguistic Tools <https://software.sil.org/oolt>

**References:**

  - General information can be found at <https://software.sil.org/teckit>.
  - Documentation is in the [`docs` directory].
  - Changes are found in the [`NEWS` file].
  - Python 3 bindings and codecs integration are in the [palaso-python] project.

[`docs` directory]: ./docs
[`NEWS` file]: ./NEWS
[palaso-python]: https://github.com/silnrsi/palaso-python

Getting TECkit
--------------

Our binary releases can be obtained from
<https://software.sil.org/teckit/#downloads>.

TECkit is also included in [TeX Live](https://www.tug.org/texlive/) and packaged
by many Linux distributions.

The source code repository and source releases are available at
<https://github.com/silnrsi/teckit>.

Building TECkit
---------------

The simplest way to build the source from the repository is as follows:

```sh
$ ./autogen.sh             # Generate the configure script
$ mkdir build              # Create a new, empty directory
$ cd build                 # Change into the new directory
$ ../configure             # Run the generated configure script
$ make                     # Build
$ make check               # Run the tests (optional)
$ cat test/dotests.pl.log  # View the test output (optional)
$ make install             # Install (optional)
```

Notes:

  - The default installation directory prefix is `/usr/local`. You most likely
    do not want this. The directory can be changed with
    `../configure --prefix=<install-dir>`.
  - Other configuration options can be found with `../configure --help`.
  - This uses an out-of-tree build in the `build` directory. This is recommended
    to avoid polluting the source directories with build products. The name of
    the directory doesn't matter (as long as it is empty), but `build` is
    ignored by `git` (see the `.gitignore`), which makes it convenient when
    looking at `git status` for example.
  - For testing on Windows, set the BINDIR variable `"set BINDIR="C:\Users\<username>\Downloads\teckit\TECkit-<version>\64-bit"`

### Dependencies

Configuration and building requires `autoconf`, `automake`, and `libtool`.

The TECkit engine and compiler require `zlib`. The build can be configured to
use the system `zlib` with `--with-system-zlib` or to use the bundled `zlib`
with `--without-system-zlib`.

The `sfconv` binary requires `libexpat`. The `configure` script checks for a
system `libexpat`. If it is found, it is used. If it is not found, the bundled
`libexpat` is used.

### Platforms

TECkit can be built on Windows, Linux, and macOS (formerly Mac OS X).

The `build-*.sh` show how releases are made for the different platforms.

#### Microsoft Windows

We do not typically test building on Windows itself. Instead, we build releases
on Ubuntu Bionic by cross-compiling for MinGW with the following packages:

  - `gcc-mingw-w64-i686`
  - `g++-mingw-w64-i686`
  - `gcc-mingw-w64-x86-64`
  - `g++-mingw-w64-x86-64`

Starting with TECkit version 2.5.7 there are several changes with the Windows
builds. First, a 64-bit build has been added to the already existing 32-bit
build. Second, the runtime files have changed. The file `libwinpthread-1.dll` is
no longer needed. The file `libstdc++-6.dll` needs to match the build (32- or
64-bit) of the rest of the code. The 32-bit build needs `libgcc_s_sjlj-1.dll`,
while the 64-bit build needs `libgcc_s_seh-1.dll`.
