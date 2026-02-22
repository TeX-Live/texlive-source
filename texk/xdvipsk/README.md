# *XDvipsk*: extended Dvips (TeXLive 2026)

It is extension to Tom Rokicki's `dvips`, a `DVI-to-PostScript` translator.  
It requires the `Kpathsea` library for path searching, `freetype` library for OpenType fonts management,  
`libtiff` and `libjpeg` libraries for bitmap images, `Lua` library for callbacks. 

## Base extensions

* a flexible inclusion of bitmap images;
* OpenType fonts support;
* font map `\special` commands with prefixes `mapfile` and `mapline`;
* `Lua` callbacks for `specials`, `drawchar`, `drawrule` and `stack`
* `ToUnicode CMaps` support through adding `GlyphNames2Unicode` dictionary to T1 and OpenType fonts;
* glyph names to unicode map in `\special` commands with prefixes `g2umapfile` and `g2umapline`;
* accepts `GlyphNames2Unicode` dictionary directly from `<FONT_FILE_NAME>.g2u` file;
* supports direct OpenType font gid encoding in `dvi`.

### Development source code repository

> <https://github.com/vtex-soft/xdvipsk.git>

Bug reports, remarks and general discussion are welcome on GitHub.  
Emails to <tex-dev@vtex.lt> are also acceptable. 

### Developers and contributors

> Arūnas Povilaitis† (primary developer of base extension code, initial implementation)   
> Sigitas Tolušis  (extension functionality design, Lua callbacks, and maintenance)  
> Mindaugas Piešina  (font management and base coding)  
> Valentinas Kriaučiukas  (glyphs maps management)  

### More info

- `xdvipsk` man page

> (This can be a bit outdated and needs some updating, just for ideas.)

- [Overview](https://github.com/vtex-soft/xdvipsk/blob/main/README.overview.md)
- [DeveloperNotes](https://github.com/vtex-soft/xdvipsk/blob/main/README.developing.md)

### Notes

Even though it’s stable now, it’s still under development, 
so its interface and functionality could change.

It is successor and minimized version of:

> <https://github.com/vtex-soft/texlive.xdvipsk.git>

See `NEWS` for changes by release, `ChangeLog` for all changes.

xdvipsk is now maintained as part of TeX Live, https://tug.org/texlive.   
Info on building only one program from TL, like this one:  
  https://tug.org/texinfohtml/tlbuild.html#Build-one-package   
TeX Live general build info: https://tug.org/texlive/build.html  
Mailing list for build issues: https://lists.tug.org/tlbuild  

`XDvipsk` is free software.
