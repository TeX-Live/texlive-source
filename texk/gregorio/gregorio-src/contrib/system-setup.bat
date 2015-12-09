@echo off
SETLOCAL ENABLEEXTENSIONS

set output="%TEMP%\system-setup.log"

echo Gregorio Windows Setup Diagnostic Tool
echo (C) 2015 The Gregorio Project.
echo.
echo Gregorio is free software: you can redistribute it and/or modify
echo it under the terms of the GNU General Public License as published by
echo the Free Software Foundation, either version 3 of the License, or
echo (at your option) any later version.
echo.
echo This program is distributed in the hope that it will be useful,
echo but WITHOUT ANY WARRANTY; without even the implied warranty of
echo MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
echo GNU General Public License for more details.
echo.
echo You should have received a copy of the GNU General Public License
echo along with this program.  If not, see http://www.gnu.org/licenses/.
echo.
echo Creating system-setup.log...

echo ###	Gregorio Windows Setup Results > %output%
echo ####	Created: %date% >> %output%
echo ----------------------------------------------------------------------------- >> %output%
echo. >> %output%
echo. >> %output%

echo ###	Windows Version >> %output%
ver >> %output%
echo. >> %output%
echo. >> %output%
echo ----------------------------------------------------------------------------- >> %output%
echo. >> %output%
echo. >> %output%

echo ###	LuaTeX Setup >> %output%
echo ####	Version >> %output%
echo. >> %output%
luatex -v >> %output% 2>&1
echo. >> %output%
echo ####	Location >> %output%
echo. >> %output%
@for %%e in (%PATHEXT%) do @for %%i in (luatex%%e) do @if NOT "%%~$PATH:i"=="" echo %%~$PATH:i >> %output% 2>&1
echo. >> %output%
echo. >> %output%
echo #### 	TEXMFLOCAL >> %output%
echo. >> %output%
for /f "delims=" %%i in ('kpsewhich --var-value TEXMFLOCAL') do set texmflocal=%%i
echo %texmflocal% >> %output% 2>&1
set texmflocal=%texmflocal:/=\%
IF NOT EXIST %texmflocal% ECHO Folder does not exist >> %output% 2>&1
echo. >> %output%
echo #### TEXINPUTS.lualatex >> %output%
kpsewhich --var-value=TEXINPUTS.lualatex >> %output% 2>&1
echo. >> %output%
echo #### LUAINPUTS.lualatex >> %output%
kpsewhich --var-value=LUAINPUTS.lualatex >> %output% 2>&1
echo. >> %output%
echo #### shell_escape >> %output%
kpsewhich --var-value=shell_escape >> %output% 2>&1
echo. >> %output%
echo #### Shell Escape Commands >> %output%
kpsewhich --var-value=shell_escape_commands >> %output% 2>&1
echo. >> %output%
echo #### openout_any >> %output%
kpsewhich --var-value=openout_any >> %output% 2>&1
echo. >> %output%
echo #### openin_any >> %output%
kpsewhich --var-value=openin_any >> %output% 2>&1
echo. >> %output%
echo. >> %output%
echo ----------------------------------------------------------------------------- >> %output%
echo. >> %output%
echo. >> %output%

echo ###	Gregorio Setup >> %output%
echo ####	Version >> %output%
echo. >> %output%
gregorio -V >> %output% 2>&1
echo. >> %output%
echo #### 	Location >> %output%
echo. >> %output%
@for %%e in (%PATHEXT%) do @for %%i in (gregorio%%e) do @if NOT "%%~$PATH:i"=="" echo %%~$PATH:i >> %output% 2>&1
echo. >> %output%
echo ####	GregorioTeX Locations >> %output%
echo. >> %output%

set files=gregoriotex.sty ^
gregoriosyms.sty ^
gregoriotex.tex ^
gregoriotex-main.tex ^
gregoriotex-chars.tex ^
gregoriotex-ictus.tex ^
gregoriotex-signs.tex ^
gregoriotex-signs.lua ^
gregoriotex-symbols.tex ^
gregoriotex-spaces.tex ^
gregoriotex-syllable.tex ^
gregoriotex-nabc.tex ^
gregoriotex-nabc.lua ^
gregoriotex.lua ^
gsp-default.tex ^
gregorio-vowels.dat ^
greciliae.ttf ^
greciliae-op.ttf ^
greextra.ttf ^
gregorio.ttf ^
gregorio-op.ttf ^
gresym.ttf ^
parmesan.ttf ^
parmesan-op.ttf ^
gregall.ttf ^
gregsmodern.ttf

for %%G in (%files%) do (
	echo ##### %%G >> %output%
	kpsewhich -all %%G >> %output% 2>&1
)
echo. >> %output%
echo ####	kpsewhich --all -engine luatex -progname lualatex gregoriotex.sty >> %output%
kpsewhich --all -engine luatex -progname lualatex gregoriotex.sty >> %output% 2>&1
echo. >> %output%
echo ####	kpsewhich --all -engine luatex gregoriotex.tex >> %output%
kpsewhich --all -engine luatex gregoriotex.tex >> %output% 2>&1
echo. >> %output%
echo. >> %output%
echo ----------------------------------------------------------------------------- >> %output%
echo. >> %output%
echo. >> %output%

echo.
echo.
echo system-setup.log created and saved in a temporary location.
echo Upon exiting this script, the log will be opened in Notepad for you.
echo Please save the file to a convenient location and email it to 
echo gregorio-users@gna.org as part of your bug report.
echo.
echo You can also create an issue at 
echo http://github.org/gregorio-project/gregorio/issues
echo and copy-paste the content of this file into the description.
echo. 
pause
start notepad %output%
