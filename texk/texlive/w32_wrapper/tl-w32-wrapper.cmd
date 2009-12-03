@echo off
rem Universal script launcher
rem
rem Originally written 2009 by Tomasz M. Trzeciak
rem Public Domain

rem Make environment changes local
setlocal enableextensions
rem Get program/script name
if not defined TL_PROGNAME set TL_PROGNAME=%~n0
rem Check if this is 'sys' version of program
set TEX_SYS_PROG=
if /i "%TL_PROGNAME:~-4%"=="-sys" (
  set TL_PROGNAME=%TL_PROGNAME:~0,-4%
  set TEX_SYS_PROG=true
)

rem Default command to execute
set CMDLINE=call :noscript "%~0" "%TL_PROGNAME%"
rem Make sure our dir is on the search path; avoid trailing backslash
set TL_ROOT=%~dp0?
set TL_ROOT=%TL_ROOT:\bin\win32\?=%
path %TL_ROOT%\bin\win32;%path%
rem Check for kpsewhich availability
if not exist "%TL_ROOT%\bin\win32\kpsewhich.exe" goto :nokpsewhich
rem Ask kpsewhich about root and texmfsys trees (the first line of output)
rem and location of the script (the second line of output)
rem (4NT shell acts wierd with 'if' statements in a 'for' loop,
rem so better process this output further in a subroutine)
for /f "tokens=1-2 delims=;" %%I in (
  'call "%~dp0kpsewhich.exe" --expand-var "$TEXMFSYSCONFIG/?;$TEXMFSYSVAR/?" --format texmfscripts ^
  "%TL_PROGNAME%.pl" "%TL_PROGNAME%.tlu" "%TL_PROGNAME%.rb" "%TL_PROGNAME%.py"'
) do (
  call :setcmdenv "%%~I" "%%~J"
)

rem By now we should have the command to execute (whatever it is)
rem Unset program name variable and execute the command
set TL_PROGNAME=
%CMDLINE% %*
rem Finish with goto :eof (it will preserve the last errorlevel)
goto :eof

REM SUBROUTINES

:setcmdenv selfautoparent texmfsysconfig texmfsysvar
rem If there is only one argument it must be a script name
if "%~2"=="" goto :setcmd
rem Otherwise, it is the first line from kpsewhich, so to set up the environment
set PERL5LIB=%TL_ROOT%\tlpkg\tlperl\lib
set GS_LIB=%TL_ROOT%\tlpkg\tlgs\lib;%TL_ROOT%\tlpkg\tlgs\fonts
path %TL_ROOT%\tlpkg\tlgs\bin;%TL_ROOT%\tlpkg\tlperl\bin;%TL_ROOT%\tlpkg\installer;%TL_ROOT%\tlpkg\installer\wget;%path%
if not defined TEX_SYS_PROG goto :eof
rem Extra stuff for sys version
set TEXMFCONFIG=%~1
set TEXMFCONFIG=%TEXMFCONFIG:/?=%
set TEXMFVAR=%~2
set TEXMFVAR=%TEXMFVAR:/?=%
rem For sys version we might have an executable in the bin dir, so check for it
if exist "%TL_ROOT%\bin\win32\%TL_PROGNAME%.exe" set CMDLINE="%TL_ROOT%\bin\win32\%TL_PROGNAME%.exe"
goto :eof

:setcmd script
rem Set command based on the script extension
if /i %~x1==.pl  set CMDLINE="%TL_ROOT%\tlpkg\tlperl\bin\perl.exe" "%~f1"
if /i %~x1==.tlu set CMDLINE="%TL_ROOT%\bin\win32\texlua.exe" "%~f1"
rem For Ruby and Python we additionally check if their interpreter is available
if /i %~x1==.rb  call :chkcmd Ruby.exe "%~f1"
if /i %~x1==.py  call :chkcmd Python.exe "%~f1"
goto :eof

:chkcmd program script
set CMDLINE=%*
rem If there is no interpreter Ruby or Python, suggest getting one
if "%~$PATH:1"=="" set CMDLINE=call :notinstalled %*
goto :eof

:notinstalled program
echo %1 not found on search path>&2
echo %~n1 is not distributed with TeX Live and has to be installed separately
exit /b 1

:noscript this_file program_name
echo %~nx1: no appropriate script or program found: "%~2">&2
exit /b 1

:nokpsewhich
echo %~nx0: kpsewhich not found: "%~dp0kpsewhich.exe">&2
exit /b 1

