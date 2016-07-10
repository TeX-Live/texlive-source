Rem 
Rem 
Rem   xhlatex.bat                                  Version 1.1
Rem   Copyright (C) 2003--2009               Eitan M. Gurari
Rem   Copyright 2009 TeX Users Group
Rem 
Rem   This work may be distributed and/or modified under the
Rem   conditions of the LaTeX Project Public License, either
Rem   version 1.3 of this license or (at your option) any
Rem   later version. The latest version of this license is
Rem   in
Rem     http://www.latex-project.org/lppl.txt
Rem   and version 1.3 or later is part of all distributions
Rem   of LaTeX version 2003/12/01 or later.
Rem 
Rem   This work has the LPPL maintenance status "maintained".
Rem 
Rem   The Current Maintainer of this work
Rem   is the TeX4ht Project.
Rem 
Rem                                          tex4ht@tug.org
Rem                               http://www.tug.org/tex4ht
Rem 
Rem 


        latex %5 \makeatletter\def\HCode{\futurelet\HCode\HChar}\def\HChar{\ifx"\HCode\def\HCode"##1"{\Link##1}\expandafter\HCode\else\expandafter\Link\fi}\def\Link#1.a.b.c.{\g@addto@macro\@documentclasshook{\RequirePackage[#1,xhtml]{tex4ht}}\let\HCode\documentstyle\def\documentstyle{\let\documentstyle\HCode\expandafter\def\csname tex4ht\endcsname{#1,xhtml}\def\HCode####1{\documentstyle[tex4ht,}\@ifnextchar[{\HCode}{\documentstyle[tex4ht]}}}\makeatother\HCode %2.a.b.c.\input  %1
        latex %5 \makeatletter\def\HCode{\futurelet\HCode\HChar}\def\HChar{\ifx"\HCode\def\HCode"##1"{\Link##1}\expandafter\HCode\else\expandafter\Link\fi}\def\Link#1.a.b.c.{\g@addto@macro\@documentclasshook{\RequirePackage[#1,xhtml]{tex4ht}}\let\HCode\documentstyle\def\documentstyle{\let\documentstyle\HCode\expandafter\def\csname tex4ht\endcsname{#1,xhtml}\def\HCode####1{\documentstyle[tex4ht,}\@ifnextchar[{\HCode}{\documentstyle[tex4ht]}}}\makeatother\HCode %2.a.b.c.\input  %1
        latex %5 \makeatletter\def\HCode{\futurelet\HCode\HChar}\def\HChar{\ifx"\HCode\def\HCode"##1"{\Link##1}\expandafter\HCode\else\expandafter\Link\fi}\def\Link#1.a.b.c.{\g@addto@macro\@documentclasshook{\RequirePackage[#1,xhtml]{tex4ht}}\let\HCode\documentstyle\def\documentstyle{\let\documentstyle\HCode\expandafter\def\csname tex4ht\endcsname{#1,xhtml}\def\HCode####1{\documentstyle[tex4ht,}\@ifnextchar[{\HCode}{\documentstyle[tex4ht]}}}\makeatother\HCode %2.a.b.c.\input  %1
        tex4ht %1  -i/tex4ht/ht-fonts/%3 -ewin32/tex4ht.env
        t4ht %1 %4 -ewin32/tex4ht.env -cvalidate
