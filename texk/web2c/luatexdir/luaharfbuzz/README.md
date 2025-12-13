[![Build + Deploy](https://github.com/harfbuzz/luaharfbuzz/actions/workflows/ci.yml/badge.svg)](https://github.com/harfbuzz/luaharfbuzz/actions/workflows/ci.yml)

# _luaharfbuzz_

Lua bindings for [Harfbuzz].

[Harfbuzz]:http://harfbuzz.org

## Contents

* [Overview](#overview)
* [Installing Harfbuzz](#installing-harfbuzz)
* [Installing _luaharfbuzz_](#installing-luaharfbuzz)
* [Documentation](#documentation)
* [Sample Code](#sample-code)
* [Development](#development)
* [Contact](#contact)

## Overview
HarfBuzz is an OpenType text shaping engine. It is used in software like Qt,
Pango, Firefox, Chromium, XeTeX and LibreOffice.

_luaharfbuzz_ provides bindings for the most common types in Harfbuzz. The
initial motivation for building it was to use Harfbuzz with the [LuaTeX]
typesetting system. However, the module isn’t tied to LuaTeX. It
can be used with any Lua codebase.

As of 2020, luaharfbuzz is bundled with [LuaTeX]. Additional support to use
the HarfBuzz renderer with LuaLaTeX and other formats is available via [luaotfload]. Read more:

* [TUG 2020 — Marcel Krüger — HarfBuzz in LuaLaTeX](https://www.youtube.com/watch?v=xPj6vNo8exY&t=2120s)
* [Typesetting Bangla script with LuaLaTeX](http://tug.org/TUGboat/tb41-1/tb127fischer-bangla.pdf)
* [Bringing world scripts to LuaTeX: The HarfBuzz experiment](https://tug.org/TUGboat/tb40-1/tb124hosny-harfbuzz.pdf)

[LuaTeX]:luatex.org
[luaotfload]: https://github.com/latex3/luaotfload

## Installing Harfbuzz

Make sure [Harfbuzz] libraries and headers are installed. before trying to
install _luaharfbuzz_

#### OS X

Install via [Homebrew](http://brew.sh/)

```
brew install harfbuzz
```
#### Ubuntu Linux

```
apt-get install libharfbuzz0b libharfbuzz-dev
```

#### Other Platforms
_Send a pull request if you want to include specific instructions to install
Harfbuzz on your preferred platform._

Before building the package, LuaRocks populates the `HARFBUZZ_INCDIR` and `HARFBUZZ_LIBDIR` to point to the correct locations. If you can populate these variables manually before running LuaRocks, you can install _luaharfbuzz_ on any system that supports Lua and Harfbuzz.

## Installing _luaharfbuzz_

#### Luarocks
If [Luarocks] and Harfbuzz are installed, _luaharfbuzz_ can be installed like this:

```
luarocks install luaharfbuzz
```

[Luarocks]: https://luarocks.org

## Documentation
* [API Docs](http://harfbuzz.github.io/luaharfbuzz/)
* [Wiki](http://github.com/harfbuzz/luaharfbuzz/wiki)

## Sample Code

Here is some sample code, showcasing the core types and methods in the API.

```lua
local harfbuzz = require('harfbuzz')
local serpent  = require('serpent') -- luarocks install serpent

-- Harfbuzz API Version
print("Harfbuzz API version", harfbuzz.version())

-- Shapers available
print("Shapers:", serpent.line({ harfbuzz.shapers() }, {comment = false}))

-- harfbuzz.Face
local face = harfbuzz.Face.new('../fonts/notonastaliq.ttf')
print('\nFace upem = '..face:get_upem())

-- harfbuzz.Font
local font = harfbuzz.Font.new(face)
local xs, xy = font:get_scale()
print("\nDefault font scale = X: "..xs..", Y: "..xy)

-- harfbuzz.Buffer
local text = "یہ" -- U+06CC U+06C1
local buf = harfbuzz.Buffer.new()
buf:add_utf8(text)

-- harfbuzz.shape (Shapes text)
print("\nShaping '"..text.."' set with Noto Nastaliq Urdu")
harfbuzz.shape(font, buf, { language = harfbuzz.Language.new("urd"), script = harfbuzz.Script.new("Arab"), direction = harfbuzz.Direction.RTL})

local glyphs = buf:get_glyphs()
print("No. of glyphs", #glyphs)
print(serpent.line(glyphs, {comment = false}))

local opts = { language = harfbuzz.Language.new("eng"), script = harfbuzz.Script.new("Latn"), direction = harfbuzz.Direction.LTR }
local amiri_face = harfbuzz.Face.new('../fonts/amiri-regular.ttf')
local amiri_font = harfbuzz.Font.new(amiri_face)

-- shaping '123' w/o features
print("\nShaping '123' set with Amiri Regular and no features")
buf= harfbuzz.Buffer.new()
buf:add_utf8("123")
harfbuzz.shape(amiri_font, buf, opts)
glyphs = buf:get_glyphs()
print(serpent.line(glyphs, {comment = false}))

-- shaping '123' with '+numr' (numerators)
print("\nShaping '123' set with Amiri Regular with 'numr' feature turned on")
buf= harfbuzz.Buffer.new()
buf:add_utf8("123")
opts.features = "+numr"
harfbuzz.shape(amiri_font, buf, opts)
glyphs = buf:get_glyphs()
print(serpent.line(glyphs, {comment = false}))
```

## Development

#### Building
You can build the package for development purposes using LuaRocks as well. It is recommended that you build it to your local tree (using `--local`) to isolate it from your actual installation. For convenience, you can use `make`:

```
make build
```

#### Testing and Linting
In order to make changes to the code and run the tests, the following dependencies need to be installed:

* [Busted](http://olivinelabs.com/busted/) – `luarocks install busted`
* [luacheck](luacheck.readthedocs.org) – `luarocks install luacheck`
* [ldoc](https://stevedonovan.github.io/ldoc/) – `luarocks install ldoc`

Run the test suite:
```
make test
```

Lint the codebase:
```
make lint
```

Generate documentation from sources:
```
make doc
```

## Contact
Open a Github issue, or email me at <deepak.jois@gmail.com>.
