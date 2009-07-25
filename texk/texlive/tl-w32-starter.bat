@echo off
rem Universal script launcher
rem
rem Public Domain
rem Originally written 2009 by Tomasz M. Trzeciak.

rem Make environment changes local
setlocal enableextensions
rem Get program/script name
set progname=%~n0
rem Check if this is 'sys' version of program
set TEX_SYS_PROG=
if /i "%progname:~-4%"=="-sys" (
  set progname=%progname:~0,-4%
  set TEX_SYS_PROG=true
)

rem Default command to execute
set command=call :noscript
rem Make sure our dir is on the search path; avoid trailing backslash
for %%I in ("%~f0\..") do path %%~fI;%path%
rem Ask kpsewhich about root and texmfsys trees (the first line of output)
rem and location of the script (the second line of output)
rem (4NT shell acts wierd with 'if' statements in a 'for' loop,
rem so better process this output further in a subroutine)
for /f "tokens=1-3 delims=;" %%I in (
  'kpsewhich --expand-var "$SELFAUTOPARENT;$TEXMFSYSCONFIG;$TEXMFSYSVAR" ^
  --format texmfscripts "%progname%.pl" "%progname%.tlu" "%progname%.rb" "%progname%.py"'
) do (
  call :setcmdenv "%%~I" "%%~J" "%%~K"
)

rem By now we should have the command to execute (whatever that is), so
rem pass through all the arguments we have and execute it
%command% %*
rem Finish with goto :eof (it will preserve the last set errorlevel)
goto :eof

REM SUBROUTINES

:setcmdenv selfautoparent texmfsysconfig texmfsysvar
rem If there is only one argument it must be a script name
if "%~2"=="" goto :setcmd
rem Otherwise, it is the first line from kpsewhich, so to set up the environment
set PERL5LIB=%~f1\tlpkg\tlperl\lib
set GS_LIB=%~f1\tlpkg\tlgs\lib;%~f1\tlpkg\tlgs\fonts
path %~f1\tlpkg\tlgs\bin;%~f1\tlpkg\tlperl\bin;%~f1\tlpkg\installer;%~f1\tlpkg\installer\wget;%path%
if not defined TEX_SYS_PROG goto :eof
rem Extra stuff for sys version
set TEXMFCONFIG=%~2
set TEXMFVAR=%~3
rem For sys version we might have an executable in the bin dir, so check for it
if exist "%~f1\bin\win32\%progname%.exe" set command="%~f1\bin\win32\%progname%.exe"
goto :eof

:setcmd script
rem Set command based on the script extension
if /i %~x1==.pl  set command=Perl.exe "%~f1"
if /i %~x1==.tlu set command=TeXLua.exe "%~f1"
rem For Ruby and Python we additionally check if their interpreter is available
if /i %~x1==.rb  call :chkcmd Ruby.exe "%~f1"
if /i %~x1==.py  call :chkcmd Python.exe "%~f1"
goto :eof

:chkcmd program script
set command=%*
rem If there is no interpreter Ruby or Python, suggest getting one
if "%~$PATH:1"=="" set command=call :noinst %*
goto :eof

:noinst program
echo %1 not found on search path>&2
echo %~n1 is not distributed with TeX Live and has to be installed separately
exit /b 1

:noscript
echo %progname%: no appropriate script or program found>&2
exit /b 1
