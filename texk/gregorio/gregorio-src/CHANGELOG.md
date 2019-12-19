# Change Log
All notable changes to this project will be documented in this file.
As of v3.0.0 this project adheres to [Semantic Versioning](http://semver.org/). It follows [some conventions](http://keepachangelog.com/).

## [5.2.1] - 2019-04-06
### Fixed
- Project names with quotes in them no longer cause GregorioTeX to not be able to find the executable.  See [#1416](https://github.com/gregorio-project/gregorio/issues/1416).
- Eliminated implicit casting from float to integer in string formatting (something no longer allowed in Lua 5.3)


## [5.2.0] - 2019-03-10
### Fixed
- `latin` is now supported as a valid language name.  This brings Latin into line with other languages where both titlecased and lowercased version of the language name (e.g. `English` and `english`) are both allowed.

### Added
- Added support for two and three letter language codes for supported languages. [This list of codes](https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes) (and language names) was used for this support.  No new languages are supported as a result of this change, just different ways of specifying the existing names.
- A non-rotated diamond-shaped variant of the punctum inclinatum.  Append a `2` in gabc to the capitalized note pitch letter to force gregorio to use this shape.  Otherwise, gregorio will try to use it when a string of these notes is at the unison within a syllable.  See [#1425](https://github.com/gregorio-project/gregorio/issues/1425).
- Support for an eighth bar (nicknamed divisio "minimis") by using `^` in gabc (see [#1424](https://github.com/gregorio-project/gregorio/issues/1424)).
- 8 new St. Gall neume glyphs have been added to the `gregall` font.

## [5.1.1] - 2018-03-25
### Fixed
- Various issues with the documentation.

## [5.1.0] - 2018-03-11
### Added
- Added support for virgula (gabc: `` `0``) and divisio minima (gabc: `,0`) on the ledger line above the staff (see [#1370](https://github.com/gregorio-project/gregorio/issues/1370)).
- Added support for a dotted bar (gabc: `:?`) (See [#1360](https://github.com/gregorio-project/gregorio/issues/1360) and [#1387](https://github.com/gregorio-project/gregorio/issues/1387))
- 10 new St. Gall neume glyphs have been added to the `gregall` font (see [#1328](https://github.com/gregorio-project/gregorio/issues/1328), [#1330](https://github.com/gregorio-project/gregorio/issues/1330), [#1337](https://github.com/gregorio-project/gregorio/issues/1337), [#1340](https://github.com/gregorio-project/gregorio/issues/1340), [#1341](https://github.com/gregorio-project/gregorio/issues/1341), [#1342](https://github.com/gregorio-project/gregorio/issues/1342), [#1343](https://github.com/gregorio-project/gregorio/issues/1343), [#1345](https://github.com/gregorio-project/gregorio/issues/1345)).
- 4 new Laon neume glyphs have been added to the `grelaon` font.
- A more modern-looking sharp symbol has been added to the `greciliae` font.  To switch to this, use `\grechangeglyph{Sharp}{greciliae}{.modern}\grechangeglyph{SharpHole}{greciliae}{.modern}` in your TeX file.  See [#1399](https://github.com/gregorio-project/gregorio/issues/1399).
- Added documentation note about loading microtype after gregoriotex (see [#1364](https://github.com/gregorio-project/gregorio/issues/1364)).

### Fixed
- Fixed issue with Tex Live 2017 latexmk not detecting auto-compiled gabc files as dependencies (see [#1367](https://github.com/gregorio-project/gregorio/issues/1367)).
- Fixed issue where an altered note (glyph) throws off the position of an episema in the glyph that immediately precedes it (see [#1379](https://github.com/gregorio-project/gregorio/issues/1379)).

## [5.0.2] - 2017-05-24
### Fixed
- Worked around an issue discovered during the TeX Live 2017 pre-test.  See [#1362](https://github.com/gregorio-project/gregorio/issues/1362).

## [5.0.1] - 2017-04-16
### Fixed
- Fixed a bug in the TeX Live compatibility code for Windows users.  Thanks to Akira Kakuto for the catch.

## [5.0.0] - 2017-04-15
### Fixed
- The printing of the commentary box is no longer tied to the printing of the inital.  As a result it is now possible to have a score which has a commentary but not an initial.  See [this thread on the user list](http://www.mail-archive.com/gregorio-users@gna.org/msg03911.html).
- Elisions after a vowel centering prefix will no longer cause the center to be placed on the vowel in the prefix.  Since prefixes are unvoiced vowels, this makes more sense than the previous behavior.  As usual, you may use the manual centering features to force the centering where you want if this does not produce what you want.  See [#1320](https://github.com/gregorio-project/gregorio/issues/1320).
- Dynmanic line spacing when a clef change occurs at the end of a line is fixed so that the line after the clef change is spaced appropriately.  See [#1285](https://github.com/gregorio-project/gregorio/issues/1285).
- Spaces in the filename of a score no longer cause Gregoriotex to fail. See [#1224](https://github.com/gregorio-project/gregorio/issues/1224).
- Glyphs for pes quadratum shapes with initio debilis are no longer missing. Use `-fqg` to engrave them.  See [#1310](https://github.com/gregorio-project/gregorio/issues/1310).
- Horizontal episemata bridge spaces more correctly.  As mentioned earlier, you can prevent this by appending `2` to the `_` on the note before the space you do not want bridged.  See [#1216](https://github.com/gregorio-project/gregorio/issues/1216).
- A rising note after an oriscus flexus will no longer generate a porrectus (see [#1220](https://github.com/gregorio-project/gregorio/issues/1220)).
- The Scribus external tool script now uses latexmk in order to handle the multi-pass features of Gregorio (see [#1236](https://github.com/gregorio-project/gregorio/issues/1236)).
- Translation text in a syllable with a trailing (forced) hyphen is no longer truncated to its first character (see [#1254](https://github.com/gregorio-project/gregorio/issues/1254)).
- A trailing (forced) hyphen in a syllable no longer generates a forced hyphen in the previous syllable (see [#1255](https://github.com/gregorio-project/gregorio/issues/1255)).
- A trailing space on a header line is now ignored (see [#1269](https://github.com/gregorio-project/gregorio/issues/1269)).
- A pes whose second note is an oriscus now obeys the oriscus orientation rules (see [#1272](https://github.com/gregorio-project/gregorio/issues/1272))

### Changed
- The default behaviour of `\gregorioscore` has been changed to autocompile.
- When the note after an oriscus is at the same pitch, the oriscus direction now depends on the first non-unison, non-oriscus note after the oriscus (see [#1179](https://github.com/gregorio-project/gregorio/issues/1179)).
- 6 new St. Gall neume glyphs have been added to the `gregall` font and glyph for nabc `to!ciG` has been changed (see [#1303](https://github.com/gregorio-project/gregorio/issues/1303), [#1305](https://github.com/gregorio-project/gregorio/issues/1305), [#1313](https://github.com/gregorio-project/gregorio/issues/1313), [#1314](https://github.com/gregorio-project/gregorio/issues/1314), [#1315](https://github.com/gregorio-project/gregorio/issues/1315)).
- Notes are now left-aligned as if all clefs had the same width as the largest clef in the score. You can get previous behavior back with `\grebolshiftcleftype{current}`, or temporary force alignment until the end of a score with `\grelocalbolshiftcleftype`. See Documentation of these functions and [#1189](https://github.com/gregorio-project/gregorio/issues/1189).
- A clef change immediately before a line break `(z)` will now typeset the new clef at the beginning of the next line.  An explicit custos `(z0)` immediately before such a clef change (or separated by only a bar) will be suppressed.  See [#1190](https://github.com/gregorio-project/gregorio/issues/1190).
- Ledger lines are now extended through notes on either side of a ledger line that crosses a stem, as long as the notes are within the same "element."  If the algorithm doesn't produce the result you want, you can use `[oll:0]` to suppress an over-the-staff ledger line on a note, `[ull:0]` to suppress an under-the-staff ledger line on a note, `[oll:1]` to force an over-the-line ledger line on a note, or [ull:1] to force an under-the-staff ledger line on a note.  Please note that other forms of `[oll:...]` and `[ull:...]` can interfere with these new settings. See [UPGRADE.md](UPGRADE.md) and [#1215](https://github.com/gregorio-project/gregorio/issues/1215) for details.
- The left stem of Dominican plicae on lines has been shortened (see [#1238](https://github.com/gregorio-project/gregorio/issues/1238)).
- Clefs on the top or bottom line adjust the spacing as if there were a note above the top line or below the bottom line, respectively (see [#1007](https://github.com/gregorio-project/gregorio/issues/1007)).
- Default spaces have been adjusted (see [#1182](https://github.com/gregorio-project/gregorio/issues/1182)).
- `\grechangenextscorelinedim` can now take a comma-separated list of line numbers as its first argument (see [#1280](https://github.com/gregorio-project/gregorio/issues/1280)).

### Added
- More cavum shapes are now available.  To use them, simply add `r` in gabc to any note in a glyph.  See [#844](https://github.com/gregorio-project/gregorio/issues/844).
- Square brackets can be placed around notes by using `[[` and `]]` to surround said notes in gabc (see [#844](https://github.com/gregorio-project/gregorio/issues/844)).
- a font `grelaon` for Laon style adiastematic notation has been added (see GregorioNabcRef.pdf for details). This is a preview, backward incompatible change are possible in future releases.
- 9 new St. Gall neume glyphs have been added to the `gregall` font.
- 5 new St. Gall neume glyphs have been added to the `gresgmodern` font.
- install-gtex.sh now generates uninstall-gtex.sh, which can be used to uninstall the TeX portion of the installation when GregorioTeX was installed from source.
- A `<clear>` tag may be added to a syllable to indicate that its text should not overlap any previous syllable (see [#1029](https://github.com/gregorio-project/gregorio/issues/1029)).
- More general support for protrusions.  The `<pr>` tag in gabc indicates where a protrusion should begin.  There is an optional argument `<pr:.5>` that allows the protrusion factor to be specified (in this example, `.5`).  Additionally, the comma, semicolon, colon, and period are automatically protruded, with configurable protrusion factors.  See GregorioRef and [#931](https://github.com/gregorio-project/gregorio/issues/931) for more information.
- `minimalinitialwidth` space, controlling the minimum amount of space an initial should take require; ignored when `manualinitialwidth` is set to something non-zero (see [#1213](https://github.com/gregorio-project/gregorio/issues/1213)).
- new scripts in `contrib/` to check the syllabation of a gabc score against hyphenation rules, such as those on [hyphen-la](https://github.com/gregorio-project/hyphen-la)
- the visibility of a particular nabc voice can now be set by `\gresetnabc{n}{(in)visible}`, where `n` is the number of the nabc voice (see [#1257](https://github.com/gregorio-project/gregorio/issues/1257)).
- `[nocustos]` may be used in gabc to prevent a custos should the line end at that point (see [#1271](https://github.com/gregorio-project/gregorio/issues/1271)).
- Musica ficta signs: Add `r6`, `r7`, and `r8` to a note for a flat, natural, or sharp (respectively) above the note (see [#1278](https://github.com/gregorio-project/gregorio/issues/1278)).
- The spacing added for low notes in the staff is now adjustable by setting dimension `noteadditionalspacelinestext`, with the number of notes settable as count `noteadditionalspacelinestextthreshold`.  See GregorioRef and [#1125](https://github.com/gregorio-project/gregorio/issues/1125) for details.
- Some counts can now be adjusted for a particular line in a score.  Use `\grechangenextscorelinecount` prior to including the score to set the desired values.  The various spacing thresholds may be changed with this command.  See GregorioRef for details.

### Deprecated
- `\gresethyphenprotrusion{percentage}`, supplanted by `\gresetprotrusionfactor{eolhyphen}{factor}`.  Note that the value the new command takes is a factor rather than a percentage.

### Removed
- `initial-style` gabc header, supplanted by the `\gresetinitiallines` TeX command.
- `biginitial` style, consolidated into the `initial` style.
- `\grescorereference`
- The Gregorio and Grana Padano fonts are no longer distributed with the Gregorio distribution archive.  They will now be available for download separately (along with their Dominican "-op" variants) in `supp_fonts-5_0_0.zip` (the version number will change to match future releases).  See [UPGRADE.md](UPGRADE.md) and [#844](https://github.com/gregorio-project/gregorio/issues/844) for details.


## [4.2.1][Unreleased]
### Fixed
- Debian build scripts now use directories compatible with the TeX Live 2016 packaging of Gregorio (see [#1241](https://github.com/gregorio-project/gregorio/issues/1241)).


## [4.2.0] - 2016-09-23
### Fixed
- When the note after an oriscus is at the same pitch, the oriscus will now point downwards by default (see [#1177](https://github.com/gregorio-project/gregorio/issues/1177)).
- When the last note in a score is not in the last syllable, it no longer merges into the (no-note) syllable(s) that follow (see [#1205](https://github.com/gregorio-project/gregorio/issues/1205)).
- A two-line initial on a two-line score no longer generates an error (see [#1139](https://github.com/gregorio-project/gregorio/issues/1139)).
- Staff lines after a two-line initial should now be sized correctly in a more consistent way (see [#1141](https://github.com/gregorio-project/gregorio/issues/1141)).
- Space at the end of line was not always correct when a line is cut in the middle of a word (see [#1155](https://github.com/gregorio-project/gregorio/issues/1155)).
- In rare cases, the very last bar or glyph of a score could appear alone at the beginning of the final line (see [#1152](https://github.com/gregorio-project/gregorio/issues/1152)).
- In cases of a syllable without note, the space between notes of the previous and next syllables was sometimes not enough (see [#1137](https://github.com/gregorio-project/gregorio/issues/1137)).
- `moraadjustment` and `moraadjustmentbar` did not scale when changing factor, they do now.
- The `900_gregorio.xml` file for Scribus now matches `main-lualatex.tex` again (see [#1087](https://github.com/gregorio-project/gregorio/issues/1087)).
- Multiple subsequent lines in gabc no longer cause TeX to fail (see [#1111](https://github.com/gregorio-project/gregorio/issues/1111)).
- Old bar spacing algorithm will no longer separate the text and bar when the bar is preceeded by a puncutm mora (see [#1078](https://github.com/gregorio-project/gregorio/issues/1078)).
- Scores which end with a no-note syllable under the new bar spacing algorithm will no longer have their text pushed out into the right hand margin.  See [#1110](https://github.com/gregorio-project/gregorio/issues/1110).
- PlainTeX will now respect underline requests in gabc.
- PlainTeX will now raise a warning when small caps are requested but `\sc` has not been defined.
- A manual custos at the end of the score will now be spaced like regular end of line custos [#1034](https://github.com/gregorio-project/gregorio/issues/1034).
- Custos glyphs heights are now counted when adjusting line heights (see [#961](https://github.com/gregorio-project/gregorio/issues/961)).

### Added
- Some vertical spaces can now be adjusted for a particular line in a score.  Use `\grechangenextscorelinedim` prior to including the score to set the desired values.  `spaceabovelines`, `spacebeneathtext`, and `spacelinestext` may be changed with this command.  See GregorioRef and [#1156](https://github.com/gregorio-project/gregorio/issues/1156) for details.
- Nabc now supports pitches `hn` and `hp` in addition to previously supported `ha` through `hm`.
- 32 new St. Gall neume glyphs have been added to the `gregall` font and glyphs for nabc `po-1pp2su1sux1` and `po-1su1sux1` have been changed.
- 31 new St. Gall neume glyphs have been added to the `gresgmodern` font and glyph for nabc `tr-1` has been changed.
- It is now possible to set `\parskip`, `\lineskip`, `\baselineskip`, and `\lineskiplimit` to have different values within a score using `\grechangedim`.  See [#1038](https://github.com/gregorio-project/gregorio/issues/1038).
- It is now possible to invert the oriscus orientation on pes quassus and oriscus flexus shapes.  Use `o0` or `o1` to force the oriscus into the orientation you desire.  It is also possible to fuse the oriscus from either direction in either orientation.  See [#898](https://github.com/gregorio-project/gregorio/issues/898) and [#972](https://github.com/gregorio-project/gregorio/issues/972).
- Formerly missing torculus figures starting with an oriscus are now rendered using neume fusion (see [#1013](https://github.com/gregorio-project/gregorio/issues/1013)).
- Gregorio will keep together (prevent line breaks between) a user-configurable number of notes (by default, 4) at the start and end of the syllable.  Further, Gregorio will prevent line breaks in a syllable with fewer than a user-configurable number of notes (by default, 10).  These values may be changed by using the `\gresetunbreakablesyllablenotes` command.  See GregorioRef for details (for the change request, see [#1044](https://github.com/gregorio-project/gregorio/issues/1044)).
- The visibility of notes and lyrics can now be toggled on an off using `\gresetlyrics` and `\gresetnotes`.  See [#1039](https://github.com/gregorio-project/gregorio/issues/1039).
- An alteration (flat, natural, or sharp) will now appear on a custos if the next note is so altered.  Use `\gresetcustosalteration{invisible}` if you prefer the old behavior.  See [#1049](https://github.com/gregorio-project/gregorio/issues/1049).
- `gregoriosyms.sty` and `gregoriotex.tex` now contain definitions for `gregoriocolor` and `grebackgroundcolor` and the appropriate score commands dealing with color now work under PlainTeX

### Changed
- In order to facilitate installation alongside TeX Live, the version number is now appended to the gregorio executable file name.  If you are running the executable directly in your custom scripts, you will need to change them to include the version number.  If you are not using the TeX-Live-packaged version of Gregorio, you will probably need to use the `--shell-escape` option when compiling your `.tex` files.  Note that in TeX Live 2016, which includes Gregorio 4.1.1, the executable filename does not include the version number, though that will change starting with TeX Live 2017.  See UPGRADE.md and [#1197](https://github.com/gregorio-project/gregorio/issues/1197) for more information.
- When the clef and the first note are at a reasonable vertical distance, `shortspaceafterlineclef` is used instead of `spaceafterlineclef` (make them equal if you don't want this feature). This is used only on the first line, when there is an initial on one line. See [#1138](https://github.com/gregorio-project/gregorio/issues/1138).
- When an alteration follows a bar, it is not completely taken into consideration in the new bar spacing algorithm (similarly to the punctum mora), this behavior can be tuned with the newly introduced space `alterationadjustmentbar`. See [#1146](https://github.com/gregorio-project/gregorio/issues/1146).
- Very small improvement in the design of the porrectus auctus and torculus resupinus auctus (see [#1169](https://github.com/gregorio-project/gregorio/issues/1169)).
- When the notes around a virgula or divisio minima are lower than two spaces below the top staff line, the spaces used are new shorter spaces with a `@short` suffix. To cancel this feature, make them the same as their normal version. See [#1144](https://github.com/gregorio-project/gregorio/issues/1144).
- Different glyphs are now used for puncta inclinata in an ascent versus in a descent.  A heuristic algorithm is used to ensure that the glyphs match when grouped together.  If the algorithm chooses the wrong shape, use `0` (for descending) or `1` (for ascending) to force an orientation.  See [UPGRADE.md](UPGRADE.md) and [#856](https://github.com/gregorio-project/gregorio/issues/856) for details.
- The shape of the fused oriscus at ambitus one in the greciliae font has been tweaked to form a nicer connection (see [#1079](https://github.com/gregorio-project/gregorio/issues/1079)).
- Syllables are now rewritten to improve ligature rendering.  This may be disabled by issuing `\gresetsyllablerewriting{off}`.  See GregorioRef for details (for the change request, see [#1098](https://github.com/gregorio-project/gregorio/issues/1098)).
- Default mode style for PlainTeX no longer tries to invoke small caps since they are not defined by default.
- Italic, bold, underlined, small capital, teletype, and colored styles now span multiple syllables.  Gregorio will also more consistently raise errors when styles are improperly started or ended.  See [UPGRADE.md](UPGRADE.md) and [#1121](https://github.com/gregorio-project/gregorio/issues/1121) for details.
- The stemmed oriscus flexus `(gOe)` is now consistent with the unstemmed oriscus flexus `(goe)` in that the oriscus points downward at the note that follows.  If you prefer to have the oriscus point upward, use `(gO1e)` for force upward orientation.
- Space before a custos appearing in the middle of a line is now set by `spacebeforeinlinecustos`.
- The space between two puncta inclinata at the unison may be tuned by changing `punctuminclinatumunisonshift` (see [#1042](https://github.com/gregorio-project/gregorio/issues/1042)).
- A podatus followed by a virga `(eghv)` will now be kept together (no line break between the shapes),  If you would like to allow a line break there, use `(eg/hv)` instead (see UPGRADE.md and [#1045](https://github.com/gregorio-project/gregorio/issues/1045)).
- Penalties must now be changed using the `\grechangecount` command.  See GregorioRef and [UPGRADE.md](UPGRADE.md) for details (for the change request, see [#1021](https://github.com/gregorio-project/gregorio/issues/1021)).
- `\greemergencystretch` must now be set by changing `emergencystretch` using `\grechangedim` instead of redefining the macro.
- An isolated stropha will now be considered part of the previous neume group and line breaks will be prevented at that point.  In order to force a line break there, use a breaking space such as `/` before the stropha in gabc.  See [#1056](https://github.com/gregorio-project/gregorio/issues/1056).
- `interwordspacetext` and it's related distances have been redefined to be smaller and dependent on a font based distance (`ex`).  They are also no longer scale with the staff size by default.  See [#1036](https://github.com/gregorio-project/gregorio/issues/1036) & [gregoriot-test#208](https://github.com/gregorio-project/gregorio-test/pull/208).
- `\grecreatedim` and `\grechangedim` can now be used to link two distances.  To do this the second argument should be the name of the master distance and the third argument should be `inherited`.  See [#962](https://github.com/gregorio-project/gregorio/issues/962).

### Removed
- `\grescorereference`
- `\grenewlinepenalty`
- `\grenobreakpenalty`
- `\greendofwordpenalty`
- `\greendofsyllablepenalty`
- `\greendafterbarpenalty`
- `\greendafterbaraltpenalty`
- `\grefinalpenalty`
- `\greendofelementpenalty`
- `\grehyphenpenalty`
- `\grebrokenpenalty`
- `\grewidowpenalty`
- `\greclubpenalty`
- `\grelooseness`
- `\gretolerance`
- `\grepretolerance`
- `\greemergencystretch`


## [4.1.5] - 2016-08-18
### Fixed
- Use node.travese_id() to find our desired nodes when doing translation centering across syllables, thereby preventing a conflict with other packages which insert nodes (such as luatex-ja).  See [#1180](https://github.com/gregorio-project/gregorio/issues/1180).
- Explicitly communicate the rescaling of `\gre@skip@temp@four` back to TeX, thereby fixing the problem with custom spacings.  See [#1199](https://github.com/gregorio-project/gregorio/issues/1199).
- With thanks to Claudio Beccari (@OldClaudio), adding a commentary no longer generates a bad `\hbox` during TeX processing (see [#1202](https://github.com/gregorio-project/gregorio/issues/1202)).

## [4.1.4] - 2016-05-29
### Fixed
- Package conflict with luatex-ja also affected the custos.  Have now fixed that problem too.  See [this thread](http://www.mail-archive.com/gregorio-users@gna.org/msg03520.html).

## [4.1.3] - 2016-05-26
### Fixed
- Package conflict with luatex-ja has been resolved.  Notes and lyrics should now appear in documents which use the luatex-ja package.  See [#1107](https://github.com/gregorio-project/gregorio/issues/1107).


## [4.1.2] - 2016-05-08
### Changed
- The parmesan font is now called granapadano.  If you were using `\gresetgregoriofont{parmesan}`, you should now use `\gresetgregoriofont{granapadano}`.  See [#1075](https://github.com/gregorio-project/gregorio/issues/1075).
- GregorioTeX is now compatible with TeX Live 2016


## [4.1.1] - 2016-03-10
### Fixed
- Custos now shouldn't go right of the staff lines (was visible only in edge cases, see [#990](https://github.com/gregorio-project/gregorio/issues/990)).
- Corrected the end-of-line shift for a ragged line break before `<eu>` blocks (see [#988](https://github.com/gregorio-project/gregorio/issues/988)).
- Additional space is now added on top of lines with very high notes or signs (see [#883](https://github.com/gregorio-project/gregorio/issues/883)).
- High notes are now taken into account for vertical placement of above lines test (see [#960](https://github.com/gregorio-project/gregorio/issues/960)).
- Forced syllable centers can be used in to override alignment issues when lyric centering is set to `syllable` or `firstletter`.  This is a restoration of old behavior (pre-4.0), but can be turned off with `\gresetgabcforcecenters{prohibit}`.  See [968](https://github.com/gregorio-project/gregorio/issues/968).
- Deminutus figures in nabc work again (see [#1015](https://github.com/gregorio-project/gregorio/issues/1015)).
- Orphaned syllables should appear less frequently at end of score (see [1019](https://github.com/gregorio-project/gregorio/issues/1019)).

### Added
- The macro `\grechangecount` now allows to change some numeric values of the configuration. This version introduces two of them: `additionaltopspacethreshold` and `additionaltopspacealtthreshold`, see GregorioRef for details.
- The penalty `grefinalpenalty` (0 by default) is added at the end of the score. Be sure you know what you're doing before modifying it.

## [4.1.0] - 2016-03-01
### Fixed
- Hyphens now shouldn't go right of the staff lines (see [#845](https://github.com/gregorio-project/gregorio/issues/845)).
- Horizontal episema on initio debilis is now sized correctly (see [#880](https://github.com/gregorio-project/gregorio/issues/880)).
- Space between the text of two syllables of the same word when no hyphen is necessary is now strictly 0 (see [#922](https://github.com/gregorio-project/gregorio/issues/922))
- A horizontal episema on the first note of a torculus deminutus is now placed correctly.  See [#926](https://github.com/gregorio-project/gregorio/issues/926).
- Compilation of documents produced with Gregorio should be slightly faster.
- The alignment of the vertical episema under a punctum inclinatum deminutus is now correct (see [#742](https://github.com/gregorio-project/gregorio/issues/742)).
- `<eu>` and `<nlba>` may now be ended on the final divisio maior/finalis (see [#743](https://github.com/gregorio-project/gregorio/issues/743)).
- Sign positioning on the first note of quadratum figures is now correct (see [#752](https://github.com/gregorio-project/gregorio/issues/752)).
- Interlinear spacing of annotations when font size was smaller than normal.  You should now see just the spacing specified by `annotationseparation`.
- Spacing is now correct and ledger lines are now typeset for flats, sharps, and naturals above and below the staff (see [#790](https://github.com/gregorio-project/gregorio/issues/790) and [#882](https://github.com/gregorio-project/gregorio/issues/882)).
- Forced hyphens at the end of words are no longer suppressed (see [#804](https://github.com/gregorio-project/gregorio/issues/804)).
- Three descending, otherwise unmodified notes are now correctly grouped as a clivis followed by a punctum.  If you prefer three notes butted up against each other, separate them with `!`.

### Changed
- `spacebeforeeolcustos` has been increased, aproximately doubling its size.  It's also been made fixed by default, but is still capapble of accepting a rubber value should you so desire.
- The names of the spaces around bars have changed, see `gsp-default` or GregorioRef for the new names. It is now possible to tune the spacing of the virgula separately from the divisio minima, and the Dominican bar separately from the divisio minor.
- The stem length determination will use the ledger line below the note with the stem (see [#863](https://github.com/gregorio-project/gregorio/issues/863)).
- Made the oriscus orientation dependent on the note that follows.  Using `1` will force the ascending oriscus and `0` will force the descending oriscus.  The old behavior may be restored by setting the `oriscus-orientation` gabc header to `legacy`.  See UPGRADE.md for details (for the change request, see [#774](https://github.com/gregorio-project/gregorio/issues/774) and [#898](https://github.com/gregorio-project/gregorio/issues/898)).
- Add new lines as needed to the gabc output. (see [#905](https://github.com/gregorio-project/gregorio/issues/905)).
- Command line output (help message, version message, error handling) has been improved, see [#891](https://github.com/gregorio-project/gregorio/issues/891)
- Windows installer no longer copies the executable into the TeX bin directory.  Instead it has the option to add its own bin directory to PATH.
- `system-setup.sh` has become `system-setup.command` so that it is double click executable on Mac.
- Post install options for Windows installer have been simplified.  There are no longer separate options for MiKTeX and TeX Live.  Instead the installer will determine which you have itself and act accordingly.
- Windows installer will check more locations for old installations to remove.
- Height of the porrectus have been altered (first note moved up, second also moved up when second ambitus is more than one) so that they are more coherent with the punctum height. The porrectus stroke drawings have also been slightly altered to be slightly thinner and sharper around the bottom note (see [#858](https://github.com/gregorio-project/gregorio/issues/858)).
- When a syllable is preceeded by a punctum mora, gregorio now ignores the punctum mora in the spacing of the syllables by default, and also adds a custom space (defaulting to 0). You can change this behavior with `\gresetshiftaftermora{}`, see GregorioRef for its arguments. To balance the output, space before a punctum mora has been made slightly thinner, more in line with old Solesmes books. (For the change requests, see [#795](https://github.com/gregorio-project/gregorio/issues/795) and [#871](https://github.com/gregorio-project/gregorio/issues/871)).
- The `--admin` option is removed from the `initexmf` call for MiKTeX installations, allowing the installer to work better on Windows 10.
- MiKTeX installations no longer copy files into TEXMFLOCAL but instead registers the texmf folder which the installer creates with MiKTeX, allowing it to use those files in place.  See [#884](https://github.com/gregorio-project/gregorio/issues/884)
- Windows installer now contains an uninstall script which will run as part of the uninstall action and thus should remove the Gregorio executable and TeX files from their copied locations (leaving a clean post-uninstall system).
- Initial handling has been simplified.  The initial style should now be specified from TeX by using the `\gresetinitiallines` command, rather than from a gabc header.  Big initials and normal initials are now governed by a single `initial` style, meant to be changed between scores as appropriate.  See [UPGRADE.md](UPGRADE.md) and GregorioRef for details (for the change request, see [#632](https://github.com/gregorio-project/gregorio/issues/632)).  Deprecations for this change are listed in the Deprecation section, below.
- `\gresethyphen` no longer manipulates `maximumspacewithoutdash`, allowing for restoration of consistent behavior after this distance has been modified.  See [#705](https://github.com/gregorio-project/gregorio/issues/705).
- The oriscus-based shapes in the greciliae font are more consistent.  The shape of a scandicus with a second ambitus of two is more consistent across all score fonts.
- Minimal space between notes of different syllables (or words) has been reduced when the second syllable starts with an alteration.
- The space between note and horizontal episema has been tightened for notes at the `c` or `k` height when there is no ledger line.  Due to the intricacies of measurement, the system tries to make a best guess as to the existence of the ledger line.  If the guess is wrong, you may use the `[hl:n]` and `[ll:n]` notations in gabc to override the guess.  See [UPGRADE.md](UPGRADE.md) for details (for aspects of change request, see [#716](https://github.com/gregorio-project/gregorio/issues/716), and [#862](https://github.com/gregorio-project/gregorio/issues/862)).
- The custos that might appear immediately before a `<eu>` block is now suppressed by default.  This behavior is controlled by the `\greseteolcustosbeforeeuouae` command.  See GregorioRef and [UPGRADE.md](UPGRADE.md) for details (for the change request, see [#761](https://github.com/gregorio-project/gregorio/issues/761)).
- Different glyphs will now be used for the liquescent stropha on a line or between lines, if supported by the font.  If you prefer the old behavior, use `\grechangeglyph{StrophaAuctaLongtail}{*}{StrophaAucta}` in your TeX file.  See [#773](https://github.com/gregorio-project/gregorio/issues/773).
- Headers not recognized by gregorio, rather than inducing an error, are now simply accepted by gregorio.  Most of the old "standard" headers, like `book` and `manuscript-location`, are now handled in this way and thus are no longer limited to one instance.  See GregorioRef for details.
- `mode` will now be converted to Roman numerals in lower case so that it shows up as small capitals using the default `modeline` style.  See [UPGRADE.md](UPGRADE.md) for details (for the change request, see [#756](https://github.com/gregorio-project/gregorio/issues/756)).
- `\grecommentary` is now less restrictive about where it occurs and need not occur directly before the score anymore.

### Added
- It is now possible to turn off rythmic signs (separately or all together) with `\greprintsigns`, see GregorioRef for details (and [#936](https://github.com/gregorio-project/gregorio/issues/936) for request).
- Space between the two bars of a divisio finalis is now tunable through `divisiofinalissep`.
- Added `notesonly` argument to `\gresetshiftaftermora` (see [#948](https://github.com/gregorio-project/gregorio/issues/948)).
- Controls for tuning horizontal episema vertical position.  See GregorioRef for details (for the change request, see [#872](https://github.com/gregorio-project/gregorio/issues/872)).
- More dimensions are tunable: `overhepisemalowshift`, `overhepisemahighshift`, `underhepisemalowshift`, `underhepisemahighshift`, `hepisemamiddleshift`, `vepisemalowshift`, `vepisemahighshift`, `linepunctummorashift`, `spacepunctummorashift`, `spaceamonepespunctummorashift`, `lineporrectuspunctummorashift`, `spaceporrectuspunctummorashift`, `raresignshift`.  See GregorioRef for details (for the change request for most of these, see comments in [#872](https://github.com/gregorio-project/gregorio/issues/872)).
- A new dimension, `intersyllablespacestretchhyphen` is now available to add stretching in the case of syllables separated by an hyphen (see comments in [#922](https://github.com/gregorio-project/gregorio/issues/922))
- A new algorithm for placing bar syllables has been added and is now the default. See UPGRADE.md for more details.  The old spacing can be activated with `\gresetbarspacing{old}`.  See the following for various aspects of the implementation:
  - [#767](https://github.com/gregorio-project/gregorio/issues/767)
  - [#919](https://github.com/gregorio-project/gregorio/issues/919)
  - [#944](https://github.com/gregorio-project/gregorio/issues/944)
  - [#978](https://github.com/gregorio-project/gregorio/issues/978)
- It is now possible to generate a font with a different stem length schema (apart from the default one, Solesmes' is available), see [fonts/README.md](fonts/README.md) for details.
- Some forms (`ed` for example) will have a different stem length if a ledger line is present. Heuristics to determine the presence of a ledger line or not are quite simple and may not always be correct. Gabc now allows you to force a note to act as if a ledger line was present with `[ll:1]`, or as if there was no ledger line, with `[ll:0]`. See Gabc section of GregorioRef for details.
- The ability to typeset an inter-glyph space using `/!` in gabc (see [#868](https://github.com/gregorio-project/gregorio/issues/868))
- Support for two-, three-, and five-line staves.  Set the `staff-lines` header to `2`, `3`, or `5`.  For all values of `staff-lines`, the note below the staff remains 'c'.  The two new notes above the staff (for a five-line staff) are `n` and `p`.  See [#429](https://github.com/gregorio-project/gregorio/issues/429).
- Salicus flexus glyphs (see [#631](https://github.com/gregorio-project/gregorio/issues/631)).
- Neume fusion, activated in gabc by `@`.  Use `@` before a clivis or a porrectus to get an unstemmed figure.  Use `@` between two notes to fuse them explicitly.  Enclose a set of notes within `@[` and `]` to automatically guess their fusion.  See GregorioRef for details (for the channge requests, see [#679](https://github.com/gregorio-project/gregorio/issues/679), [#687](https://github.com/gregorio-project/gregorio/issues/687), and [#692](https://github.com/gregorio-project/gregorio/issues/692)).
- Hollow version of the oriscus, called by adding the `r` modifier to an oriscus, as in `gor` or `gor<` (See [#724](https://github.com/gregorio-project/gregorio/issues/724)).
- Headers are now passed to TeX and may be captured in TeX by using the `\gresetheadercapture` command.  See GregorioRef for details.
- Support for half-spaces and ad-hoc spaces.  Use `/0` in gabc for a half-space between notes.  Use `/[factor]` (substituting a positive or negative real number for the scale factor) for an ad-hoc space whose length is `interelementspace` scaled by the desired factor.  See [#736](https://github.com/gregorio-project/gregorio/issues/736).
- Support for custom length ledger lines.  See GregorioRef for details (for the change request, see [#598](https://github.com/gregorio-project/gregorio/issues/598)).
- Support for a secondary clef.  Use `@` to join two clefs together, as in `c1@c4`.  The first clef is considered the primary one and will be used when computing an automatic custos before a clef change.  See [#755](https://github.com/gregorio-project/gregorio/issues/755).
- New headers for typesetting text after `mode` above the initial: `mode` will be typeset first, using the `modeline` style, then `mode-modifier`, using the `modemodifier` style, then `mode-differentia`, using the `modedifferentia` style.  See GregorioRef for details (for the change request, see [#756](https://github.com/gregorio-project/gregorio/issues/756)).
- Automatic line breaks before a `<eu>` block may be made ragged by using `\gresetbreakbeforeeuouae{ragged}`.  See GregorioRef for details (for the change request, see [#764](https://github.com/gregorio-project/gregorio/issues/764)).
- The ability to typeset simple slurs.  See GregorioRef for details (for the change request, see [#776](https://github.com/gregorio-project/gregorio/issues/776)).
- More control over the alignment of the annotation.  You can now set either the first line or the last line of the annotation as the controling line with `\gresetannotationby{firstline}` and `\gresetannotationby{lastline}` respectively.  Additionaly, within the line you can specify the top, baseline, or bottom of the line as the alignment point with `\gresetannotationvalign{top}`, `\gresetannotationvalign{baseline}`, and `\gresetannotationvalign{bottom}` respectively.  Default behavior is baseline of the top line (as it was before).  These alignment controls are applied before `annotationraise` is taken into account.  See [#768](https://github.com/gregorio-project/gregorio/issues/768) for details.
- More control over the placement of the commentary.  You can now set the amount of space between the lines of a multi-line commentary with `commentaryseparation` and the distance from the top line of the staff to the baseline of the bottom line of the commentary with `commentaryraise`.  See [#662](https://github.com/gregorio-project/gregorio/issues/662) for original request.
- Styles for the annotation and the commentary.  `annotation` has no default styling.  `commentary` defaults to footnote sized italics.
- `\grecommentary` now takes an optional argument which will add extra space between the commentary and the score for just the next score.
- The custos can now be selectively enabled/disabled at forced line breaks by appending `+` (to enable) or `-` (to disable) after the `z` or `Z` (see [#800](https://github.com/gregorio-project/gregorio/issues/800)).
- With thanks to an anonymous contributor, the shape of the MetaPost-drawn curly brace has been improved (see [#797](https://github.com/gregorio-project/gregorio/issues/797)).
- Support for elisions, text between `<e>` and `</e>` in gabc, styled according to the `elision` style, which are excluded when determining where to position the text under its notes (see [#838](https://github.com/gregorio-project/gregorio/issues/838)).
- Special characters (between `<sp>` and `</sp>` in gabc) may now be customized using the `\gresetspecial` and `\greunsetspecial` commands.  See GregorioRef for details (for the change request, see [#846](https://github.com/gregorio-project/gregorio/issues/846)).
- `\gresetbracerendering` can now take an optional argument to change the rendering of a single type of brace.  See GregorioRef for details (for the change request, see [#850](https://github.com/gregorio-project/gregorio/issues/850)).

### Deprecated
- `initial-style` gabc header, supplanted by the `\gresetinitiallines` TeX command.
- `biginitial` style, consolidated into the `initial` style.
- `\grescorereference`

### Removed
- `\GreSetStaffLinesFormat`, supplanted by `\grechangestyle{normalstafflines}...`
- `\greinitialformat`, if you were redefining this command, use `\grechangestyle{initial}...` instead
- `\grebiginitialformat`, if you were redefining this command, use `\grechangestyle{biginitial}...` instead
- `\gretranslationformat`, if you were redefining this command, use `\grechangestyle{translation}...` instead
- `\greabovelinestextstyle`, if you were redefining this command, use `\grechangestyle{abovelinestext}...` instead
- `\grelowchoralsignstyle`, if you were redefining this command, use `\grechangestyle{lowchoralsign}...` instead
- `\grehighchoralsignstyle`, if you were redefining this command, use `\grechangestyle{highchoralsign}...` instead
- `\setaboveinitialseparation`, supplanted by `\grechangedim{annotationseparation}...`
- `\scorereference`
- `\GreScoreReference`
- `\commentary`, supplanted by `\grecommentary`
- `\setgretranslationcenteringscheme`, supplanted by `\gresettranslationcentering`
- `\englishcentering`, supplanted by `\gresetlyriccentering{syllable}`
- `\defaultcentering`, supplanted by `\gresetlyriccentering{vowel}`
- `\setgrefactor`, supplanted by `\grechangestaffsize`
- `\forcecompilegabc`, supplanted by `\gresetcompilegabc{force}`
- `\autocompilegabc`, supplanted by `\gresetcompilegabc{auto}`
- `\nevercompilegabc`, supplanted by `\gresetcompilegabc{never}`
- `\includescore`, supplanted by `\gregorioscore`
- `\grenoscaledim`, supplanted by `\grescaledim{...}{no}`
- `\gresetdim`, supplanted by `\grecreatedim`
- `\setstafflinethickness`, supplanted by `\grechangestafflinethickness`
- `\grecoloredlines`, supplanted by `\gresetlinecolor`
- `\greredlines` and `\redlines`, supplanted by `\gresetlinecolor{gregoriocolor}`
- `\grenormallines` and `\normallines`, supplanted by `\gresetlinecolor{black}`
- `\greremovelines`, supplanted by `\gresetlines{invisible}`
- `\gredonotremovelines`, supplanted by `\gresetlines{visible}`
- `\GreHidePCLines`, supplanted by `\gresetlinesbehindpunctumcavum{invisible}`
- `\GreDontHidePCLines`, supplanted by `\gresetlinesbehindpunctumcavum{visible}`
- `\GreHideAltLines`, supplanted by `\gresetlinesbehindalteration{invisible}`
- `\GreDontHideAltLines`, supplanted by `\gresetlinesbehindalteration{visible}`
- `\gresetnlbintranslation`, supplanted by `\gresetbreakintranslation`
- `\greblockcustos`, supplanted by `\greseteolcustos{manual}`
- `\greenableeolshifts`, supplanted by `\greseteolshifts{enable}`
- `\gredisableeolshifts`, supplanted by `\greseteolshifts{disable}`
- `\GreUseNoramalHyphen`, supplanted by `\greseteolhyphen{normal}`
- `\GreUseZeroHyphen`, supplanted by `\greseteolhyphen{zero}`
- `\greremoveclef`, supplanted by `\gresetclef{invisible}`
- `\grenormalclef`, supplanted by `\gresetclef{visible}`
- `\AddHEpisemusBridges`, supplanted by `\gresethepisema{bridge}`
- `\RemoveHEpisemusBridges`, supplanted by `\gresethepisema{break}`
- `\UseAlternatePunctumCavum`, supplanted by `\gresetpunctumcavum{alternate}`
- `\UseNormalPunctumCavum`, supplanted by `\gresetpunctumcavum{normal}`
- `\clivisalignmentalways`, supplanted by `\gresetclivisalignment{always}`
- `\clivisalignmentnever`, supplanted by `\gresetclivisalignment{never}`
- `\clivisalignmentspecial`, supplanted by `\gresetclivisalignment{special}`
- `\greusedefaultstyle`, supplanted by `\gresetglyphsytle{default}`
- `\greusemedicaeastyle`, supplanted by `\gresetglyphsytle{medicaea}`
- `\greusehufnagelstyle`, supplanted by `\gresetglyphsytle{hufnagel}`
- `\greusemensuralstyle`, supplanted by `\gresetglyphsytle{mensural}`
- `\setspaceafterinitial`, supplanted by `\grechangedim{afterinitialshift}...`
- `\setspacebeforeinitial`, supplanted by `\grechangedim{beforeinitialshift}...`
- `\setinitialspacing`, supplanted by `\grechangedim{beforeinitialshift}...`, `\grechangedim{manualinitialwidth}...`, and `\grechangedime{afterinitialshift}...`
- `centering-scheme` gabc header, supplanted by `\gresetlyriccentering` in TeX.  See GregorioRef for syntax.
- `gregoriotex-font` gabc header, supplanted by `\gresetgregoriofont` in TeX.  See GregorioRef for syntax.
- The meaningless `gabc-version` header in gabc (see [#664](https://github.com/gregorio-project/gregorio/issues/664)).

### Known Bugs
- When there is no manual break, the spacing around bars at the end of a line in the new spacing algorithm is not perfect, this should be visible only in edge cases. If you encounter such a bad spacing, you are advised to force a line break manually. See [#959](https://github.com/gregorio-project/gregorio/issues/959).
- Above lines text does not move to account for high notes which occur in the same place.  As a result the two may overlap.  You can work around this by changing `abovelinestextraise` but this is a global setting, affecting the entire score.  See [#960](https://github.com/gregorio-project/gregorio/issues/960).
- A manual custos in a low note position will not push down the text associated with it, resulting in the two overlapping.  See [#961](https://github.com/gregorio-project/gregorio/issues/961).


## [4.0.1] - 2015-12-28
### Fixed
- `\greseteolcustos` now retains its setting across multiple score inclusions (see [#703](https://github.com/gregorio-project/gregorio/issues/703)).
- Gregorio now works against luaotfload 2.6 (see [#717](https://github.com/gregorio-project/gregorio/issues/717)).
- When beginning of line clefs are invisible and bol shifts are enabled, lyric text will no longer stick out into the margin.  Further the notes on the first and subsequent lines now align properly.  See [#683](https://github.com/gregorio-project/gregorio/issues/683).
- `\grecross` and `\grealtcross` now print the correct glyphs (see [#713](https://github.com/gregorio-project/gregorio/issues/713)).


## [4.0.0] - 2015-12-08
### Fixed
- Handling of the first syllable in gabc is now more consistent with the all other syllables.  This centers the syllable correctly when using latin syllable centering (see [#42](https://github.com/gregorio-project/gregorio/issues/42)) and makes the use of styles less surprising in the first syllable (see [#135](https://github.com/gregorio-project/gregorio/issues/135)).
- Handling of manually-placed custos is improved.  In particular, a manual custos at the end of the score should no longer be lost when the bar happens to be at the end of the line.
- Improved rendering of torculus resupinus flexus figures (see [#18](https://github.com/gregorio-project/gregorio/issues/18)).
- Horizontal episema on the second note of a scandicus is now rendered in the correct place (see [#419](https://github.com/gregorio-project/gregorio/issues/419)).
- The dot in a space directly above a punctum in a descending neume is now placed slightly higher in the space (see [#386](https://github.com/gregorio-project/gregorio/issues/386) and [Gna! bug 21737](https://gna.org/bugs/?21737)).
- Choral signs are now positioned correctly around porrectus and torculus resupinus (see [#387](https://github.com/gregorio-project/gregorio/issues/387) and [Gna! bug 22025](https://gna.org/bugs/?22025)).
- Gregorio will now try harder to select an appropriate pitch for an automatic custos (`z0`) on a clef change (see [#446](https://github.com/gregorio-project/gregorio/issues/446)).  If results are not satisfactory, use a manual custos (`+`) to select a pitch manually.
- The centering of styled text under notes is now correct (See [#509](https://github.com/gregorio-project/gregorio/issues/509)).
- Space for above lines text is now correctly added as needed, even at the beginning of a score (see [#533](https://github.com/gregorio-project/gregorio/issues/533)).
- Corrected the rendering of explicit automatic and manual custos at the end of lines when the clef change that follows it is pushed to the next line (see [#569](https://github.com/gregorio-project/gregorio/issues/569)).
- Distinguished between `eolshift` and `bolshift` giving each their own flag and user commmand for turning them on and off.  `\seteolshift{enable}` allows the lyric text to stretch under the custos at the end of the line.  `\setbolshift{enable}` aligns the beginning of each line on the notes instead of the text.  Both are on by default, but can be turned off with `\seteolshift{disable}` and `\setbolshift{disable}`.
- The spacing of manual in-line custos (`(f+)` in gabc) is now consistent with the spacing of automatic in-line custos (`(z0)` in gabc).  See [#642](https://github.com/gregorio-project/gregorio/issues/642).
- Signs on the climacus praepunctis deminutus `(ghgf~)` neume are now positioned correctly.  See [#650](https://github.com/gregorio-project/gregorio/issues/650)
- Mac installer has been made SIP compliant (i.e. it now works on El Capitan).
- Mac installer can now detect installations of TeX Live done with MacPorts or the command-line tool provided by TUG.
- Windows executable has file version information attached correctly so that the installer can properly recognize and replace the binary during an upgrade process.
- Spacing was too large when alteration begins a syllable, see [#663](https://github.com/gregorio-project/gregorio/issues/663).

### Changed
- A new, more systematic naming scheme has been created for GregorioTeX macros.  The naming scheme should reduce the chances of naming conflicts with other packages and make it easier to identify what a particular macro is for and how to use it.  Most user functions have been renamed in order to bring them into line with this scheme.  Please see GregorioRef for a complete list of the new function names.  In general, old names will still work, but they will raise a deprecation warning and will be dropped from GregorioTeX in a future relase.
- The spacing algorithm has completely changed, expect your scores to look quite different (better we hope).
- Lines are now aligned on the leftmost note instead of the leftmost letter.
- Some glyph shapes are improved a little in greciliae, it should be noticeable especially for porrectus.
- The `O` modifier in gabc now has expanded uses beyond the salicus `(egOi)`.  A stemmed oriscus will appear on a lone pitch `(gO)` or a followed by a lower pitch `(gOe)` (see [#76](https://github.com/gregorio-project/gregorio/issues/76)).  A virga strata will appear on the second note of two ascending pitches `(giO)`.
- With thanks to *Abbazia Mater Ecclesiae (IT)* for drawing the new shapes, the strophicus, liquescent strophicus, liquescent oriscus, and liquescent punctum inclinatum in greciliae have changed to better differentiate them from other shapes. If you prefer the old shapes use:
```
    \grechangeglyph{Stropha}{greciliae}{.caeciliae}
    \grechangeglyph{StrophaAucta}{greciliae}{.caeciliae}
    \grechangeglyph{OriscusDeminutus}{greciliae}{.caeciliae}
    \grechangeglyph{PunctumInclinatumAuctus}{greciliae}{.caeciliae}
```
- Default initial sizes have been chosen so that they are more appropriate when an infinitely scaling font is loaded.  LaTeX will make an automatic substitution of the closest avaialble size when such a font is not used.
- Porrectus deminutus and torculus resupinus deminutus glyphs have been updated to more closely match the current Solesmes books (see [#143](https://github.com/gregorio-project/gregorio/issues/143)).  If you prefer the old forms, use:
```
    \grechangeglyph{Porrectus*}{*}{.alt}
    \grechangeglyph{TorculusResupinus*}{*}{.alt}
```
- New (much) improved drawings for letter bars (for Versicle, Antiphon, etc.). You must fine-tune them if you use a text font other than Linux Libertine, see [UPGRADE.md](UPGRADE.md) for details.
- The default extension `gregorio` (the executable program) will use when it produces GregorioTeX files has been changed from `.tex` to `.gtex`.  Any calls to `\includescore` that use the old extension should be changed appropriately.
- Horizontal episema improvements:
  - A horizontal episema on the first two notes of a porrectus is now rendered differently than a horizontal episema on all three notes of a porrectus.  It will no longer reach to the right side of the porrectus  (see [#70](https://github.com/gregorio-project/gregorio/issues/70)).
  - Similar to appending `0` to the `_` to force a horizontal episema to appear below a note, `1` may now be appended to the `_` to force a horizontal episema to appear above the note.  These may be used simutaneously, by adding both `_0` and `_1` to the same note (see [#40](https://github.com/gregorio-project/gregorio/issues/40)).
  - It is now possible to render a small horizontal episema.  To use it, append `3` (for left-aligned), `4` (for center-aligned), or `5` (for right-aligned) to the `_` used for adding the horizontal episema in gabc (see [#40](https://github.com/gregorio-project/gregorio/issues/40)).
  - Horizontal episema "bridging" has been made more consistent and is now possible on an episema that is below the note. (see [#389](https://github.com/gregorio-project/gregorio/issues/70)).
  - It is now possible to suppress the automatic horizontal episema "bridging" across spaces within a syllable.  To use it, append `2` to the `_` on the note before the space you do not want bridged (see [#72](https://github.com/gregorio-project/gregorio/issues/72)).
  - A horizontal episema will no longer overlap with a vertical episema on the same note (see [#395](https://github.com/gregorio-project/gregorio/issues/395)).
- The dot after a torculus whose last note is on a line will now appear below the line (see [#386](https://github.com/gregorio-project/gregorio/issues/386)).  If you do not like the automatic placement of a dot after any note on a line (not just the torculus), you may specify a `0` or `1` after the `.` to force the dot to appear below or above the line, respectively.
- Choral sign dimensions have been renamed (see [#387](https://github.com/gregorio-project/gregorio/issues/387)):
  - `beforechoralsignspace` is now `beforelowchoralsignspace`.
  - `lowchoralsignshift` is now `choralsigndownshift`.
  - `highchoralsignshift` is now `choralsignupshift` and its sign is now inverted.
- Style for score elements can now be changed via the `\grechangestyle` command.  This replaces the mixed system of styling commands which could be redefined for some elements and specialized commands for applying styles to others.  See GregorioRef for details.
- Annotations with more than two lines are now supported (originally requested [on the user list](http://www.mail-archive.com/gregorio-users%40gna.org/msg00164.html) when two line annoations were made possible).  To build the annotation box use `\greannoataion`.  See GregorioRef for details.
- The `annotation` header field in `gabc` now places its value(s) above the inital if no annotation is explicitly given by the user via `\greannotation` (see [#44](https://github.com/gregorio-project/gregorio/issues/44)).
- `\grescaledim` now takes two arguments to bring it into line with the systemized naming scheme.  The second argument should be `scalable` if you want the distance to scale when the staff size changes, `fixed` if you don't.
- Gregorio is now able to make individual lines of a score taller, when the position of the note require extra space, without affecting the rest of the lines.  This is the new default behavior.  See [UPGRADE.md](UPGRADE.md) and GregorioRef for details (for the change request, see [#59](https://github.com/gregorio-project/gregorio/issues/59)).
- Braces are now rendered using MetaPost by default.  This allows the line weight to remain more consistent when braces are stretched.  The old behavior (which uses the score font instead) can be restored using `\gresetbracerendering{font}`.  See [UPGRADE.md](UPGRADE.md) and GregorioRef for details (for the change request, see [#535](https://github.com/gregorio-project/gregorio/issues/535)).
- `\grecreatedim` and `\grechangedim` now take keywords for their third argument (`scalable` and `fixed`) instead of integers (`1` and `0`) to make the more in keeping with the overall user command conventions.
- Alterations are partially ignored when aligning lines on the notes (i.e. `\gresetbolshifts{enabled}`).  They are not allowed to get any closer to the clef than `beforealterationspace` and the lyrics are not allowed to get any closer to the left-hand margin than `minimalspaceatlinebeginning`, but other than that GregorioTeX will shift them left as much as possible to make the notes align `spaceafterlineclef` away from the clef.  Note that for the default values of these distances, only the natural is small enough to acheive true alignment.
- `gregoriotex.sty` and `gregoriosyms.sty` now check to make sure that they are not both loaded.  If `gregoriotex` detects that `gregoriosyms` is loaded, then an error is raised.  If `gregoriosyms` detects that `gregoriotex` is loaded, then the loading of `gregoriosyms` is silently aborted and compilation proceeds.
- Liquescence on a bistropha or tristropha will only appear on the note(s) marked by `<` in gabc, rather than on all notes in the figure.  This means that a figure like `(gsss<)` will only have a liquescent "tail" on the final note.  If you would like all notes to be liquescent for some reason, you can use a figure like `(gs<gs<gs<)` instead.
- `alterationspace` is now a fixed dimension, see [UPGRADE.md](UPGRADE.md) for details.

### Added
- With thanks to Jakub Jelínek, St. Gallen style adiastematic notation is now handled through [nabc syntax](http://gregoriochant.org/dokuwiki/doku.php/language) (see GregorioNabcRef.pdf for details and [the new example](examples/FactusEst.gabc)). Only one line above the notes is currently handled. This is a preview, backward incompatible change are possible in future releases.
- `<eu>` tag in gabc to delimit *Euouae* block in the score. It prevents linebreaking and makes spaces tighter. See [UPGRADE.md](UPGRADE.md) for details.
- With thanks to Fr. Jacques Peron, it is now possible to embed short gabc snippets directly into a TeX document.  The command is `\gabcsnippet`.  See GregorioRef.pdf for full details.
- `[ub:x:y mm]` tag, similar to `[ob:x:y mm]`, but for braces under notes.
- `[ob:x{]` and `[ob:x}]` tag pairs for braces where the length is determined by the distance of the opening and closing tags.  `[ocb:x{]` with `[ocb:x}]`, `[ocba:x{]` with `[ocba:x}]` and `[ub:x{]` with `[ub:x}]` can be used similarly for other kinds of braces and brackets.
- The ability to substitute an arbitrary glyph for one used by GregorioTeX.  This adds four macros:
  - `\grechangeglyph` to make a score glyph substitution
  - `\greresetglyph` to remove a score glyph substitution
  - `\gredefsymbol` for (re-)defining an arbitrary non-score glyph that scales with the text
  - `\gredefsizedsymbol` for (re-)defining an arbitary non-score glyph that requires a point-size to be specified.
See GregorioRef.pdf for full details.
- Added `--with-kpathsea` option to configure script, to check input and output file against `openout_any` and `openin_any` settings of texmf.cnf (TeX Live only). Necessary to be included in `shell_escape_commands` in TeX Live.
- Support for `lualatex -recorder`.  Autocompiled gabc and gtex files will now be properly recorded so that programs like `latexmk -recorder` can detect the need to rebuild the PDF when a gabc file changes.
- A vertical episema may now be forced to appear above or below a note.  In gabc, use `'0` for the vertical episema to appear below and `'1` for the vertical episema to appear above (see [#385](https://github.com/gregorio-project/gregorio/issues/385)).
- The first word, first syllable, and first letter of the first syllable that is *not* interpreted as the initial of the score can now be styled from TeX.  Use `\grechangestyle` to modify the `firstsyllableinitial`, `firstsyllable`, and `firstword` as desired.
- The final line of a score may now be forced to be fully justified (rather than ragged) using `\gresetlastline{justified}` before including the score (see [#43](https://github.com/gregorio-project/gregorio/issues/43)).  Use `\gresetlastline{ragged}` to switch back to a ragged last line.
- `\gresethyphen{force}` forces GregorioTeX to put a hyphen between each syllable in a polysyllabic word.  `\gresethyphen{auto}` restores behavior to normal.
- Support for custom vowel centering rules.  Put a file called `gregorio-vowels.dat` into your project directory or into a directory accessible from TEXMF and add the header `language: name;` to your gabc file.  The `gregorio-vowels.dat` file describes how vowels are to be located in the *name* language.  See GregorioRef for details.
- `\gresetlinecolor` takes a named color as an argument.  As a result, the red staff lines can be made consistent with the text, even when the user changes `gregoriocolor` with `\gresetlinecolor{gregoriocolor}`.  Addresses [#21787 on the old tracker](https://gna.org/bugs/index.php?21787).
- Package option `allowdeprecated=false`. Causes all deprecated commands to raise an error and halt TeX.
- The ability to add LilyPond-like point-and-click textedit links into the PDF file to aid with debugging scores.  This must be explicitly enabled and **should be turned off** when producing files for distribution as it embeds path information into the output.  To enable this, you must pass the `-p` option to gregorio when compiling gabc files and add `\gresetpointandclick{on}` before including the score.  It may be toggled back off with `\gresetpointandclick{off}`.  See GregorioRef for details (for the change request, see [#528](https://github.com/gregorio-project/gregorio/issues/528)).
- New score fonts with glyphs unique to Dominican chant.  These fonts replace the epiphonus and the augmented liquescents with corresponding figures from Dominican liturgical books.  To use the new fonts, pass the `[op]` option to the `\gresetgregoriofont` command (i.e., `\gresetgregoriofont[op]{greciliae}`).  See GregorioRef for details (for the change request, see [#1](https://github.com/gregorio-project/gregorio/issues/1)).
- Support for "punctum cavum inclinatum" and "punctum cavum inclinatum auctus" figures.  The gabc for these are `(Gr)` and `(Gr<)`, where `G` is the capitalized pitch letter.
- `\greillumination`: allows user to specify arbitrary content (usually an image) to be used as the initial.
- New distance, `initialraise`, which will lift (or lower, if negative) the initial.
- A new type of lyric centering, enabled with `\gresetlyriccentering{firstletter}`, which aligns the neume with the first letter of each syllable.
- `\greornamentation` allows access to the two ornamentation glyphs.  The ability to access these two glyphs via `{\gregoriosymbolfont \char 75}` was broken by the new interface to the glyphs in greextra.
- The missing liquescent salicus glyphs.
- The ability to force a hyphen after an empty first syllable, enabled by default since this was the behavior prior to 4.0.  Version 4.0 has an improved spacing algorithm which will eliminate the hyphen if the notes for the first syllable are too close to the second.  To switch to this behavior, use `\gresetemptyfirstsyllablehyphen{auto}`.  See [UPGRADE.md](UPGRADE.md) and GregorioRef for details (for the change request, see [#653](https://github.com/gregorio-project/gregorio/issues/653)).
- Shell scripts for configuring TeXShop and TeXworks on a Mac.

### Deprecated
- `\GreSetStaffLinesFormat`, supplanted by `\grechangeformat{normalstafflines}...`
- `\greinitialformat`, if you were redefining this command, use `\grechangeformat{initial}...` instead
- `\grebiginitialformat`, if you were redefining this command, use `\grechangeformat{biginitial}...` instead
- `\gretranslationformat`, if you were redefining this command, use `\grechangeformat{translation}...` instead
- `\greabovelinestextstyle`, if you were redefining this command, use `\grechangeformat{abovelinestext}...` instead
- `\grelowchoralsignstyle`, if you were redefining this command, use `\grechangeformat{lowchoralsign}...` instead
- `\grehighchoralsignstyle`, if you were redefining this command, use `\grechangeformat{highchoralsign}...` instead
- `\setaboveinitialseparation`, supplanted by `\grechangedim{annotationseparation}...`
- `\scorereference`, supplanted by `\grescorereference`
- `\GreScoreReference`, supplanted by `\grescorereference`
- `\commentary`, supplanted by `\grecommentary`
- `\setgretranslationcenteringscheme`, supplanted by `\gresettranslationcentering`
- `\englishcentering`, supplanted by `\gresetlyriccentering{syllable}`
- `\defaultcentering`, supplanted by `\gresetlyriccentering{vowel}`
- `\setgrefactor`, supplanted by `\grechangestaffsize`
- `\forcecompilegabc`, supplanted by `\gresetcompilegabc{force}`
- `\autocompilegabc`, supplanted by `\gresetcompilegabc{auto}`
- `\nevercompilegabc`, supplanted by `\gresetcompilegabc{never}`
- `\includescore`, supplanted by `\gregorioscore`
- `\grenoscaledim`, supplanted by `\grescaledim{...}{no}`
- `\gresetdim`, supplanted by `\grecreatedim`
- `\setstafflinethickness`, supplanted by `\grechangestafflinethickness`
- `\grecoloredlines`, supplanted by `\gresetlinecolor`
- `\greredlines` and `\redlines`, supplanted by `\gresetlinecolor{gregoriocolor}`
- `\grenormallines` and `\normallines`, supplanted by `\gresetlinecolor{black}`
- `\greremovelines`, supplanted by `\gresetlines{invisible}`
- `\gredonotremovelines`, supplanted by `\gresetlines{visible}`
- `\GreHidePCLines`, supplanted by `\gresetlinesbehindpunctumcavum{invisible}`
- `\GreDontHidePCLines`, supplanted by `\gresetlinesbehindpunctumcavum{visible}`
- `\GreHideAltLines`, supplanted by `\gresetlinesbehindalteration{invisible}`
- `\GreDontHideAltLines`, supplanted by `\gresetlinesbehindalteration{visible}`
- `\gresetnlbintranslation`, supplanted by `\gresetbreakintranslation`
- `\greblockcustos`, supplanted by `\greseteolcustos{manual}`
- `\greenableeolshifts`, supplanted by `\greseteolshifts{enable}`
- `\gredisableeolshifts`, supplanted by `\greseteolshifts{disable}`
- `\GreUseNoramalHyphen`, supplanted by `\greseteolhyphen{normal}`
- `\GreUseZeroHyphen`, supplanted by `\greseteolhyphen{zero}`
- `\greremoveclef`, supplanted by `\gresetclef{invisible}`
- `\grenormalclef`, supplanted by `\gresetclef{visible}`
- `\AddHEpisemusBridges`, supplanted by `\gresethepisema{bridge}`
- `\RemoveHEpisemusBridges`, supplanted by `\gresethepisema{break}`
- `\UseAlternatePunctumCavum`, supplanted by `\gresetpunctumcavum{alternate}`
- `\UseNormalPunctumCavum`, supplanted by `\gresetpunctumcavum{normal}`
- `\clivisalignmentalways`, supplanted by `\gresetclivisalignment{always}`
- `\clivisalignmentnever`, supplanted by `\gresetclivisalignment{never}`
- `\clivisalignmentspecial`, supplanted by `\gresetclivisalignment{special}`
- `\greusedefaultstyle`, supplanted by `\gresetglyphsytle{default}`
- `\greusemedicaeastyle`, supplanted by `\gresetglyphsytle{medicaea}`
- `\greusehufnagelstyle`, supplanted by `\gresetglyphsytle{hufnagel}`
- `\greusemensuralstyle`, supplanted by `\gresetglyphsytle{mensural}`
- `\setspaceafterinitial`, supplanted by `\grechangedim{afterinitialshift}...`
- `\setspacebeforeinitial`, supplanted by `\grechangedim{beforeinitialshift}...`
- `\setinitialspacing`, supplanted by `\grechangedim{beforeinitialshift}...`, `\grechangedim{manualinitialwidth}...`, and `\grechangedime{afterinitialshift}...`
- `centering-scheme` gabc header, supplanted by `\gresetlyriccentering` in TeX.  See GregorioRef for syntax.
- `gregoriotex-font` gabc header, supplanted by `\gresetgregoriofont` in TeX.  See GregorioRef for syntax.
- The meaningless `gabc-version` header in gabc (see [#664](https://github.com/gregorio-project/gregorio/issues/664)).

### Removed
- GregorioXML and OpusTeX output
- Support for `lilypond-preamble`, `musixtex-preamble`, and `opustex-preamble` gabc headers.
- Support for the font Gregoria.
- Chironomy markings (gabc `u` and `U`), which were not working correctly in the first place.
- `\Vbarsmall`, `\greletterbar`, and `\greletteraltbar`, supplanted by the new glyph system, see [UPGRADE.md](UPGRADE.md).
- `\GreSetAboveInitialSeparation`, supplanted by `\grechangedim{annotationseparation}...`
- `\includetexscore`, supplanted by `\gregorioscore[n]`
- `\greincludetexscore`, supplanted by `\gregorioscore[n]`
- `\includegabcscore`, supplanted by `\gregorioscore[f]`
- `\greincludegabcscore`, supplanted by `\gregorioscore[f]`
- `\GreSetSpaceBeforeInitial`, supplanted by `\grechangedim{beforeinitialshift}...`
- `\GreSetSpaceAfterInitial`, supplanted by `\grechangedim{afterinitialshift}...`
- `\GreSetAboveInitialSeparation`, supplanted by `\grechangedim{annotationseparation}...`
- `\gresetstafflinefactor`, supplanted by `\grechangestafflinethickness`
- `greg-book` and `greg-lily-book` engines, supplanted by improved capabilities of `\gregorioscore` for compiling gabc files at time of document compilation.
- The TeXShop script for compiling gabc files.  Supplanted by the new autocompile feature of the package.

### Known Bugs - FIXED IN 4.0.1
- When beginning of line clefs are invisible and bol shifts are enabled, lyric text will stick out into the margin.  Further the notes on the first and subsequent lines do not align properly.  See [#683](https://github.com/gregorio-project/gregorio/issues/683).


## [3.0.3] - 2015-07-01
### Fixed
- Horizontal episemae on salicus figures now render correctly (See [#511](https://github.com/gregorio-project/gregorio/issues/511)).

## [3.0.2] - 2015-06-01
### Fixed
- Prevented the default small font of the above lines text from leaking into the lyrics.

## [3.0.1] - 2015-05-17
### Fixed
- Windows Installer is now compatible with MiKTeX.  See [#355](https://github.com/gregorio-project/gregorio/issues/355).

## [3.0.0] - 2015-05-14
### Changed
- [New website](http://gregorio-project.github.io) containing instructions only for new versions of Gregorio starting with this release, in English only.
- New clean Mac OSX installer (intel only).
- New incompatible format of space configuration files (`gsp-xxx.tex`).  Values are now scaled to the default staff size (see [#50](https://github.com/gregorio-project/gregorio/issues/50).  You now need to use `\gresetdim` for setting distances (`\somedistance = 3cm`) can no longer be used).  `\gresetdim` takes three arguments: the name of the distance, the desired value, and whether the distance should scale with changes in the staff size or not.  See `gsp-default.tex` for an example.
- All distances can now be set to scale with staff size, as a consequence `\grechangedim` now takes three arguments: the name of the distance, value to change the distance to (which now supports em and ex units), and whether or not this value should be scaled with changes in the staff size.  See doc/UserManual.pdf for details.
- `\setinitalspacing` , `\setspacebeforeinitial`, `\setspaceafterinitial`, and `\setaboveinitialseparation` now take an additional argument.  The new argument specifies whether the distance should scale when the staff size changes.
- Improved `\includescore` capabilities.  The `\includescore[f]` parameter has changed to `\includescore[n]` compared to version 2.4.2.  See doc/UserManual.pdf for full details and UPGRADING.md for instructions on how to upgrade your score from 2.4.2.
- Clivis stem length now follow Solesmes' books conventions (see [#31](https://github.com/gregorio-project/gregorio/issues/31)).
- Windows TeXworks configuration script no longer adds deprecated `greg-book` and `gregorio` engines (see below).
- `build.sh` and `install.sh` scripts are now used to build and install Gregorio with common options.
- Clarified post installation options for Windows installer.  What was the "Install Fonts" option is now labeled to indicate that this also adds GregorioTeX files to the texmf tree.
- `\grechangedim` now checks to make sure it only operates on existing distances and doesn't create a new one.

### Fixed
- `\includescore` not finding files for autocompile under certain circumstances.  (see [this thread](http://www.mail-archive.com/gregorio-users@gna.org/msg02346.html)).
- Spacing between a syllable and a syllable with text and only a bar was too short.
- Torculus followed by a non-liquescent note is now parsed correctly (see [#284](https://github.com/gregorio-project/gregorio/issues/284).
- Spacing after a syllable with an flat, sharp, or natural is now correct (see [#246](https://github.com/gregorio-project/gregorio/issues/246).

### Added
- `\setstafflinethickness` controls the thickness of the staff lines.  See GregorioRef.pdf for full details.
- `\gre@debug`.  Writes messages to the log file when the debug flag is set to true (can be done manually via `\debugtrue`, or via the `debug` option when loading the gregoriotex package in LaTeX).
- New documentation in PDF: GregorioRef.pdf. You can find it in the [release files](https://github.com/gregorio-project/gregorio/releases).
- A migration guide ([UPGRADE.md](UPGRADE.md))
- This CHANGELOG.
- A Windows batch file which will detect the system setup and create a report which can be useful in diagnosing installation problems.  Instructions for how to use it appear [under the installation instructions for Windows on the website.](http://gregorio-project.github.io/installation-windows.html)

### Deprecated
- OpusTeX writing and GregorioXML reading and writing features will disappear in next minor release
- The [old website](http://home.gna.org/gregorio/) contains instructions for old versions of Gregorio only, and will not be updated anymore.
- the `-O` option
- `\includetexscore`, supplanted by `\includescore[n]`
- `\greincludetexscore`, supplanted by `\includescore[n]`
- `\includegabcscore`, supplanted by `\includescore[f]`
- `\greincludegabcscore`, supplanted by `\includescore[f]`
- `\GreSetSpaceBeforeInitial`, supplanted by `\setspacebeforeinitial`
- `\GreSetSpaceAfterInitial`, supplanted by `\setspaceafterinitial`
- `\GreSetAboveInitialSeparation`, supplanted by `\setaboveinitialseparation`
- `\gresetstafflinefactor`, supplanted by `\setstafflinethickness`
- `greg-book` and `greg-lily-book` engines, supplanted by improved capabilities of `\includescore` for compiling gabc files at time of document compilation.

## 2.4.3 - 2015-03-14 [YANKED]

## 2.4.2 - 2015-02-27
### Changed
- Improved `\includescore` with GregorioTeX API version checking (auto-compiling when possibly breaking Gregorio update).
- Clivis and pes quadratum alignment now follows Solesmes' conventions more closely (see [#10](https://github.com/gregorio-project/gregorio/issues/10)).

### Fixed
- Reducing horizontal episema width.
- Reducing flat stem length.
- `mode` and `anotation-line` now do their job.
- Low episema (`_0`) under consecutive notes are now aligned correctly.
- Quilisma was melting too much with next note when in a second interval.
- Fixed custo blocking possibility.
- Fixing glyphs disapearing when importing into Illustrator.

### Added
- English centering scheme now available as GregorioTeX option.
- `\setinitialspacing` to control all spaces related to initial with one command.
- `\gremanualinitialwidth` macro to specify width of all initials.
- Virga aucta (for liquescent salicus), gabc `iv>`.
- Torculus liquescent deminutus and quilisma version, gabc `dfec~` and `dwfec~`.
- Pressus maior liquescens: `hof~`.
- Rare form of scandicus.
- Added `<c>` markup in gabc to denote a change of color.
- Added `<nlba>` markup in gabc to get areas with no line breaks.
- Possibility to center translation syllable by syllable, see [here](https://www.mail-archive.com/gregorio-users@gna.org/msg01760.html) and [here](https://www.mail-archive.com/gregorio-users@gna.org/msg01783.html).

## 2.0 - 2010-09-27
### Changed
- gregorio API changed and GregorioTeX macros prepended with `\gre`, to avoid potential name conflicts
- updated greciliae font
- fine-tuning the spacing, and making it easier for users to change the defaults
- GregorioXML reading is now optional (via `--enable-xml-read` flag)

### Fixed
- as always, fixing a lot of bugs

### Added
- Automatic Windows installer
- adding requested features: Dominican bars, choral signs, text above staff lines
- enabling comments in gabc files
- adding ability to write verbatim TeX at {note, glyph, element} level
- introducing horizontal episema bridges
- default output is now utf8 directly; the `-O`  option allows old-style TeX output, i.e. `\char XXXX`
- new static build system for packaged distributions


## 1.0 - 2009-10-19
### Changed
- changing the number of arguments of some TeX function
- changing the glyph names
- improving the spacings
- better management of the penalty in TeX so that the line changes are more consistent
- changing the markup system in gabc to be more natural

### Fixed
- fixing a lot of bugs

### Added
- adding the possibility to put a flat after the clef
- adding the possibility to put a custo before a clef change


## 0.9.2 - 2008-12-27
### Changed
- changing the number of arguments of some TeX function
- changing the glyph names

### Fixed
- fixing a lot of bugs

### Added
- LuaTeX additional functionalities


## 0.9.1 - 2008-11-23
### Changed
- changing the number of arguments of some TeX functions

### Fixed
- fixing a lot of small bugs


## 0.9 - 2008-07-25
### Changed
- stabilizing the TeX API

### Added
- adding support for Cygwin compilation
- adding too many new features to be listed


## 0.3 - 2008-01-18
### Changed
- changing the architecture of libraries and plugins

### Added
- adding support for end of lines
- adding support for compilation on MAC OSX


## 0.2.2 - 2007-06-14
### Added
- adding styles and centering in text

[3.0.0-rc1]: https://github.com/gregorio-project/gregorio/compare/v2.4.2...v3.0.0-rc1
[3.0.0-rc2]: https://github.com/gregorio-project/gregorio/compare/v3.0.0-rc1...v3.0.0-rc2
