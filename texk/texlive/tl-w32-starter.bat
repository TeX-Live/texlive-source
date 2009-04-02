@echo off
rem tl-w32-starter.bat
rem universal script starter, batch file part
rem this program calls the tl-w32-wrapper.texlua
setlocal
set ownpath=%~dp0%
texlua "%ownpath%tl-w32-wrapper.texlua" "%~dpn0" %*
endlocal
