# Gregorian Chant Fonts

Gregorio uses .ttf fonts to represent the various glyphs in Gregorian chant.  While there are hundreds of unique glyphs when you look at the entire body of Gregorian chant, these glyphs are usually composites of a relatively small number of components.  Rather than manage the huge number of glyphs, therefore, this project maintains the smaller component set for its fonts and then assembles these components into the various glyphs as part of the build process.  This minimizes the work load for the developers while still enabling customization at the glyph (rather than the component) level within documents created with Gregorio.

Currently there are three fonts maintained as part of this project:

 * Greciliae, an adaptation of [Caeciliae](http://marello.org/caeciliae/) for use with Gregorio
 * Gregorio, the original font produced as part of this project
 * Grana Padano, an adaptation of Parmesan from [Lilypond](http://www.lilypond.org/index.html)

Each of these fonts comes with separate file for solid, hollow, and hole glyphs (holes are the "filling" in a hollow glyph which prevent an object behind the hollow glyph from showing through).

Each file also has an "-op" variant in which certain glyph shapes are tweaked to be more like the shapes found in specifically Dominican chant collections.

## Font Packaging

Greciliae (in all its varieties) is packaged as the default font and is installed when the package itself is installed using any of the available methods.

Gregorio and Grana Padano (in all their varieties) are distributed separately in `supp_fonts-$GREGORIOVERSION.zip` (fonts are version specific and will only work with the matching version of Gregorio).

## Font generation

 The fonts are distributed pre-built, but if you really need to compile them:

 * install [fontforge](http://fontforge.github.io) with python extension
 * run `make fonts` (Greciliae only) or `make really-all-fonts` (all three fonts) in this directory (using the `-j` argument can save you some time here)
 * then you can test them directly, or install them (see next section)

## Font installation

Greciliae is installed automatically by all the normal installation methods (see [Installation](http://gregorio-project.github.io/installation.html) and follow the link to the instructions specific to your platform).

`supp_fonts-$GREGORIOVERSION.zip` comes with a Lua script (`install_supp_fonts.lua`) which can be used to install Gregorio and Grana Padano.  To use this script, unzip the archive, open Terminal (or Command Prompt, if you are on Windows), change to the directory of the decompressed archive, and then run `texlua install_supp_fonts.lua`.  This command can take an argument indicating where you want the fonts to go:

 * `auto` (optional): the same folder as Greciliae
 * `system`: the appropriate font folder in `$TEXMFLOCAL`
 * `user`: the appropriate font folder in `$TEXMFHOME`
 * `<dir>`: the name of an alternate texmf root directory you want to use

Additionally, if you are building from a git clone, `install-gtex.sh` and `install_supp_fonts.lua` will install all the fonts you have built, not just the ones they are normally distributed with.

**Note:** All the above methods assume you only need to access the fonts from within a TeX document.  If you want to use the fonts in other programs, then you will need to consult the documentation appropriate to your platform and manually move, copy, or link the fonts to the necessary location.

## Stem length

Stem length are entirely configurable in gregorio, but are set during the time of font creation, so you cannot change it without modifying the font.

You can change stem length schema in a font when building it, by passing the `-sc` option to `squarize.py`, followed by the stem length schema you want your font to get. Currently available schemas are

- default, adapted from the 1934 Antiphonale Monasticum, see comments in [stemsschema.py](stemsschema.py) for more information
- solesmes, a schema provided by the Abbey of Solesmes

If you want a version of a font built with Solesmes' schema, or would like to see a new schema implemented, please contact the developers.

As an example, here is how to use greciliae with solesmes schema:

- cd in the `font/` directory (this one)
- run `fontforge -script squarize.py greciliae-base.sfd -o greciliae-solesmes.ttf -c greciliae.json -n greciliae-solesmes -sc solesmes`
- it will produce `gregorio-solesmes.ttf`, copy it in the same place as `greciliae.ttf`
- use it in your TeX file: `\gresetgregoriofont{greciliae-solesmes}`

# Ancient notation fonts

The file [gregall.sfd](gregall.sfd) contains the images on which the author has drawn the fonts. The images come from:

- St. Gallen, Stiftsbibliothek (http://www.e-codices.unifr.ch)
- Einsiedeln, Stiftsbibliothek (http://www.e-codices.unifr.ch)

and are published here with the consent of these two institutions, which we thank. You can see the source for each glyph in [GregorioNabcRef](../doc/GregorioNabcRef.tex). Anyone wanting to improve/fork the font can base on this file, at the condition to release the new font under the [SIL Open Font License](http://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=OFL).

# The case of Gregoria

Support for Gregoria has been dropped.

[Gregoria](http://www.anatoletype.net/projects/gregoria) cannot be used by Gregorio directly (although it was the primary goal of Gregorio when it was created). As the font is not free, it's not possible to use the same process as [Caeciliae](http://marello.org/caeciliae/), because it would require the distribution of a modified version.
