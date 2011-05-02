#!/bin/sh
# showchar: show character with information
# usage:
#       showchar Font-Name Char-Name | lpr -Pprinter

if [ $# != 2 ]; then
        echo "Usage: `basename $0` Font-Name Char-Name" >&2
        exit 1
fi

cat <<EOF
%!
% Get character metrics and bounding box for $1
/ns 30 string def
/fname /$1 def
/cname /$2 def
/fn fname findfont 1000 scalefont def
/thin 0.7 def
/reduction 2.0 def

/sn { ns cvs show } def
/sc { (\()show exch sn (,)show sn (\))show } def

/Times-Roman findfont 10 scalefont setfont

72 242 translate % one inch in, 3 1/2 inch up
newpath 0 0 moveto
gsave   % print character name
   0 468 rmoveto
   ($2 from font $1 displayed on `date` by `basename $0` 1.00 (C) AJCD 1991)
   show 
grestore

gsave
   thin setlinewidth  % cross hairs
   -9 0 rmoveto 18 0 rlineto
   -9 -9 rmoveto 0 18 rlineto
   stroke  % position info
   -9 -9 moveto ((0,0)) stringwidth pop neg 0 rmoveto
   ((0,0)) show
grestore

% create encoding with a single character at all positions
/MyEncoding 256 array def

0 1 255 { % fill out with notdefs
   MyEncoding exch cname put
} for

fn dup length dict begin
  {1 index /FID ne {def} {pop pop} ifelse} forall
  /Encoding MyEncoding def
  currentdict
end /newfont exch definefont
/fn exch def
/en MyEncoding def

gsave  % draw character (enlarged)
  .75 setgray
  fn 1 reduction div scalefont setfont
  (\000) show
grestore

% show character info
fn /Metrics known {
   dup fn /Metrics get exch get 
   dup type /arraytype eq {
      dup length 2 eq
      {1 get 0} {dup 2 get exch 3 get} ifelse
   } {
      round 0
   } ifelse
} {
   gsave
      fn setfont (\000) stringwidth round exch round exch
   grestore
} ifelse
gsave   % show width
   thin setlinewidth
   2 copy
   reduction div exch reduction div exch rlineto
   gsave
      1 1 rmoveto sc
   grestore
   -4 -3 rlineto 0 6 rmoveto 4 -3 rlineto
   stroke
grestore

gsave
   gsave  % get bounding box
      fn setfont
      (\000) true charpath flattenpath pathbbox
   grestore
   thin setlinewidth
   3 index reduction div  % put coords on top of stack
   3 index reduction div
   3 index reduction div
   3 index reduction div
   3 index 3 index moveto
   gsave  % llx lly
      0 -9 rmoveto 7 index 7 index sc
   grestore
   1 index 3 index lineto
   gsave  % urx lly
      0 -9 rmoveto 5 index 7 index sc
   grestore
   1 index 1 index lineto
   gsave  % urx ury
      0 9 rmoveto 5 index 5 index sc
   grestore
   3 index 1 index lineto
   gsave  % llx ury
      0 9 rmoveto 7 index 5 index sc
   grestore
   closepath stroke
grestore
showpage
EOF
