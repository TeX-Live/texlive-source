mendex  --- Index processor
===========================

The program `mendex` is a general purpose hierarchical index generator.
It is almost compatible with `makeindex`, and additional features
for Japanese (handling readings of kanji words etc.) are also available.

### Features

See manual [mendex.1](./mendex.1) in roff format.

### History

`mendex` is originally developed by ASCII Corporation and
ASCII Media Works, Inc.

The source codes are currently maintained in TeX Live and GitHub repositories.
Following features are added by Japanese TeX Development Community:

* option `-U`: Set input/output character encoding to UTF-8.
* option `-I enc`: Set internal character encoding to `enc`.
* option `--help`: Show summary of options.
* option `--`: Arguments after `--` are not taken as options.


About mendex-doc package
------------------------

The `mendex-doc` package provides documentation for `mendex`.
Source code of the program is not included in it.

### Repository

The bundle is maintained on GitHub:
https://github.com/texjporg/tex-jp-build  (sources), and
https://github.com/texjporg/mendex-doc  (documents).

If you have any issues, please let us know from the above page or
by e-mail <issue@texjp.org>.

### License

The package may be distributed and/or modified under the terms of
the 3-clause BSD license (see [LICENSE](./LICENSE)).

### Release Date

$RELEASEDATE

Japanese TeX Development Community
