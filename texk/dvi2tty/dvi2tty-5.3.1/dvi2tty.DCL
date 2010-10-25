$ !
$ !	Compile dvi2tty for VAX/VMS under VAXC
$ !
$ !
$ !	April 25, 1990
$ !
$ !	Robert Schneider
$ !	Department of Petroleum Engineering
$ !	The University of Texas at Austin
$ !	< robert@perv.pe.utexas.edu >
$ !
$ SAY = "WRITE SYS$OUTPUT"
$ ASK = "INQUIRE /NOPUNCTUATION ANSWER"
$ !
$ !	You can stick options for the C compiler in CC_DEFINES.  As
$ !	an example you can be redundant and define CC_DEFINES to be
$ !	"/DEFINE=VMS".
$ !
$ CC_DEFINES = ""
$ !
$ SAY " "
$ ASK "Compile sources?  "
$ inquire_test = ANSWER .eqs. "Y"
$ if .not. inquire_test then goto linkstep
$ !
$ SAY " "
$ SAY "Compiling sources ..."
$ SAY " "
$ !
$ SAY "DVI2TTY.C ..."
$ CC 'CC_DEFINES DVI2TTY.C
$ SAY "DVISTUFF.C ..."
$ CC 'CC_DEFINES DVISTUFF.C
$ SAY "DISDVI.C ..."
$ CC 'CC_DEFINES DISDVI.C
$ !
$ linkstep:
$ SAY " "
$ SAY "Linking dvi2tty ..."
$ link /exe=dvi2tty.exe dvi2tty,dvistuff,sys$library:vaxcrtl/lib
$ SAY "Linking disdvi ..."
$ link /exe=disdvi.exe disdvi,sys$library:vaxcrtl/lib
$ SAY " "
$ SAY "Be sure and copy DVI2TTY.EXE to wherever you store your TeX"
$ SAY "images and define the symbol;"
$ SAY " "
$ SAY "$ dvi2tty :== $path:dvi2tty.exe"
$ SAY " "
$ SAY "in your TeX setup command file so it may be accessed.  'path'"
$ SAY "is the directory location where you store your TeX images."
$ SAY " "
$ SAY "Done."
$ SAY " "
$ exit

