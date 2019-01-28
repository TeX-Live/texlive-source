LCDF Typetools NEWS
===================

## Version 2.108 – 27.Jan.2019

* Handle more fonts.

* `otftotfm -a`: Run `updmap-sys` by default.


## Version 2.107 – 22.Feb.2018

* Some corrections to output of `--math-spacing`.

* Reduce undefined behavior.

* Handle some incorrect fonts more gracefully.


## Version 2.106 – 21.Jun.2016

* Minor updates, mostly involving licensing of Adobe code and data.


## Version 2.105 – 15.Sep.2015

* Several crash fixes.

* `otfinfo -g`: Print all Unicode mappings for a glyph.

* t1lint: Support counter control hints.

* Thanks to Github issue contributors.


## Version 2.104 – 7.Jul.2014

* Fix a nit with format-1 chaining context substitutions.


## Version 2.103 – 6.Jul.2014

* otftotfm: Address a problem experienced by Bob Tennent where a
  ligature setting wasn't included in the output VPL.


## Version 2.102 – 4.Jul.2014

* otftotfm: Fix the construction of multi-character secondary
  replacements; they were given enormous widths. Reported by Wydra
  Dennis.


## Version 2.101 – 16.Jun.2014

* otftotfm: Handle fonts with unexpected transformation matrices,
  such as CFF fonts with 2048 units per em.

* otftotfm: Add `--x-height` option.

* otftotfm: Handle more TrueType fonts by fixing otftotfm bugs and
  compensating for some TTF bugs.

* Thanks to Bob Tennent and Marc Penninga.


## Version 2.100 – 8.Jan.2014

* otftotfm: On some fonts, ligature handling could enter an infinite
  loop. Fix this. Reported by Marc Penninga and Bob Tennent.

* Fix updmap runs.

* Build updates for C++11 and other compilers.


## Version 2.99 – 13.Aug.2013

* cfftot1: Correct bug that rarely corrupted Type 1 output fonts.
  Reported by Sebastian Schubert.


## Version 2.98 – 10.Apr.2013

* Fix alignment bug that corrupted output fonts on some platforms
  (MIPS especially). Reported by Norbert Preining.


## Version 2.97 – 25.Oct.2012

* Same.


## Version 2.96 – 25.Oct.2012

* Build improvements inspired by Peter Breitenlohner.


## Version 2.95 – 21.Sep.2012

* Fix cfftot1: Don't crash on problematic fonts. Bob Tennent report.


## Version 2.94 – 3.Aug.2012

* More compilation updates.


## Version 2.93 – 23.Jul.2012

* Correct compilation problem reported by Dennis Veatch.

* Update OpenType feature, script, and language lists, and some other nits.


## Version 2.92 – 14.Aug.2011

* Correct horrible x86-64 byte-order issue.  Bug reported by Michael Ummels.


## Version 2.91 – 13.Jun.2011

* Correct some compilation problems on unusual systems reported by Vladimir
  Volovich.


## Version 2.90 – 3.Jun.2011

* Correct crash on glyph names that start with a dot.  Marc van Dongen
  report.


## Version 2.89 – 2.Jun.2011

* Better handle combinations of fonts and options that would create so many
  substitutions that otftotfm would run out of memory.  Marc van Dongen
  report.

* Thanks to Peter Breitenlohner.


## Version 2.88 – 8.Mar.2011

* Warning reduction.


## Version 2.87 – 27.Jan.2011

* Avoid double-free error in t1testpage.  Reported by Kurt Pfeifle.


## Version 2.86 – 16.Dec.2010

* Support version 4 of the OpenType OS/2 table.  Reported by Thomas
  Zumbrunn.


## Version 2.85 – 26.Sep.2010

* Correct otftotfm bug with glyphlist.txt mappings like I_J.


## Version 2.84 – 15.Sep.2010

* otftotfm: Avoid crashes on very large fonts and other odd situations.
  Bugs reported by Martin Schröder.

* Include new Unicode mappings for glyph names from MSAM and MSBM.

* Bug fixes to texglyphlist.txt Unicode mappings.  Thanks to Clea F. Rees.


## Version 2.83 – 23.Apr.2010

* otftotfm: If `-fkern` loads no GPOS lookups, apply the TTF kern table (if
  it exists).  This should use the TTF kern table strictly more often than
  the previous version.  Based on draft changes to the OpenType spec.

* Update with changes from TeX Live.  Thanks to Peter Breitenlohner.


## Version 2.82 – 19.Jan.2010

* otftotfm: Correctly handle multiply-encoded glyphs (for example, if "j"
  ends up encoded in more than one slot, apply the j => j.smcp feature to
  both occurrences).  Reported by Michael Ummels.


## Version 2.81 – 7.Jan.2010

* otftotfm: Fix "em" and "ex" measurements (QUAD and XHEIGHT font
  dimensions) for TrueType fonts.  Problem reported by Nico Schlömer.

* otftotfm: Add `--type42` option.


## Version 2.80 – 10.Nov.2009

* otftotfm: Add `--fixed-width`, `--proportional-width`, and `--italic-angle`
  options.  Requested by Karl Berry.


## Version 2.79 – 12.Jun.2009

* otftotfm: Use TrueType "kern" tables to satisfy the `-fkern` feature if
  GPOS information isn't available.  Requested by Nico Schlömer.

* ttftotype42: Split very large non-glyf tables, as well as very large glyf
  tables.  Requested by Mark DeVries.

* otfinfo: The `-T` option dumps a table's contents to standard out.

* Introduce and use a consistent hashcode_t type.  Reported by Karl Berry.


## Version 2.78 – 6.Apr.2009

* t1lint: Report warnings when a font charstring command has too many
  arguments.  Requested by Han The Thanh.


## Version 2.77 – 6.Apr.2009

* Font library changes: correctly implement binary search in a couple
  places -- unexpected sizes could lead to overflow and bad behavior.
  Reported by Mark DeVries for ttftotype42 and CharisSILR.


## Version 2.76 – 27.Mar.2009

* mmpfb: Ensure all output subroutines end in the "return" command; some
  type 1 processors treat a subroutine ending in "endchar" as an error.
  Reported by Melissa O'Neill.

* mmpfb, cfftot1: Ensure that stem3 hints (hstem3, vstem3) meet the
  necessary constraints, even despite rounding.

* t1lint: Add checks for some more problems, such as subroutines that do
  not end in "return."  Reported/requested by Melissa O'Neill and Han The
  Thanh.


## Version 2.75 – 22.Feb.2009

* Report a helpful warning if automatic mode is specified to a version
  without kpathsea support.  Question from Keith Briggs.


## Version 2.74 – 16.Feb.2009

* Correctly detect <unistd.h>.  Problem reported by C.M. Connelly.


## Version 2.73 – 15.Feb.2009

* cfftot1 bug fix: Avoid introducing stray "rmoveto" commands and attendant
  visual artifacts into the output.  The problem was caused by some
  coordinate system confusion.  Reported by John Owens, who had tried to
  convert Inconsolata.


## Version 2.72 – 27.Oct.2008

* t1rawafm: Add missing newlines; thanks, Michael Zedler.

* otftotfm: Understand "UniXXXX" glyph names.  They're not standard, but
  it's pretty obvious what they imply.  Reported by Vasile Gaburici.


## Version 2.71 – 8.Aug.2008

* otftotfm: Correctly install TrueType files when `--force`.  Reported by
  Vasile Gaburici.


## Version 2.70 – 8.Aug.2008

* Add new t1rawafm program, which generates a "raw" (kernless and
  ligatureless) AFM file given a font file (PFB/PFA).

* otftotfm: When installing, `--automatic` ignores files existing in the
  current directory.  Confusion reported by Vasile Gaburici.

* Type 1 parsing: Parse fonts with rare encoding formats, such as bases
  other than 10.


## Version 2.69 – 5.May.2008

* Mini portability fix for problem reported by Vladimir Volovich.


## Version 2.68 – 2.May.2008

* otftotfm: Tweak the ligature sorting algorithm.  Prefer lowercase
  ligatures to mixed-case and uppercase ligatures; prefer the conventional
  f-ligatures to all others.  Reported by Ulrich Dirr.


## Version 2.67 – 25.Apr.2008

* otftotfm: Improve font name construction for base fonts.  Reported by
  Ulrich Dirr.


## Version 2.66 – 3.Aug.2007

* otftotfm: TrueType fonts had inappropriately large kerning pairs; fixed.
  Reported by Marc Penninga.


## Version 2.65 – 22.Jul.2007

* t1testpage: Fix bug triggered by fonts with empty encoding slots.
  Reported by Michael Zedler.


## Version 2.64 – 25.Jun.2007

* Handle Extension format GPOS and GSUB lookup tables, used for very large
  fonts.  Reported by Marc Penninga.

* cfftot1, t1lint: Correct misunderstanding of the Flex spec: there MUST be
  an rmoveto in between any two Flex subroutines, even if that rmoveto
  doesn't move anywhere.  (Question: Are hmoveto/vmoveto acceptable?  Adobe
  Reader seems to think so.)  Reported by John Owens with respect to
  Caslon-Antique.


## Version 2.63 – 12.Jun.2007

* Correctly handle fonts with 4-byte character code cmap tables.  Reported
  by Alexey Vikhlinin.


## Version 2.62 – 11.Apr.2007

* t1dotlessj: Do not use the same UniqueID as the input font.  Inspired by
  Reinhard Kotucha.

* otftotfm: Generate a map line even if there was a missing character in
  some UNICODING.  Reported by Andreas Bühmann.


## Version 2.61 – 25.Mar.2007

* otftotfm: Automatically reduce DESIGNUNITS and try again if a font has
  humungocharacters that overflow PL files' limited range of allowed real
  numbers.  Reported by John Owens.


## Version 2.60 – 27.Feb.2007

* otftotfm: Rearrange order of virtual fonts so most-frequently-used font
  comes in position 0.  Requested by Michael Zedler.

* otftotfm: Apply letterspacing to "dotlessj".  Maybe finally this is right.


## Version 2.59 – 23.Feb.2007

* otftotfm: %POSITION commands don't add glyphs to the encoding.  Requested
  by Achim Blumensath.


## Version 2.58 – 22.Feb.2007

* otftotfm: More of the same (with respect to "Germandbls"), again reported
  by Michael Zedler.


## Version 2.57 – 21.Feb.2007

* otftotfm: More of the same: "emptyslot" glyphs don't generate spurious
  base fonts.  Again reported by Michael Zedler.


## Version 2.56 – 13.Feb.2007

* otftotfm: Guess what?


## Version 2.55 – 13.Feb.2007

* otftotfm: Finally fix (?) letterspacing for simulated characters.


## Version 2.54 – 12.Feb.2007

* otftotfm: Include letterspacing for simulated characters.  Bug reported
  by Michael Zedler.


## Version 2.53 – 11.Feb.2007

* otftotfm: Positionings and letterspacing apply even with
  `--base-encodings`.  Bug reported by Michael Zedler.


## Version 2.52 – 6.Feb.2007

* otftotfm: `--base-encodings` fixes for dotless-J fonts.


## Version 2.51 – 6.Feb.2007

* otftotfm: Some `--base-encodings` fixes.


## Version 2.50 – 6.Feb.2007

* otftotfm: Add `--base-encodings` option.  Requested by Michael Zedler.


## Version 2.49 – 11.Jan.2007

* Report certain types of invalid 'size' features that occasionally occur
  in old fonts.  Thanks to John Owens and Read Roberts.


## Version 2.48 – 11.Dec.2006

* Translate font names into UTF-8 encoding.  Reported by John Owens.


## Version 2.47 – 10.Dec.2006

* otfinfo: Add new names, such as "Preferred Family", to `otfinfo -i` output.
  Requested by John Owens.


## Version 2.46 – 29.Oct.2006

* otftotfm: Base metrics files now reflect the actual base font metrics
  more accurately (rather than containing virtual-font-only metrics
  information, such as letterspacing).  This should make it easier to tell,
  using "diff", whether two base metrics files contain the same data.

* otftotfm: `--no-type1` does not affect dotless-j font generation; use
  `--no-dotlessj` for that (Michael Zedler).

* otftotfm: Don't generate virtual fonts unless you have to (Michael
  Zedler).


## Version 2.45 – 17.Sep.2006

* Include new ttftotype42 program.


## Version 2.44 – 16.Sep.2006

* otfinfo/otftotfm: Support new fonts whose 'size' features are defined
  correctly.  Thanks to Read Roberts for defining the compatibility check.


## Version 2.43 – 22.Aug.2006

* otftotfm: Check for availability of kpse_opentype_format.  Bug reported
  by Carsten Luckmann.


## Version 2.42 – 22.Aug.2006

* otftotfm: Any `--altselector-char` is actually encoded, so you can use it as
  a regular character as well as an altselector.  Suggested by Carsten
  Luckmann.


## Version 2.41 – 12.Aug.2006

* otftotfm: Some TrueType fixes.  Problems reported by Michael Zedler.


## Version 2.40 – 1.Aug.2006

* otftotfm: Initial support for TrueType-flavored OpenType fonts.  Inspired
  because John Owens is working with Microsoft's Calibri et al.


## Version 2.39 – 11.Jul.2006

* cfftot1: Unify some subroutines previously missed, for slightly smaller
  output.  Reported by Michael Zedler.


## Version 2.38 – 8.May.2006

* otfinfo: Add `--info` option.  Requested by John Owens.


## Version 2.37 – 25.Jan.2006

* otftotfm: Add 'ringfitted'.  Requested by Michael Zedler.  Required bug
  fix.


## Version 2.36 – 9.Nov.2005

* otfinfo: Add `-a/--family` option.  Based on patch from Ottavio G. Rizzo.

* otftotfm: Fix bug where `-q` would inhibit map line output, reported by
  Achim Blumensath.


## Version 2.35 – 3.Oct.2005

* otftotfm: Allow `--ligkern "T h=:T_h"` (note lack of spaces around '=:').
  Inspired by Michael Saunders.

* otftotfm: Split TeX extensions out from 'glyphlist.txt' into
  'texglyphlist.txt', leaving 'glyphlist.txt' exactly as distributed by
  Adobe.  Otftotfm reads both files.  Requested by Werner Lemberg.

* otftotfm: Add `% WARNMISSING` and `--warn-missing`, so that missing
  characters result in blots and cause warnings when processed by dvips.
  Requested by Michael Zedler.

* otfinfo: Add `-v/--font-version` option and document `-t/--tables` option.

* t1reencode: Add many more standard encodings to t1reencode, and fix
  existing ones.  Patch from Peter Betzler.


## Version 2.34 – 11.Jun.2005

* otftotfm: Search for 'glyphlist.txt' using kpathsea, and set kpathsea
  program name to 'lcdftools'.  Requested by Karl Berry.


## Version 2.33 – 3.Jun.2005

* otftotfm: Include more secondary replacements, including double-bar,
  centered asterisk, per-ten-thousand, and so forth.  Patch from Michael
  Zedler.

* otftotfm: Add `% POSITION` and `--position`.  Requested by Michael
  Zedler.


## Version 2.32 – 31.May.2005

* otftotfm: Report correct dimensions for characters only in the base
  encoding.  Reported by Michael Zedler.


## Version 2.31 – 30.May.2005

* otftotfm: Fix crash tickled by constructed characters, reported by
  Michael Zedler.

* Compilation fixes suggested by Nelson H.F. Beebe.


## Version 2.30 – 8.May.2005

* otftotfm: Add support for 'dblbracketleft' and 'dblbracketright'.
  Secondary replacement can add characters to the font.

* otftotfm: Ligkern commands 'A {5} B' and 'A {L} B' can coexist, leading
  to a 5-character kern and no ligature.  Requested by Michael Zedler.


## Version 2.29 – 7.May.2005

* otftotfm: Fix crash reported by Ryuji Suzuki.

* otftotfm: Add support for 'capitalcompwordmark' and
  'ascendercompwordmark', requested by Michael Zedler.

* otftotfm: Use OpenType OS/2 table to determine x-height, if available.


## Version 2.28 – 25.Apr.2005

* otftotfm: `--math-spacing` doesn't set italic corrections to 0.  Thanks
  to Achim Blumensath.


## Version 2.27 – 24.Apr.2005

* otftotfm: Add support for setting kerns: `--ligkern "A {5} B"`.  Thanks
  to Achim Blumensath for a patch.

* otftotfm: Add preliminary support for heuristically-derived math accent
  positions via a 'skewchar' argument to `--math-spacing`.  Thanks again to
  Achim Blumensath.


## Version 2.26 – 2.Apr.2005

* otftotfm: Support more kinds of substitution.

* otftotfm: Support old-style and new-style chaining context substitutions
  using Adobe's procedure.  Older fonts had erroneous substitutions because
  of a software error; newer fonts don't.

* otftotfm: Fix crash tickled by newer versions of MinionPro and other
  fonts, reported by Michael Zedler and Oliver M. Haynold.


## Version 2.25 – 10.Mar.2005

* otftotfm: Ignore unencoded default ligkerns (don't try to encode their
  characters).  Reported by Michael Zedler.


## Version 2.24 – 10.Mar.2005

* otftotfm: Fix assertion failure introduced in 2.23.


## Version 2.23 – 8.Mar.2005

* otftotfm: Fix bug present since 2.20 where, for example, `--unicoding
  "germandbls =: SSsmall"` was ignored in favor of a named 'germandbls'
  character.  Reported by Michael Zedler.


## Version 2.22 – 2.Mar.2005

* otftotfm: Include default ligatures unless `--no-default-ligkern` is
  given.  This seems cleaner than the previous semantics (which included
  the default ligatures unless there were ligatures in the encoding and/or
  the command line), but it is incompatible.  Inspired by question from
  Christopher Swingely.


## Version 2.21 – 16.Feb.2005

* otftotfm: Base fonts include no kerns or ligatures.  Requested by Michael
  Zedler.

* cfftot1: Correctly handle default values for CFF fonts, so that, for
  example, isFixedPitch is defined to false even when the font doesn't
  mention it.  Requested by Huver.


## Version 2.20 – 9.Feb.2005

* otftotfm: Add `--space-factor` and `--math-spacing` options, based on
  patches from Achim Blumensath.

* otftotfm: Improve handling of explicit `--ligkern` ligatures: they override
  default ligatures, and any characters mentioned are shoehorned into the
  encoding.  Catalyzed by Michael Zedler.


## Version 2.19 – 4.Feb.2005

* otftotfm: Add `--subs-filter`, `--include-subs`, `--exclude-subs`, and
  `--clear-subs` options (inspired by patch from Achim Blumensath).

* otftotfm: Update documentation and behavior for newer teTeX
  installations.  For instance, run the system `updmap` by default (unless
  you give the `--no-updmap` option).  This makes automatic mode much
  easier to set up.

* otftotfm: Output pltotf and vptovf messages to standard error (Achim
  Blumensath).

* otftotfm: Protect arguments given to the shell (Achim Blumensath).

* otfinfo: Add `-g` option to query all glyphs in a font.


## Version 2.18 – 26.Jan.2005

* otftotfm: Fix bug that could cause infinite loops on FreeBSD machines.


## Version 2.17 – 4.Jan.2005

* Add t1reencode program.  Requested by Ralph Aichinger.

* otftotfm: Add `--output-encoding` option.

* t1lint: Check that UniqueID and XUID values are in range.


## Version 2.16 – 19.Nov.2004

* `--include-alternates` and `--exclude-alternates` options only apply to
  features that appear later in the options list.

* Fix bug where not all `--altselector-feature` features would be used.
  Problem reported by Emil Lohse.

* Add `--clear-alternates` option.


## Version 2.15 – 21.Sep.2004

* AIX compile fixes.  Reported by Vladimir Volovich.

* Include RPM .spec file, provided by C.M. Connelly.


## Version 2.14 – 16.Sep.2004

* Use AM_MAINTAINER_MODE.  Suggested by Karl Berry.

* IRIX compile fixes.  Reported by Olaf Weber.


## Version 2.13 – 12.Sep.2004

* otftotfm: Check $TEXMF if $VARTEXMF has no writable directory.  Reported
  by Simon Patarin.

* Add `--enable-selfauto-loc` configuration option, enabled by default, to
  help otftotfm run with TeX configuration files that use SELFAUTODIR and
  related variables.  Suggested by Thomas Esser; mechanism from dvipng.


## Version 2.12 – 19.Aug.2004

* Catch bug that would affect otftotfm on 64-bit machines.


## Version 2.11 – 18.Aug.2004

* otftotfm: Adjust TDS 1.1 support thanks to Olaf Weber.


## Version 2.10 – 18.Aug.2004

* otftotfm: Add preliminary support for the TeX Directory Structure 1.1
  standard.

* Bug fix that should let us compile on Cygwin.  Problem reported by
  Christian Gudrian.


## Version 2.9 – 10.Aug.2004

* otftotfm: Add `--letter-feature` option, inspired by Michael Zedler.


## Version 2.8 – 5.Aug.2004

* otftotfm: Fix bug where missing GPOS or GSUB tables would cause an abort.
  Reported by Ryuji Suzuki.


## Version 2.7 – 3.Aug.2004

* otftotfm: Bug fix for obscure cases involving 'dotlessj' characters: the
  output VPL could contain a reference to "(SETCHAR X)", which is illegal.
  Reported by Marco Kuhlmann.


## Version 2.6 – 12.Jul.2004

* t1testpage adds preliminary support for font smoke proofs with `--smoke`.
  Requested by Karl Berry.

* t1testpage adds `--glyph` option.


## Version 2.5 – 6.Jul.2004

* Support compilation with gcc-3.4.1.  Reported by Thomas Esser.


## Version 2.4 – 30.Jun.2004

* cfftot1: Fix off-by-one bug where the encoding of the last encoded
  character was ignored, reported by Detlev Droege.

* otftotfm: Some internal changes; bugs are possible.


## Version 2.3 – 15.Jun.2004

* otftotfm: Add `--default-ligkern` and `--no-encoding-commands` options,
  to address problem with "t1.enc" reported by Ulrich Dirr.

* otftotfm documentation updates.


## Version 2.2 – 8.May.2004

* otftotfm: 't1dotlessj' errors don't prevent 'psfonts.map' from being
  updated.  Reported by Stephen Moye.

* t1dotlessj: Report different kinds of errors with different exit statuses.


## Version 2.1 – 5.Apr.2004

* otftotfm: Warn if no encoding specified.  Requested by Zsolt Kiraly.

* otftotfm: Improve documentation, particularly by adding example run with
  ".fd" file.


## Version 2.0 – 21.Mar.2004

* mmpfb, t1dotlessj, t1lint, t1testpage: On Windows, add _O_BINARY flag
  when appropriate. Reported by Fabrice Popineau.


## Version 1.99 – 24.Feb.2004

* otftotfm: Fix overfull encoding bug that could cause an assertion
  failure, reported by Adam Lindsay.

* otftotfm: Speed improvement.


## Version 1.98 – 22.Feb.2004

* otftotfm: Add support for 'SSsmall' glyph. Einar Smith noted that the
  OpenType 'smcp' feature doesn't translate the sharp-S character to
  small-caps "SS" in most fonts. Now, add `--unicoding "germandbls =:
  SSsmall"` to get that behavior.

* otftotfm: Compilation fix for problem reported by Nelson H.F. Beebe.


## Version 1.97 – 6.Feb.2004

* cfftot1: Fix bug in handling fonts with supplemental encodings. Problem
  reported by Eike <eiked>.


## Version 1.96 – 11.Jan.2004

* otftotfm: Handle alternate characters like 'Q.alt' in the input encoding:
  map them to the actual alternate glyph, rather than to the base Unicode
  value ('Q'). Reported by Ulrich Dirr.
 

## Version 1.95 – 3.Jan.2004

* otftotfm: Add automatic support for t1dotlessj. If the desired encoding
  has a 'dotlessj' character, and the input font doesn't, then otftotfm
  will run 't1dotlessj', create a dotless-J font, and include it using
  virtual fonts.

* otftotfm: Avoid warnings about bad "(STOP)" commands in pltotf
  (introduced by the `--min-kern` facility).


## Version 1.91 – 31.Dec.2003

* mmpfb, t1dotlessj: Fix sidebearing problems.


## Version 1.90 – 29.Dec.2003

* mmpfb: Fix behavior with Adobe Jenson and other fonts with intermediate
  masters, and `--minimize` output, which had been broken since 1.65.

* Add t1dotlessj program.


## Version 1.88 – 23.Dec.2003

* otfinfo: Change coding to be friendlier to older C++ compilers. Requested
  by Ulrich Dirr.


## Version 1.87 – 22.Dec.2003

* otftotfm: Fix `--extend` bug reported by Ulrich Dirr (the expansion factor
  was formerly applied to character heights and depths, not widths).


## Version 1.86 – 19.Dec.2003

* otftotfm: Change where `--base` is added to support pdftex: If the font
  name is "WarnoPro+10", the base font name is "WarnoPro--base+10".
  Reported by Ulrich Dirr.


## Version 1.85 – 10.Dec.2003

* otftotfm: Add `--altselector-feature` option, so you can specify the
  features activated by `--altselector-char`.  Defaults to salt and dlig.

* otftotfm: Fix intermittent hang.


## Version 1.80 – 4.Dec.2003

* otftotfm: Add `--altselector-char` option, and `--include-alternates` and
  `--exclude-alternates` options.  These options support access to alternate
  characters through ligatures, using a mechanism originally planned by Sivan
  Toledo ("Exploiting Rich Fonts", TUGboat 21(2), 2000).  Requested by Martin
  Budaj.

* Add default encoding for 'SS' character (same as 'Germandbls'). Reported
  by Ulrich Dirr.


## Version 1.75 – 3.Dec.2003

* cfftot1: Handle the case when 'hintmask' is the first operator in a Type 2
  charstring. Reported by Tom Kacvinsky.


## Version 1.70 – 1.Dec.2003

* otftotfm: Add `--min-kern` option requested by Ulrich Dirr.

* mmafm: Add `--min-kern` as a preferred synonym for `--kern-precision`.


## Version 1.67 – 29.Nov.2003

* otftotfm: Don't run off the end of an array. Bug reported and patch
  provided by Akira Kakuto.


## Version 1.66 – 24.Nov.2003

* cfftot1: Don't generate "currentfile eexec" twice. Bug reported by Adam
  Lindsay.


## Version 1.65 – 24.Nov.2003

* otftotfm: Include default ligatures if user does not specify ligatures.
  Requested by Adam Lindsay.

* otftotfm: Generated VPL includes FONTDSIZE when referring to base. Bug
  reported by Adam Lindsay.

* Preliminary support for CID-keyed OpenType fonts.


## Version 1.60 – 7.Oct.2003

* cfftot1: Support fonts using the 'seac' operator. Requested by Ralf
  Koenig.


## Version 1.52 – 4.Sep.2003

* otftotfm: Add `--design-size` option requested by Johannes Kuester.

* t1testpage: Fix `--help` and add manual page at C.M. Connelly's request.


## Version 1.51 – 2.Sep.2003

* otftotfm: Generated TFM and PL files have DESIGNSIZE set to the font's
  actual design size, as read from the 'size' feature. Requested by
  Johannes Kuester.


## Version 1.50 – 26.Aug.2003

* Minor compilation fix; problem reported by Nelson H.F. Beebe.


## Version 1.50b4 – 26.Aug.2003

* otfinfo: Minor compilation fix from Tom Kacvinsky.


## Version 1.50b3 – 25.Aug.2003

* cfftot1: Emit UniqueID as an integer. Reported by Tom Kacvinsky.

* otftotfm: `--without-kpathsea` fix. Patch provided by Adam Lindsay.

* More compilation fixes provided by Tom Kacvinsky.


## Version 1.50b2 – 24.Aug.2003

* otfinfo: New program, requested by Adam Lindsay.

* otftotfm: The `--query-scripts` and `--query-features` options are no
  longer supported. Use `otfinfo -s` and `otfinfo -f` instead.

* otftotfm: Fix crash reported by Adam Lindsay.

* otftotfm: In automatic mode, after modifying a 'psfonts.map' file, run
  the script 'TEXMF/dvips/updmap' if present.

* Compilation fixes for Solaris provided by Tom Kacvinsky and Nelson
  H.F. Beebe.


## Version 1.50b1 – 20.Aug.2003

* Integrate mminstance (the mmafm and mmpfb programs), and bump the version
  number to reflect this.


## Version 0.53 – 10.Aug.2003

* Fix problem with Type 1 output in `--without-kpathsea` reported by Adam
  Lindsay.

* Fix crash on encodings containing 'emptyslot' reported by Marco Kuhlmann.


## Version 0.52 – 7.Aug.2003

* Attempt to fix some build problems reported by Nelson H.F. Beebe.


## Version 0.51 – 5.Aug.2003

* otftotfm: Fix crashes with small encodings and absent boundary
  characters. Reported by Bruce D'Arcus.

* otftotfm: Add `--boundary-char` and `--kpathsea-debug` options.


## Version 0.50 – 4.Aug.2003

* otftotfm: Handle more complex substitutions, such as those required to
  support `-fordn` and `-ffrac`.

* otftotfm: The output virtual and base fonts can have different
  "encodings" with overlapping encoding slots. This can make fonts more
  compact.

* When assigning slots to introduced characters, otftotfm prefers
  characters introduced by earlier lookups. This follows the spirit of the
  OpenType specification, since early lookups in some ways "override" later
  ones. The previous scoring mechanism remains in force within each
  individual lookup.

* otftotfm: Rewrite GsubEncoding to Metrics, changing its fundamental
  abstraction (to two-ligatures). Simpler and cleaner overall.

* otftotfm TODO: Ligatures that apply to middle or right context (for
  example, the two substitutions "a b c d => a b c y" and "b c => x" should
  combine to "a b c d => a x y", but they won't yet).


## Version 0.19 – 30.Jul.2003

* otftotfm: Add support for 'emptyslot' UNICODINGs. Requested by Marco
  Kuhlmann.


## Version 0.18 – 9.Jul.2003

* Otftotfm will now synthesize characters for some T1 glyphs automatically,
  specifically 'cwm' (compound word mark), 'visualspace', and 'Germandbls'.
  Requested by Marco Kuhlmann.

* The glyphlist.txt file contains Unicode mappings for character names
  found in the BlueSky Computer Modern math italic and symbol fonts.

* It also contains fake Unicode mappings for the 'cwm', 'visualspace', and
  'Germandbls' characters found in EC.enc.

* otftotfm: Don't output a KRN between two characters if there exists a LIG
  for those two characters.


## Version 0.17 – 6.Jul.2003

* otftotfm: Ligatures removed with LIGKERN commands won't show up in the
  encoding.

* Improve scoring heuristics by which otftotfm decides which characters are
  more important (for when there isn't enough encoding space for all new
  glyphs).


## Version 0.16 – 6.Jul.2003

* otftotfm: In automatic mode, store dvips files (encodings and
  psfonts.map) in 'TEXMF/dvips/VENDOR', rather than 'TEXMF/dvips'. Users of
  previous versions will probably want to move their 'TEXMF/dvips/a_*.enc'
  and 'TEXMF/dvips/lcdftools.map' files to a 'TEXMF/dvips/lcdftools/'
  directory, and run 'mktexlsr TEXMF'.

* otftotfm: When there isn't enough encoding space for all new glyphs,
  prefer shorter ligatures made out of regular letters -- for instance,
  prefer f_j to f_f_j, and T_h to f_iacute. Requested by Bruce D'Arcus.

* otftotfm: Add `--ligkern` and `--unicoding` options.

* otftotfm: Add `--coding-scheme` option and `% CODINGSCHEME` encoding
  comment, to define the PL/TFM coding scheme for the font. Apparently
  fontinst actually looks at the coding scheme. Also, when you specify a
  coding scheme, set DESIGNUNITS to 1, again to satisfy fontinst. Requested
  by Marco Kuhlmann.

* otftotfm: Remove virtual font 'N.vf' when installing a regular font
  'N.tfm' in automatic mode. This reduces the risk that an old virtual font
  will screw up your installation.


## Version 0.15 – 4.Jul.2003

* otftotfm: Bug fix: Don't multiply apply substitutions and kerns from the
  same lookup.

* otftotfm: Supports simple left-contextual substitutions, necessary for
  ExPontoPro's 'calt' (Contextual Alternates) feature.


## Version 0.14 – 2.Jul.2003

* otftotfm: Add `-L/--letterspacing` option at Bruce D'Arcus's request.

* otftotfm: Hypothetically supports the contextual substitutions necessary
  for the 'init' (Initial Forms) feature.

* otftotfm: When you make a virtual font named "foo", remove any entries
  for "foo" from VENDOR.map.


## Version 0.13 – 27.Jun.2003

* `--without-kpathsea` works. Problem reported by Adam Lindsay.


## Version 0.12 – 27.Jun.2003

* otftotfm: Better error messages when directories cannot be found.
  Reported by Bruce D'Arcus.


## Version 0.11 – 26.Jun.2003

* otftotfm: Handles the contextual substitutions necessary for 'cswh'
  (Contextual Swash) and 'fina' (Terminal Forms) features.


## Version 0.10 – 26.Jun.2003

* otftotfm: Update ls-R files for new encodings.


## Version 0.9 – 25.Jun.2003

* otftotfm: psfonts.map lines contain the file name only (no directories).
  Requested by Norbert Preining.

* otftotfm: Properly report errors when encoding files can't be found.

* Fix kpathsea searching and dependency problems reported by Bruce D'Arcus
  and Claire Connelly, among others.

* Fix templates to allow compilation with GCC 2.95.


## Version 0.8 – 23.Jun.2003

* otftopl has been renamed to otftotfm. The new program takes different
  options. Automatic mode is particularly different, and TFM output is now
  the default.

* otftotfm: Automatic mode now sticks files into a TeX Directory Structure
  hierarchy. This works well with many TeX installations. It also
  automatically translates OpenType fonts into Type 1 PostScript with
  cfftot1 (unless you configure with `--disable-cfftot1`), and modifies a
  psfonts.map file for each font. See the manual page for more information.

* otftotfm: Generates virtual fonts when required to support glyph
  positioning features. (`-f cpsp` is one example.)

* otftotfm: Warns when a feature could not be completely implemented.

* otftotfm: Add `--verbose` and `--no-create` options, among others
  (`--no-virtual`, `--map-file`, `--vendor`, `--typeface`, `--no-type1`...).

* otftotfm: Encoding files are now named "a_SIGNATURE.enc", not
  "auto_SIGNATURE.enc".

* The configure script now searches for the kpathsea library, since
  otftotfm's automatic mode depends on kpathsea. Provide
  `--without-kpathsea` to disable it.


## Version 0.7 – 13.Jun.2003

* cfftot1: Fix bug to handle MinionPro-Italic without crashing.


## Version 0.6 – 12.Jun.2003

* cfftot1: Fix definitions of Subrs entries 1 and 2; now fonts with flex
  hints will work.

* cfftot1: Reduce noise generated by fonts with flex hints.

* cfftot1: Generates valid character strings for characters whose first
  point is at the origin. (Previously, such charstrings wouldn't begin with
  a "moveto".)

* otftopl: Support simple context substitutions and chained context
  substitutions. Required to support ACaslonPro-Italic's "swsh" feature.
  Reported by Adam Lindsay <atl@comp.lancs.ac.uk>.

* otftopl: Prefer `--query-features` and `--query-scripts` to
  `--print-features` and `--print-scripts`.

* otftopl: Better warnings and error messages for bad LIGKERN/UNICODING
  commands in encoding files, and when there isn't enough room in an
  encoding for ligature glyphs.

* t1lint: Reads stdin if no filenames supplied on the command line.


## Version 0.5 – 5.Jun.2003

* Template reorganization so the typetools compile with GCC 3.3.


## Version 0.4 – 3.Jun.2003

* otftopl: Added `--slant` and `--extend` options.

* otftopl's generated encodings have slightly different form, and are thus
  friendlier to ps2pk's bad parser.


## Version 0.3 – 3.Jun.2003

* Fixed bug: cfftot1 produced invalid results for fonts with encodings
  other than StandardEncoding, due to a misbehavior in the way Type 1 fonts
  were stored. Reported by Vladimir Volovich <vvv@vsu.ru>.


## Version 0.2 – 3.Jun.2003

* Fix CFF parsing bugs and configure errors reported by Vladimir Volovich
  <vvv@vsu.ru>.


## Version 0.1 – 2.Jun.2003

* Initial release.

* See also the ONEWS file for older news about mmafm and mmpfb.
