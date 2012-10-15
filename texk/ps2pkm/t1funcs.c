/* $XConsortium: t1funcs.c,v 1.10 92/05/12 18:07:55 gildea Exp $ */
/* Copyright International Business Machines,Corp. 1991
 * All Rights Reserved
 *
 * License, subject to the license given below, to use,
 * copy, modify, and distribute this software * and its
 * documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear
 * in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation,
 * and that the name of IBM not be used in advertising or
 * publicity pertaining to distribution of the software
 * without specific, written prior permission.
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
 *
 * Author: Jeffrey B. Lotspiech, IBM Almaden Research Center
 *   Modeled on spfuncs.c by Dave Lemke, Network Computing Devices, Inc
 *   which contains the following copyright and permission notices:
 *
 * Copyright 1990, 1991 Network Computing Devices;
 * Portions Copyright 1987 by Digital Equipment Corporation and the
 * Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this protoype software
 * and its documentation to Members and Affiliates of the MIT X Consortium
 * any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Network Computing Devices, Digital or
 * MIT not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.
 *
 * NETWORK COMPUTING DEVICES, DIGITAL AND MIT DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL NETWORK COMPUTING DEVICES, DIGITAL OR MIT BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
 
#include    "types.h"
#include    <string.h>
#include    "ffilest.h"
#ifdef XSERVER
#include    <X11/Xfuncs.h>
#include    "FSproto.h"
#endif
#include    "t1intf.h"

#include "objects.h"
#include "spaces.h"
#include "regions.h"
#include "t1stdio.h"
#include "util.h"
#include "fontfcn.h"

typedef char *encoding[256];

int Type1OpenScalable(char **ev, struct _Font **ppFont, int flags,
		      struct _FontEntry *entry, char *fileName,
		      struct _FontScalable *vals, fsBitmapFormat format,
		      ULONG fmask, double efactor,double slant);
static int Type1GetGlyphs(struct _Font *pFont, ULONG count,
			  unsigned char *chars, FontEncoding charEncoding,
			  ULONG *glyphCount, struct _CharInfo **glyphs );
void Type1CloseFont(struct _Font *pFont);
extern int Type1GetInfoScalable(struct _FontPathElement *fpe,
				struct _FontInfo *pInfo,
				struct _FontEntry *entry,
				struct _FontName *fontName,
				char *fileName,
				struct _FontScalable *Vals);
static int Type1GetMetrics(struct _Font *pFont, ULONG count,
			   unsigned char *chars, FontEncoding charEncoding,
			   ULONG *glyphCount, xCharInfo **glyphs );
static void fillrun(register char *p,pel x0,pel x1,int bit);
extern char * Xalloc(int size);
extern void Xfree(void*);
static void fill(char *dest,int h,int w,struct region *area,
		 int byte,int bit,int wordsize);
extern Bool fontfcnA(char *env,int *mode);
extern struct region *fontfcnB(struct XYspace *S, unsigned char *code,
			       int *lenP, int *mode);
extern int FontDefaultFormat(int *bit, int *byte, 
			     int *glyphs, int *scan);
extern int CheckFSFormat(int format,int fmask,int *bit,int *byte,int *scan,int *glyph,int *image);
extern void QueryFontLib(char *env,char *infoName,void *infoValue,int *rcodeP);
extern void T1FillFontInfo(struct _Font *pFont,struct _FontScalable *Vals,char *Filename,char *Fontname);
extern void T1InitStdProps(void);
extern int FontFileRegisterRenderer(FontRendererRec *);

extern psfont *FontP;
extern psobj *ISOLatin1EncArrayP;
 
#define DECIPOINTSPERINCH 722.7

extern int currentchar; /* for error reporting */

/*ARGSUSED*/
int Type1OpenScalable (ev, ppFont, flags, entry, fileName, vals, format,
			fmask, efactor, slant)
    encoding		ev;
    FontPtr             *ppFont;
    int                 flags;
    FontEntryPtr        entry;
    char                *fileName;
    FontScalablePtr     vals;
    fsBitmapFormat      format;
    fsBitmapFormatMask  fmask;
    DOUBLE              efactor;
    DOUBLE              slant;
{
       extern struct XYspace *IDENTITY;
 
       FontPtr     pFont;
       int         bit,
                   byte,
                   glyph,
                   scan,
                   image;
       int pad,wordsize;     /* scan & image in bits                         */
       int size;             /* for memory size calculations                 */
       struct XYspace *S;    /* coordinate space for character               */
       struct region *area;
       DOUBLE scale;         /* scale factor for font                        */
       DOUBLE txx, tyx, txy, tyy;
       CharInfoRec *glyphs;
       register int i;
       int len,rc;
       struct type1font *type1;
       char *p;

       /* set up default values */
       FontDefaultFormat(&bit, &byte, &glyph, &scan);
       /* get any changes made from above */
       rc = CheckFSFormat(format, fmask, &bit, &byte, &scan, &glyph, &image);
       if (rc != Successful)
               return rc;
 
       pad                = glyph * 8;
       wordsize           = scan * 8;
 
#define  PAD(bits, pad)  (((bits)+(pad)-1)&-(pad))
 
       pFont = (FontPtr) Xalloc(sizeof(FontRec));
       if (pFont == NULL)
           return AllocError;
 
       type1 = (struct type1font *)Xalloc(sizeof(struct type1font));
       if (type1 == NULL) {
               Xfree(pFont);
               return AllocError;
       }
       (void) memset(type1, 0, sizeof(struct type1font));

       scale = (DOUBLE) vals->pixel;

      /* Code added by E. Schenk. July 25, 1992.
       * The following code reads the FontMatrix for the font,
       * and transforms the font using it. This is necessary
       * for fonts that are just an oblique, or extend of
       * a normal font. 
       */
       {  float fontmatrix[6];
	  int rc;
	  QueryFontLib(fileName,"FontMatrix",fontmatrix,&rc);
	  if (!rc) {
	     S = (struct XYspace *) Transform(IDENTITY,
		    fontmatrix[0], fontmatrix[1], fontmatrix[2], fontmatrix[3]);
	  }
	  else {
   	     S = (struct XYspace *) Scale(IDENTITY,0.001,0.001);
	  }
       }
       /* End of additional code */
       
       if (efactor == 1.0 && slant == 0.0)
	  S = (struct XYspace *)Permanent(Scale(S, scale, - scale));
       else {
          txx = (DOUBLE)vals->x * efactor * vals->point / DECIPOINTSPERINCH;
          tyy = (DOUBLE)vals->y * vals->point / DECIPOINTSPERINCH;
          tyx = 0.0;
          txy = (DOUBLE)vals->x * slant * vals->point / DECIPOINTSPERINCH;
          S = (struct XYspace *) Permanent(Transform(S, txx, tyx, txy, -tyy));
       }
 
       glyphs = type1->glyphs;
 
       /* load font if not already loaded */
       if (!fontfcnA(fileName, &rc))
	   return (rc);

       for (i=0; i < 256-FIRSTCOL; i++) {
               LONG h,w;
               LONG paddedW;

	       if (ev[i] == NULL) continue;
	       len = strlen(ev[i]);

               currentchar = i;

               rc = 0;
	       area = fontfcnB(S, (unsigned char *)ev[i], &len, &rc);
               if (rc < 0) {
                       rc = BadFontName;
                       break;
               }
               else if (rc > 0)
                       continue;
 
               if (area == NULL)
                       continue;
 
               h       = area->ymax - area->ymin;
               w       = area->xmax - area->xmin;
               paddedW = PAD(w, pad);
 
               if (h > 0 && w > 0) {
                       size = h * paddedW / 8;
                       glyphs[i].bits = (char *)Xalloc(size);
                       if (glyphs[i].bits == NULL) {
                               rc = AllocError;
                               break;
                       }
               }
               else {
                       h = w = 0;
                       area->xmin = area->xmax = 0;
                       area->ymax = area->ymax = 0;
               }
 
               glyphs[i].metrics.leftSideBearing  = area->xmin;
               glyphs[i].metrics.characterWidth   = NEARESTPEL(area->ending.x - area->origin.x);
               glyphs[i].metrics.rightSideBearing = w + area->xmin;
               glyphs[i].metrics.descent          = area->ymax - NEARESTPEL(area->origin.y);
               glyphs[i].metrics.ascent           = h - glyphs[i].metrics.descent;
 
               if (h > 0 && w > 0) {
                       (void) memset(glyphs[i].bits, 0, size);
                       fill(glyphs[i].bits, h, paddedW, area, byte, bit, wordsize );
               }
 
               Destroy(area);
       }

       if (i != 256 - FIRSTCOL) {
               for (i--; i >= 0; i--)
                       if (glyphs[i].bits != NULL)
                               Xfree(glyphs[i].bits);
               Xfree(type1);
               Xfree(pFont);
               return rc;
       }
       type1->pDefault    = NULL;
 
       pFont->format      = format;
 
       pFont->bit         = bit;
       pFont->byte        = byte;
       pFont->glyph       = glyph;
       pFont->scan        = scan;
 
       pFont->info.firstCol = FIRSTCOL;
       pFont->info.lastCol  = 255;
       pFont->info.firstRow = 0;
       pFont->info.lastRow  = 0;
 
       pFont->get_metrics = Type1GetMetrics;
       pFont->get_glyphs  = Type1GetGlyphs;
       pFont->unload_font = Type1CloseFont;
       pFont->refcnt = 0;
       pFont->maxPrivate = -1;
       pFont->devPrivates = 0;
 
       pFont->fontPrivate = (unsigned char *) type1;
 
       T1FillFontInfo(pFont, vals, fileName, entry->name.name);
 
       *ppFont = pFont;
       return Successful;
}

/* NOTE: ILH removed tests ...->characterWidth != 0 below because zero-width
         does not mean the character is empty, simply that it has zero escapement. */
 
static int
Type1GetGlyphs(pFont, count, chars, charEncoding, glyphCount, glyphs)
    FontPtr     pFont;
    ULONG count;
    register unsigned char *chars;
    FontEncoding charEncoding;
    ULONG *glyphCount;  /* RETURN */
    CharInfoPtr *glyphs;        /* RETURN */
{
    unsigned int firstRow;
    unsigned int numRows;
    CharInfoPtr *glyphsBase;
    register unsigned int c;
    register CharInfoPtr pci;
    unsigned int r;
    CharInfoPtr pDefault;
    register struct type1font *type1Font;
    register int firstCol;
 
    type1Font  = (struct type1font *) pFont->fontPrivate;
    firstCol   = pFont->info.firstCol;
    pDefault   = type1Font->pDefault;
    glyphsBase = glyphs;

    switch (charEncoding) {

    case Linear8Bit:
    case TwoD8Bit:
        if (pFont->info.firstRow > 0)
            break;
        while (count--) {
                c = (*chars++);
                if (c >= firstCol)
                    *glyphs++ = &type1Font->glyphs[c-firstCol];
                else if (pDefault)
                    *glyphs++ = pDefault;
        }
        break;
    case Linear16Bit:
        while (count--) {
                c = *chars++ << 8;
                c = (c | *chars++);
                if (c < 256 && c >= firstCol)
                    *glyphs++ = &type1Font->glyphs[c-firstCol];
                else if (pDefault)
                    *glyphs++ = pDefault;
        }
        break;
 
    case TwoD16Bit:
        firstRow = pFont->info.firstRow;
        numRows = pFont->info.lastRow - firstRow + 1;
        while (count--) {
            r = (*chars++) - firstRow;
            c = (*chars++);
            if (r < numRows && c < 256 && c >= firstCol)
                *glyphs++ = &type1Font->glyphs[(r << 8) + c - firstCol];
            else if (pDefault)
                *glyphs++ = pDefault;
        }
        break;
    }
    *glyphCount = glyphs - glyphsBase;
    return Successful;
}
 
static int
Type1GetMetrics(pFont, count, chars, charEncoding, glyphCount, glyphs)
    FontPtr     pFont;
    ULONG count;
    register unsigned char *chars;
    FontEncoding charEncoding;
    ULONG *glyphCount;  /* RETURN */
    xCharInfo **glyphs;         /* RETURN */
{
    static CharInfoRec nonExistantChar;
 
    int         ret;
    struct type1font *type1Font;
    CharInfoPtr oldDefault;
 
    type1Font = (struct type1font *) pFont->fontPrivate;
    oldDefault = type1Font->pDefault;
    type1Font->pDefault = &nonExistantChar;
    ret = Type1GetGlyphs(pFont, count, chars, charEncoding, glyphCount, (CharInfoPtr *) glyphs);
    type1Font->pDefault = oldDefault;
    return ret;
}
 
void Type1CloseFont(pFont)
       FontPtr pFont;
{
       register int i;
       struct type1font *type1;
 
       type1 = (struct type1font *) pFont->fontPrivate;
       for (i=0; i < 256 - pFont->info.firstCol; i++)
               if (type1->glyphs[i].bits != NULL)
                        Xfree(type1->glyphs[i].bits);
       Xfree(type1);

       if (pFont->info.props)
	   Xfree(pFont->info.props);

       if (pFont->info.isStringProp)
	   Xfree(pFont->info.isStringProp);

       Xfree(pFont);
}
 
 
 
static void fill(dest, h, w, area, Byte, bit, wordsize)
       register char *dest;  /* destination bitmap                           */
       int h,w;              /* dimensions of 'dest', w padded               */
       register struct region *area;  /* region to write to 'dest'           */
       int Byte,bit;         /* flags; LSBFirst or MSBFirst                  */
       int wordsize;         /* number of bits per word for LSB/MSB purposes */
{
       register struct edgelist *edge;  /* for looping through edges         */
       register char *p;     /* current scan line in 'dest'                  */
       register int y;       /* for looping through scans                    */
       register int wbytes = w / 8;  /* number of bytes in width             */
       register pel *leftP,*rightP;  /* pointers to X values, left and right */
       int xmin = area->xmin;  /* upper left X                               */
       int ymin = area->ymin;  /* upper left Y                               */
 
       for (edge = area->anchor; VALIDEDGE(edge); edge = edge->link->link) {
 
               p = dest + (edge->ymin - ymin) * wbytes;
               leftP = edge->xvalues;
               rightP = edge->link->xvalues;
 
               for (y = edge->ymin; y < edge->ymax; y++) {
                       fillrun(p, (pel)(*leftP++ - xmin), (pel)(*rightP++ - xmin), bit);
                       p += wbytes;
               }
       }
/*
Now, as an afterthought, we'll go reorganize if odd byte order requires
it:
*/
       if (Byte == LSBFirst && wordsize != 8) {
               register int i;
 
               switch (wordsize) {
                   case 16:
                   {
                       register unsigned short data,*p;
 
                       p = (unsigned short *) dest;
 
                       for (i = h * w /16; --i >= 0;) {
                               data = *p;
                               *p++ = (data << 8) + (data >> 8);
                       }
                       break;
                   }
                   case 64:
                   case 32:
                   {
                       register ULONG data,*p;
 
                       p = (ULONG *) dest;
 
                       for (i = h * w / 32; --i >= 0;) {
                               data = *p;
                               *p++ = (data << 24) + (data >> 24)
                                      + (0xFF00 & (data >> 8))
                                      + (0xFF0000 & (data << 8));
                       }
                       if (wordsize == 64) {
 
                               p = (ULONG *) dest;
 
                               for (i = h * w / 64; --i >= 0;) {
                                       data = *p++;
                                       p[-1] = p[0];
                                       *p++ = data;
                               }
                       }
                       break;
                   }
                   default:
                       t1_abort("xiFill: unknown format");
               }
       }
 
}
 
#define  ALLONES  0xFF
 
static void fillrun(register char *p, pel x0, pel x1, int bit)
{
       register int startmask,endmask;  /* bits to set in first and last char*/
       register int middle;  /* number of chars between start and end + 1    */
 
       if (x1 <= x0)
               return;
       middle = x1/8 - x0/8;
       p += x0/8;
       x0 &= 7;  x1 &= 7;
       if (bit == LSBFirst) {
               startmask = ALLONES << x0;
               endmask = ~(ALLONES << x1);
       }
       else {
               startmask = ALLONES >> x0;
               endmask = ~(ALLONES >> x1);
       }
       if (middle == 0)
               *p++ |= startmask & endmask;
       else {
               *p++ |= startmask;
               while (--middle > 0)
                       *p++ = ALLONES;
               *p |= endmask;
       }
}
 
 
static FontRendererRec renderers[] = {
  { ".pfa", 4, (int (*)()) 0, Type1OpenScalable,
        (int (*)()) 0, Type1GetInfoScalable, 0 },
  { ".pfb", 4, (int (*)()) 0, Type1OpenScalable,
        (int (*)()) 0, Type1GetInfoScalable, 0 }
};
 
Type1RegisterFontFileFunctions()
{
    int i;
 
    T1InitStdProps();
    for (i=0; i < sizeof(renderers) / sizeof(FontRendererRec); i++)
            FontFileRegisterRenderer(&renderers[i]);
}
