$! EJG - 02-Jul-1996 - Made from Make.com
$! PWD - 03-Mar-1997 - Add t1part.c
$! Changes:
$! 
$! Added HPS define to compilation.
$! Skip all the questions at the beginning and go straight to compile.
$! Reorder modules (bbox and emspecial) to be in alphabetical order
$! during compile.
$! Add hps.c to compile.
$! Add hps to link.
$! Remove option file from links.
$! Add /standard=vaxc to all cc commands
$! Remove FONTLIB (if included, need to define FLINAME and FLIPATH, but
$! before I do this I need to find out what a font library is!)
$! 
$! PWD: add t1part.c to compile and link
$!      change TEX$POSTSCRIPT to TEX_POSTSCRIPT
$ !
$ !     dvips for VAX/VMS
$ !
$ !
$ !     Use to compile and link dvips with VAXC under VMS.  Before using
$ !     this command file you must set the following definitions according
$ !     to your own environment;
$ !
$ !     TFMPATH         - The directory where TFM files live ( needed for
$ !                       the compilation of dvips.c ).  Be sure and include
$ !                       the needed directory separator in the path ( i.e.
$ !                       TEX_FONTS: )
$ !     PKPATH          - The directory where PK files live ( needed for the
$ !                       compilation of dvips.c ).  You should also decided
$ !                       whether you need VMS_ROOTED ( needed for the
$ !                       compilation of loadfont.c ) defined so that
$ !                       PKPATH will be interpreted as a rooted directory
$ !                       under VMS.  Be sure and include the needed directory
$ !                       separator in the path ( i.e. TEX_FONTS: ).
$ !     HEADERPATH      - The directory where the PostScript prologue file
$ !                       live ( needed for the compilation of output.c &
$ !                       dvips.c).  Be sure and include a trailing comma in
$ !                       your definition of the header path as dvips uses this
$ !                       definition as a path for both PostScript prologue
$ !                       files and files which are included with \special
$ !                       options.  This definition should be a comma
$ !                       separated list of directories where dvips will
$ !                       look for a specified file.  As an example;
$ !
$ !                       "HEADERPATH=""TEX_POSTSCRIPT:,SYS$LOGIN:,"""
$ !
$ !                       to look first in TEX_POSTSCRIPT:, then in SYS$LOGIN:,
$ !                       and finally in the current default directory.
$ !     CONFIGPATH      - The directory where the configuration file lives
$ !                       ( needed for the compilation of resident.c ).  Be
$ !                       sure and include the needed directory separator
$ !                       in the path ( i.e. TEX_POSTSCRIPT: ).
$ !
$ !     FIGPATH         - Where the .ps and .eps files are
$ !
$ !     After dvips has been compiled and linked the user will be given the
$ !     option of compressing the PostScript prologue files which are used
$ !     by dvips and copying the resultant prologue files to the specifed
$ !     HEADERPATH.  The user will also be given the option of copying the
$ !     dvips image file to the area where TeX related images are stored.
$ !     Answering "yes" to either of these options should be done only if
$ !     the user has write priviledges to the directories which are specified.
$ !
$ !     Command file by: Robert Schneider
$ !                      Department of Petroleum Engineering
$ !                      The University of Texas at Austin
$ !
$ !                      October 1989
$ !
$ !     Updates by:      Earle Ake
$ !                      Science Applications International Corporation
$ !                      Dayton, Ohio
$ !                      ake@dayton.saic.com
$ !
$ !                      Updated for DVIPS 5.34
$ !                      August 1990
$ !
$ !                      Ted Nieland
$ !                      Control Data Corporation
$ !                      DECUS TeX Collection Editor
$ !                      ted@nieland.dayton.oh.us
$ !
$ !                      Updated for DVIPS 5.47, February 1991
$ !
$ !                      Updated for DVIPS 5.474, March 1992
$ !                      Earle F. Ake
$ !
$!                       Updated for DVIPS 5.493, Sept 1992
$!                       Added HAVE_GETCWD,ANSI in CC for
$!                       dvips.c and resident.c
$!                       Max Calvani
$!                       calvani@astrpd.astro.it
$!
$!                       Updated for DVIPS 5.495, Oct. 1992
$!                       calvani@astrpd.astro.it
$!
$!                       Updated for DVIPS 5.499, Jan. 1993
$!                       Added FONTLIB support
$!                       calvani@astrpd.astro.it
$!
$ on error then goto bad_exit
$ on severe_error then goto bad_exit
$ !
$ TFMPATH = "TEX_FONTS:"
$ PKPATH = "TEX_PKDIR:"
$ VFPATH = "TEX_VFDIR:"
$ HEADERPATH = "TEX_POSTSCRIPT:,TEX_INPUTS:,SYS$LOGIN:,SYS$DISK:[]," ! PWD
$ PLACEHEADER_DIR = "TEX_POSTSCRIPT:,TEX_ROOT:[TEX.DVIDRIVERS]" ! Changed by PWD
$ CONFIGPATH = "TEX_POSTSCRIPT:"   !Changed by PWD
$ FIGPATH = "TEX_INPUTS:,SYS$DISK:"
$ TPIC = ",TPIC"
$ EMTEX = ",EMTEX"
$! FONTLIB = ",FONTLIB"	!EJG 02-Jul-1996
$ FONTLIB = ""		!EJG 02-Jul-1996
$ VMS_ROOTED = ",VMS_ROOTED"
$ TEXEXEPATH = "TEX_DISK:[TEX.EXE]"
$ DEBUG = ",DEBUG"
$ HPS = ",HPS"		!EJG 02-Jul-1996
$ GOTO DOCOMPILE	!EJG 02-Jul-1996
$ !
$ write sys$output " "
$ inquire/nop ANSWER "Have you read the file VMS_INSTALL.TXT [no]?  "
$ if ANSWER .eqs. "" then ANSWER = 0
$ if ANSWER then goto read_instructions
$   write sys$output " "
$   write sys$output "Please read the VMS_INSTALL.TXT file,  edit this command"
$   write sys$output "file if necessary, and then execute this file again."
$   write sys$output " "
$   exit
$ read_instructions:
$ write sys$output " "
$ inquire/nop ANSWER "Compile sources [no]?  "
$ if ANSWER .eqs. "" then ANSWER = 0
$ if .not. ANSWER then goto linkstep
$ !
$ get_definitions:
$ write sys$output " "
$ !
$ inquire/nop ANSWER "TFM path [''TFMPATH']?  "
$ if ANSWER .nes. "" then TFMPATH = ANSWER
$ inquire/nop ANSWER "PK path [''PKPATH']?  "
$ if ANSWER .nes. "" then PKPATH = ANSWER
$ inquire/nop ANSWER "VF path [''VFPATH']?  "
$ if ANSWER .nes. "" then VFPATH = ANSWER
$ inquire/nop ANSWER "PostScript HEADER path [''HEADERPATH']?  "
$ if ANSWER .nes. "" then HEADERPATH = ANSWER
$ inquire/nop ANSWER "PostScript CONFIG path [''CONFIGPATH'])?  "
$ if ANSWER .nes. "" then CONFIGPATH = ANSWER
$ inquire/nop ANSWER "FIG path [''FIGPATH'])?  "
$ if ANSWER .nes. "" then FIGPATH = ANSWER
$ inquire/nop ANSWER "Do you want TPIC support [yes]?  "
$ if ANSWER .eqs. "" then ANSWER = 1
$ if .not. ANSWER then TPIC = ""
$ inquire/nop ANSWER "Do you want EMTEX support [yes]?  "
$ if ANSWER .eqs. "" then ANSWER = 1
$ if .not. ANSWER then EMTEX = ""
$ inquire/nop ANSWER "Do you want FONTLIB support [yes]?  "
$ if ANSWER .eqs. "" then ANSWER = 1
$ if .not. ANSWER then FONTLIB = ""
$ inquire/nop ANSWER "Do you want DEBUG support [yes]?  "
$ if ANSWER .eqs. "" then ANSWER = 1
$ if .not. ANSWER then DEBUG = ""
$ inquire/nop ANSWER "Is ''PKPATH' a rooted directory [yes]?   "
$ if ANSWER .eqs. "" then ANSWER = 1
$ if .not. ANSWER then VMS_ROOTED = ""
$ write sys$output " "
$ write sys$output "dvips will be compiled with the following definitions;"
$ write sys$output " "
$ write sys$output "TFMPATH = ",TFMPATH
$ write sys$output "PKPATH = ",PKPATH
$ write sys$output "VFPATH = ",VFPATH
$ write sys$output "HEADERPATH = ",HEADERPATH
$ write sys$output "CONFIGPATH = ",CONFIGPATH
$ write sys$output "FIGPATH = ",FIGPATH
$ if TPIC .eqs. "" then -
  write sys$output "TPIC support is not enabled."
$ if TPIC .nes. "" then -
  write sys$output "TPIC support is enabled."
$ if FONTLIB .eqs. "" then -
  write sys$output "FONTLIB support is not enabled."
$ if FONTLIB .nes. "" then -
  write sys$output "FONTLIB support is enabled."
$ if EMTEX .eqs. "" then -
  write sys$output "EMTEX support is not enabled."
$ if EMTEX .nes. "" then -
  write sys$output "EMTEX support is enabled."
$ if DEBUG .eqs. "" then -
  write sys$output "DEBUG support is not enabled."
$ if DEBUG .nes. "" then -
  write sys$output "DEBUG support is enabled."
$ if VMS_ROOTED .eqs. "" then -
  write sys$output PKPATH," is not a rooted directory."
$ if VMS_ROOTED .nes. "" then -
  write sys$output PKPATH," is a rooted directory."
$ write sys$output " "
$ inquire/nop ANSWER "Is this correct [yes]?  "
$ if ANSWER .eqs. "" then ANSWER = 1
$ if .not. ANSWER then goto get_definitions
$ !
$DOCOMPILE:		!EJG 02-Jul-1996
$ if VMS_ROOTED .nes. "" then PKPATH = "''PKPATH'[%d]%f.PK"
$ write sys$output " "
$ write sys$output "Compiling sources ..."
$ write sys$output " "
$ write sys$output "afm2tfm.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') AFM2TFM.C
$ write sys$output "bbox.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') BBOX.C
$ write sys$output "color.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') COLOR.C
$ write sys$output "dopage.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS''EMTEX') DOPAGE.C
$ write sys$output "dosection.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') DOSECTION.C
$ write sys$output "dospecial.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS''TPIC') DOSPECIAL.C
$ write sys$output "download.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') DOWNLOAD.C
$ write sys$output "dpicheck.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') DPICHECK.C
$ write sys$output "drawps.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS''TPIC') DRAWPS.C
$ write sys$output "dviinput.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') DVIINPUT.C
$ write sys$output "dvips.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS',"TFMPATH=""''TFMPATH'""","PKPATH=""''PKPATH'""", -
      "VFPATH=""''VFPATH'""","FIGPATH=""''FIGPATH'""",HAVE_GETCWD,ANSI, -
      "CONFIGPATH=""''CONFIGPATH'""","HEADERPATH=""''HEADERPATH'""") DVIPS.C
$ write sys$output "emspecial.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS''EMTEX') EMSPECIAL.C
$ write sys$output "finclude.c ..."
$ cc/standard=vaxc /DEF=(VMS'DEBUG''HPS') FINCLUDE.C
$ write sys$output "flib.c ..."
$ cc/standard=vaxc /DEF=(VMS'DEBUG''HPS''FONTLIB') FLIB.C
$ write sys$output "fontdef.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') FONTDEF.C
$ write sys$output "header.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') HEADER.C
$ write sys$output "hps.c ..."		!EJG 02-Jul-1996
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') HPS.C		!EJG 02-Jul-1996
$ write sys$output "loadfont.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') LOADFONT.C
$ write sys$output "makefont.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') MAKEFONT.C
$ write sys$output "output.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS',"HEADERPATH=""''HEADERPATH'""") OUTPUT.C
$ write sys$output "papersiz.c  ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') PAPERSIZ.C
$ write sys$output "pprescan.c  ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') PPRESCAN.C
$ write sys$output "prescan.c  ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') PRESCAN.C
$ write sys$output "repack.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') REPACK.C
$ write sys$output "resident.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS',"CONFIGPATH=""''CONFIGPATH'""",HAVE_GETCWD,ANSI) -
      RESIDENT.C
$ write sys$output "scalewidth.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') SCALEWIDTH.C
$ write sys$output "scanpage.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') SCANPAGE.C
$ write sys$output "search.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') SEARCH.C
$ write sys$output "skippage.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') SKIPPAGE.C
$ write sys$output "squeeze.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') SQUEEZE.C
$ write sys$output "t1part.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') T1PART.C
$ write sys$output "tfmload.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') TFMLOAD.C
$ write sys$output "unpack.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') UNPACK.C
$ write sys$output "virtualfont.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') VIRTUALFONT.C
$ !
$ !     vaxvms fixes some irritating problems with VAXC ( particulary
$ !     fseek and ftell ).  Thanks to Nelson Beebee at Utah.
$ !
$ write sys$output "vaxvms.c ..."
$ cc/standard=vaxc /define=(VMS'DEBUG''HPS') [.VMS]VAXVMS.C
$ !
$ linkstep:
$ write sys$output " "
$ write sys$output "Linking dvips ..."
$ link /exe=dvips dvips,dopage,dosection,dospecial,download,dpicheck,drawps, -
         t1part, -
         dviinput,header,hps,finclude,flib,fontdef,loadfont,tfmload,prescan, -
                  scanpage,skippage,output,scalewidth,resident,search,unpack, -
                  makefont,repack,virtualfont,vaxvms,color,papersiz,pprescan, -
                  bbox,emspecial	!EJG 02-Jul-1996
$ write sys$output " "
$ write sys$output "Linking squeeze ..."
$ link /exe=squeeze squeeze,vaxvms	!EJG 02-Jul-1996
$ write sys$output " "
$ write sys$output "Linking afm2tfm ..."
$ write sys$output " "
$ link /exe=afm2tfm afm2tfm,vaxvms	!EJG 02-Jul-1996
$ exit					!EJG 02-Jul-1996
$ !
$ inquire/nop ANSWER -
  "Do you wish to compress the PostScript prologue files [no]?   "
$ if ANSWER .eqs. "" then ANSWER = 0
$ if .not. ANSWER then goto copy_prologue
$ squeeze :== $'f$environment("DEFAULT")'squeeze.exe
$ write sys$output " "
$ set verify
$ squeeze COLOR.LPRO COLOR.PRO
$ squeeze FINCLUDE.LPRO FINCLUDE.PRO
$ squeeze TEX.LPRO TEX.PRO
$ squeeze TEXC.LPRO TEXC.PRO
$ squeeze TEXPS.LPRO TEXPS.PRO
$ squeeze SPECIAL.LPRO SPECIAL.PRO
$ squeeze CROP.LPRO CROP.PRO
$ squeeze HPS.LPRO HPS.PRO
$ verify = 'f$verify(0)
$ write sys$output " "
$ copy_prologue:
$ FIRSTPATH =  PLACEHEADER_DIR
$ inquire/nop ANSWER -
  "Do you wish to copy the prologue files to ''FIRSTPATH' [yes]?   "
$ if ANSWER .eqs. "" then ANSWER = 1
$ if .not. ANSWER then goto copy_config
$ write sys$output " "
$ set verify
$ copy COLOR.PRO 'FIRSTPATH'COLOR.PRO
$ copy FINCLUDE.PRO 'FIRSTPATH'FINCLUDE.PRO
$ copy TEX.PRO 'FIRSTPATH'TEX.PRO
$ copy TEXC.PRO 'FIRSTPATH'TEXC.PRO
$ copy TEXPS.PRO 'FIRSTPATH'TEXPS.PRO
$ copy SPECIAL.PRO 'FIRSTPATH'SPECIAL.PRO
$ copy HPS.PRO 'FIRSTPATH'HPS.PRO
$ copy CROP.PRO 'FIRSTPATH'CROP.PRO
$ set protection=(s:rwed,o:rwed,g:re,w:re) 'FIRSTPATH'COLOR.PRO
$ set protection=(s:rwed,o:rwed,g:re,w:re) 'FIRSTPATH'FINCLUDE.PRO
$ set protection=(s:rwed,o:rwed,g:re,w:re) 'FIRSTPATH'TEX.PRO
$ set protection=(s:rwed,o:rwed,g:re,w:re) 'FIRSTPATH'TEXC.PRO
$ set protection=(s:rwed,o:rwed,g:re,w:re) 'FIRSTPATH'SPECIAL.PRO
$ set protection=(s:rwed,o:rwed,g:re,w:re) 'FIRSTPATH'TEXPS.PRO
$ set protection=(s:rwed,o:rwed,g:re,w:re) 'FIRSTPATH'HPS.PRO
$ set protection=(s:rwed,o:rwed,g:re,w:re) 'FIRSTPATH'CROP.PRO
$ verify = 'f$verify(0)
$ write sys$output " "
$ copy_config:
$ FIRSTPATH =  PLACEHEADER_DIR
$ inquire/nop ANSWER -
  "Do you wish to copy the CONFIG.PS file to ''FIRSTPATH' [yes]?   "
$ if ANSWER .eqs. "" then ANSWER = 1
$ if .not. ANSWER then goto copy_image
$ write sys$output " "
$ set verify
$ copy CONFIG.PS 'FIRSTPATH'CONFIG.PS
$ set protection=(s:rwed,o:rwed,g:re,w:re) 'FIRSTPATH'CONFIG.PS
$ verify = 'f$verify(0)
$ write sys$output " "
$ copy_image:
$ inquire/nop ANSWER "Do you wish to copy DVIPS.EXE to the TeX area [yes]?   "
$ if ANSWER .eqs. "" then ANSWER = 1
$ if .not. ANSWER then goto done
$ inquire/nop ANSWER "TeX image area [''TEXEXEPATH']?  "
$ if ANSWER .nes. "" THEN TEXEXEPATH = ANSWER
$ write sys$output " "
$ set verify
$ copy dvips.exe 'TEXEXEPATH'
$ set protect=(s:rwed,o:rwed,g:re,w:re) 'TEXEXEPATH'dvips.exe
$ verify = 'f$verify(0)
$ write sys$output " "
$ done:
$ write sys$output " "
$ write sys$output "Be sure to add dvips to your CLI by using the SET "
$ write sys$output "COMMAND syntax and do the other steps which are"
$ write sys$output "necessary to finish up the installation of dvips."
$ write sys$output " "
$ goto good_exit
$ bad_exit:
$ write sys$output " "
$ write sys$output "Something's wrong here.  You might want to take a look"
$ write sys$output "at the offending code or command and start over."
$ write sys$output " "
$ exit
$ good_exit:
$ write sys$output "Done."
$ write sys$output " "
$ exit
$! --------------------- EOF -------------------------------------
