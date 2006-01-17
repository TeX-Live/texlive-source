@echo off
Rem
Rem The SmallEnv gorp is for those who have too small environment
Rem size which will cause the variables we set to be truncated
Rem but who cannot be bothered to read the Out Of Environment Space
Rem error messages spitted by the stock DOS shell
set XSRC=.
if not "%XSRC%" == "." goto SmallEnv
if "%1" == "" goto inplace
set XSRC=%1
if not "%XSRC%" == "%1" goto SmallEnv
:inplace
if exist .\kpathsea\INSTALL ren .\kpathsea\INSTALL INSTALL.txt
if exist .\web2c\INSTALL ren .\web2c\INSTALL INSTALL.txt
if exist .\dvipsk\INSTALL ren .\dvipsk\INSTALL INSTALL.txt
if exist .\dviljk\INSTALL ren .\dviljk\INSTALL INSTALL.txt
if exist .\gsftopk\INSTALL ren .\gsftopk\INSTALL INSTALL.txt
if exist .\makeindexk\INSTALL ren .\makeindexk\INSTALL INSTALL.txt
if exist .\makeindexk\INSTALL ren .\makeindexk\INSTALL INSTALL.txt
if exist .\dvipdfm\INSTALL ren .\dvipdfm\INSTALL INSTALL.txt
test -d %XSRC%
if not errorlevel 1 goto chkdir
echo %XSRC% is not a directory
goto end
:chkdir
test -f %XSRC%/configure
if not errorlevel 1 goto argsok
echo I cannot find the configure script in directory %XSRC%
goto end
:argsok
rem set SYSROOT=c:
set PATH_SEPARATOR=:
if not "%PATH_SEPARATOR%" == ":" goto SmallEnv
set PATH_EXPAND=y
if not "%PATH_EXPAND%" == "y" goto SmallEnv
if not "%HOSTNAME%" == "" goto hostdone
if "%windir%" == "" goto msdos
set OS=MS-Windows
if not "%OS%" == "MS-Windows" goto SmallEnv
goto haveos
:msdos
set OS=MS-DOS
if not "%OS%" == "MS-DOS" goto SmallEnv
:haveos
if not "%USERNAME%" == "" goto haveuname
if not "%USER%" == "" goto haveuser
echo No USERNAME and no USER found in the environment, using default values
set HOSTNAME=Unknown PC
if not "%HOSTNAME%" == "Unknown PC" goto SmallEnv
:haveuser
set HOSTNAME=%USER%'s PC
if not "%HOSTNAME%" == "%USER%'s PC" goto SmallEnv
goto userdone
:haveuname
set HOSTNAME=%USERNAME%'s PC
if not "%HOSTNAME%" == "%USERNAME%'s PC" goto SmallEnv
:userdone
set HOSTNAME=%HOSTNAME%, %OS%
:hostdone
set OS=
echo Updating configure scripts for DJGPP...
test -f %XSRC%/configure.orig
if not errorlevel 1 goto orig1
cp -p %XSRC%/configure configure.orig
goto patchmain
:orig1
if not exist configure.orig cp -p %XSRC%/configure.orig configure.orig
:patchmain
patch -o configure configure.orig %XSRC%/djgpp/cfgmain.pat
if errorlevel 1 goto PatchError
test -d %XSRC%/kpathsea
if errorlevel 1 goto DviLJk
if not exist kpathsea\nul mkdir kpathsea
if exist kpathsea\configure.orig goto orig2
test -f %XSRC%/kpathsea/configure.orig
if not errorlevel 1 cp -p %XSRC%/kpathsea/configure.orig kpathsea/configure.orig
if not exist kpathsea\configure.orig cp -p %XSRC%/kpathsea/configure kpathsea/configure.orig
:orig2
patch -o kpathsea/configure kpathsea/configure.orig %XSRC%/djgpp/cfgkpath.pat
if errorlevel 1 goto PatchError
:DviLJk
test -d %XSRC%/dviljk
if errorlevel 1 goto DviPSk
if not exist dviljk\nul mkdir dviljk
if exist dviljk\configure.orig goto orig3
test -f %XSRC%/dviljk/configure.orig
if not errorlevel 1 cp -p %XSRC%/dviljk/configure.orig dviljk/configure.orig
if not exist dviljk\configure.orig cp -p %XSRC%/dviljk/configure dviljk/configure.orig
:orig3
patch -o dviljk/configure dviljk/configure.orig %XSRC%/djgpp/cfgdvilj.pat
if errorlevel 1 goto PatchError
:DviPSk
test -d %XSRC%/dvipsk
if errorlevel 1 goto DTL
if not exist dvipsk\nul mkdir dvipsk
if exist dvipsk\configure.orig goto orig4
test -f %XSRC%/dvipsk/configure.orig
if not errorlevel 1 cp -p %XSRC%/dvipsk/configure.orig dvipsk/configure.orig
if not exist dvipsk\configure.orig cp -p %XSRC%/dvipsk/configure dvipsk/configure.orig
:orig4
patch -o dvipsk/configure dvipsk/configure.orig %XSRC%/djgpp/cfgdvips.pat
if errorlevel 1 goto PatchError
:ODviPSk
test -d %XSRC%/odvipsk
if errorlevel 1 goto DTL
if not exist odvipsk\nul mkdir odvipsk
if exist odvipsk\configure.orig goto orig40
test -f %XSRC%/odvipsk/configure.orig
if not errorlevel 1 cp -p %XSRC%/odvipsk/configure.orig odvipsk/configure.orig
if not exist odvipsk\configure.orig cp -p %XSRC%/odvipsk/configure odvipsk/configure.orig
:orig40
patch -o odvipsk/configure odvipsk/configure.orig %XSRC%/djgpp/cfgdvips.pat
if errorlevel 1 goto PatchError
:DTL
test -d %XSRC%/dtl
if errorlevel 1 goto DviDvi
if not exist dtl\nul mkdir dtl
if exist dtl\configure.orig goto orig5
test -f %XSRC%/dtl/configure.orig
if not errorlevel 1 cp -p %XSRC%/dtl/configure.orig dtl/configure.orig
if not exist dtl\configure.orig cp -p %XSRC%/dtl/configure dtl/configure.orig
:orig5
patch -o dtl/configure dtl/configure.orig %XSRC%/djgpp/cfgdtl.pat
if errorlevel 1 goto PatchError
:DViDvi
test -d %XSRC%/dvidvi
if errorlevel 1 goto LaCheck
if not exist dvidvi\nul mkdir dvidvi
if exist dvidvi\configure.orig goto orig6
test -f %XSRC%/dvidvi/configure.orig
if not errorlevel 1 cp -p %XSRC%/dvidvi/configure.orig dvidvi/configure.orig
if not exist dvidvi\configure.orig cp -p %XSRC%/dvidvi/configure dvidvi/configure.orig
:orig6
patch -o dvidvi/configure dvidvi/configure.orig %XSRC%/djgpp/cfgdvidv.pat
if errorlevel 1 goto PatchError
:LaCheck
test -d %XSRC%/lacheck
if errorlevel 1 goto SeeTeXk
if not exist lacheck\nul mkdir lacheck
if exist lacheck\configure.orig goto orig7
test -f %XSRC%/lacheck/configure.orig
if not errorlevel 1 cp -p %XSRC%/lacheck/configure.orig lacheck/configure.orig
if not exist lacheck\configure.orig cp -p %XSRC%/lacheck/configure lacheck/configure.orig
:orig7
patch -o lacheck/configure lacheck/configure.orig %XSRC%/djgpp/cfglachk.pat
if errorlevel 1 goto PatchError
:SeeTeXk
test -d %XSRC%/seetexk
if errorlevel 1 goto MakeIndexk
if not exist seetexk\nul mkdir seetexk
if exist seetexk\configure.orig goto orig8
test -f %XSRC%/seetexk/configure.orig
if not errorlevel 1 cp -p %XSRC%/seetexk/configure.orig seetexk/configure.orig
if not exist seetexk\configure.orig cp -p %XSRC%/seetexk/configure seetexk/configure.orig
:orig8
patch -o seetexk/configure seetexk/configure.orig %XSRC%/djgpp/cfgseetx.pat
if errorlevel 1 goto PatchError
:MakeIndexk
test -d %XSRC%/makeindexk
if errorlevel 1 goto GsfToPk
if not exist makeindexk\nul mkdir makeindexk
if exist makeindexk\configure.orig goto orig9
test -f %XSRC%/makeindexk/configure.orig
if not errorlevel 1 cp -p %XSRC%/makeindexk/configure.orig makeindexk/configure.orig
if not exist makeindexk\configure.orig cp -p %XSRC%/makeindexk/configure makeindexk/configure.orig
:orig9
patch -o makeindexk/configure makeindexk/configure.orig %XSRC%/djgpp/cfgmkind.pat
if errorlevel 1 goto PatchError
:GsfToPk
test -d %XSRC%/gsftopk
if errorlevel 1 goto Ps2Pkm
if not exist gsftopk\nul mkdir gsftopk
if exist gsftopk\configure.orig goto orig10
test -f %XSRC%/gsftopk/configure.orig
if not errorlevel 1 cp -p %XSRC%/gsftopk/configure.orig gsftopk/configure.orig
if not exist gsftopk\configure.orig cp -p %XSRC%/gsftopk/configure gsftopk/configure.orig
:orig10
patch -o gsftopk/configure gsftopk/configure.orig %XSRC%/djgpp/cfggsfpk.pat
if errorlevel 1 goto PatchError
:Ps2Pkm
test -d %XSRC%/ps2pkm
if errorlevel 1 goto Tex4HTk
if not exist ps2pkm\nul mkdir ps2pkm
if exist ps2pkm\configure.orig goto orig11
test -f %XSRC%/ps2pkm/configure.orig
if not errorlevel 1 cp -p %XSRC%/ps2pkm/configure.orig ps2pkm/configure.orig
if not exist ps2pkm\configure.orig cp -p %XSRC%/ps2pkm/configure ps2pkm/configure.orig
:orig11
patch -o ps2pkm/configure ps2pkm/configure.orig %XSRC%/djgpp/cfgpspkm.pat
if errorlevel 1 goto PatchError
:Tex4HTk
test -d %XSRC%/tex4htk
if errorlevel 1 goto MusixFlx
if not exist tex4htk\nul mkdir tex4htk
if exist tex4htk\configure.orig goto orig12
test -f %XSRC%/tex4htk/configure.orig
if not errorlevel 1 cp -p %XSRC%/tex4htk/configure.orig tex4htk/configure.orig
if not exist tex4htk\configure.orig cp -p %XSRC%/tex4htk/configure tex4htk/configure.orig
:orig12
patch -o tex4htk/configure tex4htk/configure.orig %XSRC%/djgpp/cfgtexht.pat
if errorlevel 1 goto PatchError
:MusixFlx
test -d %XSRC%/musixflx
if errorlevel 1 goto DviPDFm
if not exist musixflx\nul mkdir musixflx
if exist musixflx\configure.orig goto orig13
test -f %XSRC%/musixflx/configure.orig
if not errorlevel 1 cp -p %XSRC%/musixflx/configure.orig musixflx/configure.orig
if not exist musixflx\configure.orig cp -p %XSRC%/musixflx/configure musixflx/configure.orig
:orig13
patch -o musixflx/configure musixflx/configure.orig %XSRC%/djgpp/cfgmusix.pat
if errorlevel 1 goto PatchError
:DviPDFm
test -d %XSRC%/dvipdfm
if errorlevel 1 goto Web2C
if not exist dvipdfm\nul mkdir dvipdfm
if exist dvipdfm\configure.orig goto orig14
test -f %XSRC%/dvipdfm/configure.orig
if not errorlevel 1 cp -p %XSRC%/dvipdfm/configure.orig dvipdfm/configure.orig
if not exist dvipdfm\configure.orig cp -p %XSRC%/dvipdfm/configure dvipdfm/configure.orig
:orig14
patch -o dvipdfm/configure dvipdfm/configure.orig %XSRC%/djgpp/cfgdvpdf.pat
if errorlevel 1 goto PatchError
:Web2C
test -d %XSRC%/web2c
if errorlevel 1 goto skipXDVI
if not exist web2c\nul mkdir web2c
if exist web2c\configure.orig goto orig100
test -f %XSRC%/web2c/configure.orig
if not errorlevel 1 cp -p %XSRC%/web2c/configure.orig web2c/configure.orig
if not exist web2c\configure.orig cp -p %XSRC%/web2c/configure web2c/configure.orig
:orig100
patch -o web2c/configure web2c/configure.orig %XSRC%/djgpp/cfgweb2c.pat
if errorlevel 1 goto PatchError
Rem
Rem XDvi is not supported on MS-DOS
GoTo skipXDVI
if not exist xdvik\nul mkdir xdvik
if not exist xdvik\configure.orig cp -p %XSRC%/xdvik/configure xdvik/configure.orig
patch -o xdvik/configure xdvik/configure.orig %XSRC%/djgpp/cfgxdvik.pat
if errorlevel 1 goto PatchError
:skipXDVI
set CONFIG_SHELL=bash.exe
set INSTALL=${DJDIR}/bin/ginstall -c
set YACC=bison -y
set LEX=flex
set RANLIB=ranlib
if not "%RANLIB%" == "ranlib" goto SmallEnv
Rem Use a response file to avoid exceeding the 126-character limit
echo --prefix=${DJDIR} --datadir=${DJDIR}/share --srcdir=%XSRC% > cfg.rf
echo --without-x --with-editor='emacs +%%d %%s' --with-epsfwin >> cfg.rf
echo Configuring...
sh ./configure i386-pc-msdos.djgppv2 @cfg.rf
echo Done.
goto CleanUp
:SmallEnv
echo Your environment size is too small.  Please enlarge it and run me again.
set HOSTNAME=
set OS=
:CleanUp
set XSRC=
set CONFIG_SHELL=
set INSTALL=
set YACC=
set LEX=
set RANLIB=
set HOSTNAME=
if exist cfg.rf del cfg.rf
goto end
:PatchError
echo Failed to patch one or more configure scripts.  Configure NOT done.
:end
