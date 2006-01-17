@echo off
kpsewhich amstex.fmt > nul
if errorlevel 1 goto genformat
if errorlevel 0 goto okay
:genformat
fmtutil --byfmt amstex
:okay
tex -fmt=amstex -progname=tex %1 %2 %3 %4 %5 %6 %7 %8 %9
