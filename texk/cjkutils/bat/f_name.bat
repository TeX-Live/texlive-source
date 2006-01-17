@echo off
:4dosCheck
    if "%@eval[2+2]"=="4" loadbtm on
:Start
    if [%1]==[] goto Usage
    if [%1]==[/?] goto Help
    if [%2]==[] goto InputCheck
REM
REM This file is called F_NAME.BAT per default. Call it whatever you like.
REM
REM You need enough free environment space for this batch file.
REM If there's not enough free space, f_name.bat will give you a warning.
REM
REM While f_name.bat tries to test your free environment space, you'll
REM probably need to check your config.sys for the following line:
REM
REM     SHELL=C:\DOS\COMMAND.COM C:\DOS\ /E:1024 /p
REM
REM The '/E:1024' sets your environment space - it will probably need to
REM be 1024 or higher (unless you're really economical with env-space).
REM
REM If you are not sure about free space, set fp and fn with long dummy
REM strings in autoexec.bat to get enough free space, eg.:
REM
REM set fp=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
REM set fu=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
REM set fn=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
REM set fd=xx
REM set fe=xxxx
REM
REM Using dummy strings is necessary if you're going to execute
REM f_name.bat from a second command shell, eg. from Norton Commander.
REM In this case the total length reserved for f_name.bat should be up
REM to 192 bytes. My suggestion is, let the strings for fp, fu, and fn count
REM up to 186 bytes (eg. 62 each).
REM
REM Some releases of DOS (eg. some beta-releases/pre-releases of DOS 6.0
REM and some older DOS releases) does not support the batchcommand 'NOT'.
REM If you're still using such a release you cannot use this batchfile
REM since it uses this batchcommand.
REM
REM Please also note that f_name.bat cannot work properly if filespec is
REM longer than around 38 (? I don't know the exact length) characters. If
REM filespec is long, f_name.bat will fail at 'for %%x in ... do if ...'
REM lines.  The result of error is fairly unpredictable as DOS will try to
REM execute with a truncated batch command line.  So, please use the
REM batch file for short filespecs (< 38??). (On my DOS 6.22 using
REM strings above 38 chars gives me an endless loop, Claus Futtrup).
REM
REM For large batchfiles it is usually an advantage to use a
REM 'batch-compiler', which will convert the .bat file into a .com or
REM .exe file. With f_name.bat this could probably speed it up many
REM times, but it has not been possible to find a batch-compiler, which
REM will compile this batchfile. The compilers PWR14G.ZIP and
REM BAT2EX15.ZIP have been tested without success.
REM
REM This modified release is more solid when it comes to garbage input, it
REM is provided with help option, additional documentation and additional
REM checking for sufficient environment space and now remove the temporary
REM environment settings (P, Q, R, S, T) at the bottom of the batchfile. The
REM structure is changed a bit, but the basic still is made by Wonkoo Kim.
REM
REM Please do not distribute this file without the above documentation
REM included in the batch-file. If you change the file please add your
REM name as a "modifier" as done below + document your changes. We do
REM not intend to hold a copyright, though. You may use, misuse or abuse
REM this batchfile as you see fit.
REM
REM This file is provided to you on a free basis. The author/modifiers
REM do not want to be hold responsible to whatever this batchfile is
REM doing. Use of this batchfile is entirely at your own risk.
REM
REM Wonkoo Kim (wkim+@pitt.edu), August 11, 1995
REM Modified by Claus Futtrup (odie@csek.iprod.auc.dk), August 23. 1995
REM Modified by Werner Lemberg (wl@gnu.org), Jan. 10. 1996
REM   to run this batch file under 4DOS too (replacing all %%f with %%x
REM   and doubling the speed with the LOADBTM ON command)
REM Modified by Werner Lemberg (wl@gnu.org), Aug. 31. 1996
REM   to return unix-like paths additionally in the variable %fu%;
REM   the batch file will now parse e.g. \a.b\ correctly as a directory.
REM
:Usage
    echo.
    echo Usage: %0 filespec
    echo.
    echo Returns: fd (drive), fp (path), fu (unix path), fn (name), fe (extension)
    echo.
    echo Type %0 /? for additional help
    echo %0 is made by Wonkoo Kim, modified by C. Futtrup and W. Lemberg.
    echo.
    goto END
:Help
    cls
    echo %0 -- extracts the drive, path, name, and ext from file-path-name.
    echo.
    echo Return envs: FD (drive), FP (path), FU (unix path), FN (name), FE (ext)
    echo Temp envs:   P, Q, R, S, T
    echo.
    echo     INPUT FILE         fd        fp        fn        fe
    echo     ------------------ --------- --------- --------- ---------
    echo     abc                                    abc      
    echo     abc.tex                                abc       .tex
    echo     c:\abc.tex         c:        \         abc       .tex
    echo     c:tmp\abc.tex      c:        tmp\      abc       .tex
    echo     c:\tmp\abc.tex     c:        \tmp\     abc       .tex
    echo     c:tmp\tex\abc.tex  c:        tmp\tex\  abc       .tex
    echo     c:\tmp\tex\abc.tex c:        \tmp\tex\ abc       .tex
    echo.
    echo Please note this file isn't blazing fast + it needs a lot
    echo of environment space. See comments in %0 for more info.
    echo.
    echo Wonkoo Kim (wkim+@pitt.edu), August 11, 1995
    echo Modified by Claus Futtrup (odie@csek.iprod.auc.dk), August 23. 1995
    echo Modified by Werner Lemberg (wl@gnu.org), Aug 31. 1996
    goto END
:InputCheck
    for %%x in (%1) do if "%%x"=="%1" goto Okay
    echo *** ERROR: Please do not use wildcards in input file name.
    goto END
:Okay
    set p=
    set r=
    set fp=
    set fu=
    set fe=
    set fn=%1
    set s=XXXXXXXX%fn%
    set t=%fn%
    set s=
    if [%t%]==[%1] goto GetDrive
:EnvErr
    echo *** ERROR: Not enough environment space!
    echo *** See documentation at top of file '%0'
    goto END
:GetDrive
    for %%x in (/%t%) do set fn=%%x
    for %%x in (/%t%) do if not [%%x]==[%fn%] set fd=%%x
    if [%fn%]==[:] goto DriveOnly
    set t=%fn%
    for %%x in (/%t%) do set fn=%%x
    if [:%fn%]==[%t%] goto DriveFound
    set fd=
    set fn=%1
    goto GetPath
:DriveOnly
    set fn=
:DriveFound
    set fd=%fd%:
:GetPath
    set s=%fn%
:PathLoop
    set t=%s%
    for %%x in (/%t%) do set s=%%x
    set q=
    for %%x in (/%t%) do if not [%%x]==[%s%] set q=%%x
    if "\%s%"=="%t%" goto Separator
    set p=%p%%q%
    set r=%r%%q%
    if [%s%]==[%t%] goto PathCheck
    goto PathLoop
:Separator
    set fn=%s%
    set p=%p%\
    set r=%r%/
    set fp=%p%
    set fu=%r%
    goto PathLoop
:PathCheck
    if [%s%]==[\] goto NoFilename
    if not [%fn%]==[\] goto GetExt
    set fp=\
    set fu=/
    set fn=
    goto Done
:NoFilename
    set fp=%p%\
    set fu=%r%/
    set fn=
    goto Done
:GetExt
    set s=%fn%
    set fe=%fn%
    set fn=
:ExtLoop
    set t=%fe%
    for %%x in (/%t%) do set fe=%%x
    if ".%fe%"=="%t%" goto ExtFound
    if [%fe%]==[%t%] goto NoExt
    for %%x in (/%t%) do if not [%%x]==[%fe%] set fn=%fn%%%x
    goto ExtLoop
:ExtFound
    set fe=.%fe%
    goto Done
:NoExt
    if not [%fe%]==[.]  set fn=%s%
    set fe=
:Done
    echo DRIVE=%fd%  PATH=%fp%  UNIX_PATH=%fu%  NAME=%fn%  EXT=%fe%
:END
    set p=
    set q=
    set r=
    set s=
    set t=

