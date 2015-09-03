# Gregorio

Gregorio is a software application for engraving Gregorian Chant scores on a computer. Gregorio's main job is to convert a [gabc file](http://gregorio-project.github.io/gabc/index.html) (simple text representation of a score) into a [GregorioTeX file](http://gregorio-project.github.io/gregoriotex/index.html), which makes [TeX](http://gregorio-project.github.io/gregoriotex/tex.html) able to create a PDF of your score.

## Usage

Create a `.gabc` file representing your score (see our [tutorial](http://gregorio-project.github.io/tutorial/tutorial-gabc-01.html)), or fetch one from [the database](http://gregobase.selapa.net/).

Create a [LaTeX](http://en.wikipedia.org/wiki/LaTeX) file that will produce the final pdf, and include your score. See [the example](examples/main-lualatex.tex).

Compile the LaTeX file with LuaLaTeX, you get your score in PDF! You can change the LaTeX file to include other scores, create booklets, books, etc.

You can also try it online [here](http://dev.illuminarepublications.com/gregorio/).

## Installation

You need a recent and working [TeXLive](https://www.tug.org/texlive/), on top of which you just need to install latest [Gregorio release](https://github.com/gregorio-project/gregorio/releases). See [the website](http://gregorio-project.github.io/installation.html) for more details.

## Documentation

You can find documentation and useful links in the [documentation](doc/), on [the main website](http://gregorio-project.github.io/) and on [a wiki](http://gregoriochant.org).

## History

See [CHANGELOG.md](CHANGELOG.md).

## Credits

See [CONTRIBUTORS.md](CONTRIBUTORS.md).

## Contributing and bug reporting

See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

The code is mainly under the [GPLv3](https://www.gnu.org/licenses/quick-guide-gplv3.en.html) license, with fonts under the [Open Font License](http://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=OFL). See [complete license](COPYING.md) for more details.
