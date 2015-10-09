# Gregorio Windows files

This folder is dedicated to the windows installer. This file describes the steps to compile it.

### Building the gregorio executable

Building the `gregorio.exe` file cannot currently be made under Windows, but it is possible under Linux:

 * install [mingw](http://www.mingw.org/) (`aptitude install mingw32` under Debian)
 * run `./build.sh --mingw` in the main directory of the repository

This will create `src/gregorio.exe`.

### Building tds file

To be able to install GregorioTeX, the installer will need a `.tds.zip` file containing the fonts and tex files:

 * run `./install-gtex.sh tds` in the main directory of the repository, this will create `gregoriotex.tds.zip`

### Building the installer

You need [InnoSetup](http://www.jrsoftware.org/isinfo.php) to be able to compile the installer. It runs fine under [Wine](https://www.winehq.org/). Once you have it installed: run InnoSetup.exe, open the `windows/gregorio.iss` file and compile it, it will produce `windows/Output/setup.exe`. You can also run it from command line: `wine32 'C:\\Program Files\Inno Setup 5\ISCC.exe' gregorio.iss` in the `windows` directory (replace the `ISCC.exe` path with yours if different).
