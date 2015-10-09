# Contributing guide for Gregorio

## Reporting an issue

You can report a bug and request a new feature on the [bug tracker](https://github.com/gregorio-project/gregorio/issues).

Please search for existing issues before reporting a new one. 

Please do not use the issue tracker for personal support requests, instead use the mailing-list (https://gna.org/mail/?group=gregorio) or IRC: #gregorio on freenode.

### Report a bug

When you report a bug, please provide the following information:

 * version of Gregorio
 * a minimal example (gabc or gabc+tex if relevant) showing the bug
 * a screenshot of the wrong result
 * a precise description (if possible with a picture) of the expected output

### Request feature

When requesting a feature, please provide the following informations:

 * a minimal example (gabc or gabc+tex) that currently doesn't work, but would
   if the feature was done
 * a precise description (if possible with a picture) of the expected output

## Contributing code

### Development environment

It is not currently possible to compile Gregorio under Windows. The build requirements
are the same under GNU/Linux and Mac OSX:

 * [git](http://git-scm.com/)
 * the [GNU Build System](http://www.gnu.org/software/automake/manual/html_node/GNU-Build-System.html#GNU-Build-System) (often referred to as "Autotools")
 * [gcc](https://gcc.gnu.org/) (and associated tools)
 * [GNU indent](https://www.gnu.org/software/indent/) (OSX default indent won't work)
 * [pylint](http://www.pylint.org/) if you work on Python files

### Coding standards

##### C files

Gregorio targets the following:

 * C standard: [ISO C89 Standard](http://en.wikipedia.org/wiki/ANSI_C#C89)
 * compilers: [gcc](https://gcc.gnu.org/) and [clang](http://clang.llvm.org/)
 * architectures: `i586`, `amd64`, `arm`, `arm64`
 * under Linux, only [`glibc`](http://www.gnu.org/software/libc/)
 * must compile under Mac OSX natively, and under [mingw32](http://www.mingw.org/) and [mingw_w64](https://sourceforge.net/projects/mingw-w64/)
 * for inclusion with TeX Live, must compile under VisualStudio 2010 for Windows.

Specific C extensions (GNU extensions, etc.) working for all the above targets are allowed on a case-by-case basis. Patches for other compilers, architectures or libraries can be accepted if they are clean, small and non-intrusive.

Use indent on your code before commiting it, with the `.indent.pro` file in the repository: `indent path/to/my/file.c`.

##### Other files

The rest of the code uses two spaces for indentation of `.tex`, `.lua` and other files, and four spaces for `.py` files.

Gregorio provides an [`.editorconfig` file](../.editorconfig), using an [editorconfig plugin](http://editorconfig.org/#download) for your editor will configure it automatically.

Python files must output no error when inspected by `pylint`.

TeX code must use LuaTeX (more than TeX + eTeX + Omega + PDFTeX) primitives as much as possible, and, when not possible, use code compiling under PlainTeX.

### Tests

Before submitting a change, please run the tests in [gregorio-test](gregorio-project/gregorio-test) against your resulting code.

When your changes are significant, please provide a test demonstrating the change. See [test documentation](https://github.com/gregorio-project/gregorio-test/blob/master/README.md).

If your change breaks some of the existing tests, then please either fix the existing tests or propose the new results be accepted as more "correct" than the old ones.

If you are submitting a new or modified test, please create a new branch in the test repository (preferably with a name which matches the name of the branch your changes are on in the main repository) where you can make these changes.  Then create a pull request in the test repository which explains your changes and make sure to reference the corresponding pull request in the main repository.  This way those reveiwing your changes can also see what you expect the new test results to be.

### Documentation

If your code has an impact on the user, you must add it to the [changelog file](CHANGELOG.md).

You must also document it in the right places:

 * [User manual](doc/) for changes in GregorioTeX
 * the [website](http://gregorio-project.github.io/) (in [another repository](https://github.com/gregorio-project/gregorio-project.github.io)) if relevant.

### Git Workflow

The Gregorio team is following the [classical Github workflow](https://guides.github.com/introduction/flow/). More precisely it follows what is sometimes described as "[Gitflow Worflow](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow)", keeping the same branch naming convention.

### Make a pull request

Once you are ready to contribute code:

 * fork the repository and checkout your fork
 * create a new branch for the pull request you want to make
 * commit your changes to this new branch
 * make a pull request from this new branch to the relevant branch (usually `develop`)
 * the Gregorio developers will inspect and comment your pull request, and finally merge it (or not)
