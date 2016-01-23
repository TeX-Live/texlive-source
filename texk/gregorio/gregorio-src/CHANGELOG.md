# Change Log
All notable changes to this project will be documented in this file.
As of v3.0.0 this project adheres to [Semantic Versioning](http://semver.org/). It follows [some conventions](http://keepachangelog.com/).

## [Unreleased][unreleased]


## [4.1.0-beta1] - 2016-01-23
### Fixed
- The alignment of the vertical episema under a punctum inclinatum deminutus is now correct (see [#742](https://github.com/gregorio-project/gregorio/issues/742)).
- `<eu>` and `<nlba>` may now be ended on the final divisio maior/finalis (see [#743](https://github.com/gregorio-project/gregorio/issues/743)).
- Sign positioning on the first note of quadratum figures is now correct (see [#752](https://github.com/gregorio-project/gregorio/issues/752)).
- Interlinear spacing of annotations when font size was smaller than normal.  You should now see just the spacing specified by `annotationseparation`.
- Spacing is now correct and ledger lines are now typeset for flats, sharps, and naturals above and below the staff (see [#790](https://github.com/gregorio-project/gregorio/issues/790)).
- Forced hyphens at the end of words are no longer suppressed (see [#804](https://github.com/gregorio-project/gregorio/issues/804)).

### Changed
- Initial handling has been simplified.  The initial style should now be specified from TeX by using the `\gresetinitiallines` command, rather than from a gabc header.  Big initials and normal initials are now governed by a single `initial` style, meant to be changed between scores as appropriate.  See [UPGRADE.md](UPGRADE.md) and GregorioRef for details (for the change request, see [#632](https://github.com/gregorio-project/gregorio/issues/632)).  Deprecations for this change are listed in the Deprecation section, below.
- `\gresethyphen` no longer manipulates `maximumspacewithoutdash`, allowing for restoration of consistent behavior after this distance has been modified.  See [#705](https://github.com/gregorio-project/gregorio/issues/705).
- The oriscus-based shapes in the greciliae font are more consistent.  The shape of a scandicus with a second ambitus of two is more consistent across all score fonts.
- Minimal space between notes of different syllables (or words) has been reduced when the second syllable starts with an alteration.
- The space between note and horizontal episema has been tightened for notes at the `c` or `k` height when there is no ledger line.  Due to the intricacies of measurement, the system tries to make a best guess as to the existence of the ledger line.  If the guess is wrong, you may use the `[hl:n]` and `[ll:n]` notations in gabc to override the guess.  See [UPGRADE.md](UPGRADE.md) for details (for the change request, see [#716](https://github.com/gregorio-project/gregorio/issues/716)).
- The custos that might appear immediately before a `<eu>` block is now suppressed by default.  This behavior is controlled by the `\greseteolcustosbeforeeuouae` command.  See GregorioRef and [UPGRADE.md](UPGRADE.md) for details (for the change request, see [#761](https://github.com/gregorio-project/gregorio/issues/761)).
- Different glyphs will now be used for the liquescent stropha on a line or between lines, if supported by the font.  If you prefer the old behavior, use `\grechangeglyph{StrophaAuctaLongtail}{*}{StrophaAucta}` in your TeX file.  See [#773](https://github.com/gregorio-project/gregorio/issues/773).
- Made the oriscus orientation dependent on the note that follows.  Using `<` will force the ascending oriscus and `>` will force the descending oriscus.  The old behavior may be restored by setting the `oriscus-orientation` gabc header to `legacy`.  See UPGRADE.md for details (for the change request, see [#774](https://github.com/gregorio-project/gregorio/issues/774)).
- Headers not recognized by gregorio, rather than inducing an error, are now simply accepted by gregorio.  Most of the old "standard" headers, like `book` and `manuscript-location`, are now handled in this way and thus are no longer limited to one instance.  See GregorioRef for details.
- `mode` will now be converted to Roman numerals in lower case so that it shows up as small capitals using the default `modeline` style.  See [UPGRADE.md](UPGRADE.md) for details (for the change request, see [#756](https://github.com/gregorio-project/gregorio/issues/756)).
- `\grecommentary` is now less restrictive about where it occurs and need not occur directly before the score anymore.
- When a bar is preceeded by a punctum mora, gregorio now ignores the punctum mora in the bar horizontal placement by default, and also adds a custom space (defaulting to 0). You can change this behavior with `\gresetbarshiftaftermora{}`, see GregorioRef for its arguments (for the change request, see [#795](https://github.com/gregorio-project/gregorio/issues/795)).

### Added
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
- Tunable spaces for bars with text underneath: `spacearoundsmallbartext`, `spacearoundminortext`, `spacearoundmaiortext`, `spacearoundfinalistext`, `spacebeforefinalfinalistext`.  These are sized slightly larger than their "non-text" counterparts.  See GregorioRef and [UPGRADE.md](UPGRADE.md) for details (for the change request, see [#766](https://github.com/gregorio-project/gregorio/issues/766)).
- The ability to typeset simple slurs.  See GregorioRef for details (for the change request, see [#776](https://github.com/gregorio-project/gregorio/issues/776)).
- More control over the alignment of the annotation.  You can now set either the first line or the last line of the annotation as the controling line with `\gresetannotationby{firstline}` and `\gresetannotationby{lastline}` respectively.  Additionaly, within the line you can specify the top, baseline, or bottom of the line as the alignment point with `\gresetannotationvalign{top}`, `\gresetannotation{baseline}`, and `\gresetannotationvalgin{bottom}` respectively.  Default behavior is baseline of the top line (as it was before).  These alignment controls are applied before `annotationraise` is taken into account.  See [#768](https://github.com/gregorio-project/gregorio/issues/768) for details.
- More control over the placement of the commentary.  You can now set the amount of space between the lines of a multi-line commentary with `commentaryseparation` and the distance from the top line of the staff to the baseline of the bottom line of the commentary with `commentaryraise`.  See [#662](https://github.com/gregorio-project/gregorio/issues/662) for original request.
- Styles for the annotation and the commentary.  `annotation` has no default styling.  `commentary` defaults to footnote sized italics.
- `\grecommentary` now takes an optional argument which will add extra space between the commentary and the score for just the next score.
- The custos can now be selectively enabled/disabled at forced line breaks by appending `+` (to enable) or `-` (to disable) after the `z` or `Z` (see [#800](https://github.com/gregorio-project/gregorio/issues/800)).
- With thanks to an anonymous contributor, the shape of the MetaPost-drawn curly brace has been improved (see [#797](https://github.com/gregorio-project/gregorio/issues/797)).


### Deprecated
- `initial-style` gabc header, supplanted by the `\gresetinitiallines` TeX command.
- `biginitial` style, consolidated into the `initial` style.
- `\grescorereference`

### Removed
- `\GreSetStaffLinesFormat`, supplanted by `\grechangeformat{normalstafflines}...`
- `\greinitialformat`, if you were redefining this command, use `\grechangeformat{initial}...` instead
- `\grebiginitialformat`, if you were redefining this command, use `\grechangeformat{biginitial}...` instead
- `\gretranslationformat`, if you were redefining this command, use `\grechangeformat{translation}...` instead
- `\greabovelinestextstyle`, if you were redefining this command, use `\grechangeformat{abovelinestext}...` instead
- `\grelowchoralsignstyle`, if you were redefining this command, use `\grechangeformat{lowchoralsign}...` instead
- `\grehighchoralsignstyle`, if you were redefining this command, use `\grechangeformat{highchoralsign}...` instead
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
- `centering-scheme` gabc header, supplanted by `\grelyriccentering` in TeX.  See GregorioRef for syntax.
- `gregoriotex-font` gabc header, supplanted by `\gresetgregoriofont` in TeX.  See GregorioRef for syntax.
- The meaningless `gabc-version` header in gabc (see [#664](https://github.com/gregorio-project/gregorio/issues/664)).


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
- Mac installer can now detect installations of TeXLive done with MacPorts or the command-line tool provided by TUG.
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
- Added `--with-kpathsea` option to configure script, to check input and output file against `openout_any` and `openin_any` settings of texmf.cnf (TeXLive only). Necessary to be included in `shell_escape_commands` in TeXLive.
- Support for `lualatex -recorder`.  Autocompiled gabc and gtex files will now be properly recorded so that programs like `latexmk -recorder` can detect the need to rebuild the PDF when a gabc file changes.
- A vertical episema may now be forced to appear above or below a note.  In gabc, use `'0` for the vertical episema to appear below and `'1` for the vertical episema to appear above (see [#385](https://github.com/gregorio-project/gregorio/issues/385)).
- The first word, first syllable, and first letter of the first syllable that is *not* interpreted as the initial of the score can now be styled from TeX.  Use `\grechangestyle` to modify the `firstsyllableinitial`, `firstsyllable`, and `firstword` as desired.
- The final line of a score may now be forced to be fully justified (rather than ragged) using `\gresetlastline{justified}` before including the score (see [#43](https://github.com/gregorio-project/gregorio/issues/43)).  Use `\gresetlastline{ragged}` to switch back to a ragged last line.
- `\gresethyphen{force}` forces GregorioTeX to put a hyphen between each syllable in a polysyllabic word.  `\gresethyphen{auto}` restores behavior to normal.
- Support for custom vowel centering rules.  Put a file called `gregorio-vowels.dat` into your project directory or into a directory accessible from TEXMF and add the header `language: name;` to your gabc file.  The `gregorio-vowels.dat` file describes how vowels are to be located in the *name* language.  See GregorioRef for details.
- `\gresetlinecolor` takes a named color as an argument.  As a result, the red staff lines can be made consistent with the text, even when the user changes `gregoriocolor` with `\gresetlinecolor{gregoriocolor}`.  Addresses [#21787 on the old tracker](https://gna.org/bugs/index.php?21787).
- Package option `deprecated=false`. Causes all deprecated commands to raise an error and halt TeX.
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
- `centering-scheme` gabc header, supplanted by `\grelyriccentering` in TeX.  See GregorioRef for syntax.
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
