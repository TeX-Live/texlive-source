/* 
   winprint.c : implementation file
   Time-stamp: "01/01/12 10:50:56 popineau"
   
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

/*
  Supports 'print setup' and 'print' functions.

  Not so easy to implement color printing.
  Some remarks :
  - it seems no possible to use rasterops
  to do tramsparent blt
  - a part from doing banding which might be
  very expensive
  - maybe we could build a list of affected rectangles
  (rules and specials)
  - and redraw the needed parts using the appropriate mode

  */

#include "wingui.h"
#include "xdvi-config.h"
#include <commdlg.h>
#include "gsdll.h"

#define DEBUG_PRINT 0

HWND hDlgPrint = 0, hWndParent = 0;
CHAR *msgFormat = "file %s (page %d in [%d - %d])";
CHAR msgBuffer[256];
BOOL isPrinting = FALSE, isPrintingDvips = FALSE;
int iFirstPage, iLastPage;
HDC hMemBandDC;
HBITMAP oldprintDIB, printDIB;
void *lpMemBandData;
struct BITMAPINFO_256  bmiMemBand;
BOOL bPrintError = FALSE;
BOOL bUserAbort = FALSE;

/*

  Variables to save display configuration

  */

HDC save_maneDC;
int save_page, save_shrink;
struct WindowRec save_mane;
Pixel save_fore, save_back;
Boolean save_grey;
POINT saveScroll;
BOOL savedPS;
int save_maneRC;
int saveBitsPixel;
int save_u_paper_w, save_u_paper_h;
int save_offset_x, save_offset_y;

/*
  Printercapabilities
  */

unsigned int iPrintBitsPixel, iPrintSizePal, iPrintColorRes, iPrintRasterCaps;
unsigned int iPrintOffsetX, iPrintOffsetY;
unsigned int nBandHeight, current_band, nBandNumbers;
unsigned int nBandMBSize = 2;

/*
  extern definitions
  */
extern void redraw(struct WindowRec *);
extern BOOL IsOpenedDviFile();
extern void ReopenDviFile();
extern void prescan();

BOOL CALLBACK AbortProc(HDC hdc, int nCode) 
{ 
  MSG msg; 
 
  /* Retrieve and remove messages from the thread's message queue. */
  while (!bUserAbort && PeekMessage(&msg, (HWND) NULL, 0, 0, PM_REMOVE)) { 
    /* Process any messages for the Cancel dialog box. */
    if (!hDlgPrint || !IsDialogMessage(hDlgPrint, &msg)) { 
      TranslateMessage(&msg); 
      DispatchMessage(&msg); 
    } 
  } 
 
  /* Return the global isPrinting flag (which is set to FALSE if the 
     user presses the Cancel button). */
  return !bUserAbort; 

  UNREFERENCED_PARAMETER(hdc); 
  UNREFERENCED_PARAMETER(nCode); 
} 

LRESULT CALLBACK PrintDlgProc(HWND hwndDlg, UINT message, 
			      WPARAM wParam, LPARAM lParam) 
{ 
  switch (message) { 

  case WM_INITDIALOG:  /* message: initialize dialog box */
    /* Initialize the static text control. */
    SetDlgItemText(hwndDlg, IDD_FILE, xbasename(dvi_name)); 

    /* Initialize the progress bars. */
    SendDlgItemMessage(hwndDlg, IDC_PRINT_PROGRESS_PAGE, 
		       PBM_SETRANGE, (WPARAM) 0, MAKELPARAM(1, total_pages));
    SendDlgItemMessage(hwndDlg, IDC_PRINT_PROGRESS_PAGE, 
		       PBM_SETPOS, (WPARAM) 1, (LPARAM) 0);
    SendDlgItemMessage(hwndDlg, IDC_PRINT_PROGRESS_PAGE, 
		       PBM_SETBARCOLOR, 
		       (WPARAM) 0, (LPARAM) (COLORREF) MYRGB(0, 255, 40));

    SendDlgItemMessage(hwndDlg, IDC_PRINT_PROGRESS_BAND, 
		       PBM_SETRANGE, (WPARAM) 0, MAKELPARAM(1, 100));
    SendDlgItemMessage(hwndDlg, IDC_PRINT_PROGRESS_BAND, 
		       PBM_SETPOS, (WPARAM) 1, (LPARAM) 0);
    SendDlgItemMessage(hwndDlg, IDC_PRINT_PROGRESS_PAGE, 
		       PBM_SETBARCOLOR, 
		       (WPARAM) 0, (LPARAM) (COLORREF) MYRGB(0, 40, 255));

    SetFocus(GetDlgItem(hwndDlg, IDD_CANCEL));
    return 0;			/* Because it is setting focus */

  case WM_COMMAND:     /* message: received a command */
    /* User pressed "Cancel" button--stop print job. */
    bUserAbort = TRUE;
    EnableWindow(hWndParent, TRUE);
    DestroyWindow(hwndDlg);
    hDlgPrint = 0;
    return 1;
  }         

  return 0;
  UNREFERENCED_PARAMETER(lParam); 
  UNREFERENCED_PARAMETER(wParam); 
  UNREFERENCED_PARAMETER(message); 
} 

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : InitPrinting(HDC hDC, HWND hWnd, HANDLE hInst, LPSTR msg)  *
 *                                                                          *
 *  PURPOSE    : Makes preliminary driver calls to set up print job.        *
 *                                                                          *
 *  RETURNS    : TRUE  - if successful.                                     *
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/
BOOL InitPrinting(HDC hDC, HWND hWnd, HANDLE hInst, LPSTR msg)
{
  DOCINFO         DocInfo;
  
  bPrintError = FALSE;		/* no errors yet */
  bUserAbort = FALSE;		/* user hasn't aborted */
  
  hWndParent = hWnd;		/* save for Enable at Term time */
  
  hDlgPrint = CreateDialog (hInst, "ABORTPRINTDLG", hWndParent, 
			    (DLGPROC)PrintDlgProc);
  
  if (!hDlgPrint)
    return FALSE;
  
  EnableWindow (hWndParent, FALSE);        /* disable parent */
  
  /*
    Use new printing APIs...Petrus Wong 12-May-1993
  */
  if (SetAbortProc(hDC, (ABORTPROC)AbortProc) <= 0) {
    bPrintError = TRUE;
    return FALSE;
  }

  ZeroMemory(&DocInfo, sizeof(DocInfo));
  DocInfo.cbSize      = sizeof(DOCINFO);
  DocInfo.lpszDocName = (LPTSTR) msg;
  DocInfo.lpszOutput  = NULL;
  
  if (StartDoc(hDC, &DocInfo) <= 0) {
    bPrintError = TRUE;
    return FALSE;
  }
  bPrintError = FALSE;
  
  /* might want to call the abort proc here to allow the user to
   * abort just before printing begins */
  return TRUE;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  TermPrinting(HDC hDC)                                     *
 *                                                                          *
 *  PURPOSE    :  Terminates print job.                                     *
 *                                                                          *
 ****************************************************************************/
VOID TermPrinting(HDC hDC)
{
  /*
    Use new printing APIs...Petrus Wong 12-May-1993
  */
  if (!bPrintError)
    EndDoc(hDC);
  
  if (bUserAbort)
    AbortDoc(hDC);
  else {
    EnableWindow(hWndParent, TRUE);
    DestroyWindow(hDlgPrint);
    hDlgPrint = 0;
  }
  
}

LRESULT CmdFilePrSetup (HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  PAGESETUPDLG pgsdlg;
  return 0;
}

/*********************************************************************** 
 * 
 * GetPrinterDC() 
 * 
 * Uses PrinDlg common dialog for printer selection and creates a dc. 
 * Returns NULL on error. 
 * 
 ***********************************************************************/ 

HDC GetPrinterDC(HWND hWnd)
{
  PRINTDLG pd; 
  char gszDevice[256];
  char gszOutput[256];

  ZeroMemory(&pd, sizeof(pd)); 
 
  pd.lStructSize = sizeof(PRINTDLG); 
  pd.hwndOwner = hWnd;
  pd.Flags = PD_ALLPAGES | PD_RETURNDC /* | PD_PRINTSETUP */; 
  pd.nFromPage = 1; 
  pd.nToPage = total_pages; 
  pd.nMinPage = 1; 
  pd.nMaxPage = total_pages; 
  pd.nCopies = 1; 

  iFirstPage = 0;
  iLastPage = total_pages - 1;

  if (PrintDlg(&pd) == TRUE) { 
    DEVNAMES    *pDevNames; 
    DEVMODE *pDevMode;

    pDevNames = GlobalLock(pd.hDevNames); 
    pDevMode = GlobalLock(pd.hDevMode);

    lstrcpy((LPSTR)gszDevice, 
	    (LPSTR)((char *)pDevNames+pDevNames->wDeviceOffset)); 
 
    if(!lstrcmpi((LPSTR)((char *)pDevNames+pDevNames->wDeviceOffset), 
		 (LPSTR)((char *)pDevNames+pDevNames->wOutputOffset))) 
      lstrcpy((LPSTR)gszOutput, "net:"); 
    else 
      lstrcpy((LPSTR)gszOutput, 
	      (LPSTR)((char *)pDevNames+pDevNames->wOutputOffset)); 
    

#if DEBUG_PRINT
    if (pDevMode = pd.hDevMode) {
      fprintf(stderr, "devmode: name = %s, colors = %s\n",
	      pDevMode->dmDeviceName,
	      pDevMode->dmColor == DMCOLOR_COLOR ? "color" : "mono");
    }
    fprintf(stderr, "gszDevice = %s\ngszOutput = %s\n",
	    gszDevice, gszOutput);
#endif

    iFirstPage = pd.nFromPage - 1;
    iLastPage = pd.nToPage - 1;

    GlobalUnlock(pd.hDevNames); 
    GlobalUnlock(pd.hDevMode); 

    return pd.hDC; 
  } 
  else {
    CommDlgExtendedError();
    iFirstPage = iLastPage = 0;
    return NULL; 
  }
} 

BOOL SetupPageForPrinting(HDC hDC) {

  int cxPage, cyPage, cxInch, cyInch;

  /* Should we bother about the number of colors the printer
     will support ? 
     Answer : YES !!!
  */

  iPrintRasterCaps = GetDeviceCaps(hDC, RASTERCAPS);

  /* Examine the raster capabilities of the device identified by hDC 
     to verify that it supports the BitBlt function. */
 
  if (!(iPrintRasterCaps & RC_STRETCHDIB)) { 
    DeleteDC(hDC); 
    MessageBox(hWndParent, 
	       "Printer cannot display bitmaps.", 
	       "Device Error", 
	       MB_OK); 
    return FALSE; 
  } 

  if (!(iPrintRasterCaps & RC_BITMAP64)) { 
    DeleteDC(hDC); 
    MessageBox(hWndParent, 
	       "Printer cannot cope with bitmaps > 64k.", 
	       "Device Error", 
	       MB_OK); 
    return FALSE; 
  } 
  
  /* FIXME: there is a problem if Bits per Pixel is 1 ! 
     Check with the dib palette. 
  */
  iPrintBitsPixel = GetDeviceCaps(hDC, BITSPIXEL);
  iPrintSizePal = GetDeviceCaps(hDC, SIZEPALETTE);
  iPrintColorRes = GetDeviceCaps(hDC, COLORRES);
  iPrintOffsetX = GetDeviceCaps(hDC, PHYSICALOFFSETX);
  iPrintOffsetY = GetDeviceCaps(hDC, PHYSICALOFFSETY);
  /* There is a mess there with device not reporting colors ! */
  iPrintBitsPixel = 24;

  cxPage = GetDeviceCaps(hDC, HORZRES); /*  Width of printr page - pixels  */
  cyPage = GetDeviceCaps(hDC, VERTRES); /*  Height of printr page - pixels  */
  cxInch = GetDeviceCaps(hDC, LOGPIXELSX);/*  Printer pixels per inch - X  */
  cyInch = GetDeviceCaps(hDC, LOGPIXELSY);/*  Printer pixels per inch - Y  */

  SetStretchBltMode(hDC, COLORONCOLOR); 

  save_mane = mane;
  save_shrink = shrink_factor;
  save_fore = fore_Pixel;
  save_back = back_Pixel;
  save_grey = use_grey;
  saveScroll.x = xCurrentScroll;
  saveScroll.y = yCurrentScroll;
  save_page = current_page;
  save_maneDC = maneDC;
  savedPS = resource._postscript;
  save_maneRC = maneRasterCaps;
  saveBitsPixel =  maneBitsPixel;
  save_u_paper_w = unshrunk_paper_w;
  save_u_paper_h = unshrunk_paper_h;
  save_offset_x = offset_x;
  save_offset_y = offset_y;
  offset_x -= iPrintOffsetX;
  offset_y -= iPrintOffsetY;
  xCurrentScroll = yCurrentScroll = 0;
  shrink_factor = 1;
  use_grey = False;
  maneRasterCaps = iPrintRasterCaps;
  maneBitsPixel =  iPrintBitsPixel;

  unshrunk_paper_w = cxPage;
  unshrunk_paper_h = cyPage;
  unshrunk_page_w = unshrunk_dvifile_page_w;
  unshrunk_page_h = unshrunk_dvifile_page_h;
  if (unshrunk_page_h < unshrunk_paper_h)
    unshrunk_page_h = unshrunk_paper_h;
  if (unshrunk_page_w < unshrunk_paper_w)
    unshrunk_page_w = unshrunk_paper_w;

  mane.shrinkfactor = 1;
  mane.base_x = 0;
  mane.base_y = 0;
  mane.min_x = 0;
  mane.min_y = 0;
  mane.max_x = unshrunk_paper_w;
  mane.max_y = unshrunk_paper_h;
  mane.width = mane.max_x - mane.min_x;
  mane.height = mane.max_y - mane.min_y;

  init_page();

#if DEBUG_PRINT
  fprintf(stderr, "printer page (%d x %d), cx = %d, cy = %d\n",
	  cxPage, cyPage, cxInch, cyInch);
  fprintf(stderr, "bits/pixel = %d, colorres = %d, size pal = %d\n",
	  iPrintBitsPixel, iPrintColorRes, iPrintSizePal);
  fprintf(stderr, "dvi paper (%d x %d)\n", unshrunk_paper_w, unshrunk_paper_h);
  fprintf(stderr, "dvi unshrunk page (%d x %d)\n", 
	  unshrunk_page_w, unshrunk_page_h);
  fprintf(stderr, "dvi shrunk page (%d x %d)\n", page_w, page_h);
  fprintf(stderr, "offset printer (%d , %d)\n", iPrintOffsetX, iPrintOffsetY);
#endif
  
#if 0
  /* FIXME: change mode and dpi */
  resource._pixels_per_inch = cxInch;
  resource.mfmode = ?;
#endif
  
  if (!IsOpenedDviFile())
    ReopenDviFile();

  scanned_page_bak = scanned_page = -1;

  initcolor();
  reset_fonts();
#ifdef XFORM
  reset_xform_stack();
#endif

  return TRUE;
}

BOOL RestorePageForDisplay()
{
  fprintf(stderr, "Restoring for display...\n");
  
  mane = save_mane;
  shrink_factor = save_shrink;
  current_page = save_page;
  use_grey = save_grey;
  xCurrentScroll = saveScroll.x;
  yCurrentScroll = saveScroll.y;
  fore_Pixel = save_fore;
  back_Pixel = save_back;

  offset_x = save_offset_x;
  offset_y = save_offset_y;

  unshrunk_paper_w = save_u_paper_w;
  unshrunk_paper_h = save_u_paper_h;
  unshrunk_page_w = unshrunk_dvifile_page_w;
  unshrunk_page_h = unshrunk_dvifile_page_h;
  if (unshrunk_page_h < unshrunk_paper_h)
    unshrunk_page_h = unshrunk_paper_h;
  if (unshrunk_page_w < unshrunk_paper_w)
    unshrunk_page_w = unshrunk_paper_w;
  init_page();

  /*
    User might abort before config has been changed
  */
  maneBitsPixel = saveBitsPixel;
  maneRasterCaps = save_maneRC;

  foreGC = ruleGC = highGC = maneDC = save_maneDC;

  scanned_page = scanned_page_reset = -1;
#ifdef XFORM
  reset_xform_stack();
#endif

  initcolor();
  reset_fonts();

  resource._postscript = savedPS;


  return TRUE;
}

void PrepareForBanding(HDC hDC)
{

  RGBQUAD QuadWhite = { 255, 255, 255, 0};
  RGBQUAD QuadBlack = {   0,   0,   0, 0};

  /*  Create a memory DC that is compatible with the printer DC */
  hMemBandDC = CreateCompatibleDC(hDC);

  SetStretchBltMode(hMemBandDC, COLORONCOLOR); 

#ifdef TRANSFORM
  if (IS_WIN98 || IS_NT)
    SetGraphicsMode(hMemBandDC, GM_ADVANCED);
#endif

  /*
    Do banding. We can't afford more than say 6Mb at a time.
    Calculate the number of rows at the current dpi.
    Print as many bands as needed.
  */

  nBandNumbers = ROUNDUP(unshrunk_paper_w * unshrunk_paper_h * iPrintBitsPixel,
			 nBandMBSize * 1024 * 1024 * 8) + 1;
  nBandHeight = ROUNDUP(unshrunk_paper_h, nBandNumbers);

  SendDlgItemMessage(hDlgPrint, IDC_PRINT_PROGRESS_BAND, 
		     PBM_SETRANGE, 
		     (WPARAM) 0, (LPARAM) MAKELPARAM(1, nBandNumbers));
  SendDlgItemMessage(hDlgPrint, IDC_PRINT_PROGRESS_BAND, 
		     PBM_SETPOS, 
		     (WPARAM) 1, (LPARAM) 0);

  mane.base_y = 0;
  mane.max_y = nBandHeight;
  mane.height = mane.max_y - mane.min_y;

#if DEBUG_PRINT
  fprintf(stderr, "Band numbers = %d, Band height = %d, page_h = %d\n",
	  nBandNumbers, nBandHeight, unshrunk_paper_h);
#endif

  /* Create a DIB for printing into it. We might need maneDIB to do redraw
     while printing */
  printDIB = CreateDIB(hMemBandDC, unshrunk_paper_w + 1, nBandHeight + 1,
		       iPrintBitsPixel, 
		       &bmiMemBand, &lpMemBandData);

  /* FIXME : there is a problem with monochrome printing. */
  if (iPrintBitsPixel == 1) {
    bmiMemBand.bmiColors[1] = QuadWhite;
    bmiMemBand.bmiColors[0] = QuadBlack;
  }

  /* Put the DIB into the memory DC */
  oldprintDIB = SelectObject(hMemBandDC, printDIB);

  if (resource._postscript) { 
    ps_destroy();
    initGS();
  }

}

static BOOL bBandInit = FALSE;

void FreeBandingRc()
{
  SelectObject(hMemBandDC, oldprintDIB);
  DeleteObject(printDIB);

  DeleteObject(hMemBandDC);

  if (resource._postscript) { 
    ps_destroy();
    initGS();
  }
  bBandInit = FALSE;
}

void DrawPage(HDC hPrinterDC, int page)
{
  if (!IsOpenedDviFile())
    ReopenDviFile();

  current_page = page;

  bColorPage = psToDisplay = FALSE;

#if 1
  /* FIXME: really needed ? */
  init_colors();
#endif

#ifdef TRANSFORM
  /* FIXME : there should be a global stack in case
     such transformations would span over several pages. */
  if (resource.use_xform)
    reset_xfrm_stack();
#endif

  if (!bBandInit) {
    bBandInit = TRUE;
    PrepareForBanding(hPrinterDC);
  }
    
  if (scanned_page < current_page) 
    prescan();

#if DEBUG_PRINT
  fprintf(stderr, "page %d, bColorPage = %d psToDisplay = %d\n", 
	  page, bColorPage, psToDisplay);
#endif
  
  if (bColorPage || psToDisplay) {
    
    /* Specials, do banding */
    maneDC = foreGC = ruleGC = highGC = hMemBandDC;
    
    for (current_band = 0; current_band < nBandNumbers; current_band++) {
#if DEBUG_PRINT
      fprintf(stderr, "Printing page %d band %d\n",
	      page, current_band);
#endif	
      mane.base_y = current_band*nBandHeight;
      mane.max_y = min(nBandHeight, unshrunk_paper_h - current_band * nBandHeight) + 1;
      mane.height = mane.max_y - mane.min_y;

      if (resource._postscript && psToDisplay && gs_device) { 
	extern void
	  init_ps_page(int llx, int lly, int urx, int ury, int height);
	extern void gs_io(char *);
	gs_io("H initgraphics\n");
	init_ps_page(0,0,
		     mane.max_x - mane.min_x, 
		     mane.max_y - mane.min_y,
		     mane.base_y + mane.max_y);
	gs_io("\nstop\n%%xdvimark\n");
      }
      
      /* FIXME : paint background color */
      if (get_back_color(current_page) != MYRGB(255,255,255))
	fprintf(stderr, "this page is white, but should be %-8x\n", 
		get_back_color(current_page));
      
      /* FIXME: check about the + 1 on each dimension ! */
      if (PatBlt(foreGC, 0, 0, unshrunk_paper_w + 1, nBandHeight + 1, 
		 WHITENESS) == 0)
	Win32Error("Redraw Page background");
      
      /* First pass for specials */
      allowDrawingChars = FALSE;
      redraw(&mane);
      
#if DEBUG_PRINT
      fprintf(stderr, "PS state: resource._postscript = %d, psToDisplay = %d, gs_device = %d\n",
	      resource._postscript, psToDisplay, gs_device);
#endif
      
      if (resource._postscript && psToDisplay && gs_device) {
	/* if gs is active, try to make it paint its picture */
	BITMAPINFO_256 bmih;
	LPBYTE ppbyte;
	(*pgsdll_lock_device)(gs_device, 1);
	/* Get a pointer to the bitmap in gs interpreter */
	(*pgsdll_get_bitmap_row)(gs_device, &(bmih.bmiHeader), bmih.bmiColors, &ppbyte, 0);
#if DEBUG_PRINT
	fprintf(stderr, 
		"bmih : %d x %d x %d at %x (colors : used %d at %x)\n", 
		bmih.bmiHeader.biWidth, 
		bmih.bmiHeader.biHeight, 
		bmih.bmiHeader.biBitCount, 
		ppbyte, 
		bmih.bmiHeader.biClrUsed,
		bmih.bmiColors);
#endif
	if (StretchDIBits(foreGC, 
			  mane.min_x, 
			  mane.min_y, 
			  mane.max_x - mane.min_x,
			  mane.max_y - mane.min_y,
			  0, 
			  0, 
			  mane.max_x - mane.min_x,
			  mane.max_y - mane.min_y,
			  ppbyte, (LPBITMAPINFO)&bmih, 
			  (bmih.bmiHeader.biClrUsed ? DIB_PAL_COLORS : DIB_RGB_COLORS), 
			  SRCCOPY) == GDI_ERROR)
	  Win32Error("Print/GS/StretchDIBits");
	(*pgsdll_lock_device)(gs_device, 0);
      }

      /* Second pass for glyphs */
      allowDrawingChars = TRUE;      
      redraw(&mane);
#if 0
      fprintf(stderr, "before bitblt ...");
#endif

#if DEBUG_PRINT
      fprintf(stderr, "Printing band to (%d, %d) - (%d x %d)\n",
	      mane.base_x + mane.min_x, 
	      mane.base_y + mane.min_y, 
	      mane.max_x - mane.min_x,
	      mane.max_y - mane.min_y);
      
#endif

#if 0
      SelectObject(hMemBandDC, oldPrintDIB);
#endif
      if (StretchDIBits(hPrinterDC, 
			mane.base_x + mane.min_x, 
			mane.base_y + mane.min_y, 
			mane.max_x - mane.min_x,
			mane.max_y - mane.min_y,
			0, 0, 
			mane.max_x - mane.min_x,
			mane.max_y - mane.min_y,
			lpMemBandData, (LPBITMAPINFO)&bmiMemBand, 
			(bmiMemBand.bmiHeader.biClrUsed ? DIB_PAL_COLORS : DIB_RGB_COLORS), 
			SRCCOPY) == GDI_ERROR)
	Win32Error("WinPrint/StretchDIBits()");
      
      if (hDlgPrint)
	SendDlgItemMessage(hDlgPrint, IDC_PRINT_PROGRESS_BAND, 
			   PBM_SETPOS, (WPARAM) current_band, (LPARAM) 0);

    }    
  }
  else {
    /* No color or PS specials on this page, 
       draw directly onto the hPrinterDC */
    mane.base_x = 0; mane.base_y = 0;
    mane.min_x = 0; mane.max_x = unshrunk_paper_w;
    mane.min_y = 0; mane.max_y = unshrunk_paper_h;
    maneDC = foreGC = ruleGC = highGC = hPrinterDC;
    allowDrawingChars = TRUE;
    redraw(&mane);
  }

  if (hDlgPrint)
    SendDlgItemMessage(hDlgPrint, IDC_PRINT_PROGRESS_PAGE, 
		       PBM_SETPOS, (WPARAM) page, (LPARAM) 0);

}

LRESULT CmdFilePrint (HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  HDC hPrinterDC;
  int page;

  if (!(hPrinterDC = GetPrinterDC(hwnd)))
    return 0;

#if 0
  __asm int 3;
#endif

  /* Set the flag used by the AbortPrintJob dialog procedure. */

  if (InitPrinting(hPrinterDC, hwnd, hInst, dvi_name)) {
    
    /* Reset pre-scanning */
    scanned_page = scanned_page_reset = -1;

    SendDlgItemMessage(hDlgPrint, IDC_PRINT_PROGRESS_PAGE, 
		       PBM_SETRANGE, 
		       (WPARAM) 0, (LPARAM) MAKELPARAM(iFirstPage, iLastPage));
    SendDlgItemMessage(hDlgPrint, IDC_PRINT_PROGRESS_PAGE, 
		       PBM_SETSTEP, 
		       (WPARAM) 1, (LPARAM) 0);

    isPrinting = TRUE;
    if (!SetupPageForPrinting(hPrinterDC))
      goto Abort;


    for (page = iFirstPage; page <= iLastPage; page++) {

      if (StartPage(hPrinterDC) <= 0) {
	Win32Error("CmdFilePrint/StartPage");
	break;
      }

      if (bUserAbort) {
	break;
      }
      else {
	DrawPage(hPrinterDC, page);
      }

      if (EndPage(hPrinterDC) <= 0) {
	Win32Error("CmdFilePrint/StartPage");
	break;
      }
      
    }

  Abort:
    RestorePageForDisplay();
    if (bBandInit)
      FreeBandingRc();
    isPrinting = FALSE;

    TermPrinting(hPrinterDC);
  }

  DeleteDC(hPrinterDC);
  return 0;
}

#if 0

BOOL GuessPostScriptPrinter()
{
  bFound = FALSE;
  for (each device in [Devices] section of win.ini) {
    /* extract the necessary fields from the ini line */
    szDriverName = driver name extracted from ini line
      szModelName = left side of ini line (the key)
      szPort = port name extracted from ini line.

      hIC = CreateIC(szDriverName, szModelName, szPort, NULL);
    if (hIC) {
      /* see if driver supports GETTECHNOLOGY escape */
      wEscape = GETTECHNOLOGY;
      if (Escape(hIC, QUERYESCSUPPORT, sizeof(WORD), &wEscape, NULL))
	{
	  Escape(hIC, GETTECHNOLOGY, 0, NULL,

		 &szTechnology); 

	  /* Check that the string starts with PostScript
	   * by doing a case-insensitive search. Allow
	   * for the possibility that the string could be
	   * longer, like "PostScript level 2" or some other
	   * extension.
	   */
	  if (beginning of string is "PostScript")
	    bFound = TRUE;
	}
      DeleteDC(hIC);
    }

    /* if the driver has been found break out */
    if (bFound)
      break;
  }

  if (bFound) {
    PostScript driver is szDriverName, model is szModelName, port is
      szPort.

      }
}
NOTE: In the event that GETTECHNOLOGY is not supported by some printer drivers, another method need to be used to determine if the printer is a PostScript printer. One possible method is to use QUERYESCSUPPORT on escapes that are only implemented by PostScript printers. For example: 

EPSPRINTING
SETLINEJOIN
SETMITERLIMIT
SET_POLY_MODE


Similarly, you can determine a PCL printer by calling QUERYESCSUPPORT on the following escape: 

DRAWPATTERNRECT

============================================================================================================

This will print a file on a PS printer for example.

/* FILE: spool.c */

#include <windows.h>
#include <print.h>
#include <commdlg.h>
#include <string.h>

// Function prototypes
BOOL FAR PASCAL PrintFile(LPSTR, HDC, HGLOBAL, HGLOBAL);
VOID SendFile(HDC, LPSTR);
HDC  GetPrinterDC(HGLOBAL, HGLOBAL);
BOOL CALLBACK __export PrintAbortProc(HDC, int);
   
   // Play with this number
#define BUFSIZE 2048
   
// Convenient structure for use with PASSTHROUGH escape
typedef struct
{
  WORD wSize;
  BYTE bData[2];                  // placeholder
} PASSTHROUGHSTRUCT, FAR *LPPTS;
   
   
BOOL bAbort;        // Global printing abort flag
   
   
   //*************************************************************
   //
   //  PrintFile()
   //
   //  Purpose:
   //          Reads a file and copies it to a printer using the
   //          PASSTHROUGH escape.
   //
   //  Parameters:
   //      LPSTR   szFile    - Pointer to path/filename to print
   //      HDC     hPrnDC    - Handle to printer DC or NULL
   //      HGLOBAL hDevNames - Handle to DEVNAMES struct or NULL
   //      HGLOBAL hDevMode  - Handle to DEVMODE struct or NULL
   //
   //  Return:
   //      Returns nonzero for success or zero for failure.
   //
   //  Comments:
   //      hDevNames and hDevMode are only used if hPrnDC is NULL.
   //      If both hPrnDC and hDevNames are NULL, the default
   //      printer is used.
   //
   //  History:    Date       Author     Comment
   //              6/03/93    JMS        Created
   //
   //*************************************************************
   
BOOL FAR PASCAL PrintFile ( LPSTR   szFile,
			    HDC     hPrnDC,
			    HGLOBAL hDevNames,
			    HGLOBAL hDevMode )
{
  int iEsc;
  BOOL bLocalDC = TRUE; // Assume we must create a DC (hPrnDC == NULL)
   
  bAbort = FALSE;       // Haven't aborted yet
   
   
  // Make sure we have a printer DC
   
  if (!hPrnDC)
    hPrnDC = GetPrinterDC(hDevNames, hDevMode);
  else
    bLocalDC = FALSE;   // Use passed in hPrnDC
   
  if (!hPrnDC)
    return FALSE;
   
   
  // PASSTHROUGH is required. If driver doesn't support it, bail out.
   
  iEsc = PASSTHROUGH;
  if (!Escape(hPrnDC, QUERYESCSUPPORT, sizeof(int), (LPSTR)&iEsc, NULL))
    {
      bAbort = TRUE;
      goto MSFCleanUp;
    }
   
  // If we created the DC, install an abort procedure. We don't have
  // a Cancel dialog box, but the abort proc enables multitasking.
  // (Use __export and compile with -GA or -GD so we don't need
  // a MakeProcInstance.)
   
  if (bLocalDC)
    Escape (hPrnDC, SETABORTPROC, 0, (LPSTR) PrintAbortProc, NULL);
   
  // Call EPSPRINTING if it is supported (that is, if we're on a
  // PostScript printer) to suppress downloading the pscript header.
   
  iEsc = EPSPRINTING;
  if (Escape(hPrnDC, QUERYESCSUPPORT, sizeof(int), (LPSTR)&iEsc, NULL))
    {
      iEsc = 1;  // 1 == enable PASSTHROUGH (disable pscript header)
      Escape(hPrnDC, EPSPRINTING, sizeof(int), (LPSTR)&iEsc, NULL);
    }
   
  SendFile(hPrnDC, szFile); // Send file to printer (could do multiple
  // files)
   
 MSFCleanUp:                 // Done
   
  if (bLocalDC)             // Only delete DC if we created it
    DeleteDC(hPrnDC);
   
  return !bAbort;
   
} /* PrintFile() */
   
   
VOID SendFile(HDC hPrnDC, LPSTR szFile)
{
  static LPPTS lpPTS=NULL;          // Pointer to PASSTHROUGHSTRUCT
  OFSTRUCT ofs;
  HFILE hFile;
   
  hFile = OpenFile((LPSTR) szFile, &ofs, OF_READ);
  if (hFile == HFILE_ERROR)
    {
      bAbort = TRUE;  // Can't open file!
      return;
    }
   
  if (!lpPTS &&
      !(lpPTS = (LPPTS)GlobalLock(GlobalAlloc(GPTR, sizeof(WORD) +
					      BUFSIZE))))
    {
      bAbort = TRUE;  // Can't allocate memory for buffer!
      return;
    }
   
  Escape (hPrnDC, STARTDOC, 0, "", NULL);
   
  // Loop through the file, reading a chunk at a time and passing
  // it to the printer. QueryAbort calls the abort procedure, which
  // processes messages so we don't tie up the whole system.
  // We could skip the QueryAbort, in which case we wouldn't need
  // to set an abort proc at all.
   
  do {
    if ((lpPTS->wSize=_lread(hFile, lpPTS->bData, BUFSIZE)) ==
	HFILE_ERROR)
      {
	bAbort = TRUE;  // error reading file
	break;
      }
   
    Escape(hPrnDC, PASSTHROUGH, NULL, (LPSTR)lpPTS, NULL);
  }
  while ((lpPTS->wSize == BUFSIZE) && QueryAbort(hPrnDC, 0));
   
  if (!bAbort)
    Escape(hPrnDC, ENDDOC, NULL, NULL, NULL);
   
  _lclose(hFile);
   
} /* SendFile() */
   
HDC GetPrinterDC(HGLOBAL hDevNames, HGLOBAL hDevMode)
{
  HDC hdc;
  char szPrinter[64];
  LPSTR szDevice=NULL, szDriver=NULL, szOutput=NULL;
  LPDEVMODE lpdm;
   
  if (hDevNames)
    {
      LPDEVNAMES lpdn = (LPDEVNAMES) GlobalLock(hDevNames);
   
      szDriver = (LPSTR) lpdn + lpdn->wDriverOffset;
      szDevice = (LPSTR) lpdn + lpdn->wDeviceOffset;
      szOutput = (LPSTR) lpdn + lpdn->wOutputOffset;
   
      if (hDevMode)
	lpdm = (LPDEVMODE) GlobalLock(hDevMode);
    }
  else
    {                       // Get default printer info
      GetProfileString ("windows", "device", "", szPrinter, 64);
   
      if (!((szDevice = strtok (szPrinter, "," )) &&
	    (szDriver = strtok (NULL,      ", ")) &&
	    (szOutput = strtok (NULL,      ", "))))
	return NULL;    // No default printer
   
      lpdm = NULL;  // Don't use DEVMODE with default printer
    }
   
  hdc = CreateDC(szDriver, szDevice, szOutput, lpdm);
   
  if (hDevMode && lpdm)
    GlobalUnlock(hDevMode);
  if (hDevNames)
    GlobalUnlock(hDevNames);
   
  return hdc;
   
} /* GetPrinterDC() */
   
   
BOOL CALLBACK __export PrintAbortProc(HDC hdc, int code)
{
  MSG msg;
   
  while (!bAbort && PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
   
  return (!bAbort);
   
}  /* PrintAbortProc() */
   
/*** EOF: spool.c ***/
=============================================================================================================
To identify the printer as a PostScript printer, use this code: 


int gPrCode = 0;  // Set according to platform.

if( Win32s ) // Using the Win16 driver.
{
  gPrCode = PASSTHROUGH;
  if((Escape(printerIC, GETTECHNOLOGY, NULL, NULL, (LPSTR)szTech) &&
      !lstrcmp(szTech, "PostScript")) &&
     Escape(printerIC, QUERYESCSUPPORT, sizeof(int),
            (LPSTR)gPrCode, NULL )
  {
    // The printer is PostScript.
    ...
  }
     }
  else // Using Win32 driver under Windows NT.
    {
      gPrCode = POSTSCRIPT_PASSTHROUGH; // Fails with Win16 driver
      if( Escape(printerIC, QUERYESCSUPPORT, sizeof(int), (LPSTR)gPrCode,
		 NULL))
	{
	  // The printer is PostScript.
	  ...
	}
    }


  Printing
    To send PostScript data to the printer on either platform, use this code: 


// Assuming a buffer, szPSBuf, of max size MAX_PSBUF containing
// nPSData bytes of PostScript data.

    char szBuf[MAX_PSBUF+sizeof(short)];

  // Store length in buffer.
  *((short *)szBuf) = nPSData;

  // Store data in buffer.
  memcpy( (char *)szBuf + sizeof(short), szPSBuf, nPSData );

  // Note that gPrCode (set when identifying the printer) depends on
  // the platform.
  Escape( printerDC, gPrCode, (int) nPSData, szBuf, NULL );


  However, your output may appear scaled or translated incorrectly or data may be transformed off the page under Win32s. 
    The origin and scale for Windows printer drivers is not the PostScript default (bottom left/72 dpi) but is instead at the upper left and at the device scale(300 dpi). Therefore, before sending data to the printer, you may need to send a couple of PostScript commands to scale or translate the matrix. For example, for scaling, send the following escape to scale the PostScript transform to 72 dpi: 


    xres = GetDeviceCaps(printerDC, LOGPIXELSX);
  yres = GetDeviceCaps(printerDC, LOGPIXELSY);

// Two leading spaces for the following operation.
  wsprintf(szBuf, "  %d 72 div %d 72 div scale\n", xres, yres);

  // Put actual size into buffer
  *((short *)szBuf) = strlen(szBuf)-2;
  Escape( printerDC, gPrCode, strlen(szBuf)-2, szBuf, NULL );



#endif

#if 0
  /* ********************************************************************  */
  /*   */
  /*   print.c  */
  /*   */
  /*   Source file for Device-Independent Bitmap (DIB) API.  Provides  */
  /*   the following functions:  */
  /*   */
  /*   PrintWindow()       - Prints all or part of a window  */
  /*   PrintScreen()       - Prints the entire screen  */
  /*   PrintDIB()          - Prints the specified DIB  */
  /*   */
  /*  Written by Microsoft Product Support Services, Developer Support.  */
  /*  Copyright 1991-1998 Microsoft Corporation. All rights reserved.  */
  /* ********************************************************************  */
 
#define     STRICT      /*  enable strict type checking  */
 
#include <windows.h> 
#include <string.h> 
#include "dibdll.h"     /*  Header for printing dialog & DLL instance handle  */
#include "dibapi.h"     /*  Header for DIB functions  */
#include "dibutil.h"    /*  Auxiliary functions  */
 
 
  extern HANDLE   ghDLLInst;      /*  Global handle to DLL's instance  */
 
/*************************************************************** 
 * Typedefs 
 **************************************************************/ 
 
/*  Structure used for Banding  */
 
  typedef struct 
  { 
    BOOL bGraphics; 
    BOOL bText; 
    RECT GraphicsRect; 
  } BANDINFOSTRUCT; 
 
 
  /**************************************************************** 
 * Variables 
 ***************************************************************/ 
 
  HWND hDlgAbort;                    /*  Handle to Abort Dialog  */
  char szPrintDlg[] = "PRINTING";    /*  Name of Print dialog from .RC  */
  BOOL bAbort = FALSE;               /*  Abort a print operation?  */
  char gszDevice[50];                /*  Keeps track out device (e.g. "HP LaserJet")  */
  char gszOutput[50];                /*  Output device (e.g. "LPT1:")  */
 
  /*************************************************************** 
 * Function prototypes for functions local to this module 
 **************************************************************/ 
 
  BOOL CALLBACK PrintAbortProc(HDC, int); 
  int CALLBACK PrintAbortDlg(HWND, UINT, WPARAM, LPARAM); 
  WORD PrintBand(HDC, LPRECT, LPRECT, BOOL, BOOL, LPBITMAPINFOHEADER, LPSTR); 
  HDC GetPrinterDC(void); 
  void CalculatePrintRect(HDC, LPRECT, WORD, DWORD, DWORD); 
 
 
  /********************************************************************** 
 * 
 * PrintDIB() 
 * 
 * Description: 
 * 
 * This routine prints the specified DIB.  The actual printing is done 
 * in the PrintBand() routine (see below), this procedure drives the 
 * printing operation.  PrintDIB() has the code to handle both banding 
 * and non-banding printers.  A banding printer can be distinguished by 
 * the GetDeviceCaps() API (see the code below).  On banding devices, 
 * must repeatedly call the NEXTBAND escape to get the next banding 
 * rectangle to print into.  If the device supports the BANDINFO escape, 
 * it should be used to determine whether the band "wants" text or 
 * graphics (or both).  On non-banding devices, we can ignore all this 
 * and call PrintBand() on the entire page. 
 * 
 * Parameters: 
 * 
 * HDIB hDib       - Handle to dib to be printed 
 * 
 * WORD fPrintOpt  - tells which print option to use (PW_BESTFIT, 
 *                   PW_STRETCHTOPAGE, OR PW_SCALE) 
 * 
 * WORD wXScale, wYScale - X and Y scaling factors (integers) for 
 *                   printed output if the PW_SCALE option is used. 
 * 
 * LPSTR szJobName - Name that you would like to give to this print job (this 
 *                   name shows up in the Print Manager as well as the 
 *                   "Now Printing..." dialog box). 
 * 
 * Return Value:  (see errors.h for description) 
 * 
 * One of: ERR_INVALIDHANDLE 
 *         ERR_LOCK 
 *         ERR_SETABORTPROC 
 *         ERR_STARTDOC 
 *         ERR_NEWFRAME 
 *         ERR_ENDDOC 
 *         ERR_GETDC 
 *         ERR_STRETCHDIBITS 
 * 
 ********************************************************************/ 
 
  WORD PrintDIB(HDIB hDib, WORD fPrintOpt, WORD wXScale, WORD wYScale, 
		LPSTR szJobName) 
    { 
      HDC                 hPrnDC;         /*  DC to the printer  */
      RECT                rect;           /*  Rect structure used for banding  */
      LPSTR               lpBits;         /*  pointer to the DIB bits  */
      LPBITMAPINFOHEADER  lpDIBHdr;       /*  Pointer to DIB header  */
      int                 nBandCount = 0; /*  used for print dialog box to count bands  */
      WORD                wErrorCode = 0; /*  Error code to return  */
      RECT                rPrintRect;     /*  specifies the area on the printer  */
      /*  (in printer coordinates) which we  */
      /*  want the DIB to go to  */
      char                szBuffer[70];   /*  Buffer to hold message for "Printing" dlg box  */
      char                szJobNameTrunc[35];     /*  szJobName truncated to 31  */
      /*  characters, since STARTDOC can't  */
      /*  accept a string longer than 31  */
      DOCINFO             DocInfo;        /*  structure for StartDoc  */
      int                 nTemp;          /*  used to check banding capability  */
      CHAR                lpBuffer[128];  /*  Buffer for strings retrieved from resources  */
 
 
      /*  Paramter validation  */
 
      if (!hDib) 
        return ERR_INVALIDHANDLE; 
 
      /*  Get pointer to DIB header  */
 
      lpDIBHdr = (LPBITMAPINFOHEADER)GlobalLock(hDib); 
      if (!lpDIBHdr) /*  Check that we have a valid pointer  */
        return ERR_LOCK; 
      lpBits = FindDIBBits((LPSTR)lpDIBHdr); /*  Find pointer to DIB bits  */
 
      if (hPrnDC = GetPrinterDC()) 
	{ 
	  SetStretchBltMode(hPrnDC, COLORONCOLOR); 
 
	  /*  Determine rPrintRect (printer area to print to) from the  */
	  /*  fPrintOpt.  Fill in rPrintRect.left and .top from wXScale and  */
	  /*  wYScale just in case we use PW_SCALE (see the function  */
	  /*  CalculatePrintRect).  */
 
	  rPrintRect.left = wXScale; 
	  rPrintRect.top = wYScale; 
	  CalculatePrintRect(hPrnDC, &rPrintRect, fPrintOpt, lpDIBHdr->biWidth, 
			     lpDIBHdr->biHeight); 
 
	  /*  Initialize the abort procedure.  */
 
	  hDlgAbort = CreateDialog(ghDLLInst, szPrintDlg, GetFocus(), 
				   (DLGPROC)PrintAbortDlg); 
 
	  /*  ISet the text inside the dialog to the name of our print job  */
 
	  lstrcpy(szJobNameTrunc, szJobName); 
	  szJobNameTrunc[31] = '\0';           /*  Truncate string to 31 chars  */
	  LoadString(ghDLLInst, IDS_PRINTMSG, lpBuffer, sizeof(lpBuffer)); 
	  wsprintf(szBuffer, lpBuffer, (LPSTR)szJobNameTrunc); 
	  SetDlgItemText(hDlgAbort, IDC_PRINTTEXT1, (LPSTR)szBuffer); 
 
	  /*  Set global variable bAbort to FALSE.  This will get set to TRUE  */
	  /*  in our PrintAbortDlg() procedure if the user selects the  */
	  /*  CANCEL button in our dialog box  */
 
	  bAbort = FALSE; 
 
	  /*  set up the Abort Procedure  */
 
	  if (SetAbortProc(hPrnDC, (ABORTPROC)PrintAbortProc) < 0) 
	    return ERR_SETABORTPROC; 
 
	  /*  start print job  */
 
	  ZeroMemory(&DocInfo, sizeof(DOCINFO)); 
	  DocInfo.cbSize = sizeof(DOCINFO); 
	  DocInfo.lpszDocName = (LPTSTR)szJobNameTrunc; 
	  DocInfo.lpszOutput = NULL; 
 
	  if (StartDoc(hPrnDC, &DocInfo) <= 0) 
	    { 
	      /*  Oops, something happened, let's clean up here and return  */
 
	      DestroyWindow(hDlgAbort);   /*  Remove abort dialog box  */
	      DeleteDC(hPrnDC); 
	      GlobalUnlock(hDib); 
	      return ERR_STARTDOC; 
	    } 
 
	  /*  Note: the following banding code applies to Windows 3.1.  With the new  */
	  /*        printing architecture of Win32, send out both the graphics and  */
	  /*        text in one band (like a non-banding device).  This code is used  */
	  /*        for Win32s since Win32s depends on Windows 3.1 printing architecture.  */
	  /*   */
	  /*  Check if need to do banding.  If we do, loop through  */
	  /*  each band in the page, calling NEXTBAND and BANDINFO  */
	  /*  (if supported) calling PrintBand() on the band.  Else,  */
	  /*  call PrintBand() with the entire page as our clipping  */
	  /*  rectangle!  */
 
	  /*  If Wincap32 is running on Win32s, then use banding  */
 
	  nTemp = NEXTBAND; 
	  if (Escape(hPrnDC, QUERYESCSUPPORT, sizeof(int), (LPSTR)&nTemp, NULL) && 
	      (GetVersion() & 0x80000000) && (LOWORD(GetVersion()) == 3)) 
	    { 
	      BOOL                bBandInfoDevice; 
	      BANDINFOSTRUCT      biBandInfo;         /*  Used for banding  */
 
	      /*  Fill in initial values for our BandInfo Structure to  */
	      /*  tell driver we can want to do graphics and text, and  */
	      /*  also which area we want the graphics to go in.  */
 
	      biBandInfo.bGraphics = TRUE; 
	      biBandInfo.bText = TRUE; 
	      biBandInfo.GraphicsRect = rPrintRect; 
 
	      /*  Check if device supports the BANDINFO escape.  */
 
	      nTemp = BANDINFO; 
	      bBandInfoDevice = Escape(hPrnDC, QUERYESCSUPPORT, sizeof(int), 
				       (LPSTR)&nTemp, NULL); 
 
	      /*  Do each band -- Call Escape() with NEXTBAND, then the  */
	      /*  rect structure returned is the area where we are to  */
	      /*  print in.  This loop exits when the rect area is empty.  */
 
	      while (Escape(hPrnDC, NEXTBAND, 0, NULL, (LPSTR)&rect) && ! 
		     IsRectEmpty(&rect)) 
		{ 
		  char szTmpBuf[100]; 
 
 
		  /*  Do the BANDINFO, if needed.  */
 
		  if (bBandInfoDevice) 
                    Escape(hPrnDC, BANDINFO, sizeof(BANDINFOSTRUCT), (LPSTR)& 
			   biBandInfo, (LPSTR)&biBandInfo); 
		  LoadString(ghDLLInst, IDS_BANDNMBR, lpBuffer, sizeof(lpBuffer)); 
		  wsprintf(szTmpBuf, lpBuffer, ++nBandCount); 
		  SetDlgItemText(hDlgAbort, IDC_PERCENTAGE, (LPSTR)szTmpBuf); 
 
		  /*  Call PrintBand() to do actual output into band.  */
		  /*  Pass in our band-info flags to tell what sort  */
		  /*  of data to output into the band.  Note that on  */
		  /*  non-banding devices, we pass in the default bandinfo  */
		  /*  stuff set above (i.e. bText=TRUE, bGraphics=TRUE).  */
 
		  wErrorCode = PrintBand(hPrnDC, &rPrintRect, &rect, 
					 biBandInfo.bText, biBandInfo.bGraphics, lpDIBHdr, 
					 lpBits); 
		} 
	    } 
	  else 
	    { 
	      /*  Print the whole page -- non-banding device.  */
 
	      if (StartPage(hPrnDC) <= 0) 
                return ERR_STARTPAGE; 
 
	      rect = rPrintRect; 
	      LoadString(ghDLLInst, IDS_SENDINGBAND, lpBuffer, sizeof(lpBuffer)); 
	      SetDlgItemText(hDlgAbort, IDC_PERCENTAGE, lpBuffer); 
	      wErrorCode = PrintBand(hPrnDC, &rPrintRect, &rect, TRUE, TRUE, 
				     lpDIBHdr, lpBits); 
 
	      /*  Non-banding devices need a NEWFRAME  */
 
	      if (EndPage(hPrnDC) <= 0) 
                return ERR_ENDPAGE; 
	    } 
 
 
	  /*  End the print operation.  Only send the ENDDOC if  */
	  /*   we didn't abort or error.  */
 
	  if (!bAbort) 
	    { 
	      /*  We errored out on ENDDOC, but don't return here - we still  */
	      /*  need to close the dialog box, free proc instances, etc.  */
 
	      if (EndDoc(hPrnDC) <= 0) 
                wErrorCode = ERR_ENDDOC; 
 
	      DestroyWindow(hDlgAbort); 
	    } 
 
	  /*  All done, clean up.  */
 
	  DeleteDC(hPrnDC); 
	} 
      else 
        wErrorCode = ERR_GETDC;   /*  Couldn't get Printer DC!  */
 
      GlobalUnlock(hDib); 
      return wErrorCode; 
    } 
 
 
 
 
  /*  *******************************************************************  */
  /*  Auxiliary Functions  */
  /*      -- Local to this module only  */
  /*  *******************************************************************  */
 
 
  /********************************************************************* 
 * 
 * CalculatePrintRect() 
 * 
 * Given fPrintOpt and a size of the DIB, return the area on the 
 * printer where the image should go (in printer coordinates).  If 
 * fPrintOpt is PW_SCALE, then lpPrintRect.left and .top should 
 * contain WORDs which specify the scaling factor for the X and 
 * Y directions, respecively. 
 * 
 ********************************************************************/ 
 
  void CalculatePrintRect(HDC hDC, LPRECT lpPrintRect, WORD fPrintOpt, 
			  DWORD cxDIB, DWORD cyDIB) 
    { 
      int  cxPage, cyPage, cxInch, cyInch; 
 
      if (!hDC) 
        return; 
 
      /*  Get some info from printer driver  */
 
      cxPage = GetDeviceCaps(hDC, HORZRES);   /*  Width of printr page - pixels  */
      cyPage = GetDeviceCaps(hDC, VERTRES);   /*  Height of printr page - pixels  */
      cxInch = GetDeviceCaps(hDC, LOGPIXELSX);/*  Printer pixels per inch - X  */
      cyInch = GetDeviceCaps(hDC, LOGPIXELSY);/*  Printer pixels per inch - Y  */
 
      switch (fPrintOpt) 
	{ 
 
	  /*  Best Fit case -- create a rectangle which preserves  */
	  /*  the DIB's aspect ratio, and fills the page horizontally.  */
 
	  /*  The formula in the "->bottom" field below calculates the Y  */
	  /*  position of the printed bitmap, based on the size of the  */
	  /*  bitmap, the width of the page, and the relative size of  */
	  /*  a printed pixel (cyInch / cxInch).  */
 
        case PW_BESTFIT: 
	  lpPrintRect->top = 0; 
	  lpPrintRect->left = 0; 
	  lpPrintRect->bottom = (int)(((double)cyDIB * cxPage * cyInch) / 
				      ((double)cxDIB * cxInch)); 
	  lpPrintRect->right = cxPage; 
	  break; 
 
	  /*  Scaling option -- lpPrintRect's top/left contain  */
	  /*  multipliers to multiply the DIB's height/width by.  */
 
        case PW_SCALE: 
	  { 
            int     cxMult, cyMult; 
 
            cxMult = lpPrintRect->left; 
            cyMult = lpPrintRect->top; 
            lpPrintRect->top = 0; 
            lpPrintRect->left = 0; 
            lpPrintRect->bottom = (int)(cyDIB * cyMult); 
            lpPrintRect->right = (int)(cxDIB * cxMult); 
            break; 
	  } 
 
	  /*  Stretch To Page case -- create a rectangle  */
	  /*  which covers the entire printing page (note that this  */
	  /*  is also the default).  */
 
        case PW_STRETCHTOPAGE: 
 
        default: 
	  lpPrintRect->top = 0; 
	  lpPrintRect->left = 0; 
	  lpPrintRect->bottom = cyPage; 
	  lpPrintRect->right = cxPage; 
	  break; 
	} 
    } 
 
 
  /********************************************************************* 
 * 
 * PrintBand() 
 * 
 * This routine does ALL output to the printer.  It is called from 
 * the PrintDIB() routine.  It is called for both banding and non- 
 * banding printing devices.  lpRectClip contains the rectangular 
 * area we should do our output into (i.e. we should clip our output 
 * to this area).  The flags fDoText and fDoGraphics should be set 
 * appropriately (if we want any text output to the rectangle, set 
 * fDoText to true).  Normally these flags are returned on banding 
 * devices which support the BANDINFO escape. 
 * 
 ********************************************************************/ 
 
  WORD PrintBand(HDC hDC, LPRECT lpRectOut, LPRECT lpRectClip, BOOL fDoText, 
		 BOOL fDoGraphics, LPBITMAPINFOHEADER lpDIBHdr, LPSTR lpDIBBits) 
    { 
      RECT    rect;           /*  Temporary rectangle  */
      double  dblXScaling,    /*  X and Y scaling factors  */
	dblYScaling; 
      WORD    wReturn = 0;    /*  Return code  */
 
      if (fDoGraphics) 
	{ 
	  dblXScaling = ((double)lpRectOut->right - lpRectOut->left) / (double) 
	    lpDIBHdr->biWidth; 
	  dblYScaling = ((double)lpRectOut->bottom - lpRectOut->top) / (double) 
	    lpDIBHdr->biHeight; 
 
	  /*  Now we set up a temporary rectangle -- this rectangle  */
	  /*  holds the coordinates on the paper where our bitmap  */
	  /*  WILL be output.  We can intersect this rectangle with  */
	  /*  the lpClipRect to see what we NEED to output to this  */
	  /*  band.  Then, we determine the coordinates in the DIB  */
	  /*  to which this rectangle corresponds (using dbl?Scaling).  */
 
	  IntersectRect(&rect, lpRectOut, lpRectClip); 
	  if (!IsRectEmpty(&rect)) 
	    { 
	      RECT    rectIn; 
 
	      rectIn.left = (int)((rect.left - lpRectOut->left) / dblXScaling + 
				  0.5 
				  ); 
	      rectIn.top = (int)((rect.top - lpRectOut->top) / dblYScaling + 0.5); 
	      rectIn.right = (int)(rectIn.left + (rect.right - rect.left) / 
				   dblXScaling + 0.5); 
	      rectIn.bottom = (int)(rectIn.top + (rect.bottom - rect.top) / 
				    dblYScaling + 0.5); 
	      if (!StretchDIBits(hDC, rect.left, rect.top, 
				 rect.right - rect.left, rect.bottom - rect.top, 
				 rectIn.left, (int)(lpDIBHdr->biHeight) - 
				 rectIn.top - (rectIn.bottom - rectIn.top), 
				 rectIn.right - rectIn.left, rectIn.bottom - rectIn.top, 
				 lpDIBBits, (LPBITMAPINFO)lpDIBHdr, DIB_RGB_COLORS, 
				 SRCCOPY)) 
                wReturn = ERR_STRETCHDIBITS; /*  StretchDIBits() failed!  */
	    } 
	} 
 
      return wReturn; 
    } 
 
 
  /*********************************************************************** 
 * 
 * GetPrinterDC() 
 * 
 * Uses PrinDlg common dialog for printer selection and creates a dc. 
 * Returns NULL on error. 
 * 
 ***********************************************************************/ 
 
  HDC GetPrinterDC() { 
 
    PRINTDLG pd; 
 
    ZeroMemory(&pd, sizeof(pd)); 
 
    pd.lStructSize = sizeof(PRINTDLG); 
    pd.Flags = PD_RETURNDC; 
 
    if (PrintDlg(&pd) == TRUE) 
      { 
        DEVNAMES    *pDevNames = GlobalLock(pd.hDevNames); 
 
        lstrcpy((LPSTR)gszDevice, 
                (LPSTR)((char *)pDevNames+pDevNames->wDeviceOffset)); 
 
        if(!lstrcmpi((LPSTR)((char *)pDevNames+pDevNames->wDeviceOffset), 
		     (LPSTR)((char *)pDevNames+pDevNames->wOutputOffset))) 
	  lstrcpy((LPSTR)gszOutput, "net:"); 
        else 
	  lstrcpy((LPSTR)gszOutput, 
		  (LPSTR)((char *)pDevNames+pDevNames->wOutputOffset)); 
 
        GlobalUnlock(pd.hDevNames); 
        return pd.hDC; 
      } 
 
    else 
      return NULL; 
  } 
 
 
  /********************************************************************** 
 * PrintAbortProc() 
 * 
 * Abort procedure - contains the message loop while printing is 
 * in progress.  By using a PeekMessage() loop, multitasking 
 * can occur during printing. 
 * 
 **********************************************************************/ 
 
  BOOL CALLBACK PrintAbortProc(HDC hDC, int code) 
    { 
      MSG  msg; 
 
      while (!bAbort && PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 
        if (!IsDialogMessage(hDlgAbort, &msg)) 
	  { 
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
	  } 
 
      return !bAbort; 
    } 
 
  /*********************************************************************** 
 * 
 * PrintAbortDlg() 
 * 
 * 
 * This is the Dialog Procedure which will handle the "Now Printing" 
 * dialog box.  When the user presses the "Cancel" button, the 
 * global variable bAbort is set to TRUE, which causes the 
 * PrintAbortProc to exit, which in turn causes the printing 
 * operation to terminate. 
 * 
 ***********************************************************************/ 
 
  int CALLBACK PrintAbortDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
    { 
      switch (msg) 
	{ 
        case WM_INITDIALOG: 
	  { 
            char szBuffer[100]; 
 
            /*  Fill in the text which specifies where this bitmap  */
            /*  is going ("on HP LaserJet on LPT1", for example)  */
            LoadString(ghDLLInst, IDS_ABORTSTRING, msgBuffer, sizeof(msgBuffer)); 
            wsprintf(szBuffer, msgBuffer, (LPSTR)gszDevice, 
		     (LPSTR)gszOutput); 
            SetDlgItemText(hWnd, IDC_PRINTTEXT2, (LPSTR)szBuffer); 
            SetFocus(GetDlgItem(hWnd, IDCANCEL)); 
            return TRUE;     /*  Return TRUE because we called SetFocus()  */
	  } 
 
        case WM_COMMAND: 
	  bAbort = TRUE; 
	  DestroyWindow(hWnd); 
	  return TRUE; 
	  break; 
	} 
 
      return FALSE; 
    } 
 
#endif

#define NEW_DIB_FORMAT(lpbih) (lpbih->biSize != sizeof(BITMAPCOREHEADER))

  /******************************************************************************
 *                                                                            *
 *  FUNCTION   : DIBNumColors(LPVOID lpv)                                     *
 *                                                                            *
 *  PURPOSE    : Determines the number of colors in the DIB by looking at     *
 *               the BitCount and ClrUsed fields in the info block.           *
 *                                                                            *
 *  RETURNS    : The number of colors in the DIB. With DIBS with more than    *
 *               8-bits-per-pixel that have a color table table included,     *
 *               then the return value will be the number of colors in the    *
 *               color table rather than the number of colors in the DIB.     *
 *                                                                            *
 *                                                                            *
 *****************************************************************************/
  WORD DIBNumColors (LPVOID lpv)
    {
      INT                 bits;
      LPBITMAPINFOHEADER  lpbih = (LPBITMAPINFOHEADER)lpv;
      LPBITMAPCOREHEADER  lpbch = (LPBITMAPCOREHEADER)lpv;

      /*  With the BITMAPINFO format headers, the size of the palette
       *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
       *  is dependent on the bits per pixel ( = 2 raised to the power of
       *  bits/pixel).
       */
      if (NEW_DIB_FORMAT(lpbih)) {
	if (lpbih->biClrUsed != 0)
	  return (WORD)lpbih->biClrUsed;
	bits = lpbih->biBitCount;
      }
      else
	bits = lpbch->bcBitCount;

      if (bits > 8) 
	return 0; /* Since biClrUsed is 0, we dont have a an optimal palette */
      else
	return (1 << bits); 
    }

  /******************************************************************************
 *                                                                            *
 *  FUNCTION   :  ColorTableSize(LPVOID lpv)                                  *
 *                                                                            *
 *  PURPOSE    :  Calculates the palette size in bytes. If the info. block    *
 *                is of the BITMAPCOREHEADER type, the number of colors is    *
 *                multiplied by 3 to give the palette size, otherwise the     *
 *                number of colors is multiplied by 4.                        *
 *                                                                            *
 *  RETURNS    :  Color table size in number of bytes.                        *
 *                                                                            *
 *****************************************************************************/
  WORD ColorTableSize (LPVOID lpv)
    {
      LPBITMAPINFOHEADER lpbih = (LPBITMAPINFOHEADER)lpv;
    
      if (NEW_DIB_FORMAT(lpbih))
	{
	  if (((LPBITMAPINFOHEADER)(lpbih))->biCompression == BI_BITFIELDS)
	    /* Remember that 16/32bpp dibs can still have a color table */
	    return (sizeof(DWORD) * 3) + (DIBNumColors (lpbih) * sizeof (RGBQUAD));
	  else
	    return (DIBNumColors (lpbih) * sizeof (RGBQUAD));
	}
      else
	return (DIBNumColors (lpbih) * sizeof (RGBTRIPLE));
    }

  
