/* 
   winimg.c : implementation file
   Time-stamp: "00/04/06 23:50:06 Fabrice Popineau"
   
   Copyright (C) 1999
      Fabrice Popineau <Fabrice.Popineau@supelec.fr>

   This file is part of Windvi.

   Windvi is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   
   Windvi is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.
   
   You should have received a copy of the GNU General Public License
   along with Windvi; if not, write to the Free Software Foundation, 59
   Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#include "wingui.h"
#include "xdvi-config.h"

struct emunit {
   char *unit;
   float factor;
};
struct emunit emtable[] = {
  {"pt",72.27},
  {"pc",72.27/12},
  {"in",1.0},
  {"bp",72.0},
  {"cm",2.54},
  {"mm",25.4},
  {"dd",72.27/(1238.0/1157)},
  {"cc",72.27/12/(1238.0/1157)},
  {"sp",72.27*65536},
  {0,0.0}
};

#define SKIP_SPACE(p) { while(isspace(*p)) { p++; } }
#define SKIP_SPACE_AND_COMMA(p) { while(isspace(*p) || *p == ',') { p++; } }

char emstr[MAXPATHLEN];
float emwidth, emheight;

typedef struct _myBitmap {
  HANDLE hFile;
  HANDLE hMapFile;
  LPVOID pMapFile;
  LPBITMAPINFO pbmi;
  /*  PBYTE ppvBits; */
  HPALETTE hPal;
  BOOL bCoreHeader;
} myBitmap, *pmyBitmap;

/******************************Public*Routine******************************\
 *                                   
 * bSelectDIBPal
 *
 * Effects: Creates a logical palette from the DIB and select it into the DC
 *          and realize the palette. Saving the hPal in the pInfo->hPal
 *
 * Warnings: Based on Windows NT DIB support.  If PM support 16,24,32 bpp
 *           we need to modify this routine.
 *
 \**************************************************************************/  
BOOL bSelectDIBPal(HDC hDC, pmyBitmap theBitmap) 
{
  LOGPALETTE    *plogPal;   
  UINT          uiSizPal;   
  INT           i, iNumClr;   
  WORD          wBitCount;    
  LPBITMAPINFO  pbmi = theBitmap->pbmi;

  if (theBitmap->bCoreHeader) {     
	wBitCount = ((LPBITMAPCOREINFO)pbmi)->bmciHeader.bcBitCount;  
  }
  else {
	wBitCount = pbmi->bmiHeader.biBitCount;
  }
  
  switch (wBitCount) { 
  case 16:
  case 24:
  case 32:  
	/* Does PM supports these? */
	return FALSE;
  default:
	iNumClr = (1 << wBitCount);
	break;
  }
  uiSizPal = sizeof(WORD)*2 + sizeof(PALETTEENTRY)*iNumClr;
  if ((plogPal = (LOGPALETTE *) LocalAlloc(LMEM_FIXED,uiSizPal)) == NULL) {
	theBitmap->hPal = NULL;
	return FALSE;
  }
  plogPal->palVersion = 0x300;
  plogPal->palNumEntries = (WORD) iNumClr;
  if (theBitmap->bCoreHeader) {
	for (i=0; i<iNumClr; i++) {
	  plogPal->palPalEntry[i].peRed   = ((LPBITMAPCOREINFO)pbmi)->bmciColors[i].rgbtRed;
	  plogPal->palPalEntry[i].peGreen = ((LPBITMAPCOREINFO)pbmi)->bmciColors[i].rgbtGreen;
	  plogPal->palPalEntry[i].peBlue  = ((LPBITMAPCOREINFO)pbmi)->bmciColors[i].rgbtBlue;
	  plogPal->palPalEntry[i].peFlags = PC_RESERVED; 
    }
  }
  else { 
    for (i=0; i<iNumClr; i++) {
	  plogPal->palPalEntry[i].peRed   = pbmi->bmiColors[i].rgbRed;
	  plogPal->palPalEntry[i].peGreen = pbmi->bmiColors[i].rgbGreen;
	  plogPal->palPalEntry[i].peBlue  = pbmi->bmiColors[i].rgbBlue;
	  plogPal->palPalEntry[i].peFlags = PC_RESERVED; 
    }
  }
  theBitmap->hPal = CreatePalette((LPLOGPALETTE)plogPal);
  if ((theBitmap->hPal) == NULL) {
	return FALSE;
  }
#if 0
  /* FIXME : if we play with the palette her, then we must 
	 use it too when displaying glyphs. */
  if ((GetDeviceCaps(hDC, RASTERCAPS)) & RC_PALETTE) {
	SelectPalette(hDC, theBitmap->hPal, FALSE);
	RealizePalette(hDC);
  }
#endif
  GlobalFree(plogPal);
  return TRUE;
}  

/******************************Public*Routine******************************\
 *
 * LoadBitmapFile * * Effects:  Loads the bitmap from file and put into pInfo->hBmpSaved
 * 
 * Warnings: pszFileName contains the full path 
 * 
\**************************************************************************/  
pmyBitmap LoadBitmapFile(const char *filename)
{
  BOOL bSuccess;
  HANDLE hFile, hMapFile;
  LPVOID pMapFile;
  LPBITMAPINFOHEADER pbmh;
  LPBITMAPINFO pbmi;
  PBYTE pjTmp;
  ULONG sizBMI;
  INT iNumClr;
  BOOL bCoreHeader;
  pmyBitmap theBitmap = NULL;
  /*
	PFILEINFO pFileInfo;
  */
  
  bSuccess = TRUE;
  if ((hFile = CreateFile(filename, 
			  GENERIC_READ, 
			  FILE_SHARE_READ, 
			  NULL,
			  OPEN_EXISTING, 
			  FILE_ATTRIBUTE_READONLY, 
			  NULL)) == (HANDLE)-1) {
    OutputDebugString("Fail in file open");
    bSuccess = FALSE;
    goto ErrExit1;
  }
  /* Create a map file of the opened file. */
  if ((hMapFile = CreateFileMapping(hFile, 
				    NULL,
				    PAGE_READONLY, 
				    0, 
				    0, 
				    NULL)) == (HANDLE)-1) {
    Win32Error("Fail in creating map file");
    bSuccess = FALSE;
    goto ErrExit2;
  }      
  /* Map a view of the whole file */ 
  if ((pMapFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0)) == NULL) {
    Win32Error("Fail in mapping view of the Map File object");
    bSuccess = FALSE;
    goto ErrExit3;
  }
  /* Saving the DIB file handle, etc in pInfo...
     freeing existing objects, if any. */

#if 0
  bFreeRleFile(pInfo);
  pFileInfo = &(pInfo->RleData.rgFileInfo[0]);
  pFileInfo->hFile = hFile;
  pFileInfo->hMapFile = hMapFile;
  pFileInfo->lpvMapView = pMapFile;
#endif
  /* First check that it is a bitmap file. */
  if (*((PWORD)pMapFile) != 0x4d42) {
    /* 'BM' */
	fprintf(stderr, "windvi: Error, %s is not a DIB!\n", filename);
    bSuccess = FALSE;
    goto ErrExit3;
  }

  /* The file header doesn't end on DWORD boundary... */
  pbmh = (LPBITMAPINFOHEADER)((PBYTE)pMapFile + sizeof(BITMAPFILEHEADER));
  {
    BITMAPCOREHEADER bmch, *pbmch;
    BITMAPINFOHEADER bmih, *pbmih;
    PBYTE pjTmp;
    ULONG ulSiz;
    pbmch = &bmch;
    pbmih = &bmih;
    pjTmp = (PBYTE)pbmh;
    ulSiz = sizeof(BITMAPCOREHEADER);
    while (ulSiz--) {
      *(((PBYTE)pbmch)++) = *(((PBYTE)pjTmp)++);
    }
    pjTmp = (PBYTE)pbmh;
    ulSiz = sizeof(BITMAPINFOHEADER);
    while (ulSiz--) {
      *(((PBYTE)pbmih)++) = *(((PBYTE)pjTmp)++);
    }
    /* Use the size to determine if it is a BitmapCoreHeader or
       BitmapInfoHeader.
       Does PM supports 16 and 32 bpp? How? */
    if (bmch.bcSize == sizeof(BITMAPCOREHEADER)) {
      WORD wBitCount;
      wBitCount = bmch.bcBitCount;
      iNumClr = ((wBitCount == 24) ? 0 : (1 << wBitCount));
      sizBMI = sizeof(BITMAPCOREHEADER)+sizeof(RGBTRIPLE)*iNumClr;
      bCoreHeader = TRUE;
    }
    else {
      /* BITMAPINFOHEADER */
      WORD wBitCount;
      wBitCount = bmih.biBitCount;
      switch (wBitCount) {
      case 16:
      case 32:
		sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(DWORD)*3;
		break;
      case 24:
		sizBMI = sizeof(BITMAPINFOHEADER);
		break;
      default:
		iNumClr = (1 << wBitCount);
		sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*iNumClr;
		break;
      }
      bCoreHeader = FALSE;
    }
  }
  if ((pbmi = (LPBITMAPINFO) malloc(sizBMI)) == NULL) {
    bSuccess = FALSE;
    goto ErrExit3;
  }
  /* Make sure we pass in a DWORD aligned BitmapInfo to CreateDIBitmap
     Otherwise, exception on the MIPS platform
     CR!!!  Equivalent to memcpy */
  pjTmp = (PBYTE)pbmi;
  while(sizBMI--) {
    *(((PBYTE)pjTmp)++) = *(((PBYTE)pbmh)++);
  }
  
  /* assuming CreateDIBitmap() is doing a byte fetch... */
  pMapFile = (PBYTE)pMapFile + ((BITMAPFILEHEADER *)pMapFile)->bfOffBits;      
 
#if 0
  /* Select the palette into the DC first before CreateDIBitmap() */
  bSelectDIBPal(maneDC, pInfo, pbmi, bCoreHdr);
  if ((hddb = CreateDIBitmap(maneDC, 
							 (LPBITMAPINFOHEADER)pbmi,
							 CBM_INIT, 
							 pMapFile, 
							 pbmi, 
							 DIB_RGB_COLORS)) == NULL) {
	Win32Error("Fail in creating DIB bitmap from file!");
	bSuccess = FALSE;
	goto ErrExit4;
  }
#endif
#if 0
  /* Saving the DIB...free memory when the windows is closed. */
  pInfo->RleData.rgpjFrame[0] = pMapFile;
  pInfo->RleData.rgpbmi[0] = pbmi;
  pInfo->RleData.pbmi = (PBITMAPINFO) &(pInfo->RleData.rgpbmi[0]);
  pInfo->RleData.ulFrames = 1;
  pInfo->RleData.ulFiles = 1;
  /* set flag to use original DIB as source for blting so HT can be done */
  pInfo->bUseDIB = TRUE;
  pInfo->bCoreHdr = bCoreHdr;
#endif
  theBitmap = malloc(sizeof(myBitmap));
  theBitmap->hFile = hFile;
  theBitmap->hMapFile = hMapFile;
  theBitmap->pMapFile = pMapFile;
  theBitmap->pbmi = pbmi;
  theBitmap->bCoreHeader = bCoreHeader;

  return (theBitmap);
 ErrExit4:
  LocalFree(pbmi);
 ErrExit3:
  CloseHandleAndClear(&hMapFile);
 ErrExit2:
  CloseHandleAndClear(&hFile);
 ErrExit1:
  return (0);
} 
 
void
DisplayBmpFile(const char *filename, int xpos, int ypos, int width, int height)
{
  pmyBitmap theBitmap;
  RECT r;

  if (xpos < max_x && xpos + (int) width >= min_x &&
	  ypos < max_y && ypos + (int) height >= min_y) {
#if 0
	MoveToEx(foreGC, xpos, ypos, NULL);
	LineTo(foreGC, xpos + width, ypos);
	LineTo(foreGC, xpos + width, ypos + height);
	LineTo(foreGC, xpos, ypos + height);
	LineTo(foreGC, xpos, ypos);
#else
	r.left = xpos;
	r.top = ypos;
	r.right = xpos + width;
	r.bottom = ypos + height;
	if (theBitmap = LoadBitmapFile(filename)) {
	  bSelectDIBPal(maneDC, theBitmap);
	  StretchDIBits(maneDC, 
					xpos - currwin.base_x, ypos - currwin.base_y, width, height,
					0, 0, 
					theBitmap->pbmi->bmiHeader.biWidth, 
					theBitmap->pbmi->bmiHeader.biHeight,
					theBitmap->pMapFile,
					theBitmap->pbmi,
					DIB_RGB_COLORS,
					SRCCOPY);
	  free(theBitmap->pbmi);
	  CloseHandleAndClear(&(theBitmap->hMapFile));
	  CloseHandleAndClear(&(theBitmap->hFile));
	  DeleteObject(theBitmap->hPal);
	  free(theBitmap);
	}
#endif
  }
}

/* Load an enhanced MetaFile. Borrowed from the mfedit sample. */
HENHMETAFILE LoadEnhMetaFile (const char * filename)
{
  HENHMETAFILE hEmf = 0;

  const DWORD META32_SIGNATURE = 0x464D4520;      /* ' EMF' */
  const DWORD ALDUS_ID = 0x9AC6CDD7;
  const size_t APMSIZE = 22;

  HANDLE hMapFile;
  LPVOID pMapFile;
  HANDLE hFile = CreateFile (filename,
			     GENERIC_READ,
			     FILE_SHARE_READ,
			     0,
			     OPEN_EXISTING,
			     FILE_ATTRIBUTE_READONLY,
			     0);
  if (hFile == INVALID_HANDLE_VALUE) {
    return (0);
  }

  hMapFile = CreateFileMapping (hFile, 0, PAGE_READONLY, 0, 0, "YAPEMF");
  if (hMapFile == 0) {
    goto ErrorExit1;
  }

  pMapFile = MapViewOfFile (hMapFile, FILE_MAP_READ, 0, 0, 0);
  if (pMapFile == 0) {
    goto ErrorExit2;
  }

  {
    LPENHMETAHEADER pEmh = (LPENHMETAHEADER) pMapFile;
    if (pEmh->dSignature == META32_SIGNATURE) {
      hEmf = GetEnhMetaFile (filename);
      goto HLM_EXIT;
    }
    
    if (*((LPDWORD) pEmh) == ALDUS_ID) {
      DWORD uiSize = * ((LPDWORD) ((PBYTE) pMapFile + APMSIZE + 6));
      hEmf = SetWinMetaFileBits (uiSize * 2, (PBYTE) pMapFile + APMSIZE, 0, 0);
      if (hEmf == 0) {
      }
      goto HLM_EXIT;
    }
  }

  {
    UINT uiSize;
    HMETAFILE hMf = GetMetaFile (filename);
    if (hMf == 0) {
      goto ErrorExit3;
    }
    
    uiSize = GetMetaFileBitsEx (hMf, 0, 0);
    if (uiSize == 0) {
      DeleteMetaFile (hMf);
      goto ErrorExit3;
    }
    
    {
      void * pvData = malloc (uiSize);
      if (pvData == 0) {
	goto ErrorExit4;
      }
      
      uiSize = GetMetaFileBitsEx (hMf, uiSize, pvData);
      if (uiSize == 0) {
	goto ErrorExit5;
      }
    
      hEmf = SetWinMetaFileBits (uiSize, (const PBYTE) pvData, 0, 0);
  
    ErrorExit5:
      free (pvData);
    }
    
  ErrorExit4:
    DeleteMetaFile (hMf);
  }
  
 HLM_EXIT:
 ErrorExit3:
  UnmapViewOfFile (pMapFile);

 ErrorExit2:
  CloseHandleAndClear (&hMapFile);
    
 ErrorExit1:
  CloseHandleAndClear (&hFile);

  return (hEmf);
}

void
DisplayMetaFile(const char *filename, int xpos, int ypos, int width, int height)
{
  HENHMETAFILE hEmf;
  RECT r;

  if (xpos < max_x && xpos + (int) width >= min_x &&
	  ypos < max_y && ypos + (int) height >= min_y) {

#if 0
	MoveToEx(foreGC, xpos, ypos, NULL);
	LineTo(foreGC, xpos + width, ypos);
	LineTo(foreGC, xpos + width, ypos + height);
	LineTo(foreGC, xpos, ypos + height);
	LineTo(foreGC, xpos, ypos);
#endif
	r.left = xpos - currwin.base_x;
	r.top = ypos - currwin.base_y;
	r.right = xpos + width - currwin.base_x;
	r.bottom = ypos + height - currwin.base_y;

	if (hEmf = LoadEnhMetaFile(filename)) {
	  ENHMETAHEADER header;
	  int true_width, true_height;
	  GetEnhMetaFileHeader (hEmf, sizeof (header), &header);
	  true_width = ROUNDUP(header.rclBounds.right - header.rclBounds.left, shrink_factor);
	  true_height = ROUNDUP (header.rclBounds.bottom - header.rclBounds.top, shrink_factor);
#if 0
	  fprintf(stderr, "Actual dimensions of the emf file : %d x %d\n", 
			  header.rclBounds.right - header.rclBounds.left,
			  header.rclBounds.bottom - header.rclBounds.top);
#endif
	  PlayEnhMetaFile(foreGC, hEmf, &r);
	  DeleteEnhMetaFile(hEmf);
	}
  }
}

/* convert width into dpi units */
float emunits (float width, char * unit)
{
  struct emunit *p;

  for (p=emtable; p->unit; p++) {
    if (strcmp(p->unit,unit)==0) {
      return( width * pixels_per_inch / p->factor );
    }
  }
  return (-1.0); /* invalid unit */
}

void emgraph(char *name, int xpos, int ypos, int width, int height)
{
  int len = strlen(name);

#if 0
  fprintf(stderr, "emspecial: file %s, width = %f, height = %f\n",
	  emstr, emwidth, emheight);
#endif

  if (strnicmp(name+len-4, ".bmp", 4) == 0) {
    DisplayBmpFile(name, xpos, ypos, width, height);
  }
  else if (strnicmp(name+len-4, ".wmf", 4) == 0
	   || strnicmp(name+len-4, ".emf", 4) == 0) {
    DisplayMetaFile(name, xpos, ypos, width, height);
  }
  else {
    fprintf(stderr, "emgraph: %s, unknown file type\n", name);
  }
}

void emspecial(char *cp, int xpos, int ypos)
{
  int i, nskip;
  char emunit[4];

  SKIP_SPACE(cp);
  if (strnicmp(cp, "graph", 5) == 0) {
    cp += 5;
    SKIP_SPACE(cp);

    /* FIXME : what about files with a space in their name ??? */
    for (i=0; *cp && !isspace(*cp) && !(*cp==',') ; cp++)
      emstr[i++] = *cp; /* copy filename */
    emstr[i] = '\0';

    /* now get optional width and height */
    emwidth = emheight = -1.0;	/* no dimension is <= 0 */
    SKIP_SPACE_AND_COMMA(cp);

    if (*cp) {
      sscanf(cp, "%f%2s", &emwidth, emunit); /* read width */
      emwidth = emunits(emwidth,emunit); /* convert to pixels */

      for (; *cp && (*cp=='.'||isdigit(*cp)||isalpha(*cp)); cp++)
	; /* skip width dimension */

      SKIP_SPACE_AND_COMMA(cp);

      if (*cp) {
		sscanf(cp, "%f%2s", &emheight, emunit); /* read height */
		emheight = emunits(emheight,emunit) /* vactualdpi/actualdpi */;
      }
    }
    if (emstr[0]) {
      emgraph(emstr,xpos, ypos, (int)floor(emwidth/shrink_factor), (int)floor(emheight/shrink_factor));
    }
    else {
      (void)fprintf(stderr, "em:graph: no file given\n") ;
    }
  }
  else {
    fprintf(stderr, "emspecial not handled: %s\n", cp);
  }
}

