#include "wingui.h"
#include "xdvi-config.h"
#include "gsdll.h"
#include "version.h"

#ifndef WIN32
#include "WWWLib.h"
#include "WWWInit.h"
#endif

#ifdef HTEX
/* Application name and version for w3c-libwww routines.  
   This is what will show up in httpd's agent_log files. 
*/
#ifdef Omega
char *HTAppName = "owindvi";
#else
char *HTAppName = "windvi";
#endif
char *HTAppVersion = XDVERSION;

/* Create the anchor information stuff at the bottom of the page */

/* Anchor search: a dialog box */
/* anchorinfo = Ascii text widget */

char anchorsearchstring[1024];
char anchorask[] = "Access new URL:";

#endif

#define mask_mag(ev) ((ev & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)) && !(ev & (MK_SHIFT | MK_CONTROL)))
#define mask_sethome(ev) ((ev & MK_LBUTTON) && (ev & MK_SHIFT))
#define mask_srcspecial(ev) ((ev & MK_RBUTTON) && (ev & MK_SHIFT))
#define mask_opennewframe(ev) ((ev & MK_LBUTTON) && (ev & MK_CONTROL))

/*****************************************************************************
  Drawing child window 
  ****************************************************************************/
HWND hWndDraw;
BOOL bDrawKeep = FALSE;
HDC hdcDrawSave = 0;
static HRGN hrgnScroll = 0;

LRESULT CALLBACK DrawProc(HWND, UINT, WPARAM, LPARAM);

/* Drawing window message table definition. */
MSD rgmsdDraw[] =
{
    {WM_CREATE,     MsgDrawCreate    },
    /*    {WM_SIZE,       MsgDrawSize      }, */
    {WM_HSCROLL,    MsgDrawHScroll   },
    {WM_VSCROLL,    MsgDrawVScroll   },
    {WM_MOUSEMOVE,  MsgDrawMousemove },
    /* {WM_COMMAND,    MsgDrawCommand   }, */
    {WM_PAINT,      MsgDrawPaint     },
    /*    {WM_ERASEBKGND, MsgDrawEraseBkgnd}, */
    /*    {WM_NCPAINT,    MsgDrawNCPaint     }, */
    {WM_LBUTTONDOWN,MsgDrawEnterMagL },
    {WM_LBUTTONUP,  MsgDrawQuitMagL  },
    {WM_MBUTTONDOWN,MsgDrawEnterMagM },
    {WM_MBUTTONUP,  MsgDrawQuitMagL  },
    {WM_RBUTTONDOWN,MsgDrawEnterMagR },
    {WM_RBUTTONUP,  MsgDrawQuitMagL  }
};

MSDI msdiDraw =
{
    sizeof(rgmsdDraw) / sizeof(MSD),
    rgmsdDraw,
    edwpWindow
};

/* Drawing window command table definition. */
CMD rgcmdDraw[] =
{
  {IDM_FILEOPEN, CmdStub}
};

CMDI cmdiDraw =
{
    sizeof(rgcmdDraw) / sizeof(CMD),
    rgcmdDraw,
    edwpNone
};

/*****************************************************************************
  Magnifying child window 
  ****************************************************************************/
HWND hWndMagnify;
HDC magDC, magMemDC;
BOOL bMagDisp = FALSE;		/* Mag. Glass is displayed */
BOOL bSetHome = FALSE;		/* User is setting home position */
BOOL bSrcSpecial = FALSE;   /* User wants to jump to the next src special */

int magWidth, magHeight;

LRESULT CALLBACK MagnifyProc(HWND, UINT, WPARAM, LPARAM);

/* Magnifying window message table definition. */
MSD rgmsdMagnify[] =
{
    {WM_CREATE,     MsgMagnifyCreate    },
    /*    {WM_SIZE,       MsgMagnifySize      },
    {WM_HSCROLL,    MsgMagnifyHScroll   },
    {WM_VSCROLL,    MsgMagnifyVScroll   },
    {WM_MOUSEMOVE,  MsgMagnifyMousemove },
    {WM_COMMAND,    MsgMagnifyCommand   }, */
    /* {WM_ERASEBKGND, MsgMagnifyEraseBkgnd}, */
    {WM_PAINT,      MsgMagnifyPaint     }
};

MSDI msdiMagnify =
{
    sizeof(rgmsdMagnify) / sizeof(MSD),
    rgmsdMagnify,
    edwpWindow
};

/* Magnifying window command table definition. */
CMD rgcmdMagnify[] =
{
  {IDM_FILEOPEN, CmdStub}
};

CMDI cmdiMagnify =
{
    sizeof(rgcmdMagnify) / sizeof(CMD),
    rgcmdMagnify,
    edwpNone
};

/*

  Magnifying glass

 */
BOOL CreateMagnify(HWND hwndParent)
{
  extern HBRUSH foreBrush, backBrush;
  extern HPEN forePen;

  hWndMagnify = CreateWindow("MagnifyGlass",
			     NULL,
			     WS_BORDER | WS_POPUP | WS_DISABLED,
			     /* | WS_CLIPSIBLINGS, */
			     0, 0, 50, 50,
			     hwndParent,
			     NULL,
			     hInst, 
			     NULL);
  if (hWndMagnify == NULL) {
    Win32Error("CreateWindow/Magnify");
    return FALSE;
  }
  alt.win = hWndMagnify;
  magDC = GetDC(hWndMagnify);

#if 1
#ifdef TRANSFORM
  if (IS_NT)
    SetGraphicsMode(magDC, GM_ADVANCED);
#endif
#endif

  if (resource.in_memory) {
    magMemDC = CreateCompatibleDC(magDC);
#ifdef TRANSFORM
  if (IS_NT)
    SetGraphicsMode(magDC, GM_ADVANCED);
#endif
  }
  else {
    magMemDC = magDC;
  }

  SetBkMode(magDC, OPAQUE);
  SetBkColor(magDC, back_Pixel);

  return TRUE;
}


LRESULT CALLBACK MagnifyProc(HWND hwnd, UINT uMessage, 
			  WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiMagnify, hwnd, uMessage, wparam, lparam);
}

LRESULT MsgMagnifyCreate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  RECT r;
#if 0
  GetClientRect(hwnd, &r);
  if (wparam) FillRect((HDC)wparam, &r, backBrush);
#endif
  return 0;
}

LRESULT MsgMagnifyEraseBkgnd(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  RECT r;
  GetClientRect(hwnd, &r);
  FillRect((HDC)wparam, &r, backBrush);
  return TRUE;
}

LRESULT MsgMagnifyPaint(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  PAINTSTRUCT ps;
  POINT ptOrg = { 0, 0};

  BeginPaint(hwnd, &ps);

  GetWindowOrgEx(maneDrawDC, &ptOrg);

  if (resource.in_memory) {
    HBRUSH oldBrush;
#if 0
    oldBrush = SelectObject(magDC, foreBrush);
#endif
    /* We have to paint in ps.rcPaint, from memory dib. */
    if (!BitBlt(magDC, 
		ps.rcPaint.left, ps.rcPaint.top,
		ps.rcPaint.right - ps.rcPaint.left, 
		ps.rcPaint.bottom - ps.rcPaint.top,
		magMemDC, 
		ps.rcPaint.left, 
		ps.rcPaint.top, SRCCOPY))
      Win32Error("Draw Magnify/BitBlt");
#if 0
    SelectObject(magDC, oldBrush);
#endif
  }
  else {
    /* We have to paint directly onto the screen */
    extern void redraw(struct WindowRec *);
    RECT altRect;

    /* swapping to mag. glass device contexts */
    foreGC =  ruleGC =  highGC = ps.hdc;

    alt.base_x = (xMousePos + xCurrentScroll + ptOrg.x)*mane.shrinkfactor - magWidth;
    alt.base_y = (yMousePos + yCurrentScroll + ptOrg.y)*mane.shrinkfactor - magHeight;
    alt.min_x = ps.rcPaint.left;
    alt.min_y = ps.rcPaint.top;
    alt.max_x = ps.rcPaint.right;
    alt.max_y = ps.rcPaint.bottom;
    alt.width = ps.rcPaint.right - ps.rcPaint.left;
    alt.height = ps.rcPaint.bottom - ps.rcPaint.bottom;
#if 0
    fprintf(stderr, "Painting in base (%d %d) (%d %d) - (%d %d)\n",
	    alt.base_x, alt.base_y, alt.min_x, alt.min_y, alt.max_x, alt.max_y);
#endif
#if 1
    /* draw background */
    {
      RECT rectBg;
      rectBg.left = max(0, alt.min_x);
      rectBg.top = max(0, alt.min_y);
      rectBg.right = min(alt.max_x, unshrunk_paper_w - alt.base_x);
      rectBg.bottom = min(alt.max_y, unshrunk_paper_h - alt.base_y);
      FillRect(magDC, &rectBg, backBrush);
    }
#if 0
    draw_rulers(alt.width, alt.height, magDC);
#endif
#endif
    allowDrawingChars = TRUE;
    redraw(&alt);

    if (resource.reverse) {
      PatBlt(foreGC, alt.min_x, alt.min_y, alt.max_x, alt.max_y, DSTINVERT);
    }
    /* and back to drawing window dcs */
    foreGC =  ruleGC =  highGC = maneDrawDC;
  }

  EndPaint(hwnd, &ps);
  return 0;
}

/*
  Process messages for the about box.
  */

LRESULT CALLBACK DrawProc(HWND hwnd, UINT uMessage, 
			  WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiDraw, hwnd, uMessage, wparam, lparam);
}

/*
  CreateDraw(hWndParent) : creates the drawing window (child with 
  scrollbars).
  */
BOOL CreateDraw(HWND hwndParent)
{
  hWndDraw = CreateWindowEx(WS_EX_CLIENTEDGE, 
			    TEXT("ClientDrawClass"),
			    NULL,
			    WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS
			    | WS_HSCROLL | WS_VSCROLL,
			    -100, -100, 10, 10,
			    hwndParent,
			    NULL,
			    hInst, 
			    NULL);
  if (hWndDraw == NULL) {
    Win32Error("CreateWindowEx");
    return FALSE;
  }

  /* Initialize all graphic related variables */

  InitGlobalsDraw();

  SizeClientWindow(hWndDraw);
  SetScrollBars(hWndDraw);
  SetCursor(hCursArrow);
  ShowWindow(hWndDraw, SW_SHOW);

  make_temporary_dir(&temporary_dir); /* For storing temporary files... */

  if (dvi_name) {
    char            *title_name;
    char            *icon_name;
    
    /*
     *      Open the dvi file and set titles
     */
    set_directory_dvifile();
#ifdef HTEX
#ifndef WIN32
    {
      char *cp = getenv("WWWBROWSER"); 
      if (cp) browser = cp;
    }
    HTProfile_newPreemptiveClient(HTAppName, HTAppVersion); 
    HTCacheMode_setEnabled(NO);
    
#if 0   
    /* Optionally, turn on tracing of WWW library calls. */
    if (debug & DBG_HYPER) {
      WWWTRACE = SHOW_STREAM_TRACE; 
    }
#endif
#endif /* WIN32 */
    
#if T1
	/* At this point DISP, our_visual, our_depth and our_colormap must
	   be defined, and they are */
	init_t1();
#endif

    /* Open the input file.  Turn filename into URL first if needed */
    URL_aware = TRUE;

    if (!(URLbase || htex_is_url(dvi_name))) {
      char *new_name;
      int n;
      
      n = strlen(dvi_name);
      new_name = dvi_name;
      /* Find the right filename */
      if (n < sizeof(".dvi")
	  || strcmp(dvi_name + n - sizeof(".dvi") + 1, ".dvi") != 0) {
	dvi_name = xmalloc((unsigned) n + sizeof(".dvi"));
	Strcpy(dvi_name, new_name);
	Strcat(dvi_name, ".dvi");
	/* Atempt $dvi_name.dvi */
	dvi_file = xfopen(dvi_name, OPEN_MODE);
	if (dvi_file == NULL) {
	  /* Didn't work, revert to original $dvi_name */
	  free(dvi_name);
	  dvi_name = new_name;
	}
      }
      
      /* Turn filename into URL */
      /* Escape dangers, esp. # from emacs */
#ifdef WIN32
      new_name = xmalloc((unsigned) strlen(dvi_name)+6);
      strcat(strcpy(new_name,"file:"),dvi_name);
      free(dvi_name);
      dvi_name = new_name;
#else
      new_name=HTEscape(dvi_name,URL_PATH);
      free(dvi_name);
      dvi_name = xmalloc((unsigned) strlen(new_name)+6);
      strcat(strcpy(dvi_name,"file:"),new_name);
#endif
      /* Now we have the right filename, in a URL */
    }

#if 0
    __asm int 3;
#endif
    detach_anchor();
    if (!open_www_file()) {
      fprintf(stderr,"Could not open dvi file. Exiting.\n");
      Exit(1);
    }
    htex_reinit();
    URL_aware = FALSE;
#else
    open_dvi_file();
#endif /* not HTEX */

#ifdef XFORM
	reset_xform_stack();
#endif
#ifdef SRC_SPECIALS
	src_delete_all_specials();
#endif
    if (curr_page) {
      current_page = (*curr_page ? atoi(curr_page) : total_pages) - 1;
      if (current_page < 0 || current_page >= total_pages) usage();
    }
#if 1
    set_icon_and_title (dvi_name, &icon_name, &title_name, 1);
#endif

    {
      int new_shrink;
      if (resource.shrinkfactor <= 0)
	new_shrink = ChooseShrink();
      else
	new_shrink = resource.shrinkfactor;
      
      reconfig();
      ChangeZoom(new_shrink);
#if 0
      redraw_page();
      ChangePage(0);
      /* FIXME : why 2 calls ? */
      redraw_page();
#endif
      ChangePage(0);
    }
  }

  return TRUE;
}

#if 0
LRESULT MsgDrawCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  return 0;
}

LRESULT MsgDrawNCPaint(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  fprintf(stderr, "NC Paint hwnd = %x, wparam = %x, lparam = %x\n", 
	  hwnd, wparam, lparam);
  /*  return DefWindowProc(hwnd, uMessage, wparam, lparam); */
  return 0;
}
#endif

LRESULT MsgDrawMousemove(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  char szBuf[80];		/* Array for formatting mouse coordinates */
  RECT altRect, oldRect;
  extern void redraw(struct WindowRec *);
  extern double p2u_factor;
  extern const char *pos_format;
#if 0
  HRGN hrgnMag;
  RECT clipRect;
#endif
  int xDelta, yDelta;
  POINT ptOrg = { 0, 0};

  GetWindowOrgEx(maneDrawDC, &ptOrg);

  xMousePos = LOWORD(lparam);
  yMousePos = HIWORD(lparam);

  if (bMagDisp && mask_mag(wparam)) {
    POINT absPos;
    /* redraw mag. glass content */

    /* adjust magnifying glass position */
    GetWindowRect(hWndMagnify, &oldRect);
    absPos.x = xMousePos - magWidth - 1; 
    absPos.y = yMousePos - magHeight - 1;
    ClientToScreen(hwnd, &absPos);
    if (SetWindowPos(hWndMagnify, HWND_TOPMOST, 
		     absPos.x, absPos.y, 
		     magWidth*2/* +2 */, magHeight*2 /* +2 */,
		     SWP_NOACTIVATE | SWP_NOSIZE /* |  SWP_SHOWWINDOW */
		     /* | SWP_NOCOPYBITS */
		     | SWP_NOOWNERZORDER | SWP_NOZORDER) == 0)
      Win32Error("SetWindowPos");
    UpdateWindow(hwnd);
#if 1
    /* UpdateWindow(hWndMagnify); */
    /* Need to redisplay the toolbar too ! */
    UpdateWindow(hWndToolbar);
#endif
    /*
      Redraw the in-memory bitmap for the mag. glass
    */
    alt.base_x = (xMousePos + xCurrentScroll)*mane.shrinkfactor - magWidth;
    alt.base_y = (yMousePos + yCurrentScroll)*mane.shrinkfactor - magHeight;
    GetClientRect(alt.win, &altRect);
    alt.min_x = altRect.left;
    alt.min_y = altRect.top;
    alt.width = altRect.right - altRect.left;
    alt.height = altRect.bottom - altRect.top;
    alt.max_x = altRect.right;
    alt.max_y = altRect.bottom;

    /* Better than invalidating the whole rectangle, scroll the window */
    xDelta = (oldRect.left - absPos.x)*mane.shrinkfactor;
    yDelta = (oldRect.top - absPos.y)*mane.shrinkfactor;

#if 0
    /* FIXME : the whol mag glass is redrawn, not only the part invalidated 
       or is this because the laptop has no advanced graphic feature ? */
    fprintf(stderr, "scrolling by %d %d\n", xDelta, yDelta);
#endif
    if (xDelta || yDelta) {
      RECT prcUpdate;
      if (ScrollWindowEx(hWndMagnify, 
			 xDelta, yDelta,
			 NULL, NULL, NULL, &prcUpdate, 
			 SW_INVALIDATE | SW_ERASE ) == ERROR)
	Win32Error("ScrollWindowEx");
#if 0
      fprintf(stderr, "prcUpdate : (%d %d) - (%d %d) mag (%d %d)\n",
	      prcUpdate.left, prcUpdate.top,
	      prcUpdate.right, prcUpdate.bottom, magWidth, magHeight);
#endif
    }

    if (resource.in_memory) {
#if 0
      SelectObject(magMemDC, backBrush);
#endif
      if (!FillRect(magMemDC, &altRect, backBrush))
	Win32Error("FillRect Magnifying Glass");
#if 0
      /* FIXME : it would be nice to see the page's border in the mag. glass */
      PatBlt(magMemDC, 
	     max(alt.min_x, -alt.base_x), 
	     max(alt.min_y, -alt.base_y),
	     min(alt.max_x, ),
	     min(alt.max_y, ),
	     WHITENESS);
#endif
      redraw(&alt);
      /* FIXME : how to put these rulers once for all and bitblt
	 on top of them ? */
#if 0
      draw_rulers(alt.width, alt.height, magMemDC);
#endif
    }
    UpdateWindow(hWndMagnify);
  }
  sprintf(szBuf, pos_format, 
	  (xMousePos + xCurrentScroll + ptOrg.x) * mane.shrinkfactor * p2u_factor,
	  (yMousePos + yCurrentScroll + ptOrg.y) * mane.shrinkfactor * p2u_factor);
  UpdateStatusBar(szBuf, 6, 0);
  return 0;
}

void
EnterMag(HWND hwnd, int xMousePos, int yMousePos)
{
  RECT rcClient, rcPage, rcClip, rcMag;
  POINT ptClientUL, ptClientLR;
  POINT mousePos, absPos, wndOrg;

  extern int numColors;
  HWND hWndTop = GetTopWindow(NULL);

  /* Retrieve the page dimensions */
  rcPage.left = 0;
  rcPage.top = 0; 
  rcPage.right = page_w - 1;
  rcPage.bottom = page_h - 1;

  /* Get Window origin */
  GetWindowOrgEx(maneDrawDC, &wndOrg);

  mousePos.x = xMousePos + wndOrg.x;
  mousePos.y = yMousePos + wndOrg.y;

  if (!PtInRect(&rcPage, mousePos)) {
	return;
  }

  if (1 /* hWndTop == hWndMain */) {
	RECT rc;
#if 0
	fprintf(stderr, "Entering mag and saving main window\n");
#endif
	GetClientRect(hWndMain,&rc);
	BitBlt(hdcDrawSave, 0,0,rc.right,rc.bottom,
		   maneDC, 0,0,SRCCOPY);  
	bDrawKeep = TRUE; 
  }

  /* FIXME: this causes a redisplay bug if xdvi is not the topmost
     window */
  bMagDisp = TRUE;

  /* Capture mouse input */
  SetCapture(hwnd);
  /* Retrieve the screen coordinates of the client area, 
     and convert them into client coordinates.  */
  GetClientRect(hwnd, &rcClient);
  /* Intersect both rectangles. The mag. glass will be limited
     to the actual visible part of the page. */
  IntersectRect(&rcClip, &rcClient, &rcPage);
  ptClientUL.x = rcClip.left; 
  ptClientUL.y = rcClip.top;  
  /* Add one to the right and bottom sides, because the 
     coordinates retrieved by GetClientRect do not 
     include the far left and lowermost pixels.  */
  ptClientLR.x = rcClip.right; 
  ptClientLR.y = rcClip.bottom; 

  /* FIXME : GetWindowOrgEx() to translate the points */
  ClientToScreen(hwnd, &ptClientUL); 
  ClientToScreen(hwnd, &ptClientLR);  
  /* Copy the client coordinates of the client area 
     to the rcClip structure. Confine the mouse cursor 
     to the client area by passing the rcClip structure 
     to the ClipCursor function. */
#if 0
  fprintf(stderr, "Org %d %d\n", wndOrg.x, wndOrg.y);
#endif
  SetRect(&rcClip, 
	  ptClientUL.x - wndOrg.x, 
	  ptClientUL.y - wndOrg.y, 
	  ptClientLR.x - wndOrg.x,
	  ptClientLR.y - wndOrg.y);
  ClipCursor(&rcClip); 
  /* Calculates the new position and size of the magnifying glass */
  mousePos.x = xMousePos - magWidth - 1; 
  mousePos.y = yMousePos - magHeight - 1;
  /*  ClientToScreen(hwnd, &mousePos); */
  rcMag.left = mousePos.x + 1;
  rcMag.top = mousePos.y + 1;
  /* FIXME : +1 ? */
  rcMag.right = rcMag.left + 2*magWidth + 1;
  rcMag.bottom = rcMag.top + 2*magHeight + 1;

  if (resource.in_memory) {
    if (oldmagDIB) {
      /* There is an old magDIB, put it back in the DC
	 and delete the current one */
      if ((magDIB = SelectObject(magMemDC, oldmagDIB)) == NULL)
	Win32Error("SelectObject/oldmagDIB");
      if (DeleteObject(magDIB) == FALSE)
	Win32Error("DeleteObject/magDIB");
    }

    magDIB = CreateDIB(magMemDC, 2*magWidth, 2*magHeight, 16, NULL, NULL);
    if ((oldmagDIB = SelectObject(magMemDC, magDIB)) == NULL)
      Win32Error("SelectObject/magDIB");

    if (!FillRect(magMemDC, &rcMag, backBrush))
      Win32Error("FillRect magMemDC background");

    foreGC = ruleGC = highGC = magMemDC;
  }

  /* redraw mag. glass content */
  absPos.x = xMousePos - magWidth - 1; 
  absPos.y = yMousePos - magHeight - 1;
  ClientToScreen(hwnd, &absPos);
  if (SetWindowPos(hWndMagnify, HWND_TOPMOST, 
		   absPos.x, 
		   absPos.y, magWidth*2/*+2*/, magHeight*2/*+2*/,
		   SWP_NOACTIVATE | SWP_SHOWWINDOW
		   | SWP_NOOWNERZORDER | SWP_NOZORDER) == 0)
    Win32Error("SetWindowPos");
  /* ShowWindow(hWndMagnify, SW_SHOW); */
}

LRESULT MsgDrawEnterMagL(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  if (bSkipFirstClick) {
	bSkipFirstClick = FALSE;
	return 0;
  }

  if (bMagDisp || bSetHome)
	return 0;

  if (mask_sethome(wparam)) {
    RECT rcClip, rcPage, rcClient;
    POINT ptClientUL, ptClientLR;
    bSetHome = TRUE;
    SetCursor(hCursCross);
    SetCapture(hwnd);
    /* Retrieve the screen coordinates of the client area, 
       and convert them into client coordinates.  */
    GetClientRect(hwnd, &rcClient);
    /* Retrieve the page dimensions */
    rcPage.top = rcPage.left = 0;
    rcPage.right = page_w - 1;
    rcPage.bottom = page_h - 1;
    /* Intersect both rectangles. The mag. glass will be limited
       to the actual visible part of the page. */
    IntersectRect(&rcClip, &rcClient, &rcPage);
    ptClientUL.x = rcClip.left; 
    ptClientUL.y = rcClip.top;  
    /* Add one to the right and bottom sides, because the 
       coordinates retrieved by GetClientRect do not 
       include the far left and lowermost pixels.  */
    ptClientLR.x = rcClip.right; 
    ptClientLR.y = rcClip.bottom; 
    ClientToScreen(hwnd, &ptClientUL); 
    ClientToScreen(hwnd, &ptClientLR);  
    /* Copy the client coordinates of the client area 
       to the rcClip structure. Confine the mouse cursor 
       to the client area by passing the rcClip structure 
       to the ClipCursor function. */
    SetRect(&rcClip, ptClientUL.x, ptClientUL.y, 
	    ptClientLR.x, ptClientLR.y);             
    ClipCursor(&rcClip); 
    UpdateStatusBar("Setting home position...", 0, 0);
  }
  else {
#ifdef HTEX
    int	x, y;
    if (mask_opennewframe(wparam)) {
      HTeXnext_extern = 1;
    }
    else {
      HTeXnext_extern = 0;
    }
    if (pointerlocate(&x, &y)) {
      /* Only do this if there's actually an href right there */
      int ret;
      /* screen_to_page(&mane,x,y,&page,&px,&py); */
      ret = htex_handleref(current_page, x, y);
      HTeXnext_extern = 0;
      if (ret == 1) return 0;
    }
#endif
    magWidth = mg_size[0].w / 2;
    magHeight = mg_size[0].h / 2;
    EnterMag(hwnd, LOWORD(lparam), HIWORD(lparam));
  }
  return 0;
}

LRESULT MsgDrawEnterMagM(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
#if 0
  /* Allow to open a new frame only if not running in single
     instance mode ! */
  if (!resource.single_flag) {
    int	x, y;
    if (pointerlocate(&x, &y)) {
      /* Only do this if there's actually an href right there */
      int ret;
      /* screen_to_page(&mane,x,y,&page,&px,&py); */
      HTeXnext_extern = 1;
      ret = htex_handleref(current_page, x, y);
      HTeXnext_extern = 0;
      if (ret == 1) return 0;
    }
  }
#endif
  if (bSkipFirstClick) {
    bSkipFirstClick = FALSE;
    return 0;
  }

  if (bMagDisp || bSetHome)
	return 0;

  magWidth = mg_size[1].w / 2;
  magHeight = mg_size[1].h / 2;
  EnterMag(hwnd, LOWORD(lparam), HIWORD(lparam));

  return 0;
}

LRESULT MsgDrawEnterMagR(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  if (bSkipFirstClick) {
    bSkipFirstClick = FALSE;
    return 0;
  }
  if (bMagDisp || bSetHome)
	return 0;

#ifdef SRC_SPECIALS
  if (mask_srcspecial(wparam)) {
	if (src_evalMode) {
	  int x, y;
	  POINT ptOrg = { 0, 0};
	  GetWindowOrgEx(maneDrawDC, &ptOrg);
	  x = min(xMousePos + xCurrentScroll + ptOrg.x, (unsigned)page_w)/* * mane.shrinkfactor */;
	  y = min(yMousePos + yCurrentScroll + ptOrg.y, (unsigned)page_h)/* * mane.shrinkfactor */;
	  src_find_special(1, x, y);
	}
  }
  else
#endif
	{
	  magWidth = mg_size[2].w / 2;
	  magHeight = mg_size[2].h / 2;
	  EnterMag(hwnd, LOWORD(lparam), HIWORD(lparam));
	}
  return 0;
}

LRESULT MsgDrawQuitMagL(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  POINT ptOrg = { 0, 0};

  GetWindowOrgEx(maneDrawDC, &ptOrg);

  if (bSetHome) {
    char szBuf[128];
    /* Restore cursor */
    bSetHome = FALSE;
    ClipCursor(NULL);
    ReleaseCapture();
    SetCursor(hCursArrow);
    home_x = min(xMousePos + xCurrentScroll + ptOrg.x, (unsigned)page_w) * mane.shrinkfactor;
    home_y = min(yMousePos + yCurrentScroll + ptOrg.y, (unsigned)page_h) * mane.shrinkfactor;
    if (resource.sidemargin) free(resource.sidemargin);
    resource.sidemargin = pixtoa(home_x);
    if (resource.topmargin) free(resource.topmargin);
    resource.topmargin = pixtoa(home_y);
    
    wsprintf(szBuf, "Setting home to %5d, %5d", home_x, home_y);
    UpdateStatusBar(szBuf, 0, 0);
  }
  else if (bMagDisp) {
    /* removes the window */
    foreGC = ruleGC = highGC = maneDrawDC;
    ShowWindow(hWndMagnify, SW_HIDE);
    bMagDisp = FALSE;
    UpdateWindow(hWndDraw);
    /* restores the old shrink factor and redisplay page */
    ClipCursor(NULL);
    ReleaseCapture();
  }
  return 0;
}

LRESULT MsgDrawCreate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  HDC hDC = GetDC(hwnd);
  HBITMAP hBitmap;
  int width, height;

  /* Scroll Bars */
  xMinScroll = 0;
  xMaxScroll = page_w;
  /* mane.base_x =  */xCurrentScroll = 0;
  
  yMinScroll = 0;
  yMaxScroll = page_h;
  /* mane.base_y =  */yCurrentScroll = yMinScroll;

  fScroll = FALSE;
  fSize = FALSE;

  /* When the main window is created, allocate a memory context */
  /*   of the appropriate size */
  hdcDrawSave = CreateCompatibleDC(hDC);
  /* Create a bitmap buffer for hdcMemSave that is */
  /*   the same size as the physical screen */
  width = GetDeviceCaps(hDC, HORZRES);
  height = GetDeviceCaps(hDC, VERTRES);
  hBitmap = CreateCompatibleBitmap(hDC, width, height);
  SelectObject(hdcDrawSave, hBitmap);  
  DeleteObject(hBitmap);
  ReleaseDC(hwnd,hDC);

#if 0
  /* Scrolled Region */
  if (hrgnScroll ==0)
      hrgnScroll = CreateRectRgn(0, 0, 10, 10);
#endif
  return 0;
}

LRESULT MsgDrawHScroll(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  RECT r;
  int xDelta;
  int xNewPos;
  int yDelta = 0;

  bDrawKeep = FALSE;

  GetClientRect(hwnd, &r);
  
  switch (LOWORD(wparam)) {
  case SB_TOP:
    xNewPos = xMinScroll;
    break;
  case SB_BOTTOM:
    xNewPos = xMaxScroll;
    break;
  case SB_PAGEUP:
    xNewPos = xCurrentScroll - (((r.right - r.left) * 2) / 3);
    break;
  case SB_PAGEDOWN:
    xNewPos = xCurrentScroll + (((r.right - r.left) * 2) / 3);
    break;
  case SB_LINEUP:
    xNewPos = xCurrentScroll - 5;
    break;
  case SB_LINEDOWN:
    xNewPos = xCurrentScroll + 5;
    break;
  case SB_THUMBPOSITION:
    xNewPos = HIWORD(wparam);
    break;
  default:
    xNewPos = xCurrentScroll;
  }


  xNewPos = max(xMinScroll, xNewPos);
  xNewPos = min(xMaxScroll - max(r.right - 1, 0), xNewPos);

#if 0
  fprintf(stderr, "r.right = %d [%d %d %d]\n",
	  r.right, xMinScroll, xCurrentScroll, xMaxScroll);
  fprintf(stderr, "xNewPos = %d\n", xNewPos);
#endif

  if (xNewPos == xCurrentScroll)
    return 0;
      
  fScroll = TRUE;
      
  xDelta = xNewPos - xCurrentScroll;
  /* mane.base_x =  */xCurrentScroll = xNewPos;
  ScrollWindowEx(hwnd, -xDelta, -yDelta, NULL, NULL,
		 (HRGN)NULL, (LPRECT)NULL,
		 SW_INVALIDATE | SW_ERASE);
#if 1
  UpdateWindow(hwnd);
#endif      
  si.cbSize = sizeof(si);
  si.fMask = SIF_POS;
  si.nPos = xCurrentScroll;
  SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
  return 0;
}

LRESULT MsgDrawVScroll(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  RECT r;

  int xDelta = 0;
  int yNewPos;
  int yDelta;

  bDrawKeep = FALSE;

  GetClientRect(hwnd, &r);
  if (yCurrentScroll == yMinScroll
      && current_page > 0
      && (LOWORD(wparam) == SB_TOP 
	  || LOWORD(wparam) == SB_PAGEUP
	  /* || LOWORD(wparam) == SB_LINEUP */)) {
    ChangePage(-1);
    wparam = MAKEWORD(SB_BOTTOM, HIWORD(wparam));
  }
  else if (yCurrentScroll == yMaxScroll - max(r.bottom - 1, 0)
	   && current_page < total_pages - 1
	   && (LOWORD(wparam) == SB_BOTTOM 
	       || LOWORD(wparam) == SB_PAGEDOWN
	       /* || LOWORD(wparam) == SB_LINEDOWN */)) {
    ChangePage(+1);
    wparam = MAKEWORD(SB_TOP, HIWORD(wparam));
  }
  switch (LOWORD(wparam)) {
  case SB_TOP:
    yNewPos = yMinScroll;
    break;
  case SB_BOTTOM:
    yNewPos = yMaxScroll;
    break;
  case SB_PAGEUP:
    yNewPos = yCurrentScroll - (((r.bottom - r.top) * 2) / 3);
    break;
  case SB_PAGEDOWN:
    yNewPos = yCurrentScroll + (((r.bottom - r.top) * 2) / 3);
    break;
  case SB_LINEUP:
    yNewPos = yCurrentScroll - 5;
    break;
  case SB_LINEDOWN:
    yNewPos = yCurrentScroll + 5;
    break;
  case SB_THUMBPOSITION:
    yNewPos = HIWORD(wparam);
    break;
  default:
    yNewPos = yCurrentScroll;
  }
  yNewPos = max(yMinScroll, yNewPos);
  yNewPos = min(yMaxScroll - max(r.bottom - 1, 0), yNewPos);
      
  if (yNewPos == yCurrentScroll)
    return 0;
      
  fScroll = TRUE;
       
  yDelta = yNewPos - yCurrentScroll;
  /* mane.base_y =  */yCurrentScroll = yNewPos;
  ScrollWindowEx(hwnd, -xDelta, -yDelta, NULL, NULL,
		 (HRGN)NULL, (LPRECT)NULL,
		 SW_INVALIDATE | SW_ERASE);
#if 0
  UpdateWindow(hwnd);
#endif
      
  si.cbSize = sizeof(si);
  si.fMask = SIF_POS;
  si.nPos = yCurrentScroll;
  SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
  return 0;
}

LRESULT MsgDrawEraseBkgnd(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  RECT r;
  HDC hdc = (HDC)wparam;

  if (isPrinting) return TRUE;

  GetClientRect(hwnd, &r);
  
  if (resource.in_memory) {
    FillRect(hdc, &r, GetStockObject (LTGRAY_BRUSH));
  }
  else {

  if (debug & DBG_EVENT)
    fprintf(stderr, "Erasing background with colors %x\n", back_Pixel);

    FillRect(hdc, &r, GetStockObject (LTGRAY_BRUSH));
    if (dvi_file) {
      r.bottom = min(page_h - yCurrentScroll, r.bottom);
      r.right = min(page_w - xCurrentScroll, r.right);

  if (debug & DBG_EVENT)
      fprintf(stderr, "Erasing background, Page is in (%d x %d)\n",
	      r.right, r.bottom);

      FillRect(hdc, &r, backBrush);
      
    }
  }
  return TRUE;
}

/*
  We have 2 alternatives :

  - using a page for the text and BitBlt'ing this page onto the screen
  But there is a serious drawback : at scale 1 and 600dpi, a color
  page is about 34Mb... Not anybody can afford this easily. At least
  not W9x users. So we have to do banding. Anyway, this is needed for
  printing. This is the only way to guarantee that the file will be
  printed on any printer.

  - directly drawing on the screen. This is rather efficient for the display.

  We choose among these 2 methods thanks to the resource.in_memory flag.
*/

void DrawInMemory(HDC hdcDest, HDC hdcFrom, LPRECT lprcPaint)
{
  unsigned int rop;
  RECT rcGSPaint;
  extern void reset_xfrm_stack(void);

  /* If no file open, just return */
  if (!dvi_name || !*dvi_name)
    return;

  if (!bMagDisp) {
    mane.base_x = xCurrentScroll;
    mane.base_y = yCurrentScroll;
    mane.min_x = lprcPaint->left;
    mane.min_y = lprcPaint->top;
    mane.max_x = lprcPaint->right;
    mane.max_y = lprcPaint->bottom;
    mane.width = lprcPaint->right - lprcPaint->left;
    mane.height = lprcPaint->bottom - lprcPaint->top;
    
    /*
      We have the page in memory. This page consist of all the pk chars
      and rules/boxes.
      
      As pictures are drawn one onto the other, we need some kind of
      transparent blt.  AFAIK, the special rop code used here does the
      trick. But it is slow.  
      
    */
    
#ifdef TRANSFORM
    /* FIXME : there should be a global stack in case
       such transformations would span over several pages. */
    if (resource.use_xform)
      reset_xfrm_stack();
#endif
    
    /* Erasing background */
    {
      RECT r;
      r.left = mane.min_x;
      r.top = mane.min_y;
      r.right = min(page_w - xCurrentScroll, mane.max_x);
      r.bottom = min(page_h - yCurrentScroll, mane.max_y);
      FillRect(hdcFrom, &r, backBrush);
    }
    
    /* First pass for specials */
    allowDrawingChars = FALSE;
    
    redraw(&mane);
    
#if PS
    if (resource._postscript && psToDisplay) {
      rop = /* 0x008E1D7C */ SRCCOPY;
    
      /* if gs is active, try to make it paint its picture */
      if (gs_device) {
	rcGSPaint.left = lprcPaint->left+xCurrentScroll;
	rcGSPaint.right = lprcPaint->right+xCurrentScroll;
	rcGSPaint.top = lprcPaint->top+yCurrentScroll;
	rcGSPaint.bottom = lprcPaint->bottom+yCurrentScroll;
#if 0
	fprintf(stderr, "gs: redrawing\n");
#endif
	(*pgsdll_draw)(gs_device, hdcFrom, lprcPaint, &rcGSPaint);
      }
    }
    else
#endif
      rop = SRCCOPY;
    
    allowDrawingChars = TRUE;
    
    redraw(&mane);
  }

  /* We have to paint in lprcPaint. */
  if (!BitBlt(hdcDest, 
	      lprcPaint->left, lprcPaint->top,
	      lprcPaint->right - lprcPaint->left,
	      lprcPaint->bottom - lprcPaint->top,
	      hdcFrom, 
	      lprcPaint->left+xCurrentScroll, 
	      lprcPaint->top+yCurrentScroll, SRCCOPY)) {
    Win32Error("Draw Page/BitBlt");
    fprintf(stderr, "BitBlt page failed\nRectangle to be painted : l = %d t = %d r = %d b = %d\n",
	    lprcPaint->left, lprcPaint->top,
	    lprcPaint->right, lprcPaint->bottom);
    fprintf(stderr, "Rectangle to be updated : l = %d t = %d r = %d b = %d\n",
	    lprcPaint->left, lprcPaint->top, 
	    lprcPaint->right, lprcPaint->bottom);
    fprintf(stderr, "Rectangle for page : l = %d t = %d r = %d b = %d\n",
	    0, 0, page_w - xCurrentScroll, page_h - yCurrentScroll); 
    fprintf(stderr, "Page width = %d\nPage height = %d\nScroll %d %d\n",
	    page_w, page_h, xCurrentScroll, yCurrentScroll);
  }
}


void DrawOnScreen(HDC hdcDest, HDC hdcFrom, LPRECT lprcPaint)
{
  extern void reset_xfrm_stack(void);
  /*
    We want to draw directly onto the screen.
    */

  mane.base_x = xCurrentScroll;
  mane.base_y = yCurrentScroll;
  mane.min_x = lprcPaint->left;
  mane.min_y = lprcPaint->top;
  mane.max_x = lprcPaint->right;
  mane.max_y = lprcPaint->bottom;
  mane.width = lprcPaint->right - lprcPaint->left;
  mane.height = lprcPaint->bottom - lprcPaint->top;
  
  /* If no file open, just return */
  if (!dvi_name || !*dvi_name)
    return;

#ifdef TRANSFORM
  /* FIXME : there should be a global stack in case
     such transformations would span over several pages. */
  if (resource.use_xform)
    reset_xfrm_stack();
#endif

  /*
    FIXME : remove this and process directly.
  */

  /* First pass for specials */
  allowDrawingChars = FALSE;

  redraw(&mane);

#if PS
  if (resource._postscript && psToDisplay) {
    /* if gs is active, try to make it paint its picture */
    if (gs_device) {
#if 0
      fprintf(stderr, "gs: redrawing\n");
      fprintf(stderr, "repaint : %d %d %d %d\n",
	      lprcPaint->left, lprcPaint->top,
	      lprcPaint->right, lprcPaint->bottom);
#endif
      (*pgsdll_lock_device)(gs_device, 1);
      (*pgsdll_draw)(gs_device, hdcDest, lprcPaint, lprcPaint);
      (*pgsdll_lock_device)(gs_device, 0);
    }
  }

  allowDrawingChars = TRUE;

#if 0
  fprintf(stderr, "repaint : %d %d %d %d ps = %d\n",
	  lprcPaint->left, lprcPaint->top,
	  lprcPaint->right, lprcPaint->bottom, psToDisplay);
#endif
  redraw(&mane);
#endif

  if (resource.reverse) {
    PatBlt(hdcDest, lprcPaint->left, lprcPaint->top, lprcPaint->right, lprcPaint->bottom, DSTINVERT);
  }

}

void DoDrawPaint(HDC hdcDest, HDC hdcFrom, LPRECT lprcPaint)
{
  RECT rcToPaint, rcPage;

  rcPage.left = 0;
  rcPage.right = page_w - xCurrentScroll;
  rcPage.top = 0;
  rcPage.bottom = page_h - yCurrentScroll;

  IntersectRect(&rcToPaint, lprcPaint, &rcPage);

  if (resource.in_memory) 
    DrawInMemory(hdcDest, hdcFrom, &rcToPaint);
  else
    DrawOnScreen(hdcDest, hdcFrom, &rcToPaint);
}

LRESULT MsgDrawPaint(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  PAINTSTRUCT ps;
  
  /* No redrawing while printing. Should not happen, but who knows ? */
  HDC hDC = BeginPaint(hwnd, &ps);

  if (!isPrinting) {

	/* FIXME : Is there anyway to optimize this
	   wrt to the mag. glass moving ? */
    if (bDrawKeep || bMagDisp) {
      /* Repaint from the memory context */
      BitBlt(ps.hdc,  ps.rcPaint.left, ps.rcPaint.top,
	     (ps.rcPaint.right - ps.rcPaint.left),
	     (ps.rcPaint.bottom - ps.rcPaint.top),
	     hdcDrawSave, ps.rcPaint.left,ps.rcPaint.top, SRCCOPY); 
    }
    else {
      /* New data, so need to do some work */
      RECT rc;  
	  HWND hWndTop = GetTopWindow(NULL);
#if 0
	  fprintf(stderr, "Redrawing (%d, %d) - (%d x %d)\n",
			  ps.rcPaint.left, ps.rcPaint.top,
			  ps.rcPaint.right - ps.rcPaint.left,
			  ps.rcPaint.bottom - ps.rcPaint.top);
#endif
      DoDrawPaint(maneDC, maneDrawDC, &(ps.rcPaint));
      /* The screen has been redrawn manually; now update */
      /*   the memory context and set fKeep=TRUE. */
	  if (hWndTop == hWndMain /* || hWndTop == hWndDraw || hWndTop == hWndMagnify */) {
		GetClientRect(hwnd,&rc);
		BitBlt(hdcDrawSave, 0,0,rc.right,rc.bottom,
			   hDC, 0,0,SRCCOPY);  
		bDrawKeep = TRUE; 
	  }
	  else {
#if 0
		fprintf(stderr, "Active window is *not* draw window\n");
#endif
	  }
    }
  }
  EndPaint(hwnd, &ps);
  return 0;
}

/*
  FUNCTION: InitGlobalsDraw()
  PURPOSE : Initialize all graphic-relative variables needed.
            Can be done only after hWndDraw has been created.
	    Must be done before trying to open a dvi file.
*/

void InitGlobalsDraw()
{
  extern void initcolor(void);
  extern void init_xfrm_stack(void);

  currwin.win = mane.win = hWndDraw;

  /* It has its own_dc */
  maneDC = GetDC(hWndDraw);

  maneHorzRes = GetDeviceCaps(maneDC, HORZRES);
  maneVertRes = GetDeviceCaps(maneDC, VERTRES);
  maneLogPixelsX = GetDeviceCaps(maneDC, LOGPIXELSX);
  maneLogPixelsY = GetDeviceCaps(maneDC, LOGPIXELSY);
  maneBitsPixel = GetDeviceCaps(maneDC, BITSPIXEL);
  maneSizePalette = GetDeviceCaps(maneDC, SIZEPALETTE);
  maneColorRes = GetDeviceCaps(maneDC, COLORRES);
  maneRasterCaps = GetDeviceCaps(maneDC, RASTERCAPS);

#ifdef GREY
  if (maneBitsPixel >= 8 && use_grey) {
    numColors = 16;
  }
  else {
    numColors = 1;
  }
#else
  numColors = 1;
#endif

  if (!(maneRasterCaps & RC_BITBLT)) {
    MessageBox(hWndMain, 
	       "Can't use the BitBlt() function\r\non your display. Exiting...",
	       NULL, MB_OK | MB_ICONERROR | MB_APPLMODAL);
    CleanExit(1);
  }
  if (!(maneRasterCaps & RC_STRETCHDIB)) {
    MessageBox(hWndMain, 
	       "Can't use the StretchDIBits() function\r\non your display. Exiting...",
	       NULL, MB_OK | MB_ICONERROR | MB_APPLMODAL);
    CleanExit(1);
  }
  if (!(maneRasterCaps & RC_BITMAP64)) {
    MessageBox(hWndMain, 
	       "Device does not support >64k bitmaps. Exiting...",
	       NULL, MB_OK | MB_ICONERROR | MB_APPLMODAL);
    CleanExit(1);
  }
#if 0
  fprintf(stderr, "Caps = %x\n", maneRasterCaps);
  fprintf(stderr, "Hres = %d, Yres = %d, lpx = %d, lpy = %d\n",
	  maneHorzRes, maneVertRes, maneLogPixelsX, maneLogPixelsY);
  fprintf(stderr, "bits/pix = %d, size palette = %d, colorres = %d\n",
	  maneBitsPixel, maneSizePalette, maneColorRes);
#endif

#if 0
  /* Device context for drawing and the associated bitmap. */
  if ((imageDC = CreateCompatibleDC(maneDC)) == NULL)
    Win32Error("CreateCompatibleDC imageDC");
#endif
  
  if (resource.in_memory) {
    maneDrawMemDC = CreateCompatibleDC(maneDC);
    if (maneDrawMemDC == NULL)
      Win32Error("CreateCompatibleDC maneDrawMemDC");

    foreGC = ruleGC = highGC = maneDrawDC = maneDrawMemDC;

    grid1GC = grid2GC = grid3GC = maneDrawDC;
    foreGC2 = NULL;		/* not used under Win32 */
  }
  else {
    foreGC = ruleGC = highGC = maneDrawDC = GetDC(hWndDraw);
    grid1GC = grid2GC = grid3GC = foreGC;
    magMemDC = foreGC2 = NULL;		/* not used under Win32 */
  }

#ifdef TRANSFORM
  if (IS_NT)
    SetGraphicsMode(maneDrawDC, GM_ADVANCED);

  if (resource.use_xform) {
    if (IS_WIN95) {
      fprintf(stderr, "The xform feature is not available under Win95.\n");
      fprintf(stderr, "Use the View -> Options dialog box to configure\n");
      fprintf(stderr, "or run `windvi +xform' to reset the option.\n");
    }
    else {
      init_xfrm_stack();
    }
  }
#endif

  SetStretchBltMode(foreGC, COLORONCOLOR); 
  SetStretchBltMode(maneDC, COLORONCOLOR); 
 
  initcolor();


  /* This little image is not used anymore. We are doing 
     everything straight in bitmaps */
  image = XCreateImage(DISP, maneDrawDC, numColors, XYBitmap, 0,
		       (char *)NULL, 0, 0, BMBITS, 0);


  bmi1.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
  bmi1.bmiHeader.biPlanes   = 1;
  bmi1.bmiHeader.biBitCount = 1;
  bmi1.bmiHeader.biCompression        = BI_RGB;
  bmi1.bmiHeader.biXPelsPerMeter      = 0;
  bmi1.bmiHeader.biYPelsPerMeter      = 0;
  bmi1.bmiHeader.biClrUsed            = 0;
  bmi1.bmiHeader.biClrImportant       = 0;

  bmi4.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
  bmi4.bmiHeader.biPlanes   = 1;
  bmi4.bmiHeader.biBitCount = 4;
  bmi4.bmiHeader.biCompression        = BI_RGB;
  bmi4.bmiHeader.biXPelsPerMeter      = 0;
  bmi4.bmiHeader.biYPelsPerMeter      = 0;
  bmi4.bmiHeader.biClrUsed            = 0;
  bmi4.bmiHeader.biClrImportant       = 0;

  bmi8.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
  bmi8.bmiHeader.biPlanes   = 1;
  bmi8.bmiHeader.biBitCount = 8;
  bmi8.bmiHeader.biCompression        = BI_RGB;
  bmi8.bmiHeader.biXPelsPerMeter      = 0;
  bmi8.bmiHeader.biYPelsPerMeter      = 0;
  bmi8.bmiHeader.biClrUsed            = 0;
  bmi8.bmiHeader.biClrImportant       = 0;

  bmi24.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
  bmi24.bmiHeader.biPlanes   = 1;
  bmi24.bmiHeader.biBitCount = 24;
  bmi24.bmiHeader.biCompression        = BI_RGB;
  bmi24.bmiHeader.biXPelsPerMeter      = 0;
  bmi24.bmiHeader.biYPelsPerMeter      = 0;
  bmi24.bmiHeader.biClrUsed            = 0;
  bmi24.bmiHeader.biClrImportant       = 0;


  /*
    Setting geometry
    */
  /* Where to create the window ?
     Thanks to suggestions by Stanley A. Sawyer <sawyer@math.wustl.edu>
     this code has been improved.
     -------------------------------------------------------------------------
     The main window will be loaded at (xval,yval) (the upper-left (UL)
     corner of the window) with widths (xsize,ysize).
     The physical screen size is stored in global variables (maxx,maxy).
     The code below first checks if there is an instance of a program of
     type ``window class=vshowclass'' already running. If so, the new window
     is cascaded off the old by v4size pixels at the UL corner.
     ``FindWindow()'' seems to find the last-run instance, so that the
     result can be a nice cascade of windows.
     If there are no pre-existing instances, the code then checks for a
     Taskbar on either the left-hand-side or the top of the screen. If the
     Taskbar is in either position, (xval,yval) is set to avoid overlaying
     them. If not, the default xval=yval=10 is used.
     The code can be improved by checking the Taskbar position if the
     previous instance window is too far from the UL corner of the screen,
     but the following should work fine in most cases.
     -------------------------------------------------------------------------
  */
  /*
    FIXME : should we implement several policies 
    - user might want to open the new window exactly
    on top of the previous one,
    - or cascade windows,
    - or ...
  */
  {
    int off_x, off_y, width, height;
    int v4size = 25; /* cascading by this number of pixels */
    RECT rectDesktop, r;
    extern BOOL bPrevInstance;
    extern RECT rectWndPrev;
    APPBARDATA abd = { sizeof(APPBARDATA), NULL };
    int abd_width = 0, abd_height = 0;

    GetWindowRect(hWndMain, &r);
    off_x = r.left;
    off_y = r.top;
    width = r.right - r.left;
    height = r.bottom - r.top;
      
    /* Get APPBAR information */
    abd.cbSize = sizeof(APPBARDATA);
    if (SHAppBarMessage(ABM_GETTASKBARPOS, &abd)) {
      /* The Taskbar is at positions    */
      /*   abd.rc.left, abd.rc.right, abd.rc.top, abd.rc.bottom */
      /* The variable at the long screen boundary is negative, */
      /*   which locates the Taskbar.  */
#if 0
      fprintf(stderr, "Appbar : (%d %d) -- (%d %d)\n",
	      abd.rc.left, abd.rc.top,
	      abd.rc.right, abd.rc.bottom);
#endif
      if (abd.rc.left<0 && abd.rc.right < maneHorzRes/2) {
	abd_width = abd.rc.right;  
      }
      else if (abd.rc.top<0 && abd.rc.bottom < maneVertRes/2) {
	abd_height = abd.rc.bottom; 
      }
    }

    if (bPrevInstance && !resource.single_flag) {
      /* if there is a previous instance, rely on it. */
      off_x = (rectWndPrev.left> maneHorzRes / 3) 
	? 10 + v4size 
	: rectWndPrev.left+v4size;
      off_y = (rectWndPrev.top > maneVertRes / 3)  
	? 10 + v4size 
	: rectWndPrev.top+v4size; 
      width = rectWndPrev.right - rectWndPrev.left;
      height = rectWndPrev.bottom - rectWndPrev.top;
#if 0
      fprintf(stderr, "Prev inst : @(%d,%d) (%d x %d)\n",
	      off_x, off_y, width, height);
#endif
    }

    else if (geometry) {
      RECT r;
      int flag = XParseGeometry(geometry, &off_x, &off_y,
				&width, &height);
#ifndef WIN32
      if (flag & (XValue | YValue))
	size_hints.flags |= USPosition;
      if (flag & (WidthValue | HeightValue))
	size_hints.flags |= USSize;
#endif
      if (flag & XNegative) off_x += maneHorzRes - width;
      if (flag & YNegative) off_y += maneVertRes - height;
	}
    else {
      /*
	if no geometry has been given, make the main window big enough.
      */
      width = (int)(maneVertRes - abd_height * 0.618);
      height = maneVertRes - abd_height;
#if 0
      fprintf(stderr, "Nothing : @(%d,%d) (%d x %d)\n",
	      off_x, off_y, width, height);
#endif
    }
    if (!(MoveWindow(hWndMain, 
		     off_x, off_y, 
		     width, height,
		     TRUE)))
      Win32Error("no Geometry/MoveWindow");
  }
  
}
