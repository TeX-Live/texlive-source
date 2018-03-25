# Third party scripts

This folder contains various small scripts or modules for third-party softwares to use Gregorio.

## Scribus

Gregorio can be used in [Scribus](http://wiki.scribus.net/canvas/Scribus) using three files:

 * `gregorio-scribus.lua`
 * `gregorio.png`
 * `900_gregorio.xml`

To install Gregorio support in Scribus:

 * copy the three files in the editorconfig folder of scribus (`/usr/share/scribus/editorconfig` under Linux, `C:\Program Files\Scribus\share\editorconfig`
under Windows).
 * read the comments in `900_gregorio.xml` and change some variables accordingly
 * in Scribus:
   * go in `File->Preferences->External Tools`
   * click `Add` in the `Render Frames`
   * select the file `900_gregorio.xml`

After that you can insert a render frame using Gregorio, and simply
typing gabc (`right-click->edit sources`).

For more details see [the website](http://gregorio-project.github.io/introduction-scribus.html).

## Syllabation checker

The scripts in this folder allow to check the syllabation of a gabc score against syllabation rules that you must provide.

#### Dependencies

The main script, `checkSyllabation.py` is in Python3, so you obviously need it. It uses the [pyphen](http://pyphen.org/) package. You will also need the hyphenation of the text language in the `libhyphen` format in order to run it.

If you want liturgical Latin hyphenation rules, you can pick those from the [hyphen-la](https://github.com/gregorio-project/hyphen-la) project. They are available in `libhyphen` format on the [webpage](http://gregorio-project.github.io/hyphen-la/).

#### Running the scripts

Once you have the hyphenation rules in the `libhyphen` format, you can check one score by running

```
checkSyllabation.py -p path/to/file.dic my_file.gabc
```

where `path/to/file.dic` is the path to the hyphenation rules file (default is `hyph_la_liturgical.dic` in the current directory). You can also analyze all gabc scores of a directory (with subdirectories) by replacing `my_file.gabc` by the name of the directory. By default, the script analyzes the current directory. The script outputs the report to `stdout` on Unix systems, and to `check-syllabation.log` on Windows systems.

You can see more options by running

```
checkSyllabation.py --help
```

#### Hyphenation problems

If you encounter problems of hyphenation, meaning you disagree with the proposed hyphenation, please contact the author of the hyphenation rules you are using ([here](https://github.com/gregorio-project/hyphen-la/issues) for `hyphen-la` for example).

If you encounter problems because the script got confused by a gabc construct you are using or has bugs not directly related to hyphenation, please report it on the [gregorio tracker](https://github.com/gregorio-project/gregorio/issues).

## Kde editors (Kate, Kedit)

You can use `gabc.xml` for syntax highlighting in kate (and Kedit, etc.). To
install it, copy the file in `/usr/share/apps/katepart/syntax`. This path
may vary according to your distribution.

## Gnome editors (Gedit)

You can use `gabc.lang` for syntax highlighting in GtkSourceView (Gedit, etc.). To
install it, copy the file in `/usr/share/gtksourceview-3.0/language-specs`. This path
may vary according to your distribution.

## Emacs

To use gregorio in [Emacs](https://www.gnu.org/software/emacs/), see [this script](https://github.com/cajetanus/gregorio-mode.el).

## Vim

To get gabc syntax highlighting in [Vim](http://www.vim.org/):

 * copy `gabc.vim` in `/usr/share/vim/vimcurrent/syntax`
 * add the line `au BufNewFile,BufRead *.gabc setf gabc`
somewhere near lines of the same type (near line 100 for example) in
`/usr/share/vim/vimcurrent/filetype.vim`. This will autodetect the filetypes.

## BBedit and TextWrangler (OSX only)

To get gabc syntax highlight in [BBedit](http://www.barebones.com/products/bbedit/) or [TextWrangler](http://www.barebones.com/products/textwrangler/), copy `gabc-syntax.plist` into `~/Library/Application Support/TextWrangler/Language Modules/` and restart BBEdit or TextWrangler.

## Script to process gabc score

`gprocess` is a perl script to instantly compile and show the result of a gabc
file. You can modify some variables directly into the script.
