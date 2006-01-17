@echo off
REM Staszek Wawrykiewicz (staw@gust.org.pl). March 2000
REM This batch file installs the local TeX tree for running
REM programs directly from the CD TeX Live 5 (Windows 9x/2000/NT)
REM It allows changing some parameters (MF mode for dvips and windvi, etc.)
REM and building local format files.
REM Run: mkloctex g c
REM where: %1 - CD ROM drive letter; %2 - HD drive letter for local tree
if "%1"=="" goto HELPA
if "%2"=="" goto HELPB
%2:
cd \
md TeX
md TeX\texmf
md TeX\texmf\dvips
md TeX\texmf\dvips\config
md TeX\texmf\tex
md TeX\texmf\tex\generic
md TeX\texmf\tex\generic\config
md TeX\texmf\web2c
copy %1:\texmf\dvips\config\config.ps %2:\TeX\texmf\dvips\config\*.*
copy %1:\texmf\tex\generic\config\language.dat %2:\TeX\texmf\tex\generic\config\*.*
copy %1:\texmf\web2c\fmtutil.cnf %2:\TeX\texmf\web2c\*.*
copy %1:\texmf\web2c\mktex.cnf %2:\TeX\texmf\web2c\*.*
copy %1:\texmf\web2c\texmf.cnf %2:\TeX\texmf\web2c\*.*

goto OKOK
:HELPA
echo Missing parameter: should be the letter of your CD-ROM drive

goto FINIS
:HELPB
echo Missing parameter: should be the letter of HD drive for local TeX tree

goto FINIS
:OKOK
echo.
echo 1. Add %1:\bin\win32 directory to your environment PATH,
echo 2. Add: set TEXMFCNF=%2:/TeX/texmf/web2c to your environment
echo 3. Modify %2:\TeX\texmf\web2c\texmf.cnf accordingly, e.g.,
echo      TEXMFMAIN = %1:/texmf
echo      VARTEXMF = %2:/TeX/texmf
echo.
echo 4. Remember to run `mktexlsr' after any changes.

:FINIS
mktexlsr
pause
