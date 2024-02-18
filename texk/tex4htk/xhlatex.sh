#!/bin/sh
# stop at first error
set -e

# No interaction on the TeX runs is desirable.
# Simpler to do that here than on the individual commands;
# let's hope exec redirections are portable enough.
exec </dev/null


# xhlatex (2024-01-23-13:46), generated from tex4ht-mkht.tex
# Copyright 2009-2020 TeX Users Group
# Copyright 2003-2009 Eitan M. Gurari
#
# This work may be distributed and/or modified under the
# conditions of the LaTeX Project Public License, either
# version 1.3 of this license or (at your option) any
# later version. The latest version of this license is in
#   http://www.latex-project.org/lppl.txt
# and version 1.3 or later is part of all distributions
# of LaTeX version 2003/12/01 or later.
#
# This work has the LPPL maintenance status "maintained".
#
# The Current Maintainer of this work
# is the TeX4ht Project <https://tug.org/tex4ht>.
#
# If you modify this file, changing the
# version identification be appreciated.


        latex $5 '\makeatletter\def\HCode{\futurelet\HCode\HChar}\def\HChar{\ifx"\HCode\def\HCode"##1"{\Link##1}\expandafter\HCode\else\expandafter\Link\fi}\def\Link#1.a.b.c.{\g@addto@macro\@documentclasshook{\RequirePackage[#1,xhtml]{tex4ht}}\let\HCode\documentstyle\def\documentstyle{\let\documentstyle\HCode\expandafter\def\csname tex4ht\endcsname{#1,xhtml}\def\HCode####1{\documentstyle[tex4ht,}\@ifnextchar[{\HCode}{\documentstyle[tex4ht]}}}\makeatother\HCode '$2'.a.b.c.\input ' $1
        latex $5 '\makeatletter\def\HCode{\futurelet\HCode\HChar}\def\HChar{\ifx"\HCode\def\HCode"##1"{\Link##1}\expandafter\HCode\else\expandafter\Link\fi}\def\Link#1.a.b.c.{\g@addto@macro\@documentclasshook{\RequirePackage[#1,xhtml]{tex4ht}}\let\HCode\documentstyle\def\documentstyle{\let\documentstyle\HCode\expandafter\def\csname tex4ht\endcsname{#1,xhtml}\def\HCode####1{\documentstyle[tex4ht,}\@ifnextchar[{\HCode}{\documentstyle[tex4ht]}}}\makeatother\HCode '$2'.a.b.c.\input ' $1
        latex $5 '\makeatletter\def\HCode{\futurelet\HCode\HChar}\def\HChar{\ifx"\HCode\def\HCode"##1"{\Link##1}\expandafter\HCode\else\expandafter\Link\fi}\def\Link#1.a.b.c.{\g@addto@macro\@documentclasshook{\RequirePackage[#1,xhtml]{tex4ht}}\let\HCode\documentstyle\def\documentstyle{\let\documentstyle\HCode\expandafter\def\csname tex4ht\endcsname{#1,xhtml}\def\HCode####1{\documentstyle[tex4ht,}\@ifnextchar[{\HCode}{\documentstyle[tex4ht]}}}\makeatother\HCode '$2'.a.b.c.\input ' $1
        tex4ht -f/$1  -i~/tex4ht.dir/texmf/tex4ht/ht-fonts/$3
        t4ht -f/$1 $4 -cvalidate ## -d~/WWW/temp/ -m644 



