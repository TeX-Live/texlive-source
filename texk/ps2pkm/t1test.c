/* $XConsortium: t1test.c,v 1.2 91/10/10 11:19:53 rws Exp $ */
/* Copyright International Business Machines,Corp. 1991
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is
 * hereby granted, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation,
 * and that the name of IBM not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.
 *
 * IBM PROVIDES THIS SOFTWARE "AS IS", WITHOUT ANY WARRANTIES
 * OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT
 * LIMITED TO ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS.  THE ENTIRE RISK AS TO THE QUALITY AND
 * PERFORMANCE OF THE SOFTWARE, INCLUDING ANY DUTY TO SUPPORT
 * OR MAINTAIN, BELONGS TO THE LICENSEE.  SHOULD ANY PORTION OF
 * THE SOFTWARE PROVE DEFECTIVE, THE LICENSEE (NOT IBM) ASSUMES
 * THE ENTIRE COST OF ALL SERVICING, REPAIR AND CORRECTION.  IN
 * NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#include <stdlib.h>
#include "ffilest.h"
#ifdef XSERVER
#include "FSproto.h"
#endif
 
void Display();
 
#define DECIPOINTSPERINCH 722.7
#define DEFAULTRES 75
#define DEFAULTPOINTSIZE 120
 
FontScalableRec vals;
FontEntryRec entry;
 
typedef char *encoding[256];

/* PostScript ISOLatin1Encoding */

char *ISOLatin1[256] = {
	 /* '000  000  "00 */ NULL,
	 /* '001  001  "01 */ NULL,
	 /* '002  002  "02 */ NULL,
	 /* '003  003  "03 */ NULL,
	 /* '004  004  "04 */ NULL,
	 /* '005  005  "05 */ NULL,
	 /* '006  006  "06 */ NULL,
	 /* '007  007  "07 */ NULL,
	 /* '010  008  "08 */ NULL,
	 /* '011  009  "09 */ NULL,
	 /* '012  010  "0A */ NULL,
	 /* '013  011  "0B */ NULL,
	 /* '014  012  "0C */ NULL,
	 /* '015  013  "0D */ NULL,
	 /* '016  014  "0E */ NULL,
	 /* '017  015  "0F */ NULL,
	 /* '020  016  "10 */ NULL,
	 /* '021  017  "11 */ NULL,
	 /* '022  018  "12 */ NULL,
	 /* '023  019  "13 */ NULL,
	 /* '024  020  "14 */ NULL,
	 /* '025  021  "15 */ NULL,
	 /* '026  022  "16 */ NULL,
	 /* '027  023  "17 */ NULL,
	 /* '030  024  "18 */ NULL,
	 /* '031  025  "19 */ NULL,
	 /* '032  026  "1A */ NULL,
	 /* '033  027  "1B */ NULL,
	 /* '034  028  "1C */ NULL,
	 /* '035  029  "1D */ NULL,
	 /* '036  030  "1E */ NULL,
	 /* '037  031  "1F */ NULL,
	 /* '040  032  "20 */ "space",
	 /* '041  033  "21 */ "exclam",
	 /* '042  034  "22 */ "quotedbl",
	 /* '043  035  "23 */ "numbersign",
	 /* '044  036  "24 */ "dollar",
	 /* '045  037  "25 */ "percent",
	 /* '046  038  "26 */ "ampersand",
	 /* '047  039  "27 */ "quoteright",
	 /* '050  040  "28 */ "parenleft",
	 /* '051  041  "29 */ "parenright",
	 /* '052  042  "2A */ "asterisk",
	 /* '053  043  "2B */ "plus",
	 /* '054  044  "2C */ "comma",
	 /* '055  045  "2D */ "hyphen",
	 /* '056  046  "2E */ "period",
	 /* '057  047  "2F */ "slash",
	 /* '060  048  "30 */ "zero",
	 /* '061  049  "31 */ "one",
	 /* '062  050  "32 */ "two",
	 /* '063  051  "33 */ "three",
	 /* '064  052  "34 */ "four",
	 /* '065  053  "35 */ "five",
	 /* '066  054  "36 */ "six",
	 /* '067  055  "37 */ "seven",
	 /* '070  056  "38 */ "eight",
	 /* '071  057  "39 */ "nine",
	 /* '072  058  "3A */ "colon",
	 /* '073  059  "3B */ "semicolon",
	 /* '074  060  "3C */ "less",
	 /* '075  061  "3D */ "equal",
	 /* '076  062  "3E */ "greater",
	 /* '077  063  "3F */ "question",
	 /* '100  064  "40 */ "at",
	 /* '101  065  "41 */ "A",
	 /* '102  066  "42 */ "B",
	 /* '103  067  "43 */ "C",
	 /* '104  068  "44 */ "D",
	 /* '105  069  "45 */ "E",
	 /* '106  070  "46 */ "F",
	 /* '107  071  "47 */ "G",
	 /* '110  072  "48 */ "H",
	 /* '111  073  "49 */ "I",
	 /* '112  074  "4A */ "J",
	 /* '113  075  "4B */ "K",
	 /* '114  076  "4C */ "L",
	 /* '115  077  "4D */ "M",
	 /* '116  078  "4E */ "N",
	 /* '117  079  "4F */ "O",
	 /* '120  080  "50 */ "P",
	 /* '121  081  "51 */ "Q",
	 /* '122  082  "52 */ "R",
	 /* '123  083  "53 */ "S",
	 /* '124  084  "54 */ "T",
	 /* '125  085  "55 */ "U",
	 /* '126  086  "56 */ "V",
	 /* '127  087  "57 */ "W",
	 /* '130  088  "58 */ "X",
	 /* '131  089  "59 */ "Y",
	 /* '132  090  "5A */ "Z",
	 /* '133  091  "5B */ "bracketleft",
	 /* '134  092  "5C */ "backslash",
	 /* '135  093  "5D */ "bracketright",
	 /* '136  094  "5E */ "asciicircum",
	 /* '137  095  "5F */ "underscore",
	 /* '140  096  "60 */ "quoteleft",
	 /* '141  097  "61 */ "a",
	 /* '142  098  "62 */ "b",
	 /* '143  099  "63 */ "c",
	 /* '144  100  "64 */ "d",
	 /* '145  101  "65 */ "e",
	 /* '146  102  "66 */ "f",
	 /* '147  103  "67 */ "g",
	 /* '150  104  "68 */ "h",
	 /* '151  105  "69 */ "i",
	 /* '152  106  "6A */ "j",
	 /* '153  107  "6B */ "k",
	 /* '154  108  "6C */ "l",
	 /* '155  109  "6D */ "m",
	 /* '156  110  "6E */ "n",
	 /* '157  111  "6F */ "o",
	 /* '160  112  "70 */ "p",
	 /* '161  113  "71 */ "q",
	 /* '162  114  "72 */ "r",
	 /* '163  115  "73 */ "s",
	 /* '164  116  "74 */ "t",
	 /* '165  117  "75 */ "u",
	 /* '166  118  "76 */ "v",
	 /* '167  119  "77 */ "w",
	 /* '170  120  "78 */ "x",
	 /* '171  121  "79 */ "y",
	 /* '172  122  "7A */ "z",
	 /* '173  123  "7B */ "braceleft",
	 /* '174  124  "7C */ "bar",
	 /* '175  125  "7D */ "braceright",
	 /* '176  126  "7E */ "asciitilde",
	 /* '177  127  "7F */ NULL,
	 /* '200  128  "80 */ NULL,
	 /* '201  129  "81 */ NULL,
	 /* '202  130  "82 */ NULL,
	 /* '203  131  "83 */ NULL,
	 /* '204  132  "84 */ NULL,
	 /* '205  133  "85 */ NULL,
	 /* '206  134  "86 */ NULL,
	 /* '207  135  "87 */ NULL,
	 /* '210  136  "88 */ NULL,
	 /* '211  137  "89 */ NULL,
	 /* '212  138  "8A */ NULL,
	 /* '213  139  "8B */ NULL,
	 /* '214  140  "8C */ NULL,
	 /* '215  141  "8D */ NULL,
	 /* '216  142  "8E */ NULL,
	 /* '217  143  "8F */ NULL,
	 /* '220  144  "90 */ "dotlessi",
	 /* '221  145  "91 */ "grave",
	 /* '222  146  "92 */ "acute",
	 /* '223  147  "93 */ "circumflex",
	 /* '224  148  "94 */ "tilde",
	 /* '225  149  "95 */ "macron",
	 /* '226  150  "96 */ "breve",
	 /* '227  151  "97 */ "dotaccent",
	 /* '230  152  "98 */ "dieresis",
	 /* '231  153  "99 */ NULL,
	 /* '232  154  "9A */ "ring",
	 /* '233  155  "9B */ "cedilla",
	 /* '234  156  "9C */ NULL,
	 /* '235  157  "9D */ "hungarumlaut",
	 /* '236  158  "9E */ "ogonek",
	 /* '237  159  "9F */ "caron",
	 /* '240  160  "A0 */ "space",
	 /* '241  161  "A1 */ "exclamdown",
	 /* '242  162  "A2 */ "cent",
	 /* '243  163  "A3 */ "sterling",
	 /* '244  164  "A4 */ "currency",
	 /* '245  165  "A5 */ "yen",
	 /* '246  166  "A6 */ "brokenbar",
	 /* '247  167  "A7 */ "section",
	 /* '250  168  "A8 */ "dieresis",
	 /* '251  169  "A9 */ "copyright",
	 /* '252  170  "AA */ "ordfeminine",
	 /* '253  171  "AB */ "guillemotleft",
	 /* '254  172  "AC */ "logicalnot",
	 /* '255  173  "AD */ "hyphen",
	 /* '256  174  "AE */ "registered",
	 /* '257  175  "AF */ "macron",
	 /* '260  176  "B0 */ "degree",
	 /* '261  177  "B1 */ "plusminus",
	 /* '262  178  "B2 */ "twosuperior",
	 /* '263  179  "B3 */ "threesuperior",
	 /* '264  180  "B4 */ "acute",
	 /* '265  181  "B5 */ "mu",
	 /* '266  182  "B6 */ "paragraph",
	 /* '267  183  "B7 */ "periodcentered",
	 /* '270  184  "B8 */ "cedilla",
	 /* '271  185  "B9 */ "onesuperior",
	 /* '272  186  "BA */ "ordmasculine",
	 /* '273  187  "BB */ "guillemotright",
	 /* '274  188  "BC */ "onequarter",
	 /* '275  189  "BD */ "onehalf",
	 /* '276  190  "BE */ "threequarters",
	 /* '277  191  "BF */ "questiondown",
	 /* '300  192  "C0 */ "Agrave",
	 /* '301  193  "C1 */ "Aacute",
	 /* '302  194  "C2 */ "Acircumflex",
	 /* '303  195  "C3 */ "Atilde",
	 /* '304  196  "C4 */ "Adieresis",
	 /* '305  197  "C5 */ "Aring",
	 /* '306  198  "C6 */ "AE",
	 /* '307  199  "C7 */ "Ccedilla",
	 /* '310  200  "C8 */ "Egrave",
	 /* '311  201  "C9 */ "Eacute",
	 /* '312  202  "CA */ "Ecircumflex",
	 /* '313  203  "CB */ "Edieresis",
	 /* '314  204  "CC */ "Igrave",
	 /* '315  205  "CD */ "Iacute",
	 /* '316  206  "CE */ "Icircumflex",
	 /* '317  207  "CF */ "Idieresis",
	 /* '320  208  "D0 */ "Eth",
	 /* '321  209  "D1 */ "Ntilde",
	 /* '322  210  "D2 */ "Ograve",
	 /* '323  211  "D3 */ "Oacute",
	 /* '324  212  "D4 */ "Ocircumflex",
	 /* '325  213  "D5 */ "Otilde",
	 /* '326  214  "D6 */ "Odieresis",
	 /* '327  215  "D7 */ "multiply",
	 /* '330  216  "D8 */ "Oslash",
	 /* '331  217  "D9 */ "Ugrave",
	 /* '332  218  "DA */ "Uacute",
	 /* '333  219  "DB */ "Ucircumflex",
	 /* '334  220  "DC */ "Udieresis",
	 /* '335  221  "DD */ "Yacute",
	 /* '336  222  "DE */ "Thorn",
	 /* '337  223  "DF */ "Germandbls",
	 /* '340  224  "E0 */ "agrave",
	 /* '341  225  "E1 */ "aacute",
	 /* '342  226  "E2 */ "acircumflex",
	 /* '343  227  "E3 */ "atilde",
	 /* '344  228  "E4 */ "adieresis",
	 /* '345  229  "E5 */ "aring",
	 /* '346  230  "E6 */ "ae",
	 /* '347  231  "E7 */ "ccedilla",
	 /* '350  232  "E8 */ "egrave",
	 /* '351  233  "E9 */ "eacute",
	 /* '352  234  "EA */ "ecircumflex",
	 /* '353  235  "EB */ "edieresis",
	 /* '354  236  "EC */ "igrave",
	 /* '355  237  "ED */ "iacute",
	 /* '356  238  "EE */ "icircumflex",
	 /* '357  239  "EF */ "idieresis",
	 /* '360  240  "F0 */ "eth",
	 /* '361  241  "F1 */ "ntilde",
	 /* '362  242  "F2 */ "ograve",
	 /* '363  243  "F3 */ "oacute",
	 /* '364  244  "F4 */ "ocircumflex",
	 /* '365  245  "F5 */ "otilde",
	 /* '366  246  "F6 */ "odieresis",
	 /* '367  247  "F7 */ "divide",
	 /* '370  248  "F8 */ "oslash",
	 /* '371  249  "F9 */ "ugrave",
	 /* '372  250  "FA */ "uacute",
	 /* '373  251  "FB */ "ucircumflex",
	 /* '374  252  "FC */ "udieresis",
	 /* '375  253  "FD */ "yacute",
	 /* '376  254  "FE */ "thorn",
	 /* '377  255  "FF */ "ydieresis"};

int main(argc, argv)
       int argc;
       char *argv[];
{
       int h;
       char temp[80];
       char file[80];
       char glyphcode[1];
       FontPtr fontptr;
       CharInfoRec *glyphs[1];
       int count;
       int code;
       int rc = -1;
       float efactor = 1.0, slant = 0.0;
       encoding *ev = &ISOLatin1;
 
       T1FillVals(&vals);
       Type1RegisterFontFileFunctions();
       entry.name.name = "-adobe-utopia-medium-r-normal--0-0-0-0-p-0-iso8859-1";
 
       for (;;) {
               printf("T1TEST: ");
               gets(temp);
               glyphcode[0] = '\0';
 
               switch(temp[0]) {
 
                   case 'E':
                       if (1 != sscanf(&temp[2], "%f", &efactor))
                               printf("expansion factor?\n");
                       break;
 
                   case 'S':
                       if (1 != sscanf(&temp[2], "%f", &slant))
                               printf("slant?\n");
                       break;
 
                   case 'c':
                       if (1 != sscanf(&temp[2], "%c", glyphcode))
                               printf("glyph code?\n");
                       break;
 
                   case 'x':
                       if (1 != sscanf(&temp[2], "%x", &code))
                               printf("glyph code?\n");
                       else
                               glyphcode[0] = code;
                       break;
 
                   case 'd':
                       if (1 != sscanf(&temp[2], "%d", &code))
                               printf("glyph code?\n");
                       else
                               glyphcode[0] = code;
                       break;
 
                   case 'h':
                       if (1 != sscanf(&temp[2], "%d", &h))
                               printf("height?\n");
                       vals.pixel = h;
		       vals.point = (vals.pixel * DECIPOINTSPERINCH) / vals.y;
                       rc = Type1OpenScalable(*ev, &fontptr, 0, &entry,
			       file, &vals, 0, 0, efactor, slant);
                       break;
 
                   case 'f':
                       if (1 != sscanf(&temp[2], "%s", file))
                               printf("file name?\n");
                       rc = Type1OpenScalable(*ev, &fontptr, 0, &entry,
			       file, &vals, 0, 0, efactor, slant);
                       break;
 
                   case 't':
                       if (1 != sscanf(&temp[2], "%s", file))
                               printf("file name?\n");
                       vals.pixel = 8;
                       rc = Type1OpenScalable(*ev, &fontptr, 0, &entry,
			       file, &vals, 0, 0, efactor, slant);
                       if (rc != Successful) break;
                       vals.pixel = 20;
                       rc = Type1OpenScalable(*ev, &fontptr, 0, &entry,
			       file, &vals, 0, 0, efactor, slant);
                       if (rc != Successful) break;
                       vals.pixel = 50;
                       rc = Type1OpenScalable(*ev, &fontptr, 0, &entry,
			       file, &vals, 0, 0, efactor, slant);
                       glyphcode[0] = 'A';
                       printf("From font '%s':\n", file);
                       break;
 
                   case 'q':
                       return 0;
 
                   default:
                       printf("unknown command '%c', must one of 'ESqfchdxt'\n", temp[0]);
 
               }
               if (rc == Successful) {
                      if (glyphcode[0] != '\0') {
                              (*fontptr->get_glyphs)(fontptr, 1, glyphcode, 0, &count, glyphs);
                              if (count > 0)
                                      Display(glyphs[0]);
                              else
                                      printf("Code %x not valid in this font\n", glyphcode[0]);
                      }
               }
               else
                      printf("Bad font (rc = %d, file='%s')\n", rc, file);
       }
}
 
void Display(glyph)
       CharInfoRec *glyph;
{
       int h,w;
       unsigned char *p;
       int data;
       int i;
 
       p = (unsigned char *) glyph->bits;
 
       printf("Metrics: left=%d, right=%d, w=%d, above=%d, below=%d\n",
               glyph->metrics.leftSideBearing,
               glyph->metrics.rightSideBearing,
               glyph->metrics.characterWidth,
               glyph->metrics.ascent,
               glyph->metrics.descent);
 
       for (h=glyph->metrics.ascent + glyph->metrics.descent; --h >= 0;) {
               w = glyph->metrics.rightSideBearing - glyph->metrics.leftSideBearing;
               while (w > 0) {
                       data = *p++;
                       for (i=0; i<8; i++) {
                               if (--w < 0)
                                       break;
                               if (data & 0x80)
                                       printf("X");
                               else
                                       printf(".");
                               data <<= 1;
                       }
               }
               printf("\n");
       }
}
 
T1FillVals(vals)
    FontScalablePtr vals;
{
    fsResolution *res;
    int         x_res = DEFAULTRES;
    int         y_res = DEFAULTRES;
    int         pointsize = DEFAULTPOINTSIZE;  /* decipoints */
    int         num_res;
 
    /* Must have x, y, and pixel */
    if (!vals->x || !vals->y || !vals->pixel) {
        res = (fsResolution *) GetClientResolutions(&num_res);
        if (num_res) {
            if (res->x_resolution)
                x_res = res->x_resolution;
            if (res->y_resolution)
                y_res = res->y_resolution;
            if (res->point_size)
                pointsize = res->point_size;
        }
        if (!vals->x)
            vals->x = x_res;
        if (!vals->y)
            vals->y = y_res;
        if (!vals->point) {
            if (!vals->pixel) vals->point = pointsize;
            else vals->point = (vals->pixel * DECIPOINTSPERINCH) / vals->y;
        }
        if (!vals->pixel)
            vals->pixel = (vals->point * vals->y) / DECIPOINTSPERINCH;
        /* Make sure above arithmetic is normally in range and will
           round properly. +++ */
    }
}
 
int CheckFSFormat(format, fmask, bit, byte, scan, glyph, image)
       int format,fmask,*bit,*byte,*scan,*glyph,*image;
{
       *bit = *byte = 1;
       *glyph = *scan = *image = 1;
       return Successful;
 
}
 
char *MakeAtom(p)
       char *p;
{
       return p;
}
GetClientResolutions(resP)
       int *resP;
{
       *resP = 0;
};

char *Xalloc(size)
       int size;
{
       extern char *malloc();
       return(malloc(size));
}
 
void Xfree()
{
       free();
}

FontDefaultFormat() { ; }
 
FontFileRegisterRenderer() { ; }
 
GenericGetBitmaps() { ; }
GenericGetExtents() { ; }
 
FontParseXLFDName() { ; }
FontComputeInfoAccelerators() { ; }
