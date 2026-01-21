# xdvips(k)+lua+freetype 2024.9 (dvips 2024.1, web2c+kpathsea 6.4.0) (TeXLive 2025/dev)

## New options

    -g  Mode of logging into the file named DVIFILENAME.xdvips.log. Default
        on.  In case of a successful work, the log file contains the only
        message !!!Success!!!.

    -H  32-bit turbo mode for inclusion of PostScript graphics (writes EPS
        files directly to PS file) using 10 MB dynamic buffer. Default `off`.

    -I<pixel-form filters>  Resizing mode for bitmap images included with
        `em:graph` specials. Default `off`.
        <pixel-form filters> is a comma-separated tuple of up til four pairs
        <pixel form>:<filter>, where <pixel form> can be one of:
            'BW':   black/white pixels of 1-bit size
            'GR':   gray (8-bit pixels)
            'RGB':  colored 24-bit pixels
            'CMYK': colored 32-bit pixels
        and <filter> can be one of the following:
            'b':  Box filter
            't':  Bilinear filter
            'B':  B-spline filter
            'm':  Mitchell and Netravali's two-parameter bicubic filter
            'l':  Lanczos-windowed sinc filter
            'c':  Catmull–Rom and Overhauser splines
            'r':  Resample image (remove rows and colums in the bitmap)
            'w1': Windows GDI filter BLACKONWHITE mode
            'w2': Windows GDI filter WHITEONBLACK mode
            'w3': Windows GDI filter COLORONCOLOR mode
            'w4': Windows GDI filter HALFTONE mode.
        Not all <pixel form>:<filter> combinations are possible:
           - filters 'w*' can be used on MS Windows systems only and just for
             BW, GR, and RGB pixel forms; for CMYK any 'w*' filter is replaced
             by 'r' filter
           - on Linux and other systems filters 'w*' are changed to 'r' filter
           - for BW graphics only filters 'r' and 'w*' are applicable

    -I (without filters)  Resizing mode with the following filter tuples:
        BW:w1,GR:w3,RGB:w3,CMYK:r  - on Windows
        BW:r,GR:r,RGB:r,CMYK:r     - on other systems.

    -j  T1 fonts partial download. Default `off` (contrary to dvips).

    -J  Download only needed characters from OT fonts. Default `on`.

    -lua `Lua` script file with optional functions:
         - prescan_specials_callback(special, table)
           table with keys: `hh`, `vv`, `pagenum`
         - scan_specials_callback(special, table)
           table with keys: `hh`, `vv`, `pagenum`
         - after_prescan_callback(table)
           table with keys: `hpapersize`, `vpapersize`, `hoff`, `voff`, `actualdpi`, 
                             `vactualdpi`, `num`, `den`, `mag`, `totalpages`
         - after_drawchar_callback(table)
           table with keys: `charcode`, `cid`, `pixelwidth`, `rhh`, `rvv`, `dir`,
                            `lastfont`, `tounicode`
         - after_drawrule_callback(table)
           table with keys: `hh`, `vv`, `dir`, `rw`, `rh`
         - process_stack_callback(table)
           table with keys: `cmd`, `hh`, `vv`, `dir`

    -W  Extended search mode for image files indicated by `em:graph` specials:
        when no file with the specified name is found, the file names with
        other extensions - `.pcx`,`.bmp`,`.tif`,`.jpg`,`.png` - are tried.
        Default `off`.

    -noluatex  Mode of disabled LuaTeX extensions. Switches off support of
        OpenType fonts. Default `on`.

    -noToUnicode  Mode without generation of maps (to Unicode) files for OT
        fonts, that can be used by Acrobat Distiller to enable text search.
        Default `on`.

    -Q  Mode of skipping VTeX specials: any content of \special commands prefixed
        with `mt:`, `vtex:`, `MC:`, `BMC:` or `EMC:` is ignored without an
        error message. Default `off`.

## New features

  *  `BMP`, `PCX`, `TIFF`, `JPEG`, `PNG` files (`BW`, `Gray`, `RGB`, `CMYK`, Indexed `RGB`),
     indicated in `TeX` special with prefix `em:graph`, are included in the
     generated `PS` file in the right way

  *  the parameter `HiResBoundingBox` is inserted in the `PS` file

  *  do not stop on unknown `\special`

  *  accepts font map `\special` commands with prefixes `mapfile` and `mapline`
     (`pdftex` primitive analoques; valid for OpenType fonts also)

  *  `Lua` callbacks for `dvi` specials, `drawchar`, `drawrule` and `stack`

  *  inserts `GlyphNames2Unicode` dictionary to `FontInfo` dictionary for T1 and OpenType fonts 

  *  accepts glyph names to unicode `\special` commands with prefixes `g2umapfile` and `g2umapline`

  *  accepts `GlyphNames2Unicode` dictionary directly from `<FONT_FILE_NAME>.g2u` file

  *  supports `dvi` OpenType font gid encoding with appropriate mapline and activated by
     `\special{vtex:settings.xdvipsk.opentype={enc=gid}}`
     
## Dependencies

```
    kpathsea (from TL tree)
    lua53 (from TL tree)
```

### Extra library for OpenType font handling

```
    freetype2 (from TL tree)
```

### Extra libraries necessary for graphic handling

```
    libpng (from TL tree)
    zlib (from TL tree) 
    jpeglib - version 9.2.0
    libtiff - version 4.7.0
```

### Adapted libraries

```
    graflib -- simplified and adapted code from the FreeImage library
    otflib  -- adapted code from dvipdfmx
```

## New modules

```
    charcode.c
    emspecialex.c
    glyphlist_table.h (generated from glyphlist_table.lua)
    glyphlist_table.lua
    glyphmap.c
    glyphmap.h
    luamap.c
    sfntload.c
    writecid.c
    utarray.h
    uthash.h
    texcid.lpro - PostScript procset with comments
    xdvipsk_tounicode.h (generated from xdvipsk_tounicode.lua)
    xdvipsk_tounicode.lua
```

## Removed modules

```
    afm2tfm.c
    emspecial.c
    flib.c (to be failed with C preprocessor macro FONTLIB defined)
```

## Changed modules

All changes are identified by the `C` preprocessor macro `XDVIPSK` definition:

```
    \#ifdef XDVIPSK
    \#endif
```

## Development

Source code repository at:

> <https://github.com/vtex-soft/xdvipsk.git>

Email bug reports, remarks, etc. to <tex-dev@vtex.lt>.

## Remarks

It is successor and minimized version of:

> <https://github.com/vtex-soft/texlive.xdvipsk.git>

