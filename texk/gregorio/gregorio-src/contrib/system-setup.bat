@echo off
SETLOCAL ENABLEEXTENSIONS EnableDelayedExpansion

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
echo #### TEXMFDIST >> %output%
kpsewhich --var-value=TEXMFDIST >> %output% 2>&1
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
echo ####	Locations and Versions >> %output%
for /f "delims=" %%G in ('where /f gregorio*') do (
  echo %%G >> %output%
  for /f "delims=" %%H in ('%%G -V') do echo %%H >> %output% 2>&1
  echo. >> %output%
)
echo ####	GregorioTeX Locations >> %output%
echo. >> %output%

:: Files using GREGORIO_VERSION in {}
set files=gregoriosyms.sty ^
gregoriotex-chars.tex ^
gregoriotex-main.tex ^
gregoriotex-nabc.tex ^
gregoriotex-signs.tex ^
gregoriotex-spaces.tex ^
gregoriotex-syllable.tex ^
gregoriotex-common.tex ^
gregoriotex-symbols.tex

for %%G in (%files%) do (
	echo ##### %%G >> %output%
	for /f "delims=" %%H in ('kpsewhich -all %%G') do (
		set loc=%%H
		set loc=!loc:/=\!
		echo !loc! >> %output%
		for /f "delims=" %%I in ('findstr /r "GREGORIO_VERSION" "!loc!"') do set ver=%%I
		set ver=!ver:*{=!
		set ver=!ver:*{=!
		set trash=}!ver:*}=!
		call set ver=%%ver:!trash!=%%
		echo !ver! >> %output% 2>&1
		set ver=
	)
)

:: Files using GREGORIO_VERSION in spaces
set files=gregoriotex-nabc.lua ^
gregoriotex-signs.lua ^
gregoriotex-symbols.lua

for %%G in (%files%) do (
	echo ##### %%G >> %output%
	for /f "delims=" %%H in ('kpsewhich -all %%G') do (
		set loc=%%H
		set loc=!loc:/=\!
		echo !loc! >> %output%
		for /f "delims=" %%I in ('findstr /r "GREGORIO_VERSION" "!loc!"') do set ver=%%I
		set ver=!ver:*N =!
		echo !ver! >> %output% 2>&1
		set ver=
	)
)

:: Files using GREGORIO_VERSION in ''
set files=gregoriotex.lua

for %%G in (%files%) do (
	echo ##### %%G >> %output%
	for /f "delims=" %%H in ('kpsewhich -all %%G') do (
		set loc=%%H
		set loc=!loc:/=\!
		echo !loc! >> %output%
		for /f "delims=" %%I in ('findstr /r "GREGORIO_VERSION" "!loc!"') do set ver=%%I
		set ver=!ver:*'=!
		set trash='!ver:*'=!
		call set ver=%%ver:!trash!=%%
		echo !ver! >> %output% 2>&1
		set ver=
	)
)

:: Files using PARSE_VERSION_DATE_LTX
set files=gregoriotex.sty ^
gregoriotex.tex

for %%G in (%files%) do (
	echo ##### %%G >> %output%
	for /f "delims=" %%H in ('kpsewhich -all %%G') do (
		set loc=%%H
		set loc=!loc:/=\!
		echo !loc! >> %output%
		for /f "delims=" %%I in ('findstr /r "PARSE_VERSION_DATE_LTX" "!loc!"') do set ver=%%I
		set ver=!ver:*v=!
		set trash=G!ver:*G=!
		call set ver=%%ver:!trash!=%%
		echo !ver! >> %output% 2>&1
		set ver=
	)
)

:: Font Files
set files=greciliae.ttf ^
greciliae-hollow.ttf ^
greciliae-hole.ttf ^
greciliae-op.ttf ^
greciliae-op-hollow.ttf ^
greciliae-op-hole.ttf ^
greextra.ttf ^
gregorio.ttf ^
gregorio-hollow.ttf ^
gregorio-hole.ttf ^
gregorio-op.ttf ^
gregorio-op-hollow.ttf ^
gregorio-op-hole.ttf ^
granapadano.ttf ^
granapadano-hollow.ttf ^
granapadano-hole.ttf ^
granapadano-op.ttf ^
granapadano-op-hollow.ttf ^
granapadano-op-hole.ttf ^
gregall.ttf

for %%G in (%files%) do (
	echo ##### %%G >> %output%
	for /f "delims=" %%H in ('kpsewhich -all %%G') do (
		set loc=%%H
		set loc=!loc:/=\!
		echo !loc! >> %output%
		otfinfo --font-version "!loc!" >> %output% 2>&1
	)
)

:: Unversioned and Obsolete Files
set files=gregorio-vowels.dat ^
gsp-default.tex ^
gregoriotex-ictus.tex ^
gresym.ttf ^
parmesan.ttf ^
parmesan-op.ttf ^
gregsmodern.ttf

for %%G in (%files%) do (
	echo ##### %%G >> %output%
	for /f "delims=" %%H in ('kpsewhich -all %%G') do (
		set loc=%%H
		set loc=!loc:/=\!
		echo !loc! >> %output%
	)
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
echo gregorio-users@googlegroups.com as part of your bug report.
echo.
echo You can also create an issue at
echo http://github.com/gregorio-project/gregorio/issues
echo and copy-paste the content of this file into the description.
echo.
pause
start notepad %output%
