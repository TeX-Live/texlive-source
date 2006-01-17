@echo off
REM Given a DVI file, this command file carries out some of the steps
REM needed to produce an A5 booklet suitable for folding and stapling.
REM It is assumed the user has set up a page format suitable for A5 paper.
REM The fancy pagination is done by Tom Rokicki's DVIDVI program.
REM Note that 148mm = A5 width = half A4 height.
REM
REM We assume that the following symbols have been defined:
REM a5booklet :== @disk$utils:[utilities.tex.dvidvi]a5booklet
REM dvidvi    :== $disk$utils:[utilities.tex.dvidvi]dvidvi
REM
if not "%1" == "" goto :file_given
echo.
echo.
echo Usage:  a5booklet file[.dvi]
echo.
goto :exit

:file_given
dvidvi -m 4:-1,2(148mm,0mm) %1 part1
dvidvi -m 4:-3,0(148mm,0mm) %1 part2
REM
REM Now explain how to preview or PSPRINT the new DVI files.
REM
echo.
echo.
echo PART1.DVI and PART2.DVI have been created; each part contains the pages that
echo must be printed on the same side of the paper.  In both parts, each A4 page
echo contains two side-by-side A5 pages from your original document.
echo.
echo To preview these DVI files you need to tell DVISCR to display each page
echo in a landscaped orientation; for example:
echo.
echo      $ dviscr part1
echo.
echo To create an A5 booklet suitable for folding and stapling, carry out the
echo following steps (assuming you are using a LaserWriter):
echo. 
echo   1. $ psprint /land /nobanner /notify part1
echo   2. When this job has finished, take the output and put it back into
echo      the input cassette without changing the orientation in any way.
echo   3. $ psprint /land part2
echo   4. Depending on the number of pages, the final output may not be collated.
echo      You may need to change the placement of the bottom sheet(s).
echo.
echo.

:exit

