/*
 *   emspecialex.c
 *   This routine handles the emTeX special commands.
 */
#include "xdvips.h" /* The copyright notice in that file is included too!*/

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
/*
 *   The external declarations:
 */
#include "protos.h"
#include "bitmap.h"

#if defined(WIN32)
#include "wingdi.h"
#endif

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define PALVERSION   0x300
#define tobyte(x) ((x/8) + (x%8 ? 1 : 0))

extern Boolean RESIZE_MODE;
extern char RESIZE_FILTER_BW;
extern char RESIZE_FILTER_Gray;
extern char RESIZE_FILTER_RGB;
extern char RESIZE_FILTER_CMYK;
extern int GDI_STRETCH_MODE_BW;
extern int GDI_STRETCH_MODE_Gray;
extern int GDI_STRETCH_MODE_RGB;
extern Boolean EMSEARCH_MODE;

#ifdef EMTEX
/* emtex specials, added by rjl */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

static long emmax = 161;

/*
 *   We define these seek constants if they don't have their
 *   values already defined.
 */
#ifndef SEEK_SET
#define SEEK_SET (0)
#endif
#ifndef SEEK_END
#define SEEK_END (2)
#endif

struct empt {
   struct empt *next;
   shalfword point;
   integer x, y;
};

static struct empt **empoints = NULL;
boolean emused = FALSE;  /* true if em points used on this page */
integer emx, emy;

struct emunit {
   const char *unit;
   float factor;
};
struct emunit emtable[] = {
  {"pt",72.27F},
  {"pc",72.27F/12},
  {"in",1.0F},
  {"bp",72.0F},
  {"cm",2.54F},
  {"mm",25.4F},
  {"dd",72.27F/(1238.0F/1157)},
  {"cc",72.27F/12/(1238.0F/1157)},
  {"sp",72.27F*65536},
  {0,0.0F}
};

const char *extarr[] =
{ ".pcx", ".bmp", ".tif", ".jpg", ".png", NULL };

static void emgraph(char *filename, float emwidth, float emheight);

/* clear the empoints array if necessary */
void
emclear(void)
{
   int i;
   if (emused && empoints)
      for (i=0; i<emmax; i++)
         empoints[i] = 0;
   emused = FALSE;
}

/* put an empoint into the empoints array */
static struct empt *
emptput(shalfword point, integer x, integer y)
{
   struct empt *p;
   int start;

   emused = TRUE;
   start = point % emmax;
   p = empoints[start];
   while ( p ) {
      if ( p->point == point )
         break;
      p = p->next;
   }
   if (p == 0) {
      p = (struct empt *)mymalloc(sizeof(struct empt));
      p->next = empoints[start];
      empoints[start] = p;
   }
   p->point = point;
   p->x = x;
   p->y = y;
   return(p);
}

/* get an empoint from the empoints array */
static struct empt *
emptget(shalfword point)
{
   struct empt *p;
   int start;

   start = point % emmax;
   if (emused == TRUE) {
      p = empoints[start];
      while (p) {
	 if (p->point == point)
	    return p;
	 p = p->next;
      }
   }
   sprintf(errbuf,"!em: point %d not defined",point);
   specerror(errbuf);
   return(NULL); /* never returns due to error */
}


/* convert width into dpi units */
static float
emunits(float width, char *unit)
{
struct emunit *p;
	for (p=emtable; p->unit; p++) {
	   if (strcmp(p->unit,unit)==0)
		return( width * actualdpi / p->factor );
	}
	return (-1.0); /* invalid unit */
}

/* The main routine for \special{em:graph ...} called from dospecial.c */
/* the line cut parameter is not supported (and is ignored) */

void
emspecial(char *p)
{
float emwidth, emheight;
shalfword empoint1, empoint2;
struct empt *empoint;
char emunit[30];
char emstr[500];
char *emp;

        hvpos();
	for (emp = p+3; *emp && isspace((unsigned char)*emp); emp++); /* skip blanks */
	if (strncmp(emp, "linewidth", 9) == 0) {
	   /* code for linewidth */
	   for (emp = emp+9; *emp && isspace((unsigned char)*emp); emp++); /* skip blanks */
	   sscanf(emp, "%f%2s", &emwidth, emunit);
	   emwidth = emunits(emwidth,emunit);
	   if (emwidth!=-1.0) {
	      snprintf(emstr, sizeof(emstr), "%.1f setlinewidth", emwidth);
	      cmdout(emstr);
#ifdef DEBUG
   if (dd(D_SPECIAL))
      fprintf(stderr, "em special: Linewidth set to %.1f dots\n",
		emwidth);
#endif
	   } else {
	      sprintf(errbuf,"Unknown em: special width");
	      specerror(errbuf);
	   }
	}
        else if (strncmp(emp, "moveto", 6) == 0) {
#ifdef DEBUG
   if (dd(D_SPECIAL))
#ifdef SHORTINT
      fprintf(stderr, "em special: moveto %ld,%ld\n", hh, vv);
#else
      fprintf(stderr, "em special: moveto %d,%d\n", hh, vv);
#endif
#endif
           emx = hh;
           emy = vv;
        }
        else if (strncmp(emp, "lineto", 6) == 0) {
#ifdef DEBUG
   if (dd(D_SPECIAL))
#ifdef SHORTINT
      fprintf(stderr, "em special: lineto %ld,%ld\n", hh, vv);
#else
      fprintf(stderr, "em special: lineto %d,%d\n", hh, vv);
#endif
#endif
	   cmdout("np");
	   numout(emx);
	   numout(emy);
	   cmdout("a");
	   numout(hh);
	   numout(vv);
	   cmdout("li");
	   cmdout("st");
           emx = hh;
           emy = vv;
        }
	else if (strncmp(emp, "point", 5) == 0) {
           if (empoints == NULL) {
              empoints =
              (struct empt **)mymalloc((integer)emmax * sizeof(struct empt *));
              emused = TRUE;
              emclear();
           }
	   for (emp = emp+5; *emp && isspace((unsigned char)*emp); emp++) /* skip blanks */
           ;
           empoint1 = (shalfword)atoi(emp);
           empoint = emptput(empoint1,hh,vv);
#ifdef DEBUG
   if (dd(D_SPECIAL))
#ifdef SHORTINT
      fprintf(stderr, "em special: Point %d is %ld,%ld\n",
#else
      fprintf(stderr, "em special: Point %d is %d,%d\n",
#endif
		empoint->point, empoint->x, empoint->y);
#endif
	}
	else if (strncmp(emp, "line", 4) == 0) {
	   for (emp = emp+4; *emp && isspace((unsigned char)*emp); emp++) /* skip blanks */
           ;
           empoint1 = (shalfword)atoi(emp);
	   for (; *emp && isdigit((unsigned char)*emp); emp++) /* skip point 1 */
           ;
	   if ( *emp && strchr("hvp",*emp)!=0 )
	      emp++;  /* skip line cut */
	   for (; *emp && isspace((unsigned char)*emp); emp++) /* skip blanks */
           ;
	   if ( *emp && (*emp==',') )
	      emp++; /*  skip comma separator */
	   for (; *emp && isspace((unsigned char)*emp); emp++) /* skip blanks */
           ;
           empoint2 = (shalfword)atoi(emp);
	   for (; *emp && isdigit((unsigned char)*emp); emp++) /* skip point 2 */
           ;
	   if ( *emp && strchr("hvp",*emp)!=0 )
	      emp++;  /* skip line cut */
	   for (; *emp && isspace((unsigned char)*emp); emp++) /* skip blanks */
           ;
	   if ( *emp && (*emp==',') )
	      emp++; /*  skip comma separator */
	   emwidth = -1.0;
	   emunit[0]='\0';
	   sscanf(emp, "%f%2s", &emwidth, emunit);
	   emwidth = emunits(emwidth,emunit);
#ifdef DEBUG
   if (dd(D_SPECIAL))
      fprintf(stderr, "em special: Line from point %d to point %d\n",
		empoint1, empoint2);
#endif
	   cmdout("np");
	   if (emwidth!=-1.0) {
#ifdef DEBUG
   if (dd(D_SPECIAL))
   fprintf(stderr,"em special: Linewidth temporarily set to %.1f dots\n",
		emwidth);
#endif
	   	strcpy(emstr,"currentlinewidth");
	   	cmdout(emstr);
	        snprintf(emstr, sizeof(emstr), "%.1f setlinewidth", emwidth);
	        cmdout(emstr);
	   }
           empoint = emptget(empoint1);
	   numout(empoint->x);
	   numout(empoint->y);
	   cmdout("a");
           empoint = emptget(empoint2);
	   numout(empoint->x);
	   numout(empoint->y);
	   cmdout("li");
	   cmdout("st");
	   if (emwidth!=-1.0) {
	   	strcpy(emstr,"setlinewidth");
	   	cmdout(emstr);
	   }
	}
	else if (strncmp(emp, "message", 7) == 0) {
           fprintf_str(stderr, "em message: %s\n", emp+7);
	}
	else if (strncmp(emp, "graph", 5) == 0) {
	   int i;
	   for (emp = emp+5; *emp && isspace((unsigned char)*emp); emp++); /* skip blanks */
	   memset(emstr, 0, sizeof(emstr));
	   for (i = 0; *emp && !isspace((unsigned char)*emp) && !(*emp == ','); emp++) {
	      if ((int)strlen(emstr) - 2 >= (int)sizeof(emstr)) {
                specerror("em:graph: special too long, truncating");
                break;
	      }
	      emstr[i++] = *emp; /* copy filename */
	   }
	   emstr[i] = '\0';
	   /* now get optional width and height */
	   emwidth = emheight = -1.0;	/* no dimension is <= 0 */
	   for (; *emp && ( isspace((unsigned char)*emp) || (*emp==',') ); emp++)
	    ;  /* skip blanks and comma */
	   if (*emp) {
	      sscanf(emp, "%f%2s", &emwidth, emunit); /* read width */
	      emwidth = emunits(emwidth,emunit); /* convert to pixels */
	      for (; *emp && (*emp=='.'||isdigit((unsigned char)*emp)||isalpha((unsigned char)*emp)); emp++)
	       ; /* skip width dimension */
	      for (; *emp && ( isspace((unsigned char)*emp) || (*emp==',') ); emp++)
	       ;  /* skip blanks and comma */
	      if (*emp) {
	         sscanf(emp, "%f%2s", &emheight, emunit); /* read height */
	         emheight = emunits(emheight,emunit)*vactualdpi/actualdpi;
	      }
	   }
	   if (emstr[0]) {
	      emgraph(emstr,emwidth,emheight);
	   }
	   else {
              specerror("em:graph: no file given");
	   }
	}
	else {
           sprintf(errbuf,
	      "Unknown em: command (%s) in \\special will be ignored", p);
           specerror(errbuf);
	}
	return;
   }


/* em:graph routines */

/* The graphics routines currently decode 5 types of widely used graphics    */
/* files: .pcx, .bmp, .png, .jpg, .tif.                                      */

void
imagehead(char *filename, int wide, int high, int dpi,
	  float emwidth, float emheight)
{
	char *fullname = NULL, *name;
	if (!quiet) {
#ifdef KPATHSEA
	    fullname = (char *)kpse_find_file (filename, pictpath, 0);
#endif
	    if (!fullname)
		name = filename;
	    else
		name = fullname;
	    if (strlen(name) + prettycolumn > STDOUTSIZE) {
		fprintf(stderr,"\n");
		prettycolumn = 0;
	    }
	    (void)fprintf_str(stderr,"<%s",name);
	    (void)fflush(stderr);
	    prettycolumn += 2+strlen(name);
	    if (fullname) free (fullname);
	}
	hvpos();
	nlcmdout("@beginspecial @setspecial");
	if (!disablecomments) {
		cmdout("%%BeginDocument: em:graph");
		cmdout(filename);
		newline();
	}
	cmdout("gsave");
	newline();
	/* set the image size */
	if (emwidth <= 0.0)  emwidth = (float)wide / dpi * actualdpi;
	if (emheight <= 0.0)  emheight = (float)high / dpi * vactualdpi;
	floatout(emwidth*72/actualdpi);
	floatout(emheight*72/vactualdpi);
	newline();
	cmdout("scale");
#ifdef DEBUG
	if (dd(D_SPECIAL)) {
	  (void)fprintf(stderr, 
	    "\nem:graph: %s width  %d pixels scaled to %.1f pixels\n",
	    filename, wide, emwidth);
	  (void)fprintf_str(stderr, 
	    "em:graph: %s height %d pixels scaled to %.1f pixels\n",
	    filename, high, emheight);
   	}
#endif
}

void
imagetail(void)
{
	nlcmdout("grestore") ;
	if (!disablecomments) {
	    (void)fprintf(bitfile, "\n%%%%EndDocument\n") ;
	    linepos = 0;
	}
	nlcmdout("@endspecial") ;
	if (!quiet) {
	    (void)fprintf(stderr,">");
	    (void)fflush(stderr);
	}
}

#if defined(WIN32)
HPALETTE FIDIBLogPalette(HDC hDC, FIBITMAP *FIDib)
{
    LPSTR       lpDIBHdr;				// pointer to DIB header
    HPALETTE    hPal;					// handle to a palette
    LPLOGPALETTE        lpPal;          // pointer to a logical palette
	int			wNumColors, i;
	LPBITMAPINFO lpbmi;

    // if invalid handle, return NULL 

    if (!FIDib)
        return NULL;

    // get a pointers to the DIB header

	lpDIBHdr = (LPSTR)Bitmap_GetInfoHeader(FIDib);

    // select and realize palette

	hPal = NULL;
	switch (Bitmap_GetBPP(FIDib)) {
	case 8:
		wNumColors = 256;
		break;
	case 1:
		wNumColors = 2;
		break;
	default:
		wNumColors = 0;
	}
    if (wNumColors)
    {
        lpPal = (LPLOGPALETTE)mymalloc(sizeof(LOGPALETTE) +
                sizeof(PALETTEENTRY) * wNumColors);
        if ( lpPal == NULL )
	        return NULL;

        lpPal->palVersion = PALVERSION;
        lpPal->palNumEntries = (WORD) wNumColors;
        
        lpbmi = (LPBITMAPINFO)lpDIBHdr;
		for (i = 0; i < wNumColors; i++)
        {
            lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
            lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
            lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
            lpPal->palPalEntry[i].peFlags = 0;
        }
    }
	else {
	    wNumColors = (1 << (GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES)));
		if ( wNumColors > 256 )
			wNumColors = 256;

        lpPal = (LPLOGPALETTE)mymalloc(sizeof(LOGPALETTE) +
                sizeof(PALETTEENTRY) * wNumColors);
        if ( lpPal == NULL )
			return NULL;

		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD) wNumColors;

	    // Copy the current system palette into our logical palette

		GetSystemPaletteEntries(hDC, 0, wNumColors,
			    (LPPALETTEENTRY)(lpPal->palPalEntry));
	}

    // create the palette and get handle to it
	hPal = CreatePalette(lpPal);
	free(lpPal);
	if ( ! hPal ) {
        return NULL;
	}

	return hPal;
}

HBITMAP copyToBitmap(HDC hDC, FIBITMAP *FIDib, Boolean initBitmap, void **DibBits)
{
    LPBITMAPINFO lpDIBHdr;			// pointer to DIB header
    HBITMAP     hBitmap;            // handle to device-dependent bitmap
	DWORD k;
	BYTE *source, *dest;

    // if invalid handle, return NULL 

    if (!FIDib)
        return NULL;

    // get a pointers to the DIB header

	lpDIBHdr = (LPBITMAPINFO)Bitmap_GetInfoHeader(FIDib);
	lpDIBHdr->bmiHeader.biSizeImage = Bitmap_GetPitch(FIDib) * Bitmap_GetHeight(FIDib);

    // create bitmap from DIB info. and bits
	if ( lpDIBHdr->bmiHeader.biBitCount > 1 ) {
		hBitmap = CreateDIBSection( hDC, lpDIBHdr, DIB_RGB_COLORS, DibBits, NULL, 0 );
		if ( hBitmap ) {
			if ( initBitmap ) {
				for (k = 0; k<Bitmap_GetHeight(FIDib); k++) {
					source = Bitmap_GetScanLine(FIDib, k);
					dest = (BYTE *)*DibBits + k * WIDTHBYTES(Bitmap_GetWidth(FIDib) * Bitmap_GetBPP(FIDib));
					memcpy(dest, source, Bitmap_GetPitch(FIDib));
				}
			}
		}
	}
	else {
		if ( initBitmap )
		    hBitmap = CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)lpDIBHdr, CBM_INIT,
				Bitmap_GetBits(FIDib), (LPBITMAPINFO)lpDIBHdr, DIB_RGB_COLORS);
		else
		    hBitmap = CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)lpDIBHdr, 0,
		        NULL, (LPBITMAPINFO)lpDIBHdr, DIB_RGB_COLORS);
	}

    // return handle to the bitmap
    return hBitmap;
}

FIBITMAP *copyFromBitmap(HDC dc, HBITMAP hbmp) {
	if(hbmp) { 
		int Success;
        BITMAP bm;
		int nColors;
		FIBITMAP *_dib = NULL;

		// Get informations about the bitmap
        GetObject(hbmp, sizeof(BITMAP), (LPSTR) &bm);

		// Allocate a new FIBITMAP
		if ((_dib = Bitmap_AllocateT(FIT_BITMAP, (WORD)bm.bmWidth, (WORD)bm.bmHeight,
					(WORD)bm.bmBitsPixel,0,0,0)) == NULL) {
			return NULL;
		}

		// The GetDIBits function clears the biClrUsed and biClrImportant BITMAPINFO members (dont't know why) 
		// So we save these infos below. This is needed for palettized images only. 
		nColors = Bitmap_GetColorsUsed(_dib);

		// Copy the pixels
		Success = GetDIBits(dc,								// handle to DC
								hbmp,						// handle to bitmap
								0,							// first scan line to set
								Bitmap_GetHeight(_dib),	// number of scan lines to copy
								Bitmap_GetBits(_dib),	// array for bitmap bits
								Bitmap_GetInfo(_dib),	// bitmap data buffer
								DIB_RGB_COLORS				// RGB 
								);
		if(Success == 0) {
			ReleaseDC(NULL, dc);
			return NULL;
        }
        ReleaseDC(NULL, dc);

		// restore BITMAPINFO members
		
		Bitmap_GetInfoHeader(_dib)->biClrUsed = nColors;
		Bitmap_GetInfoHeader(_dib)->biClrImportant = nColors;

		return _dib;
    }

	return NULL;
}

FIBITMAP *ResizeBitmapWnd(char *filename, int mode, FIBITMAP *bitmap, int destWidth, int destHeight)
{
	HDC hSourceDC, hDestDC;
    HPALETTE    hPal, hSourcePal, hDestPal;
	HBITMAP hSourceBitmap, hDestBitmap;
	HGDIOBJ objSrc, objDest;
	BYTE *SrcDibBits, *DestDibBits;
	int oldmode, i;
	FIBITMAP *dib;
	RGBQUAD *palette;

	hSourceDC = CreateCompatibleDC(GetDC(NULL));
	hPal = FIDIBLogPalette(hSourceDC,bitmap);
	if ( hPal == NULL ) {
		DeleteDC(hSourceDC);
		sprintf(errbuf, "em:graph: Unable to create image logical palette\n");
		specerror(errbuf);
		return NULL;
	}
	hSourcePal = SelectPalette(hSourceDC, hPal, FALSE);
	RealizePalette(hSourceDC);
	hSourceBitmap = copyToBitmap(hSourceDC,bitmap,true,(void **)&SrcDibBits);
	if ( hSourceBitmap == NULL ) {
		SelectPalette(hSourceDC, hSourcePal, TRUE);
		RealizePalette(hSourceDC);
		DeleteObject(hPal);
		DeleteDC(hSourceDC);
		sprintf(errbuf, "em:graph: Unable to create source image HBITMAP\n");
		specerror(errbuf);
		return NULL;
	}
	objSrc = SelectObject (hSourceDC,hSourceBitmap);
	hDestDC = CreateCompatibleDC(GetDC(NULL));
	hDestPal = SelectPalette(hDestDC, hPal, FALSE);
	RealizePalette(hDestDC);
	dib = Bitmap_AllocateT(FIT_BITMAP, destWidth, destHeight, Bitmap_GetBPP(bitmap), 0, 0, 0);
	if ((Bitmap_GetColorType(bitmap) == FIC_MINISBLACK) && (Bitmap_GetBPP(bitmap) == 8)) {
		palette = Bitmap_GetPalette(dib);
		for (i = 0; i < 256; i++) {
			palette[i].rgbBlue = i;
			palette[i].rgbGreen = i;
			palette[i].rgbRed = i;
			palette[i].rgbReserved = 0;
		}
	}
	hDestBitmap = copyToBitmap(hDestDC,dib,false,(void **)&DestDibBits);
	Bitmap_Unload(dib);
	if ( hDestBitmap == NULL ) {
		SelectPalette(hSourceDC, hSourcePal, TRUE);
		RealizePalette(hSourceDC);
		DeleteObject(hSourceBitmap);
		DeleteDC(hSourceDC);
		SelectPalette(hDestDC, hDestPal, TRUE);
		RealizePalette(hDestDC);
		DeleteDC(hDestDC);
		DeleteObject(hPal);
		sprintf(errbuf, "em:graph: Unable to create destination image HBITMAP\n");
		specerror(errbuf);
		return NULL;
	}
	objDest = SelectObject (hDestDC,hDestBitmap);
	oldmode = SetStretchBltMode(hDestDC,mode);
	if ( ! StretchBlt(hDestDC,0,0,destWidth,destHeight,hSourceDC,0,0,
		Bitmap_GetWidth(bitmap), Bitmap_GetHeight(bitmap), SRCCOPY)) {
		SelectPalette(hSourceDC, hSourcePal, TRUE);
		RealizePalette(hSourceDC);
		SelectPalette(hDestDC, hDestPal, TRUE);
		RealizePalette(hDestDC);
		DeleteObject(hPal);
		DeleteObject(hSourceBitmap);
		DeleteObject(hDestBitmap);
		SetStretchBltMode(hDestDC,oldmode);
		DeleteDC(hSourceDC);
		DeleteDC(hDestDC);
		sprintf(errbuf, "em:graph: Unable to resize image %s\n",filename);
		specerror(errbuf);
		return NULL;
	}
	GdiFlush();
	dib = copyFromBitmap(hDestDC,hDestBitmap);
	SetStretchBltMode(hDestDC,oldmode);
	SelectPalette(hSourceDC, hSourcePal, TRUE);
	RealizePalette(hSourceDC);
	SelectPalette(hDestDC, hDestPal, TRUE);
	RealizePalette(hDestDC);
	DeleteObject(hPal);
	DeleteObject(hSourceBitmap);
	DeleteObject(hDestBitmap);
	DeleteDC(hSourceDC);
	DeleteDC(hDestDC);
	if ( dib == NULL ) {
		sprintf(errbuf, "em:graph: Unable to copy image from HBITMAP\n");
		specerror(errbuf);
		return NULL;
	}

	return dib;
}
#endif

FIBITMAP *ResizeBitmap(FIBITMAP *bitmap, int destWidth, int destHeight, char filter)
{
	BITMAP_FILTER FIfilter;
	FIBITMAP *dst;
	FIICCPROFILE *icc_src, *icc;

	switch ( filter ) {
	case 'b':
		FIfilter = FILTER_BOX;
		break;
	case 't':
		FIfilter = FILTER_BILINEAR;
		break;
	case 'B':
		FIfilter = FILTER_BSPLINE;
		break;
	case 'l':
		FIfilter = FILTER_LANCZOS3;
		break;
	case 'm':
		FIfilter = FILTER_BICUBIC;
		break;
	case 'c':
		FIfilter = FILTER_CATMULLROM;
		break;
	default:
		FIfilter = FILTER_BICUBIC;
	}
	dst = Bitmap_Rescale(bitmap, destWidth, destHeight, FIfilter);
	if ( dst ) {
		icc_src = Bitmap_GetICCProfile(bitmap);
		if ( icc_src == NULL ) {
			if (Bitmap_GetColorType(bitmap) == FIC_CMYK) {
				icc = Bitmap_CreateICCProfile(dst, NULL, 0);
				icc->flags = FIICC_COLOR_IS_CMYK;
			}
		}
		else {
			icc = Bitmap_CreateICCProfile(dst, icc_src->data, icc_src->size);
			icc->flags = icc_src->flags;
		}
	}
	return dst;
}

unsigned
EmReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	return (unsigned)fread(buffer, size, count, (FILE *)handle);
}

int
EmSeekProc(fi_handle handle, long offset, int origin) {
	return fseek((FILE *)handle, offset, origin);
}

long
EmTellProc(fi_handle handle) {
	return ftell((FILE *)handle);
}

void
bitmapgraph(FILE *f, BitmapIO *io, char *filename, BITMAP_FORMAT fif, float emwidth, float emheight)
{
	BITMAPINFOHEADER *bm;
	RGBQUAD *palette;

	unsigned char isblack[256];
	unsigned char rr;
	unsigned char gg;
	unsigned char bb;
	unsigned char c = 0;

	unsigned char *line;
	char *pshexa;

	int clrtablesize;
	int dpi;
	int i;
	int j;

	unsigned char emask = 0;
	integer ewidth = 0;
	float mw, mh;
	int nWidth, nHeight;

	//Resize conditions
	unsigned int RESIZE_COND = actualdpi / 400;
	unsigned int RESIZE_CONDV = vactualdpi / 400;

	FIBITMAP *bitmap, *tmpbmp;
	BITMAP_COLOR_TYPE bct;
	unsigned int bpp;

	bitmap = NULL;
	switch (fif) {
		case FIF_PCX:
			bitmap = Bitmap_LoadFromHandle(FIF_PCX, io, (fi_handle)f, 0);
			break;
		case FIF_BMP:
			bitmap = Bitmap_LoadFromHandle(FIF_BMP, io, (fi_handle)f, 0);
			break;
		case FIF_TIFF:
			bitmap = Bitmap_LoadFromHandle(FIF_TIFF, io, (fi_handle)f, TIFF_CMYK);
			break;
		case FIF_JPEG:
			bitmap = Bitmap_LoadFromHandle(FIF_JPEG, io, (fi_handle)f, JPEG_ACCURATE | JPEG_CMYK);
			break;
		case FIF_PNG:
			bitmap = Bitmap_LoadFromHandle(FIF_PNG, io, (fi_handle)f, PNG_DEFAULT);
			break;
	}
	if ( bitmap == NULL ) {
		sprintf(errbuf, "em:graph: Unable to read file %s\n",filename);
		specerror(errbuf);
		return;
	}
	if (Bitmap_GetImageType(bitmap) != FIT_BITMAP) {
		sprintf(errbuf, "em:graph: image %s format not supported\n", filename);
		specerror(errbuf);
		Bitmap_Unload(bitmap);
		return;
	}

	bct = Bitmap_GetColorType(bitmap);
	bpp = Bitmap_GetBPP(bitmap);
	if (bct == FIC_RGBALPHA) {
		tmpbmp = Bitmap_ConvertRGBATo24Bits(bitmap);
		if (tmpbmp == NULL) {
			sprintf(errbuf, "em:graph: Unable to convert RGBALPHA image from file %s\n", filename);
			specerror(errbuf);
			Bitmap_Unload(bitmap);
			return;
		}
		Bitmap_Unload(bitmap);
		bitmap = tmpbmp;
	}
	else if (((bct == FIC_RGB) && (bpp == 32))) {
		tmpbmp = Bitmap_ConvertTo24Bits(bitmap);
		if (tmpbmp == NULL) {
			sprintf(errbuf, "em:graph: Unable to convert RGBALPHA image from file %s\n", filename);
			specerror(errbuf);
			Bitmap_Unload(bitmap);
			return;
		}
		Bitmap_Unload(bitmap);
		bitmap = tmpbmp;
	}

    /* get BITMAP header */
	bm = Bitmap_GetInfoHeader(bitmap);
	dpi = (int)((float)Bitmap_GetDotsPerMeterX(bitmap) / 100 * 2.54 + 0.5);

	if ( RESIZE_MODE ) {
		nWidth = bm->biWidth; 
		nHeight = bm->biHeight;
		if (emwidth > 0.0) {
			mw = (float)bm->biWidth / dpi * actualdpi;
			if ( fabsf(mw - emwidth) > RESIZE_COND ) {
				nWidth = (int)((float)bm->biWidth / mw * emwidth + 0.5); 
			}
		}
		if (emheight > 0.0) {
			mh = (float)bm->biHeight / dpi * vactualdpi;
			if ( fabsf(mh - emheight) > RESIZE_CONDV ) {
				nHeight = (int)((float)bm->biHeight / mh * emheight + 0.5); 
			}
		}
		if ( (nWidth != bm->biWidth) || (nHeight != bm->biHeight) ) {
			//Resize bitmap
			switch (bm->biBitCount) {
			case 1:
				//Mono bitmap
				if ( RESIZE_FILTER_BW == 'w' ) {
#if defined(WIN32)
					//WinGDI resize
					tmpbmp = ResizeBitmapWnd(filename,GDI_STRETCH_MODE_BW,bitmap,nWidth,nHeight);
					if ( tmpbmp == NULL ) {
						Bitmap_Unload(bitmap);
						return;
					}
#else
					//resize with resample filter
					tmpbmp = Bitmap_Resample(bitmap, nWidth, nHeight);
					if ( tmpbmp == NULL ) {
						sprintf(errbuf, "em:graph: Unable to resize BW image %s\n",filename);
						specerror(errbuf);
						Bitmap_Unload(bitmap);
						return;
					}
#endif
				}
				else {
					tmpbmp = Bitmap_Resample(bitmap, nWidth, nHeight);
					if ( tmpbmp == NULL ) {
						sprintf(errbuf, "em:graph: Unable to resize BW image %s\n",filename);
						specerror(errbuf);
						Bitmap_Unload(bitmap);
						return;
					}
				}
				break;
			case 4:
			case 8:
				if (Bitmap_GetColorType(bitmap) == FIC_PALETTE) {
					if ( RESIZE_FILTER_RGB == 'w' ) {
#if defined(WIN32)
						//WinGDI resize
						tmpbmp = ResizeBitmapWnd(filename,GDI_STRETCH_MODE_RGB,bitmap,nWidth,nHeight);
						if ( tmpbmp == NULL ) {
							Bitmap_Unload(bitmap);
							return;
						}
#else
						//resize with resample filter
						tmpbmp = Bitmap_Resample(bitmap,nWidth,nHeight);
						if ( tmpbmp == NULL ) {
							sprintf(errbuf, "em:graph: Unable to resize BW image %s\n",filename);
							specerror(errbuf);
							Bitmap_Unload(bitmap);
							return;
						}
#endif
					}
					else {
						if ( RESIZE_FILTER_RGB == 'r' ) {
							//resize with resample filter
							tmpbmp = Bitmap_Resample(bitmap, nWidth, nHeight);
							if ( tmpbmp == NULL ) {
								sprintf(errbuf, "em:graph: Unable to resize image %s\n",filename);
								specerror(errbuf);
								Bitmap_Unload(bitmap);
								return;
							}
						}
						else {
							//resize with math filter
							tmpbmp = ResizeBitmap(bitmap,nWidth,nHeight,RESIZE_FILTER_RGB);
							if ( tmpbmp == NULL ) {
								sprintf(errbuf, "em:graph: Unable to resize image %s\n",filename);
								specerror(errbuf);
								Bitmap_Unload(bitmap);
								return;
							}
						}
					}
				}
				else {
					if ( RESIZE_FILTER_Gray == 'w' ) {
#if defined(WIN32)
						//WinGDI resize
						tmpbmp = ResizeBitmapWnd(filename,GDI_STRETCH_MODE_Gray,bitmap,nWidth,nHeight);
						if ( tmpbmp == NULL ) {
							Bitmap_Unload(bitmap);
							return;
						}
#else
						//resize with resample filter
						tmpbmp = Bitmap_Resample(bitmap,nWidth,nHeight);
						if ( tmpbmp == NULL ) {
							sprintf(errbuf, "em:graph: Unable to resize BW image %s\n",filename);
							specerror(errbuf);
							Bitmap_Unload(bitmap);
							return;
						}
#endif
					}
					else {
						if ( RESIZE_FILTER_Gray == 'r' ) {
							//resize with resample filter
							tmpbmp = Bitmap_Resample(bitmap, nWidth, nHeight);
							if ( tmpbmp == NULL ) {
								sprintf(errbuf, "em:graph: Unable to resize image %s\n",filename);
								specerror(errbuf);
								Bitmap_Unload(bitmap);
								return;
							}
						}
						else {
							//resize with math filter
							tmpbmp = ResizeBitmap(bitmap,nWidth,nHeight,RESIZE_FILTER_Gray);
							if ( tmpbmp == NULL ) {
								sprintf(errbuf, "em:graph: Unable to resize image %s\n",filename);
								specerror(errbuf);
								Bitmap_Unload(bitmap);
								return;
							}
						}
					}
				}
				break;
			case 24:
				if ( RESIZE_FILTER_RGB == 'w' ) {
#if defined(WIN32)
					//WinGDI resize
					tmpbmp = ResizeBitmapWnd(filename,GDI_STRETCH_MODE_RGB,bitmap,nWidth,nHeight);
					if ( tmpbmp == NULL ) {
						Bitmap_Unload(bitmap);
						return;
					}
#else
					//resize with resample filter
					tmpbmp = Bitmap_Resample(bitmap,nWidth,nHeight);
					if ( tmpbmp == NULL ) {
						sprintf(errbuf, "em:graph: Unable to resize BW image %s\n",filename);
						specerror(errbuf);
						Bitmap_Unload(bitmap);
						return;
					}
#endif
				}
				else {
					if ( RESIZE_FILTER_RGB == 'r' ) {
						//resize with resample filter
						tmpbmp = Bitmap_Resample(bitmap, nWidth, nHeight);
						if ( tmpbmp == NULL ) {
							sprintf(errbuf, "em:graph: Unable to resize image %s\n",filename);
							specerror(errbuf);
							Bitmap_Unload(bitmap);
							return;
						}
					}
					else {
						//resize with math filter
						tmpbmp = ResizeBitmap(bitmap,nWidth,nHeight,RESIZE_FILTER_RGB);
						if ( tmpbmp == NULL ) {
							sprintf(errbuf, "em:graph: Unable to resize image %s\n",filename);
							specerror(errbuf);
							Bitmap_Unload(bitmap);
							return;
						}
					}
				}
				break;
			case 32:
				if ( (RESIZE_FILTER_CMYK == 'w') || (RESIZE_FILTER_CMYK == 'r') ) {
					//resize with resample filter
					tmpbmp = Bitmap_Resample(bitmap, nWidth, nHeight);
					if ( tmpbmp == NULL ) {
						sprintf(errbuf, "em:graph: Unable to resize image %s\n",filename);
						specerror(errbuf);
						Bitmap_Unload(bitmap);
						return;
					}
				}
				else {
					//resize with math filter
					tmpbmp = ResizeBitmap(bitmap,nWidth,nHeight,RESIZE_FILTER_CMYK);
					if ( tmpbmp == NULL ) {
						sprintf(errbuf, "em:graph: Unable to resize image %s\n",filename);
						specerror(errbuf);
						Bitmap_Unload(bitmap);
						return;
					}
				}
				break;
			default:
				tmpbmp = Bitmap_Clone(bitmap);
			}
			Bitmap_Unload(bitmap);
			bitmap = tmpbmp;
			bm = Bitmap_GetInfoHeader(bitmap);
		}
	}

	imagehead(filename,(int)bm->biWidth,(int)bm->biHeight,dpi,emwidth,emheight);

	/* determine the size of the color table to read */
	clrtablesize = Bitmap_GetColorsUsed(bitmap);

	/* read in the color table */
	palette = Bitmap_GetPalette(bitmap);

	pshexa = NULL;

	switch (bm->biBitCount) {
	case 1:
		for (i = 0; i < clrtablesize; i++) {
			bb = palette[i].rgbBlue;
			gg = palette[i].rgbGreen;
			rr = palette[i].rgbRed;
			isblack[i] = (rr < 0xff) || (gg < 0xff) || (bb < 0xff);
		}
		pshexa = (char *) mymalloc((integer)tobyte(bm->biWidth));
		/* output the postscript image size preamble */
		cmdout("/picstr");
		numout((integer)tobyte(bm->biWidth));
		cmdout("string def");
		numout((integer)bm->biWidth);
		numout((integer)bm->biHeight);
		cmdout("false");
		cmdout("[");
		numout((integer)bm->biWidth);
		numout((integer)0);
		numout((integer)0);
		numout((integer)bm->biHeight);
		numout((integer)0);
		numout((integer)bm->biHeight);
		cmdout("]");
		nlcmdout("{currentfile picstr readhexstring pop} imagemask");
		if (bm->biWidth%8)
			emask = (1<<(8-(bm->biWidth%8)))-1;	/* mask for edge of bitmap */
		else
			emask = 0;
		ewidth = tobyte(bm->biWidth);
		for (i = 0; i < bm->biHeight; i++) {
			memset(pshexa,0xff,tobyte(bm->biWidth));
			line = Bitmap_GetScanLine(bitmap,i);
		    if (isblack[0])
			for (j = 0; j < ewidth ; j++)
				pshexa[j] = line[j];
		    else
			for (j = 0; j < ewidth ; j++)
				pshexa[j] = ~line[j];
		    pshexa[ewidth-1] |= emask;
			newline();
			mhexout((unsigned char *) pshexa,(long)tobyte(bm->biWidth));
		}
		break;
	case 4:
		tmpbmp = Bitmap_ConvertTo8Bits(bitmap);
		Bitmap_Unload(bitmap);
		bitmap = tmpbmp;
		bm = Bitmap_GetInfoHeader(bitmap);
		clrtablesize = Bitmap_GetColorsUsed(bitmap);
		palette = Bitmap_GetPalette(bitmap);
	case 8:
		if (Bitmap_GetColorType(bitmap) == FIC_PALETTE) {
			cmdout("/picstr");
			numout((integer)(bm->biWidth));
			cmdout("string def");
			cmdout("/rgbstr");
			numout((integer)(bm->biWidth * 3));
			cmdout("string def");
			nlcmdout("/read8{currentfile picstr readhexstring pop /rgb_ind 0 def");
			nlcmdout("       {3 mul pal exch 3 getinterval rgbstr exch rgb_ind exch putinterval");
			nlcmdout("       /rgb_ind rgb_ind 3 add def}forall rgbstr}bind def");
			nlcmdout("/pal <");
			pshexa = (char *) mymalloc((integer)(clrtablesize * 3));
			for (i = 0; i < clrtablesize; i++) {
				pshexa[i*3] = palette[i].rgbRed;
				pshexa[i*3+1] = palette[i].rgbGreen;
				pshexa[i*3+2] = palette[i].rgbBlue;
			}
			mhexout((unsigned char *) pshexa,(long)(clrtablesize * 3));
			free(pshexa);
			nlcmdout("> def");
			numout((integer)bm->biWidth);
			numout((integer)bm->biHeight);
			numout((integer)8);
			cmdout("[");
			numout((integer)bm->biWidth);
			numout((integer)0);
			numout((integer)0);
			numout((integer)bm->biHeight);
			numout((integer)0);
			numout((integer)bm->biHeight);
			cmdout("]");
			nlcmdout("{read8} false 3 colorimage");
			pshexa = (char *) mymalloc((integer)(bm->biWidth));
			for (i = 0; i < bm->biHeight; i++) {
				memset(pshexa,0xff,bm->biWidth);
				line = Bitmap_GetScanLine(bitmap,i);
				memcpy(pshexa,line,bm->biWidth);
				newline();
				mhexout((unsigned char *) pshexa,(long)bm->biWidth);
			}
		}
		else {
			pshexa = (char *) mymalloc((integer)(bm->biWidth));
			cmdout("/picstr");
			numout((integer)(bm->biWidth));
			cmdout("string def");
			numout((integer)bm->biWidth);
			numout((integer)bm->biHeight);
			numout((integer)8);
			cmdout("[");
			numout((integer)bm->biWidth);
			numout((integer)0);
			numout((integer)0);
			numout((integer)bm->biHeight);
			numout((integer)0);
			numout((integer)bm->biHeight);
			cmdout("]");
			nlcmdout("{currentfile picstr readhexstring pop} image");
			for (i = 0; i < bm->biHeight; i++) {
				memset(pshexa,0xff,bm->biWidth);
				line = Bitmap_GetScanLine(bitmap,i);
				memcpy(pshexa,line,bm->biWidth);
				newline();
				mhexout((unsigned char *) pshexa,(long)bm->biWidth);
			}
		}
		break;
	case 24:
		pshexa = (char *) mymalloc((integer)(bm->biWidth * 3));
		/* output the postscript image size preamble */
		cmdout("/picstr");
		numout((integer)(bm->biWidth * 3));
		cmdout("string def");
		numout((integer)bm->biWidth);
		numout((integer)bm->biHeight);
		numout((integer)8);
		cmdout("[");
		numout((integer)bm->biWidth);
		numout((integer)0);
		numout((integer)0);
		numout((integer)bm->biHeight);
		numout((integer)0);
		numout((integer)bm->biHeight);
		cmdout("]");
		nlcmdout("{currentfile picstr readhexstring pop} false 3 colorimage");
		for (i = 0; i < bm->biHeight; i++) {
			memset(pshexa,0xff,bm->biWidth * 3);
			line = Bitmap_GetScanLine(bitmap,i);
			for (j = 0; j < bm->biWidth; j++) {
				pshexa[j * 3] = line[j * 3];
				pshexa[j * 3 + 1] = line[j * 3 + 1];
				pshexa[j * 3 + 2] = line[j * 3 + 2];
			}
			newline();
			mhexout((unsigned char *) pshexa,(long)(bm->biWidth * 3));
		}
		break;
	case 32:
		pshexa = (char *) mymalloc((integer)(bm->biWidth * 4));
		/* output the postscript image size preamble */
		cmdout("/picstr");
		numout((integer)(bm->biWidth * 4));
		cmdout("string def");
		numout((integer)bm->biWidth);
		numout((integer)bm->biHeight);
		numout((integer)8);
		cmdout("[");
		numout((integer)bm->biWidth);
		numout((integer)0);
		numout((integer)0);
		numout((integer)bm->biHeight);
		numout((integer)0);
		numout((integer)bm->biHeight);
		cmdout("]");
		nlcmdout("{currentfile picstr readhexstring pop} false 4 colorimage");
		for (i = 0; i < bm->biHeight; i++) {
			memset(pshexa,0xff,bm->biWidth * 3);
			line = Bitmap_GetScanLine(bitmap,i);
			for (j = 0; j < bm->biWidth; j++) {
				pshexa[j*4] = line[j*4];
				pshexa[j*4+1] = line[j*4+1];
				pshexa[j*4+2] = line[j*4+2];
				pshexa[j*4+3] = line[j*4+3];
			}
			newline();
			mhexout((unsigned char *) pshexa,(long)(bm->biWidth * 4));
		}
		break;
	}

	imagetail() ;
	if ( pshexa )
		free(pshexa);
	Bitmap_Unload(bitmap);
}
/* ------------------------------------------------------------------------ */

void
emgraph(char *filename, float emwidth, float emheight)
{
	BitmapIO io;
	char fname[260];
	FILE *f;
	char *env;
	int i;

	strcpy(fname, filename);

	/* find the file */
	f = search(figpath, fname, READBIN);
	if (f == (FILE *)NULL) {
   	    if ( (env = getenv("DVIDRVGRAPH")) != NULL )
#ifdef KPATHSEA
		f = search((kpse_file_format_type)env[0],filename,READBIN);
#else
		f = search(env,filename,READBIN);
#endif
	}
	/* if still haven't found it try adding extensions */
	if ( (f == (FILE *)NULL) && EMSEARCH_MODE ) {
	    i = 0;
	    while (extarr[i] != NULL) {
		strcpy(fname, filename);
		env = strrchr(fname,'.');
		if ( env )
			*env = 0;
		strcat(fname, extarr[i]);
		f = search(figpath, fname, READBIN);
		if (f == (FILE *)NULL) {
	    	    if ( (env = getenv("DVIDRVGRAPH")) != NULL )
#ifdef KPATHSEA
			f = search((kpse_file_format_type)env[0],filename,READBIN);
#else
			f = search(env,filename,READBIN);
#endif
		}
		if (f != (FILE *)NULL)
		    break;
		i++;
	    }
	}

	io.read_proc = EmReadProc;
	io.seek_proc = EmSeekProc;
	io.tell_proc = EmTellProc;
	io.write_proc = NULL;
	if (f != (FILE *)NULL) {
		if (Bitmap_ValidateFromHandle(FIF_BMP, &io, (fi_handle)f))
			bitmapgraph(f, &io, fname, FIF_BMP, emwidth, emheight);
		else if (Bitmap_ValidateFromHandle(FIF_PCX, &io, (fi_handle)f))
			bitmapgraph(f, &io, fname, FIF_PCX, emwidth, emheight);
		else if (Bitmap_ValidateFromHandle(FIF_JPEG, &io, (fi_handle)f))
			bitmapgraph(f, &io, fname, FIF_JPEG, emwidth, emheight);
		else if (Bitmap_ValidateFromHandle(FIF_PNG, &io, (fi_handle)f))
			bitmapgraph(f, &io, fname, FIF_PNG, emwidth, emheight);
		else if (Bitmap_ValidateFromHandle(FIF_TIFF, &io, (fi_handle)f))
			bitmapgraph(f, &io, fname, FIF_TIFF, emwidth, emheight);
		else {
			snprintf(fname, sizeof(fname), "em:graph: %s: Unknown file format", filename);
			error(fname);
		}
	}
	else {
		snprintf(fname, sizeof(fname), "em:graph: %s: File not found", filename);
			error(fname);
	}
	if (f != (FILE *)NULL)
	    close_file(f);
}

#else
void
emspecial(char *p)
{
	sprintf(errbuf,"emTeX specials not compiled in this version");
	specerror(errbuf);
}
#endif /* EMTEX */
