#!/bin/sh

if [ $# -ne 1 ]; then
  echo "usage: $0 font-name | gsnd - >font-name.afm" >&2
  exit 1
fi

cat << EOF
%!
% produce .afm for $1
% (c) 1993 by Robert Joop <rj@rainbow.in-berlin.de>
% inspired by two other versions of this theme which are
%	getafm 1.00 (c) AJCD
% and getafm.ps by an unknown author,
%	modified by J. Daniel Smith <dsmith@mailhost.aa.cad.slb.com>

% Metrics dictionary code added by AJCD, 7/6/93

/getafmdict 100 dict dup begin

  /buf 256 string def
  /buf2 16 string def

  /prany	% dict dictname printname -> dict
  {
    2 index 2 index cvn known
    {
      print % printname
      ( ) print
      1 index exch cvn get =
    }
    {
      (Comment /FontInfo contains no /) print
      2 copy eq
      {
        = % printname
	pop % dictname
      }
      {
	exch
	print % dictname
	(, therefore no ) print
	= % printname
      }
      ifelse
    }
    ifelse
  }
  bind def

  /printfontname
  {
    (FontName)dup prany
  }
  bind def

  /printfontinfo
  {
    dup /FontInfo known
    {
      dup /FontInfo get
	(FullName)dup prany
	(FamilyName)dup prany
	(Weight)dup prany
	(ItalicAngle)dup prany
	(isFixedPitch)(IsFixedPitch) prany
	(UnderlinePosition)dup prany
	(UnderlineThickness)dup prany
	(Version)(version) prany
	(Notice)dup prany
      pop
    }
    {
      (Comment Font lacks a /FontInfo!)=
    }
    ifelse
  }
  bind def

  /prbbox % llx lly urx ury -> -
  {
    4 1 roll 3 1 roll exch % swap top 4 elements
    4 { ( ) print buf cvs print } repeat
  }
  bind def

  /getbbox % fontdict chardict character -> fontdict chardict llx lly urx ury
  {
    gsave
      2 index setfont 0 0 moveto
      false charpath flattenpath pathbbox
    grestore
  }
  bind def

  /printmiscinfo
  {
    dup /FontBBox known
    {
      (FontBBox) print
      dup /FontBBox get aload pop prbbox ()=
    }
    {
      (Comment missing required /FontBBox)=
      quit
    }
    ifelse
    2 copy exch get
      dup /H known
      1 index /x known and
      1 index /d known and
      1 index /p known and
      dup /looksRoman exch def
      {
	(CapHeight ) print
	(H) getbbox
	ceiling cvi = pop pop pop
	(XHeight ) print
	(x) getbbox
	ceiling cvi = pop pop pop
	(Ascender ) print
	(d) getbbox
	ceiling cvi = pop pop pop
	(Descender ) print
	(p) getbbox
	pop pop floor cvi = pop
      }
      {
	(Comment font doesn't contain H, x, d and p; therefore no CapHeight, XHeight, Ascender and Descender)=
      }
      ifelse
    pop
    dup /Encoding get
      [
	[ (ISOLatin1Encoding) /ISOLatin1Encoding ]
	[ (AdobeStandardEncoding) /StandardEncoding ]
      ]
      {
	aload pop dup where
	{
	  exch get 2 index eq
	  {
	    (EncodingScheme ) print
	    buf cvs =
	  }
	  {
	    pop
	  }
	  ifelse
	}
	{
	  pop pop
	}
	ifelse
      }
      forall
    pop
  }
  bind def

  /printcharmetric
  {
    % chardictname fontdict charnamedict encoding charindex charname

    4 index dup length dict dup begin exch
      {
	1 index /FID ne
	2 index /UniqueID ne
	and
	{
	  1 index /Encoding eq { 256 array copy } if
	  def
	}
	{ pop pop }
	ifelse
      }
      forall
    end
    dup /Encoding get 32 3 index put
    /f2 exch definefont
    setfont

    (C ) print
    1 index buf cvs print

    ( ; WX ) print
% Metrics entries are:
%       1 number:               which is the character width
%       an array of 2 numbers:  which are the left sidebearing and width
%       an array of 4 numbers:  x & y left sidebearing, width and height
    dup 5 index         % /charname fontdict
    dup /Metrics known {
       /Metrics get exch 2 copy known {
          get dup type /arraytype eq {
             dup length 2 eq
             {1 get} {2 get} ifelse
          } if
          round cvi buf cvs print
       } {
          pop pop ( ) stringwidth pop round cvi buf cvs print
       } ifelse
    } {
       pop pop ( ) stringwidth pop round cvi buf cvs print
    } ifelse

    ( ; N ) print
    dup buf cvs print

    ( ; B) print
    gsave
      newpath 0 0 moveto
      ( ) true charpath flattenpath pathbbox
    grestore
    2 { ceiling cvi 4 1 roll } repeat
    2 { floor cvi 4 1 roll } repeat
    prbbox

    looksRoman
    {
      [
	[ /f [ /i /f /l ] ]
	[ /ff [ /i /l ] ]
      ]
      {
	aload pop 1 index 3 index eq
	{
	  { 
	    1 index buf cvs
	    length
	    1 index buf2 cvs dup length
	    2 index add
	    buf
	    4 2 roll putinterval
	    buf 0
	    3 -1 roll getinterval
	    dup cvn
	    7 index
	    exch known
	    {
	      exch
	      ( ; L ) print
	      buf2 cvs print
	      ( ) print
	      print
	    }
	    {
	      pop pop
	    }
	    ifelse
	  }
	  forall
	  pop 
	}
	{
	  pop pop
	}
	ifelse
      }
      forall
    }
    if
    pop

    ( ;)=
  }
  bind def

  /printcharmetrics
  {
    (StartCharMetrics ) print
    2 copy exch get length 1 sub buf cvs =

    256 dict dup begin
      1 index /Encoding get
      { null def }
      forall
    end
    % chardictname fontdict charnamedict
    1 index /Encoding get
    0 1 255
    {
      % encoding index
      2 copy get
      dup /.notdef eq { pop } { printcharmetric } ifelse
      pop % index
    } for

    -1
    3 index 5 index get
    {
      pop
      dup /.notdef eq
      { pop }
      {
	% chardictname fontdict charnamedict encoding charindex charname
	dup 4 index exch known
	{ pop }
	{ printcharmetric }
	ifelse
      }
      ifelse
    }
    forall
    % charnamedict encoding index
    pop pop pop

    (EndCharMetrics)=
  }
  bind def

  /printfontmetrics
  {
    (StartFontMetrics 3.0)=
    (Comment Produced by getafm 3.0 (which is by rj@rainbow.in-berlin.de))=

    printfontname
    printfontinfo
    printmiscinfo
    printcharmetrics

    (EndFontMetrics)=
  }
  bind def

end def

/getafm
{
  getafmdict begin
    save exch
      findfont 1000 scalefont 

      null
      [ /CharDefs /CharData /CharProcs /CharStrings ]
      {
	2 index 1 index known { exch } if
	pop
      }
      forall
      dup null eq
      {
	(can't find dictionary with character data!)=
	quit
      }
      if
      exch % dictname fontdict

      printfontmetrics

      pop pop
    restore
  end
}
bind def

/$1 getafm

EOF
