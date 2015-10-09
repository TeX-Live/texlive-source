# Upgrade Manual

This file contains instructions to upgrade to a new release of Gregorio.

## 4.0

### Font changes

The music fonts (greciliae, gregorio, and parmesan) have changed with this release.  If the neumes do not appear or look strange in the output, you may need to clear your LuaTeX font cache by using `luaotfload-tool --cache=erase`.

### GregorioTeX and fontspec

Changes to the LaTeX kernel have made it so that if you use the fontspec and gregoriotex pacakges, you must load the fontspec package first.

### Command Name Systemization

A naming scheme for GregorioTeX commands has been established and so most commands have had their names changed in order to bring them into line with the new scheme.  Some have also had their syntax changed.  Briefly, user commands all now have a `\gre` prefix (to prevent name colisions with other packages) and groups of commands which altered a single setting have been replaced by a single command which takes an argument specifying the value of the setting.  The notable exception to this are the two main commands: `\gregorioscore` (replaces `\includescore`) and `\gabcsnippet`.  See GregorioRef for the complete list of new command names and their syntax.

Old command names should still work for now, but will raise a deprecation warning which indicates the name of the correct command to use. Exceptions are noted below:

- `\grescaledim`: This function now takes two arguments.  The second should be `yes`, `true`, or `on` to acheive the old behavior.

Additionally a new package option has been added. The option `deprecated=false` is helpful if you wish to ensure that your TeX file is compliant with the new naming system. This option causes all deprecated commands to raise an error, halting TeX, thus allowing you to actively find all deprecated commands and update them in your TeX file.

### Barred letters

If you use barred letters and a font other than Linux Libertine in your document, you must adjust the horizontal placement of the bar on the letter.  To do so, use the `\gresimpledefbarglyph` macro.  For example, use `\gresimpledefbarredsymbol{A}{0.3em}` in your preamble, tweaking the second argument to have a good result (same for R and V).  If using LaTeX, you can alternately use the `\grelatexsimpledefbarredsymbol` macro, which allows control over bold and italic versions of the barred letter.  See the documentation of `\gresimpledefbarredsymbol` and `\grelatexsimpledefbarredsymbol` in the PDF documentation for more details.

If you were using `\Vbarsmall`, `\greletterbar`, and `\greletteraltbar`, you must use `\gresimpledefbarredsymbol` to redefine your barred letters (see PDF documentation for details).

### .gtex extension

The `gregorio` executable now uses the `.gtex` extension by default (instead of `.tex`, formerly) for GregorioTeX files that it produces.  If you use `\includescore{file.tex}`, then you should change this to `\includescore{file.gtex}` or use the newer autocompilation feature.

### Custom spacings

If you are using custom spacings, please update the values of `interwordspacetext`, `intersyllablespacenotes` and `interwordspacenotes` to match their new definitions (in the comments in `gsp-default.tex`).

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

The gabc `centering-scheme` header is now deprecated and will disappear soon.  Use the `\grelyriccentering` command from TeX instead.  If you were using `centering-scheme: latine;` in gabc, now use `\grelyriccentering{vowel}` in the TeX file that includes the gabc.  If you were using `centering-scheme: english;` in gabc, now use `\grelyriccentering{syllable}` in the TeX file that includes the gabc.

Using the gabc header will, in this release, do that for you, but it will produce a deprecation warning.  This header will no longer be available in the next release.

### Annotations

Support for annotations with an arbitrary number of lines has been added.  To facilite this, the old functions which added annotations to specific lines (either the first or the second) are consolidated into a single function `\greannotation` which builds the annotations line by line.  If you used the old functions for adding annotations, then you should switch out those functions for the new one.

The distance associated with the annotations has also been renamed (from `aboveinitialseparation` to `annotationseparation`) and supplemented by a new distance (`annotationraise`).  The first still controls the spacing between the lines of the annoation.  The second controls the position of the annotation relative to the score, and thus replaces the second argument in the old functions.  By default, annotations are positioned so that the baseline of the first line is aligned with the top line of the staff.  Positive values of `annotationraise` will push the annotation up while negative values will push it down.  If you were previously using the second argument to `\gresetfirstlineaboveinitial` to adjust the spacing, you will need to convert this to call:

    \grechangedim{annotationraise}{0.1cm}{1}

Since the baseline position of annotations is a change from the old behavior, you will need to adjust the value of the distance a bit to achieve the desired positioning.

As is normal, calls to the deprecated command names will raise a warning but still work.  However, there is one caveat: the old functions will always add the annotations to the bottom of the annotation list, regardless of the order in which they are called.  Previously, you could call `\gresetsecondannotation` before `\gresetfirstannotation` and still have the first annotation appear on top.   Which annotation appears on top is now determined by the order in which the functions are called.

#### The Annotation Header

The `annotation` header in gabc files now passes its value(s) to gregoriotex for placement above the inital letter of the score. Up to two `annotation` fields are supported, the first for the first line above the initial, and the second field for the second line.

    annotation: 1 Ant.;
    annotation: VII a;

Full TeX markup is also accepted:

    annotation: {\color{red}1 Ant.};
    annotation: {\color{red}VII a};

If you define annotation(s) in the main TeX file via `\greannotation`, then the `annotation` header field in the gabc file will *not* overwrite that definition.

#### Variable Height Line Spacing

Gregorio is now able to make individual lines of a score taller when the position of the note requires extra space, without affecting the rest of the lines.  This is the new default behavior.  If you prefer the uniform line heights of earlier versions, use:

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

Gregorio now uses MetaPost (by default) to render braces.  This allows fine-tuning to better maintain optical line weight when braces are stretched.  Since the shapes are slightly different, you may prefer the old behavior, which may be restored by using:

    \gresetbracerendering{font}

Within a TeX document, you can switch back to MetaPost brace rendering by using:

    \gresetbracerendering{metapost}

You can freely switch between the two behaviors within a TeX document.

#### Keyword argument for `\grecreatedim` and `\grechangedim`

In order to better match the new command naming conventions, the third argument of `\grecreatedim` and `\grechangedim` should now be `scaling` or `fixed` instead of `1` or `0`, respectively.

#### Liquescence on bistropha and tristropha

In order to be more consistent, making a bistropha or tristropha liquescent by adding `<` in gabc will now only affect the note thus marked.  This means that a tristropha like `(gsss<)` will only show the final stropha as liquescent, which is different from the older behavior of making every stropha liquescent.  If you prefer the old behavior, mark every note explicitly as liquescent with something like `(gs<gs<gs<)`.

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
