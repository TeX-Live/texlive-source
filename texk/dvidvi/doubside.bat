@echo off
REM Given a DVI file, this command file carries out the steps
REM needed to print a dvi file in double side.
REM The fancy pagination is done by Tom Rokicki's DVIDVI program.
REM
if not "%1" == "" goto :file_given
echo.
echo Usage:  doubside file[.dvi]
echo.
go to :exit
:file_given
dvidvi -m2:-1 %1 part1
echo.
dvidvi -m2:0  %1 part2
REM
REM Now explain how to preview or PSPRINT the new DVI files.
REM
echo.
echo.
echo PART1.DVI and PART2.DVI have been created; each part contains the pages that
echo must be printed on the same side of the paper.
echo To print double sided documents, carry out the
echo following steps (assuming you are using a LaserWriter):
echo.
echo     1. $ psprint part1
echo     2. When this job has finished, take the output and put it back into
echo        the input cassette without changing the orientation in any way.
echo     3. $ psprint part2
echo     4. Depending on the number of pages, the final output may not be collated.
echo        You may need to change the placement of the bottom sheet(s).
echo.

:exit

