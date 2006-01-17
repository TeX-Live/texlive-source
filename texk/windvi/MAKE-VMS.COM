$! MAKE-VMS.COM
$! A DCL `Make' procedure for compiling and linking XDVI.
$!
$! PLEASE NOTE, this file was inherided from plain Xdvi.  It has not
$! been tested or reviewed for use with Xdvik. - janl@math.uio.no
$!
$! If you do want only the plain vanilla Xdvi just type @make_vms
$!
$! At the moment only support for DPS is really working to display inline 
$! postscript. To compile with DPS invoke the com as @make_vms dps
$! You'll need a post-June 95 version of the xvmsutils installed at your
$! site (This package is available at 
$! http://axp616.gsi.de:8080/www/vms/xvmsutils.html)
$!
$! If you want to compile using the Xaw lib to get buttons and scrollbars on
$! Xdvi you will have to invoke the build as @make_vms "" tool 
$! (or @make_vms dps tool). In this case you need the Xaw3d lib installed, 
$! which you might get from http://axp616.gsi.de:8080/www/vms/xaw.html.
$! Please edit the option file created during installation of the package 
$! to remove all references to shareable image and copy it to the dire where
$! Xdvi resides with a name of xaw3dalpha.opt or xaw3dvax.opt.
$!
$! In case of problems with the install you might contact me at 
$! m.zinser@gsi.de (preferred) or eurmpz@eur.sas.com
$! 
$!
$ ON ERROR THEN GOTO EXIT
$ ON CONTROL_Y THEN GOTO EXIT
$ OLD_VERIFY = f$verify (0)
$!
$ MAY_USE_DECC = 1
$!
$ IF F$GETSYI("HW_MODEL") .GE. 1024
$ THEN
$   ARCH_NAME="Alpha"
$   ARCH_PREF="AXP_"
$   HAVE_DECC_VAX = 0
$   USE_DECC_VAX = 0
$   CC="CC/stand=VAX/ansi/ext=comm/obj=.''ARCH_PREF'obj"
$   WRITE SYS$OUTPUT "Compiling on Alpha AXP using DECC"
$ ELSE
$   ARCH_NAME="VAX"
$   ARCH_PREF="VAX_"
$   IF F$search("sys$system:DECC$compiler.exe") .NES. ""
$   THEN
$!    DECC for VAX available (and maybe VAXC, too!)
$     HAVE_DECC_VAX = 1
$     IF HAVE_DECC_VAX .AND. MAY_USE_DECC
$     THEN
$!      We use DECC: 
$       USE_DECC_VAX = 1
$       CC="CC/DECC/stand=VAX/ext=comm/obj=.''ARCH_PREF'obj"
$     ELSE
$!      We use VAXC: 
$       USE_DECC_VAX = 0
$       CC="CC/VAXC/obj=.''ARCH_PREF'obj"
$     ENDIF
$   ELSE
$!    only VAXC available
$     HAVE_DECC_VAX = 0
$     USE_DECC_VAX = 0
$     CC="CC/obj=.''ARCH_PREF'obj"
$   ENDIF
$   IF USE_DECC_VAX
$   THEN
$     WRITE SYS$OUTPUT "Compiling on VAX using DECC"
$   ELSE
$     WRITE SYS$OUTPUT "Compiling on VAX using VAXC"
$   ENDIF
$ ENDIF
$ LDFLAGS = ""
$ if f$trnlnm("X11").eqs. "" then DEFINE X11 DECW$INCLUDE
$ if f$trnlnm("SYS").eqs ""
$  then
$   IF USE_DECC_VAX .AND. (F$TRNLNM("DECC$LIBRARY_INCLUDE") .NES. "")
$    THEN
$      DEFINE SYS DECC$LIBRARY_INCLUDE
$    ELSE
$      DEFINE SYS SYS$LIBRARY
$    ENDIF
$ endif
$ if f$trnlnm("DPS").eqs. "" then define dps xdps$include
$!
$ ps_dps = ""
$ ps_gs = ""
$ tool = "NOTOOL"
$ IF f$edit(p1,"UPCASE") .eqs. "DPS" THEN ps_dps = "PSDPS"
$ IF f$edit(p1,"UPCASE") .eqs. "GS" THEN ps_gs = "PSGS"
$ if f$edit(p2,"UPCASE") .eqs. "TOOL" then tool = "TOOL"
$!
$ ps_options = ""
$ ps_head = ""
$ varobjs = ""
$ IF ps_dps .EQS. "PSDPS"
$  THEN
$    ps_head = "PSHEADER"
$    ps_options = ps_options + ", VFORK, PS_DPS"
$    varobjs = varobjs + "PSDPS.''ARCH_PREF'OBJ, "
$  ENDIF
$ IF ps_gs .EQS. "PSGS"
$  THEN
$    ps_head = "PSHEADER"
$    ps_options = ps_options + ", PS_GS"
$    varobjs = varobjs + "PSGS.''ARCH_PREF'OBJ, "
$  ENDIF
$!
$ OPEN/WRITE optfile VMS_XDVI.'ARCH_PREF'OPT
$ if (tool.eqs."TOOL") .or. (ps_dps.EQS."PSDPS")
$  then
$!
$!  Find out which X-Version we're running.  This will fail for older
$!  VMS versions (i.e., v5.5-1).  Therefore, choose DECWindows XUI for
$!  default.
$!
$  On Error Then GoTo XUI
$  @sys$update:decw$get_image_version sys$share:decw$xlibshr.exe decw$version
$  if f$extract(4,3,decw$version).eqs."1.0"
$  then
$    write optfile "Sys$share:DECW$DWTLIBSHR.EXE/Share"
$  endif
$  if f$extract(4,3,decw$version).eqs."1.1"
$  then
$    write optfile "sys$share:decw$xtshr.exe/share"
$  endif
$  if f$extract(4,3,decw$version).eqs."1.2"
$  then
$    write optfile "sys$share:decw$xtlibshrr5.exe/share"
$  endif
$  GoTo MAIN
$!
$ XUI:
$!
$    write optfile "Sys$share:DECW$DWTLIBSHR.EXE/Share"
$ MAIN:
$ endif
$ IF ps_dps .EQS. "PSDPS"
$ THEN
$   write optfile "x11vms:xvmsutils.olb/lib"
$!
$   WRITE optfile "sys$share:xdps$dpsclientshr/share"
$   WRITE optfile "sys$share:xdps$dpsbindingsshr/share"
$   WRITE optfile "sys$share:xdps$dpslibshr/share"
$ ENDIF
$ WRITE optfile "sys$share:decw$xlibshr/share"
$ if tool .eqs. "TOOL"
$  then
$   write optfile "XMULIBSHR/SHARE"
$   write optfile "XAW3DLIBSHR/SHARE"
$ endif
$ IF (ARCH_NAME .EQS. "VAX") .AND. (.NOT. USE_DECC_VAX)
$ THEN
$   WRITE optfile "sys$share:vaxcrtl/share"
$   option_file = "VMS_XDVI.''ARCH_PREF'OPT"	! for linking SQUEEZE
$ ELSE
$   option_file = "nl:"				! for linking SQUEEZE
$ ENDIF
$ CLOSE optfile
$ if (tool.eqs."TOOL") 
$   then
$     copy/nolog xaw3d'arch_name'.opt,VMS_XDVI.'ARCH_PREF'OPT;-1 -
                                                      VMS_XDVI.'ARCH_PREF'OPT
$     tool = "TOOL, BUTTONS"
$ endif
$!
$ DEFS = "VMS, 'tool', USE_PK, USE_GF, TEXXET, GREY, MAKEPK'ps_options'"
$ CC /DEFINE = ('DEFS') DVI_INIT.C
$ CC /DEFINE = ('DEFS') DVI_DRAW.C
$ CC /DEFINE = ('DEFS') GF.C
$ CC /DEFINE = ('DEFS') PK.C
$ CC /DEFINE = ('DEFS') VF.C
$ CC /DEFINE = ('DEFS') UTIL.C
$ If ps_dps .EQS. "PSDPS"
$ then
$!
$!  This definition of FIG_PATH and HEADER_PATH is site specific. You may 
$!  want to change it!!!
$!
$   CC /DEFINE = ('DEFS', "DEFAULT_FONT_PATH=""TEX_FONTS""", -
       "DEFAULT_VF_PATH=""TEX_VFDIR""", -
       "DEFAULT_FONT_SIZES=""300/329/360/432/518/622/746""",-
       "DEFAULT_FIG_PATH=""TEX_INPUTS:,SYS$DISK:""",-
       "DEFAULT_HEADER_PATH=""DVI_INPUTS:,TEX_INPUTS:,SYS$LOGIN:,SYS$DISK:[]""")-
        FONT_OPEN.C
$ else
$   CC /DEFINE = ('DEFS', "DEFAULT_FONT_PATH=""TEX_FONTS""", -
       "DEFAULT_VF_PATH=""TEX_VFDIR""", -
       "DEFAULT_FONT_SIZES=""300/329/360/432/518/622/746""") FONT_OPEN.C
$ endif
$ CC /DEFINE = ('DEFS') SPECIAL.C
$ CC /DEFINE = ('DEFS') EVENTS.C
$ IF ps_dps .NES. ""
$  THEN
$    CC /DEFINE = ('DEFS') PSDPS.C
$  ENDIF
$ IF ps_gs .NES. ""
$  THEN
$    CC /DEFINE = ('DEFS') PSGS.C
$  ENDIF
$ IF ps_head .NES. ""
$  THEN
$    CC /DEFINE = ('DEFS') SQUEEZE.C
$    LINK /EXE=.'ARCH_PREF'EXE SQUEEZE.'ARCH_PREF'OBJ, 'option_file' /OPTION
$    SQUEEZE := $SYS$DISK:[]SQUEEZE.'ARCH_PREF'EXE
$    SQUEEZE PSHEADER.TXT PSHEADER.C
$    CC /DEFINE = ('DEFS') PSHEADER.C
$    varobjs = varobjs + "PSHEADER.''ARCH_PREF'OBJ,"
$  ENDIF
$ CC /DEFINE = ('DEFS') XDVI.C
$!
$ LINK 'LDFLAGS'/EXE=.'ARCH_PREF'EXE XDVI.'ARCH_PREF'OBJ, -
     DVI_INIT.'ARCH_PREF'OBJ, DVI_DRAW.'ARCH_PREF'OBJ, -
     GF.'ARCH_PREF'OBJ, PK.'ARCH_PREF'OBJ, VF.'ARCH_PREF'OBJ, -
     UTIL.'ARCH_PREF'OBJ, FONT_OPEN.'ARCH_PREF'OBJ, SPECIAL.'ARCH_PREF'OBJ, -
     EVENTS.'ARCH_PREF'OBJ, 'varobjs' VMS_XDVI.'ARCH_PREF'OPT/OPTION
$ DELETE/NOLOG VMS_XDVI.'ARCH_PREF'OPT;*
$!
$EXIT:
$ dummy=f$verify ('old_verify')
$ EXIT
