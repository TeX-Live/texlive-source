/*  $Header: /home/cvsroot/dvipdfmx/src/asl_charset.h,v 1.3 2004/01/30 18:34:19 hirata Exp $

    This is dvipdfmx, an eXtended version of dvipdfm by Mark A. Wicks.

    Copyright (C) 2002 by Jin-Hwan Cho and Shunsaku Hirata,
    the dvipdfmx project team <dvipdfmx@project.ktug.or.kr>

    Copyright (C) 1998, 1999 by Mark A. Wicks <mwicks@kettering.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#ifndef _ASL_CHARSET_H_
#define _ASL_CHARSET_H_

/*
 * Adobe Standard Latin Character Set - from PLRM 3rd. ed.
 *
 * Characters marked as EXT are present in the extended (315-character)
 * Latin character set, but not in the original (229-character) set.
 *
 * This list contains 314 characters, Maybe one character missing.
 *
 * Most of strings are also found in cff_stdstr.h.
 * CFF Standard Strings contains all glyph names from the original ASL
 * character set (plus Expert set). But it does not contains glyph names
 * found only in the extented version.
 *
 */

#ifdef _ENCODINGS_C_
#define ASL_CHARSET_MAX 314
static const char *const
ASL_Charset[ASL_CHARSET_MAX] = {
  "A",
  "AE",
  "Aacute",
  "Abreve",	/* EXT */
  "Acircumflex",
  "Adieresis",
  "Agrave",
  "Amacron",	/* EXT */
  "Aogonek",	/* EXT */
  "Aring",
  "Atilde",
  "B",
  "C",
  "Cacute",	/* EXT */
  "Ccaron",	/* EXT */
  "Ccedilla",
  "D",
  "Dcaron",	/* EXT */
  "Dcroat",	/* EXT */
  "Delta",	/* EXT */
  "E",
  "Eacute",
  "Ecaron",	/* EXT */
  "Ecircumflex",
  "Edieresis",
  "Edotaccent",	/* EXT */
  "Egrave",
  "Emacron",	/* EXT */
  "Eogonek",	/* EXT */
  "Eth",
  "F",
  "G",
  "Gbreve",	/* EXT */
  "Gcommaaccent",	/* EXT */
  "H",
  "I",
  "Iacute",
  "Icircumflex",
  "Idieresis",
  "Idotaccent",	/* EXT */
  "Igrave",
  "Imacron",	/* EXT */
  "Iogonek",	/* EXT */
  "J",
  "K",
  "Kcommaaccent",	/* EXT */
  "L",
  "Lacute",	/* EXT */
  "Lcaron",	/* EXT */
  "Lcommaaccent",	/* EXT */
  "Lslash",
  "M",
  "N",
  "Nacute",	/* EXT */
  "Ncaron",	/* EXT */
  "Ncommaaccent",	/* EXT */
  "Ntilde",
  "O",
  "OE",
  "Oacute",
  "Ocircumflex",
  "Odieresis",
  "Ograve",
  "Ohungarumlaut",	/* EXT */
  "Omacron",	/* EXT */
  "Oslash",
  "Otilde",
  "P",
  "Q",
  "R",
  "Racute",	/* EXT */
  "Rcaron",	/* EXT */
  "Rcommaaccent",	/* EXT */
  "S",
  "Sacute",	/* EXT */
  "Scaron",
  "Scedilla",	/* EXT */
  "Scommaaccent",	/* EXT */
  "T",
  "Tcaron",	/* EXT */
  "Tcommaaccent",	/* EXT */
  "Thorn",
  "U",
  "Uacute",
  "Ucircumflex",
  "Udieresis",
  "Ugrave",
  "Uhungarumlaut",	/* EXT */
  "Umacron",	/* EXT */
  "Uogonek",	/* EXT */
  "Uring",	/* EXT */
  "V",
  "W",
  "X",
  "Y",
  "Yacute",
  "Ydieresis",
  "Z",
  "Zacute",	/* EXT */
  "Zcaron",
  "Zdotaccent",	/* EXT */
  "a",
  "aacute",
  "abreve",	/* EXT */
  "acircumflex",
  "acute",
  "adieresis",
  "ae",
  "agrave",
  "amacron",	/* EXT */
  "ampersand",
  "aogonek",	/* EXT */
  "aring",
  "asciicircum",
  "asciitilde",
  "asterisk",
  "at",
  "atilde",
  "b",
  "backslash",
  "bar",
  "braceleft",
  "braceright",
  "bracketleft",
  "bracketright",
  "breve",
  "brokenbar",
  "bullet",
  "c",
  "cacute",	/* EXT */
  "caron",
  "ccaron",	/* EXT */
  "ccedilla",
  "cedilla",
  "cent",
  "circumflex",
  "colon",
  "comma",
  "commaaccent",	/* EXT */
  "copyright",
  "currency",
  "d",
  "dagger",
  "daggerdbl",
  "dcaron",	/* EXT */
  "dcroat",	/* EXT */
  "degree",
  "dieresis",
  "divide",
  "dollar",
  "dotaccent",
  "dotlessi",
  "e",
  "eacute",
  "ecaron",	/* EXT */
  "ecircumflex",
  "edieresis",
  "edotaccent",	/* EXT */
  "egrave",
  "eight",
  "ellipsis",
  "emacron",	/* EXT */
  "emdash",
  "endash",
  "eogonek",	/* EXT */
  "equal",
  "eth",
  "exclam",
  "exclamdown",
  "f",
  "fi",
  "five",
  "fl",
  "florin",
  "four",
  "fraction",
  "g",
  "gbreve",	/* EXT */
  "gcammaaccent",	/* EXT */
  "germandbls",
  "grave",
  "greater",
  "greaterequal",	/* EXT */
  "guillemotleft",  /* MISSPELED BY ADOBE */
  "guillemotright", /* MISSPELED BY ADOBE */
  "guilsinglleft",
  "guilsinglright",
  "h",
  "hungarumlaut",
  "hyphen",
  "i",
  "iacute",
  "icircumflex",
  "idieresis",
  "igrave",
  "imacron",	/* EXT */
  "iogonek",	/* EXT */
  "j",
  "k",
  "kcommaaccent",	/* EXT */
  "l",
  "lacute",	/* EXT */
  "lcaron",	/* EXT */
  "lcommaaccent",	/* EXT */
  "less",
  "lessequal",	/* EXT */
  "logicalnot",
  "lozenge",	/* EXT */
  "lslash",
  "m",
  "macron",
  "minus",
  "mu",
  "multiply",
  "n",
  "nacute",	/* EXT */
  "ncaron",	/* EXT */
  "ncommaaccent",	/* EXT */
  "nine",
  "notequal",	/* EXT */
  "ntilde",
  "numbersign",
  "o",
  "oacute",
  "ocircumflex",
  "odieresis",
  "oe",
  "ogonek",
  "ograve",
  "ohungarumlaut",	/* EXT */
  "omacron",	/* EXT */
  "one",
  "onehalf",
  "onequarter",
  "onesuperior",
  "ordfeminine",
  "ordmasculine",
  "oslash",
  "otilde",
  "p",
  "paragraph",
  "parenleft",
  "parenright",
  "partialdiff",	/* EXT */
  "percent",
  "period",
  "periodcentered",
  "perthousand",
  "plus",
  "plusminus",
  "q",
  "question",
  "questiondown",
  "quotedbl",
  "quotedblbase",
  "quotedblleft",
  "quotedblright",
  "quoteleft",
  "quoteright",
  "quotesinglbase",
  "quotesingle",
  "r",
  "racute",	/* EXT */
  "radical",	/* EXT */
  "rcaron",	/* EXT */
  "rcommaaccent",	/* EXT */
  "registered",
  "ring",
  "s",
  "scacute",	/* EXT */
  "scaron",
  "scedilla",	/* EXT */
  "scommaaccent",	/* EXT */
  "section",
  "semicolon",
  "seven",
  "six",
  "slash",
  "space",
  "sterling",
  "summation",	/* EXT */
  "t",
  "tcaron",	/* EXT */
  "tcommaaccent",	/* EXT */
  "thorn",
  "three",
  "threequarters",
  "threesuperior",
  "tilde",
  "trademark",
  "two",
  "twosuperior",
  "u",
  "uacute",
  "ucircumflex",
  "udieresis",
  "ugrave",
  "uhungarumlaut",	/* EXT */
  "umacron",	/* EXT */
  "underscore",
  "uogonek",	/* EXT */
  "uring",	/* EXT */
  "v",
  "w",
  "x",
  "y",
  "yacute",
  "ydieresis",
  "yen",
  "z",
  "zacute",	/* EXT */
  "zcaron",
  "zdotaccent",	/* EXT */
  "zero"
};
#endif /* _ENCODINGS_C_ */

#endif /* _ASL_CHARSET_H_ */
