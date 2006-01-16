@echo off
rem Because of a bug in nmake, these variables have to be set
rem before running nmake, for their values to be overriden by
rem those from the Makefile
set TEXMFMAIN=foo
set TEXMF=foo
set TEXMFCNF=foo
set MKTEXFMT=foo
set GROFF_TMAC_PATH=foo
set GROFF_FONT_PATH=foo
set LATEX2HTML=foo