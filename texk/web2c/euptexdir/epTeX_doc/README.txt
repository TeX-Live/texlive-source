----------------------------------------------------------------------
******                  e-pTeX build 110102                     ******
----------------------------------------------------------------------

    This program, ``e-pTeX'', is development of e-TeX, and it has
features of Japanese pTeX. Because of this, perhaps ``pe-TeX'' is 
better name for this software, but because of historical reasons, 
I decided to continue to use ``e-pTeX'' as the name of this software.
(When I developed e-pTeX first, this is ``e-TeX extension of pTeX''.
 And moreover, there was another ``peTeX'' (about 2007.12--2008.1, 
 abandoned by appearance of e-pTeX) by Akira Kakuto.)

    In addition to features of pTeX, e-pTeX can use 256 math font 
families (this feature is realized by fam256.ch, based on Omega's 
source code). This extension developed in mind that pTeX usually use
more two families than original TeX (`Mincho' and `Gothic').

    e-pTeX is developed as the project in the course named ``Computing
for Mathematicians II''(http://ks.ms.u-tokyo.ac.jp/ (in Japanese), 2007
Winter), and it is licensed under modified BSD (same as pTeX, ptexenc).

    Thanks to Noriyuki Abe, Akira Kakuto, Takuji Tanaka, Takayuki Yato, 
Yusuke Kuroki and others for many help.

                                             Hironori Kitagawa (H7K)
                                          h_kitagawa2001 at yahoo.co.jp
(in Japanese) https://github.com/h-kitagawa/eptex-wiki/blob/main/README.md




** bug list
----------------------------------------------------------------------
1. The e-TRIP test fails around `last_node_type'.
2. Compatibility for pTeX is not well-tested.

** Files
----------------------------------------------------------------------
 >README.txt                 this file
 >Changelog 
  INSTALL.txt
 
  all.sh                     \
  common.sh                  | bash scripts
  reautoconf-parallel.diff   |
  scripts/                   /

  eptex/

     >eptex-*-texlive2010.diff
     >euptex-*-up0.30-texlive2010.diff
                             main patches

      eptex-101231-pdfcreationdate.diff
                             a patch to kanji.* for \pdfcreationdate (unused)

      eptex.src              modified from etex.src for e-pTeX
      eptexdefs.lib          modified from etexdefs.lib for e-pTeX
      euptex.am              a fragment of Makefile for e-upTeX
      euptex.src             modified from etex.src for e-upTeX
      language.def           language settings for eptex.fmt, euptex.fmt

      pdfstrcmp.ch           a patch to support \pdfstrcmp
      ptexenc-110102-toUCS_export.diff

  ptex/

      ptexfam-common-texlive2010.diff 
                             patch for supporting (e-)(u)pTeX
      ptex-base.ch.0711.diff
      ptex-base.ch.0814-tl10.diff
      ptex-base.ch.nullfont.diff
      updmap-live2009-4b.diff
      updmap-no-r.patch
      updmap-nostop.patch
      xdvik-20100725-texlive2010.diff
      japanese.zip

  doc/                     documents (all in Japanese)

     >eptexdoc.*           main document of e-pTeX
      fam256d.tex          \
      fam256p.tex          |- auxiliary files for eptexdoc.tex
      styles.tex           /
      resume.pdf           development record in 
                           ``Computing for Mathematicians II''


** environment:
----------------------------------------------------------------------
Gentoo Linux 10.0 (amd64)
gcc-4.5.2, autoconf-2.65-r1, automake-1.11.1, m4-1.4.15

It is assumed that /bin/bash works.
