# Upgrade Manual

This file contains instructions to upgrade to a new release of Gregorio.  These instructions are cumulative; meaning that you should apply the instructions from all versions between the one you are upgrading from and the current version.

## Unreleased

Gregorio will now use an "upright" punctum inclinatum shape for a series of unison punctum inclinatum notes over a given syllable.  When there is a single punctum inclinatum, Gregorio will use a heuristic to choose a shape.  In any of these cases, if the result is not satisfactory, especially in context with nearby syllables, you may select a different shape by using `0` (for the descending shape) or `1` (for the ascending shape) after the capitalized letter indicating the punctum inclinatum.  Forced selection of the upright shape is also possible by using `2` after the capitalized letter indicating the punctum inclinatum.

## 5.1

No additional steps are needed to upgrade from 5.0 to 5.1.

## 5.0

### Auto-compilation

The `autocompile` option is now the default option for the behavior of `\gregorioscore`.  If you want to go back to the old, manual behavior, then you should pass the `nevercompile` option when loading `gregoriotex`.

### Additional fonts

With the addition of more cavum shapes, the number of font options which come with Gregorio (the package) by default has been reduced in order to control the download size.  From now on only Greciliae (and its "-op" Dominican variant) will be installed by default.  The Gregorio and Grana Padano fonts, as well as their "-op" Dominican variants, will now be available as a separate download.  If you were using these fonts, either download `supp_fonts-5_0_0.zip` and install them or switch to Greciliae.

To install the new versions of Gregorio (the font) and Grana Padano, unzip `supp_fonts-5_0_0.zip`, navigate to the uncompressed folder in Terminal (or Command Prompt on Windows) and execute `texlua install_supp_fonts.lua` after you have installed the main Gregorio distribution.  This script will look for the location of Greciliae and copy the other fonts to that location.  If you wish to control the installation of the fonts manually, the script takes an optional argument with one of the following values:

 * `auto` (optional): the same folder as Greciliae
 * `system`: the appropriate font folder in `$TEXMFLOCAL`
 * `user`: the appropriate font folder in `$TEXMFHOME`
 * `<dir>`: the name of an alternate texmf root directory you want to use

Additionally, if you are building the fonts yourself from a git clone, `install-gtex.sh` and `install_supp_fonts.lua` will install all the fonts you have built, not just the ones they are normally distributed with.

**Note:** All the above methods assume you only need to access the fonts from within a TeX document (and thus are designed to put them into the correct folder in a texmf tree).  If you want to use the fonts in other programs, then you will need to consult the documentation appropriate to your platform and/or the program and manually move, copy, or link the fonts to the necessary location.

### Ledger lines

As of version 5.0, ledger lines are extended through notes on either side of a ledger line that crosses a stem, as long as the notes are within the same element.

The algorithm for this is simple so it can be predictable, and it cannot take into account spacing adjustments made in TeX.  This means it may not produce the exactly desired results.  In order to get the results you want, you can override the automatic behavior in gabc:

- `[oll:1]` will force an over-the-staff ledger line on a note.
- `[oll:0]` will suppress an over-the-staff ledger line on a note.
- `[ull:1]` will force an under-the-staff ledger line on a note.
- `[ull:0]` will suppress an under-the-staff ledger line on a note.

The other `oll` and `ull` forms take precendence over and will interfere with the above settings, so if you are using them, you may need to adjust them to get the output you want.

Note: You may need to use a construct such as `/!` to keep notes that are separated in the same element.  For example, `abcV` is two elements (`ab` and `cV`), so the ledger line on `b` is not extended to `cV`.  In contrast, `ab/!cV` is one element, so the ledger line on `b` is extended to `cV`.

### End-of-line hyphen protrusion factor

As of version 5.0, the `\gresethyphenprotrusion{percentage}` command is deprecated.  To set this protrusion factor, use `\gresetprotrusionfactor{eolhyphen}{factor}` instead.  Note that the `factor` taken by the new command is a factor rather than the percentage taken by the deprecated command, so for example, use `\gresetprotrusionfactor{eolhyphen}{0.5}` instead of `\gresethyphenprotrusion{50}`.

### Oriscus orientation at the unison

As of version 5.0, when the note after the oriscus is at the same pitch as the oriscus, the oriscus will point towards the first non-unison note after the oriscus or downwards if at the end of the score.  Use the `0` (for downwards) `1` (for upwards) modifiers to force a different orientation.

### Elisions in vowel centering

As of version 5.0, elisions after a vowel centering prefix will not cause the center to be placed on the vowel at the end of the prefix.  Since elisions are unvoiced vowels, this makes more sense than the previous behavior.  However, if this change does not produce your desired output, you may surround the center manually with `{` and `}` in gabc.

## 4.2

### Executable file name

In order to facilitate installation alongside TeX Live, the version number is now appended to the gregorio executable file name.  In version 4.2.0, the filename is `gregorio-4_2_0`.  If you run the executable directly, you will need to modify your procedures and/or scripts to use the new name.  Alternately, creating a symbolic link, if your system supports it, may work for you.

If you auto-compile or force-compile your GABC files and are *not* using the Gregorio packaged with TeX Live, you will probably need to use the `--shell-escape` option when compiling your `.tex` files.  Alternately, you can add the new filename to your system's `shell_escape_commands` TeX option.

Unfortunately, there appears to be no easier way to let a user-installed Gregorio coexist with the TeX-Live-packaged Gregorio.  Please note: in TeX Live 2016, which includes Gregorio 4.1.1, the executable filename does not include the version number, though that will change starting with TeX Live 2017.

### Stemmed oriscus flexus orientation

As of version 4.2, the orientation of the stemmed oriscus flexus `(gOe)` is consistent with the unstemmed oriscus flexus `(goe)` in that the oriscus points downwards (since the note which follows is of lower pitch).  If you prefer the oriscus to point upwards, you will need to use the `1` modifier (as in `(gO1e)`), which will force an upward orientation of the oriscus.

### Podatus followed by a virga

As of version 4.2, a podatus followed by a virga of the same or higher pitch as the second note in the podatus (e.g., `(eghv)`) will be kept together, disallowing a line break between the two shapes.  If you would like to allow a line break between the two shapes, use a `/` or some other breakable space between them (e.g., `(eg/hv)`).

### Penalties

As of version 4.2, the penalties should be changed by using `\grechangecount` rather than the old way of redefining macros.  The following tunable values are available (they have the same name as their old macro counterparts, minus the `gre`):

- `brokenpenalty`
- `clubpenalty`
- `endafterbaraltpenalty`
- `endafterbarpenalty`
- `endofelementpenalty`
- `endofsyllablepenalty`
- `endofwordpenalty`
- `finalpenalty`
- `hyphenpenalty`
- `looseness`
- `newlinepenalty`
- `nobreakpenalty`
- `pretolerance`
- `tolerance`
- `widowpenalty`

See GregorioRef for descriptions.

Additionally, if you were redefining `\greemergencystretch`, you should now be changing the `emergencystretch` dimension using the `\grechangedim` command.

### Custos with alteration

If the note following a custos has an alteration (flat, natural, or sharp), the custos will now also have that alteration typeset before it.  If you prefer the old behavior, use `\gresetcustosalteration{invisible}`.  This setting may be switched to `visible` and `invisible` between scores.

### Punctum inclinatum orientation

As of version 4.2, a different glyph is used for puncta inclinata in an ascent versus puncta inclinata in a descent.  However, because the two shapes clash with each other, Gregorio will attempt to use the same shape for all notes within a string of puncta inclinata.  This is accomplished by a heuristic algorithm that determines the glyph to use, but tastes differ, so you may override the shape by appending `0` (to force descending) or `1` (to force ascending) after the letter used for the punctum inclinatum.

Two additional distances have been added to handle strings of puncta inclinata which both ascend and descend.  `descendingpunctuminclinatumascendingshift` will be used between two descending punctum inclinatum glyphs which ascend in pitch, and `ascendingpunctuminclinatumdescendingshift` will be used between two ascending punctum inclinatum glyphs which descend in pitch.

### Styles spanning syllables

From version 4.2, gabc styles persist through syllables so you can now, for instance, style an entire verse differently from the rest of the piece by starting the style at the beginning of the verse and ending it at the end of the verse.  This covers italics (`<i>`), bold (`<b>`), small capitals (`<sc>`), underlined text (`<ul>`), colored text (`<c>`), and "teletype" text (`<tt>`).

Prior to version 4.2, Gregorio inconsistenly indicated style errors.  Now, errors like ending a style that is not started and starting a style that is already started will be caught more consistently.  If you are getting errors to this effect, double-check the styles in your score to make sure that styles are started and ended properly.


## 4.1.2

### Changes to texmf tree

To better comply with the TeX Live directory structure, the location of `gregoriotex.sty` and `gregoriosyms.sty` have changed.

*Linux and other users installing from source:* `install-gtex.sh` should remove the old versions before installing the new one.

*Mac users installing via pkg installer:* The installer does not attempt to remove the old versions before installing the new ones.  Please run Uninstall-Gregorio.pkg before installing 4.2 to ensure that the old versions don't cause any problems.

*Windows users:* Your installer already should remove old versions of GregorioTeX before installing the new ones.

As always, please file a bug report if you have any problems with the update process.

### Parmesan font renamed to Grana Padano

To avoid conflicts with the LilyPond Parmesan font, the Gregorio Parmesan font is now called Grana Padano.  If you were using `\gresetgregoriofont{parmesan}`, you should now use `\gresetgregoriofont{granapadano}`.


## 4.1.1

### Vertical spacings

High notes are now taken into account in vertical spacings. If you would like to come back to the old way, use `\grechangecount{additionaltopspacethreshold}{6}`. High notes also influence vertical placement of alt text. If you would like to come back to the old way, give a high value to `\grechangecount{additionaltopspacealtthreshold}{6}`.

### Last line of score behavior

`\grelooseness` is now `-1` by default. This should prevent orphaned syllable at end of score. If you prefer the old behavior, use `\def\grelooseness{\looseness}` before including your scores.

## 4.1

### Initial handling

Initial handling has been simplified.  This deprecates the `initial-style` gabc header and the `biginitial` style.

Rather than using the `initial-style` gabc header, set the number of lines the initial should use in Tex with the `\gresetinitiallines` command.  It currently accepts 0 (for no initial), 1 (for a one-line initial), or 2 (for a two-line initial).

As for the `biginitial` style, the intent is that the `initial` style be changed within the TeX document whenever the initial style should change, regardless of how many lines the initial requires.  For example, before including a score that has a two-line initial, you would probably want to redefine the `initial` style to use a larger font.  Then, before including a score which requires a one-line initial, you would then redefine the `initial` style back to what it was before.

In order to allow a smoother transition to the new behavior, GregorioTeX will behave differently depending on whether or not deprecated usage is allowed (controlled by the `[allowdeprecated]` package option).  If deprecated usage is allowed, GregorioTeX will use the deprecated `biginitial` style for big initials.  If not, GregorioTeX will use the `initial` style for big initials.

Since the `biginitial` style will disappear with Gregorio 5.0, please consider disabling deprecated usage (set `[allowdeprecated=false]` when using the `gregoriotex` package) and use the `initial` style as just described.

### Spacing between notes

When the next syllable starts with an alteration, the minimal space between notes of the current syllable and notes of the current syllable is handled by the new spaces `intersyllablespacenotes@alteration` and `interwordspacenotes@alteration`. Set them in your custom spacings file if needed.

When a syllable ends with a punctum mora, Gregorio 4.1 doesn't consider the punctum mora completely in horizontal spacing. This can result in next syllable being closer. To balance the output, `spacebeforesigns` has been made shorter. If you want to go back to the old behavior, use:

- `\gresetshiftaftermora{never}` if you want to remove it completely
- `\gresetshiftaftermora{barsonly}` if you want to remove it for non-bar syllables but keep it for bar syllables
- `\grechangedim{spacebeforesigns}{0.05469 cm plus 0.00455 cm minus 0.00455 cm}{scalable}` to go back to the old spacing before punctum mora

The `\gresetshiftaftermora` macro can take other arguments for controlling how this should work, see GregorioRef for details.

### Horizontal episemata on high and low notes

Prior to version 4.1, Gregorio reserved space between notes at the `c` and `k` heights and their horizontal episemata for a "ledger line" that might appear between them.  However, if the ledger line did not appear, the episema would appear to be too far from the note.

Starting with version 4.1, Gregorio attempts to reduce the space between the note and its episema if it doesn't think there is a "ledger line" there.  However, due to the intricacies of distances and measurement in TeX, Gregorio might guess wrong.  In this case, you can override the guess by using the `[hl:n]` (for a line above the staff) and `[ll:n]` (for a line below the staff) notations in gabc.  If you put a `0` for `n`, Gregorio will assume there is no ledger line, and if you put a `1` for `n`, Gregorio will assume there is a ledger line.  This notation will have to be placed after every note which should be thus modified.

Note: Using `[hl:n]` and `[ll:n]` **will not** add a ledger line if it doesn't exist or remove one if it does.  It simply affects whether Gregorio will act as if one is there or not.

If you prefer the old behavior, you may switch this off by issuing `\gresetledgerlineheuristic{disable}` in your TeX document.  You may switch it back on with `\gresetledgerlineheuristic{enable}`.

### Custos before EUOUAE blocks

In the past, Gregorio handled the notes of an `<eu>` block like any other, which meant that a custos would appear before the `<eu>` block if it happened to start on a new line.  However, the '<eu>' block is not a continuation of the melody, but rather a reminder of the ending to use for the paired psalm tone.  As a result, a custos immediately before an EUOUAE block is now suppressed by default.  If you desire the old behaviour, use `\greseteolcustosbeforeeuouae{auto}` in your TeX document.  To once again suppress the custos, use `\greseteolcustosbeforeeuouae{suppressed}`.

### Spacing around bars

#### New algorithm

A new algorithm has been implemented and is turned on by default.  We hope this will be a significant improvement over the previous one.  The primary goal of the new algorithm is to place the bar line exactly between the notes which surround it and do the same with the text associated with the bar line.  However, it will also keep the text and bar reasonably close together and with sufficent space between the bar syallable and the surrounding syllables (possibly introducing space to meet the competing interests).  The new algorithm introduces several new spaces allowing the user to tune the algorithm:
  - `maxbaroffsettextright`
  - `maxbaroffsettextleft`
  - `maxbaroffsettextright@nobar`
  - `maxbaroffsettextleft@nobar`
  - `maxbaroffsettextright@eol`
  - `maxbaroffsettextleft@eol`
  - `bar@*@standalone@text` (where `*` is the type of bar)
  - `bar@*@standalone@notext` (where `*` is the type of bar)
  - `interwordspacetext@bars`
  - `interwordspacetext@bars@euouae`
  - `interwordspacetext@bars@notext`
  - `interwordspacetext@bars@notext@euouae`
  - `bar@rubber`
If you prefer the old bar spacing algorithm, you can revert to that with:

```
\gresetbarspacing{old}
```

Another big change in this algorithm is that empty syllables, i.e. ones without any notes, are now treated as a 0-width bar, not as a 0-width note.  Thus if you were using this sort of syllable to insert space into your score, chances are the results will be different under the new algorithm and will require you to re-tweak your scores.


#### Renaming of spaces

The spaces around bars are now:

- `bar@virgula`
- `bar@minima` (both replacing `spacearoundsmallbars`)
- `bar@minor` (replacing `spacearoundminor`)
- `bar@maior` (replacing `spacearoundmaior`)
- `bar@finalis` (replacing `spacearoundfinalis`)
- `bar@finalifinalis` (replaceing `spacebeforefinalfinalis`)
- `bar@dominican` (new)

They have variants for the new bar spacing algorithm only:

- `bar@xxx@standalone@text` (where `xxx` is `virgula` or `minima`, etc.) are used when the bar is in a standalone syllable with text
- `bar@xxx@standalone@notext` for bars in a standalone syllable without text

### Hyphens going right of staff lines

Hyphens should now not go right of staff lines. If you prefer the old behavior, `\gresethyphenprotrusion{100}` should restore it.

### Oriscus orientation

The oriscus orientation (whether it points up or down) is now dependent on the note the follows, even if the note is not directly connected to the oriscus (as it would be in a salicus or a pressus).  Appending a `1` to an unconnected oriscus in gabc will force the oriscus to point upwards and `0` will force the oriscus to point downwards.

Prior to version 4.1, Gregorio automatically oriented the oriscus when it was within a neume, but left the orientation up the user otherwise.  If you prefer this behavior, set the `oriscus-orientation` header in gabc to `legacy`.  In legacy mode, the orientation of an unconnected oriscus will (by default) point downwards and may be reversed by appending `<` or `>` to the oriscus.

### Mode number

The style for the mode number has been bold and small capitals for a long time, but the number was rendered as upper-case Roman numerals, defeating the nuance of small capitals.  As of version 4.1, Gregorio will use lower-case Roman numerals (by default) that will then appear as small capitals.  This will change how the mode number appears above the initial if you have been using the `mode` header to typeset them.  If you prefer the earlier appearance, use `\gresetmodenumbersystem{roman-majuscule}` in your TeX file for upper-case Roman numerals.  The other options available are `roman-minuscule` (the new default), for lower-case Roman numerals, and `arabic`, for Arabic numerals.

### Score reference macros

If you were using `\scorereference`, `\GreScoreReference`, and/or `\grescorereference`, stop using them.  If you need to capture the `manuscript-reference` header, use the new header capture feature (see GregorioRef for details).

### Three descending notes

Three descending notes that have no other markings (such as liquescentia) will now be rendered as a clivis followed by a punctum.  The older, incorrect behavior was to group them as three punctums joined by `!`.  If you prefer this behavior, explicitly separate the notes with `!`.


## 4.0

### Font changes

The music fonts (greciliae, gregorio, and parmesan) have changed with this release.  If the neumes do not appear or look strange in the output, you may need to clear your LuaTeX font cache by using `luaotfload-tool --cache=erase`.

### GregorioTeX and fontspec

Changes to the LaTeX kernel have made it so that if you use the fontspec and gregoriotex pacakges, you must load the fontspec package first.

### Command Name Systemization

A naming scheme for GregorioTeX commands has been established and so most commands have had their names changed in order to bring them into line with the new scheme.  Some have also had their syntax changed.  Briefly, user commands all now have a `\gre` prefix (to prevent name colisions with other packages) and groups of commands which altered a single setting have been replaced by a single command which takes an argument specifying the value of the setting.  The notable exception to this are the two main commands: `\gregorioscore` (replaces `\includescore`) and `\gabcsnippet`.  See GregorioRef for the complete list of new command names and their syntax.

Old command names should still work for now, but will raise a deprecation warning which indicates the name of the correct command to use. Exceptions are noted below:

- `\grescaledim`: This function now takes two arguments.  The second should be `yes`, `true`, `on`, or `scalable` to acheive the old behavior.

Additionally a new package option has been added. This option, `allowdeprecated=false`, is helpful if you wish to ensure that your TeX file is compliant with the new naming system. This option causes all deprecated commands to raise an error, halting TeX, thus allowing you to actively find all deprecated commands and update them in your TeX file.

### Barred letters

If you use barred letters and a font other than Linux Libertine in your document, you must adjust the horizontal placement of the bar on the letter.  To do so, use the `\gresimpledefbarglyph` macro.  For example, use `\gresimpledefbarredsymbol{A}{0.3em}` in your preamble, tweaking the second argument to have a good result (same for R and V).  If using LaTeX, you can alternately use the `\grelatexsimpledefbarredsymbol` macro, which allows control over bold and italic versions of the barred letter.  See the documentation of `\gresimpledefbarredsymbol` and `\grelatexsimpledefbarredsymbol` in the PDF documentation for more details.

If you were using `\Vbarsmall`, `\greletterbar`, and `\greletteraltbar`, you must use `\gresimpledefbarredsymbol` to redefine your barred letters (see PDF documentation for details).

### .gtex extension

The `gregorio` executable now uses the `.gtex` extension by default (instead of `.tex`, formerly) for GregorioTeX files that it produces.  If you use `\includescore{file.tex}`, then you should change this to `\includescore{file.gtex}` or use the newer autocompilation feature.

### Custom spacings

If you are using custom spacings, please update the values of `interwordspacetext`, `intersyllablespacenotes` and `interwordspacenotes` to match their new definitions (in the comments in `gsp-default.tex`). Also note that `alterationspace` is now fixed and cannot take `plus` or `minus` values.

### Euouae blocks

You are advised to surround your *Euouae* blocks by the new `<eu>` tag. For instance,

    E(i) u(i) o(i) u(h) a(h) e(fe..)

can become

    <eu>E(i) u(i) o(i) u(h) a(h) e</eu>(fe..)

This will prevent line breaking, so if you were using so-called *no linebreak areas* (with `{` in gabc) just for Euouae blocks, you can switch to this new tag, which will make things clearer and allow further spacing customization.

### Horizontal episema improvements

Changes to the way the horizontal episema is placed and "bridged" to other notes within the syllable may cause Gregorio to render things differently in the more esoteric (or bug-ridden) cases.  If you are depending on the old behavior, you might need to add suffixes to the `_` in gabc to get what you want:

- Add `0` to force the episema to appear below the note.
- Add `1` to force the episema to appear above the note.
- Add `2` to prevent Gregorio from attempting to connect this episema to the next.
- Add `3` to use a small episema, aligned to the left of the note.
- Add `4` to use a small episema, centered in the middle of the note.
- Add `5` to use a small episema, aligned to the right of the note.

Note: `3`, `4`, and `5` encompass a new feature and are listed here only for completeness.

### Choral sign dimension renames

- `beforechoralsignspace` has been renamed to `beforelowchoralsignspace`.
- `lowchoralsignshift` has been renamed to `choralsigndownshift`.
- `highchoralsignshift` has been renamed to `choralsignupshift` and its sign (direction of shift) inverted.

### Styling score elements

Changing the styling of text elements of the score (the initial, translations, etc.) formerly required the user to redefine a command which took an argument, while changing the styling of the staff lines had a command specialized to that purpose.  All formats can now be changed via the `\grechangestyle` command.  This command takes two or three arguments.  The first argument, required, is the name of the format to be altered.  The defined formats are:

- `initial`: normal initials
- `biginitial`: big (2-line) initials
- `translation`: translation text
- `abovelinestext`: above lines text (`alt` in gabc)
- `normalstafflines`: the full length staff lines
- `additionalstafflines`: the short lines behind notes above or below the staff.  This style defaults to inheriting changes to `normalstafflines`.
- `lowchoralsign`: low choral signs
- `highchoralsign`: high choral signs
- `modeline`: the mode annotation above the initial if the content of the mode header in gabc is rendered.
- `firstword`: the first word of the score (excluding the score initial)
- `firstsyllable`: the first syllable of the first word of the score (excluding the score initial).
- `firstsyllableinitial`: the first letter of the first word of the score which is not the score initial.

The second argument, also required, is the code necessary to turn on the styling.  The third argument, optional and enclosed in square braces (`[` and `]`), is the code necessary to turn off the styling (e.g. if the code to turn on the styling contains a `\begin{environment}` then the code to turn it off must have the matching `\end{environment}`).  The third argument is optional because not all styling commands have explicit off switches.

While the old way of changing the styles is still supported, you should switch to this new method to future-proof your scores.

Examples: Let's say you previously had the following in your LaTeX document:

    \renewcommand{\greabovelinetextstyle}[1]{{\small\it #1}}

This would have made the text which was wrapped with `<alt></alt>` in your gabc file appear small and italicized in your score.  To update this to the new system you would replace the above line with the following:

    \grechangestyle{abovelinetext}{\small\it}

### Centering scheme

The gabc `centering-scheme` header is now deprecated and will disappear soon.  Use the `\gresetlyriccentering` command from TeX instead.  If you were using `centering-scheme: latine;` in gabc, now use `\gresetlyriccentering{vowel}` in the TeX file that includes the gabc.  If you were using `centering-scheme: english;` in gabc, now use `\gresetlyriccentering{syllable}` in the TeX file that includes the gabc.

Using the gabc header will, in this release, do that for you, but it will produce a deprecation warning.  This header will no longer be available in the next release.

### Annotations

Support for annotations with an arbitrary number of lines has been added.  To facilite this, the old functions which added annotations to specific lines (either the first or the second) are consolidated into a single function `\greannotation` which builds the annotations line by line.  If you used the old functions for adding annotations, then you should switch out those functions for the new one.

The distance associated with the annotations has also been renamed (from `aboveinitialseparation` to `annotationseparation`) and supplemented by a new distance (`annotationraise`).  The first still controls the spacing between the lines of the annoation.  The second controls the position of the annotation relative to the score, and thus replaces the second argument in the old functions.  By default, annotations are positioned so that the baseline of the first line is aligned with the top line of the staff.  Positive values of `annotationraise` will push the annotation up while negative values will push it down.  If you were previously using the second argument to `\gresetfirstlineaboveinitial` to adjust the spacing, you will need to convert this to call:

    \grechangedim{annotationraise}{0.1cm}{1}

Since the baseline position of annotations is a change from the old behavior, you will need to adjust the value of the distance a bit to achieve the desired positioning.

As is normal, calls to the deprecated command names will raise a warning but still work.  However, there is one caveat: the old functions will always add the annotations to the bottom of the annotation list, regardless of the order in which they are called.  Previously, you could call `\gresetsecondannotation` before `\gresetfirstannotation` and still have the first annotation appear on top.   Which annotation appears on top is now determined by the order in which the functions are called.

#### The Annotation Header

The `annotation` header in gabc files now passes its value(s) to GregorioTeX for placement above the inital letter of the score. Up to two `annotation` fields are supported, the first for the first line above the initial, and the second field for the second line.

    annotation: 1 Ant.;
    annotation: VII a;

Full TeX markup is also accepted:

    annotation: {\color{red}1 Ant.};
    annotation: {\color{red}VII a};

If you define annotation(s) in the main TeX file via `\greannotation`, then the `annotation` header field in the gabc file will *not* overwrite that definition.

#### Variable Height Line Spacing

GregorioTeX is now able to make individual lines of a score taller when the position of the note requires extra space, without affecting the rest of the lines.  This is the new default behavior.  If you prefer the uniform line heights of earlier versions, use:

    \gresetlineheightexpansion{uniform}

Within a TeX document, you can switch back to variable line heights by using:

    \gresetlineheightexpansion{variable}

You can freely switch between the two behaviors within a TeX document, but only between scores.

This new behavior requires two passes (two runs of lualatex) in order to do the calculation correctly.  On the first pass, lualatex will warn you with the message

    Module gregoriotex warning: Line heights or variable brace lengths may have changed. Rerun to fix.

As you edit your document, lualatex will only show this warning if an extra run is required.

Alternately, you can use something like latexmk to automate this activity.  Instructions for using latexmk are outside the scope of this document.  Please consult the latexmk documentation for more information on using it.

It should also be noted that this new feature, besides requiring two passes, also imposes a small performance penalty on each pass to do the additional computation.  This is negligible for small documents, but may have more noticeable impact as documents get larger.

#### MetaPost brace rendering

GregorioTeX now uses MetaPost (by default) to render braces.  This allows fine-tuning to better maintain optical line weight when braces are stretched.  Since the shapes are slightly different, you may prefer the old behavior, which may be restored by using:

    \gresetbracerendering{font}

Within a TeX document, you can switch back to MetaPost brace rendering by using:

    \gresetbracerendering{metapost}

You can freely switch between the two behaviors within a TeX document.

#### Keyword argument for `\grecreatedim` and `\grechangedim`

In order to better match the new command naming conventions, the third argument of `\grecreatedim` and `\grechangedim` should now be `scaling` or `fixed` instead of `1` or `0`, respectively.

#### Liquescence on bistropha and tristropha

In order to be more consistent, making a bistropha or tristropha liquescent by adding `<` in gabc will now only affect the note thus marked.  This means that a tristropha like `(gsss<)` will only show the final stropha as liquescent, which is different from the older behavior of making every stropha liquescent.  If you prefer the old behavior, mark every note explicitly as liquescent with something like `(gs<gs<gs<)`.

### Empty first syllable hyphen

Prior to version 4.0, GregorioTeX would put a hyphen below an empty first syllable (which happens when the first syllable consists only of the initial).  The spacing algorithm is improved in 4.0, but that eliminates the hyphen if there is not enough space between the note of the first syllable and the next syllable.  The `\gresetemptyfirstsyllablehyphen` command controls this behavior.  It defaults to `force`, which restores the pre-4.0 behavior, but may be changed to `auto` (i.e., `\gresetemptyfirstsyllablehyphen{auto}`) if you prefer the space-based behavior.

### gabc-version header

The gabc-version header has become meaningless.  It is now deprecated.  Please remove it from your gabc files if you had been using it.

## 3.0
### TeX Live 2013

Because of changes necessary to future-proof the fonts, please upgrade to at least TeX Live 2013 if you are using an older version.  If the neumes do not appear or look strange in the output, you may also need to clear your LuaTeX font cache by using `luaotfload-tool --cache=erase`.

### Score inclusion

When migrating to this release, you should start to use the new `\includescore` system, as other ways of score inclusion are deprecated and will start disapear soon. See GregorioRef.pdf for details.

#### For users of version 2.4.2

In version 2.4.2, an early version of the feature to auto-compile gabc score was added.  Because of the confusion this caused with old users upgrading to 2.4.2, the `\includescore` command now works in a backwards-compatible way so scores created using this command for versions of Gregorio prior than 2.4.2 should work with no change.  However, for users of 2.4.2, the following changes are necessary:

- If you were using `\includescore[f]`, the equivalent command is now `\includescore[n]`, but due to this change, you should be able to just use `\includescore` without the optional parameter.
- If you were using the auto-compile feature, you should use `\autocompilegabc` in your tex file prior to using `\includescore`.  This is only necessary once and **does not need to be used before each use** of `\includescore`.

### Custom dimension changes

If you use custom space definitions (`gsp-foo.tex`), the format have changed, and you must rewrite it completely. See the new `gsp-default.tex` for an example, and GregorioRef.pdf for documentation.

### Color definition

If you want to change color in gabc, using the `<c>` markup is the safest way, and you should migrate to that, as Gregorio introduced mechanisms that may break badly formatted color changes (e.g. `\color{red}foo\color{black}`).
