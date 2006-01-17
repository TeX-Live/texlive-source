/*
  This file gathers all color related functions.

  Problems related to color support :

  - color support means supporting the specials introduced by the
  color.sty LaTeX package 

  1 - supporting 8bit color modes means build a palette for each page

  2 - supporting \pagebackground{} means tracking which fore/background
  color at the beginning of each page ! Is it possible using prescaning ?
  That means keeping track in a stack of the background changes
  and pages where they did occur. Idem for fore colors. Then it should
  be possible at beginning of each page to have the right fore_Pixel
  and back_Pixel values.  

  */

#include "wingui.h"
#include "xdvi-config.h"

#define DEBUG_COLOR 0
  
LOGPALETTE *lpLogPal = NULL;
PALETTEENTRY pe;		/* Dummy, used everywhere */
int iLengthPal = 0;

/* Memorize the page background colors changes */
struct _page_bk {
  COLORREF color;
  int page;			/* page where the \pagecolor{} occurs */
} PageBackColor[1024];

struct _page_fr {
  COLORREF color;
  int page;			/* this page begins with a non-empty color
				   stack */
  lpCRefStack stack;		/* Stack of COLORREF to memorize the state
				 when this page is entered... */
} PageForeColor[1024];

int LastBackChange = 0, LastForeChange = 0;

/* Stack of colors used during prescanning. 
   Whenever this stack is not empty at the end of the page,
   it is memoized for the next page.
*/

lpCRefStack scan_fore_colors;

extern RGBQUAD QuadWhite, QuadBlack;

/* CRefStack functions */

lpCRefStack InitCRefStack(int size) 
{
  lpCRefStack stack;
  stack = (lpCRefStack)xmalloc(sizeof(CRefStack));
  stack->s = (COLORREF *)xmalloc(sizeof(COLORREF)*size); 
  stack->max = size;
  stack->i = 0;
  return stack;
}

BOOL CRefIsEmpty(lpCRefStack s)
{
  return (s->i == 0);
}

BOOL CRefIsInit(lpCRefStack s)
{
  return ((s->i == 1) && (CRefTop(s) == MYRGB(0,0,0)));
}

void CRefPush(lpCRefStack s, COLORREF c)
{
  if (s->i < s->max) {
    s->s[s->i] = c;
#if DEBUG_COLOR
    fprintf(stderr, "CRefPush %-8x\n", c);
#endif
    ++(s->i);
  }
  else {
    fprintf(stderr, "Error: too many pushes into stack !\n");
#if 0
    MessageBox(hWndMain, "Max color stack depth reached", NULL, 
	       MB_APPLMODAL | MB_ICONHAND | MB_OK );
#endif
  }
}

COLORREF CRefPop(lpCRefStack s)
{
  if (s->i > 0) {
    --(s->i);
#if DEBUG_COLOR
    fprintf(stderr, "CRefPop %-8x\n", s->s[s->i]);
#endif
    return(s->s[s->i]);
  }
  else {
    fprintf(stderr, "Error : too many pop from the stack !\n");
#if 0
    MessageBox(hWndMain, "Popped too many colors from stack", NULL, 
	       MB_APPLMODAL | MB_ICONHAND | MB_OK);
#endif
    return s->s[0];
  }
}

COLORREF CRefTop(lpCRefStack s)
{
  if (s->i > 0) {
    return s->s[s->i - 1];
  }
  else {
    fprintf(stderr, "Error : no top, stack is empty !\n");
    return MYRGB(0,0,0);
  }
}

lpCRefStack CRefSave(lpCRefStack s)
{
  lpCRefStack new;
  new = (lpCRefStack)xmalloc(sizeof(CRefStack));
  new->max = new->i = 0;
  new->s = NULL;
  if (s->i > 0) {
    new->s = (COLORREF *)xmalloc(s->i*sizeof(COLORREF));
    memcpy(new->s, s->s, s->i*sizeof(COLORREF));
    new->i = new->max = s->i;
  }
  return new;
}

void CRefRestore(lpCRefStack dst, lpCRefStack src)
{
  if (dst->max < src->i) {
    fprintf(stderr, "CRefRestore : not enough space !\n");
    return;
  }
  dst->i = src->i;
  memcpy(dst->s, src->s, src->i*sizeof(COLORREF));
}

void CRefDump(lpCRefStack s, char *msg)
{
  int i;
  fprintf(stderr, "%s ", msg);
  if (s) {
    fprintf(stderr, "max %d, top %d ", s->max, s->i);
    for(i = 0; i < s->i; i++)
      fprintf(stderr, "%-8x ", s->s[i]);
    fprintf(stderr, "\n");
  }
  else
    fprintf(stderr, "(null)\n");
}

void CRefResetInit(lpCRefStack s, COLORREF c)
{
  s->i = 0;
  CRefPush(s, c);
}

void CRefFree(lpCRefStack s)
{
  if (s != NULL) {
    if (s->s != NULL)
      free(s->s);
    free(s);
  }
}

COLORREF ColorInvert(COLORREF c)
{
  return MYRGB(255-GetRValue(c), 255-GetGValue(c), 255-GetBValue(c));
}

  
RGBQUAD Color2Quad(COLORREF c)
{
  RGBQUAD q;
  q.rgbReserved = 0;
  q.rgbRed = GetRValue(c);
  q.rgbGreen = GetGValue(c);
  q.rgbBlue = GetBValue(c);
  return q;
}

void
dump_colors(char *msg)
{
  int i;
  fprintf(stderr, "%s\n", msg);
  fprintf(stderr," scanned_page = %d, page  = %d\n", scanned_page, current_page);
  for (i = 0; i <= LastBackChange; i++)
    fprintf(stderr, "PageBackColor[%d] = (%d, %-8x)\n",
	    i, PageBackColor[i].page, PageBackColor[i].color);
  for (i = 0; i < LastForeChange; i++) {
    fprintf(stderr, "PageForeColor[%d] = (%d, %-8x)\n",
	    i, PageForeColor[i].page, PageForeColor[i].color);
    CRefDump(PageForeColor[i].stack, "local stack");
  }
  CRefDump(scan_fore_colors, "scan_fore_colors");
  CRefDump(color_stack, "color_stack");

  fprintf(stderr, "\n");
}
  
#ifdef GREY

void initialize_palette()
{
  int i;
  /* The palette should be initialized with 16 grey levels right now. */
  /* Later on, we could add the grid colors. */
#if DEBUG_COLOR
  fprintf(stderr, "Initializing Palette");
#endif

  if (lpLogPal)
    free(lpLogPal);

  lpLogPal = (LPLOGPALETTE) xmalloc(sizeof(LOGPALETTE) 
				    + 256*sizeof(PALETTEENTRY));
  lpLogPal->palVersion = 0x300;
  lpLogPal->palNumEntries = iLengthPal = 16;

  pe.peFlags = PC_NOCOLLAPSE;
  for (i = 0; i < 16; i++) {
    pe.peRed = GetRValue(palette[i]);
    pe.peGreen = GetGValue(palette[i]);
    pe.peBlue = GetBValue(palette[i]);
    lpLogPal->palPalEntry[i] = pe;
  }
    
}

COLORREF add_color_palette(COLORREF c)
{
  int i, i_min;
  int min = 256*3, dist;
  COLORREF cret = c;

  pe.peRed = GetRValue(c);
  pe.peGreen = GetGValue(c);
  pe.peBlue = GetBValue(c);

  /* Look if any value close is already in there */
  for (i_min = i = 0; i < iLengthPal; i++)
    if ((dist = abs(lpLogPal->palPalEntry[i].peRed - pe.peRed)
	 + abs(lpLogPal->palPalEntry[i].peGreen - pe.peGreen)
	 + abs(lpLogPal->palPalEntry[i].peBlue - pe.peBlue)) < min) {
      i_min = i;
      min = dist;
    }

  if (min <= 3 || iLengthPal == 256) {
    cret = MYRGB(lpLogPal->palPalEntry[i_min].peRed,
		 lpLogPal->palPalEntry[i_min].peGreen,
		 lpLogPal->palPalEntry[i_min].peBlue);
  }
  else {
#if DEBUG_COLOR
    fprintf(stderr, "adding %-8x to palette in position %d\n", c,
	    lpLogPal->palNumEntries + 1);
#endif
    
    lpLogPal->palPalEntry[iLengthPal++] = pe;
    lpLogPal->palNumEntries += 1;
  }
#if DEBUG_COLOR
  fprintf(stderr, "add_color_palette %-8x -> %-8x\n", c, cret);
#endif

  setup_palette(pal_rgb, 16);

  return cret;
}

void setup_palette(RGBQUAD *palette, int num)
{  
  return;
  /* Put up a palette if device is palettized */
  /* FIXME : is it meaningful at all ? */
  if (maneRasterCaps & RC_PALETTE) {
    HPALETTE hpal, hOldPal;
#if DEBUG_COLOR
    fprintf(stderr, "Setting up palette...\n");
#endif
    if ((hpal = CreatePalette(lpLogPal)) == NULL)
      Win32Error("CreatePalette");
    if ((hOldPal = SelectPalette(maneDC, hpal, TRUE)) == NULL)
      Win32Error("SelectPalette");
    DeleteObject(hOldPal);
    if (RealizePalette(maneDC) == GDI_ERROR)
      Win32Error("RealizePalette");
  }
}

/*
  Initialize only pixeltbl[] and palette[] with black on white.
*/
Boolean
init_pix(COLORREF back, COLORREF fore)
{
  static	Boolean	use_palette;
  static	int	shrink_allocated_for = 0;
  static	float	oldgamma	= 0.0;
  static        COLORREF old_back, old_fore;
  static        Boolean bFirstTime = True;


  int	i;
  Boolean	warn = False;
  extern BOOL bColorPage;
  unsigned int	sf_squared;
  BYTE fc_red, fc_green, fc_blue;
  BYTE bc_red, bc_green, bc_blue;

#if DEBUG_COLOR
  fprintf(stderr, "Init Pix : back %-8x fore %-8x\n",
	  back, fore);
#endif
  /* Maybe nothing to do ! */

  /* We can't do antialiasing on 8 bits depth screen. */
  if ((back == CLR_INVALID) 
      || ((maneBitsPixel <= 8) && (fore != MYRGB(0,0,0)))) {
    int limit = 5;

#if 0
    if (back == CLR_INVALID) fprintf(stderr, "Back is invalid\n");
    fprintf(stderr, "maneBitsPixel = %d\n", maneBitsPixel);
#endif

    for(i = 0; i < limit; i++) {
      pal_bw[i] = QuadWhite;
      pal_mask[i] = QuadBlack;
    }
    for(i = limit; i < 16; i++) {
      pal_bw[i] = QuadBlack;
      pal_mask[i] = Color2Quad(fore_Pixel);
    }
    old_back = back;
    old_fore = fore;
    return False;
  }

  if (!bFirstTime 
      && (old_fore == fore) && (old_back == back)
      && (gamma == oldgamma)) {
    return False;
  }

  bFirstTime = False;
  old_back = back;
  old_fore = fore;
  oldgamma = gamma;

  /* FIXME : should use back and fore ? */
  fc_red = GetRValue(fore);
  fc_green = GetGValue(fore);
  fc_blue = GetBValue(fore);
  bc_red = GetRValue(back);
  bc_green = GetGValue(back);
  bc_blue = GetBValue(back);
  
#if DEBUG_COLOR
  fprintf(stderr, "Initializing pix fore = %-8x, back = %-8x ...\n", fore, back);
#endif

  for (i = 0; i < 16; ++i) {
    double	frac = gamma > 0 ? pow((double) i / 15, 1 / gamma)
      : 1 - pow((double) (15 - i) / 15, -gamma);
    
    palette[i] = MYRGB((frac * ((double) fc_red - bc_red) + bc_red),
			      (frac * ((double) fc_green - bc_green) + bc_green),
		       (frac * ((double) fc_blue - bc_blue) + bc_blue));
  }  
  
    /*
      pal_bw is a black & white palette, only background is white.
      pal_rgb is white on background and palette for the rest.
      These palettes are used to do transparent bitblt.
      */

  pal_bw[0] = QuadWhite;
  pal_mask[0] = QuadBlack;
  pal_rgb[0] = Color2Quad(palette[0]);
  for (i = 1; i < 16; i++) {
    pal_bw[i] = QuadBlack;
    pal_mask[i] = pal_rgb[i] = Color2Quad(palette[i]);
  }
  
  memcpy(bmi4.bmiColors, pal_rgb, 16*sizeof(RGBQUAD));
  
    /* Make sure fore_ and back_Pixel are part of the palette */
  fore_Pixel = MYRGB(GetRValue(palette[15]), GetGValue(palette[15]),
		     GetBValue(palette[15]));
  back_Pixel = MYRGB(GetRValue(palette[0]), GetGValue(palette[0]),
		     GetBValue(palette[0]));

  oldgamma = gamma;

#if DEBUG_COLOR
  fprintf(stderr, "Palette: \n");
  for (i = 0; i < 16; i++)
    fprintf(stderr, "color %d = %-8x %-8x %-8x\n", i, 
	    pal_rgb[i], pal_bw[i], pal_mask[i]);
#endif

  if (mane.shrinkfactor == 1) return warn;

  sf_squared = mane.shrinkfactor * mane.shrinkfactor;
  
  if (shrink_allocated_for != mane.shrinkfactor) {
    if (pixeltbl != NULL) {
      free((char *) pixeltbl);
      if (pixeltbl_t != NULL)
	free((char *) pixeltbl_t);
    }
    pixeltbl = xmalloc((sf_squared + 1) * sizeof(Pixel));
    if (foreGC2 != NULL)
      pixeltbl_t = xmalloc((sf_squared + 1) * sizeof(Pixel));
    shrink_allocated_for = mane.shrinkfactor;
  }
  for (i = 0; i <= sf_squared; ++i)
    pixeltbl[i] =
      palette[(i * 30 + sf_squared) / (2 * sf_squared)];
  
  return warn;
}

#endif /* GREY */

void scan_init_fore_color()
{

  bColorPage = !CRefIsEmpty(scan_fore_colors)
    && !CRefIsInit(scan_fore_colors);

#if DEBUG_COLOR
  fprintf(stderr, "scan_init_fore_color : top stack = %-8x, fore Pixel = %-8x\n",
	  CRefTop(scan_fore_colors), fore_Pixel);
#endif

#if 0
  if (((scanned_page == 0 ) || (scanned_page == current_page)) && (maneBitsPixel <= 8)) {
    initialize_palette();
  }
  dump_colors("scan_init");
#endif
}

void scan_get_last_fore_color()
{
  /*  if ((c = scan_fore_colors[scan_fore_index]) != MYRGB(255, 255, 255)) { */
#if DEBUG_COLOR
  CRefDump(scan_fore_colors, "scan_get_last_fore_color :");
#endif
  if (/* !CRefIsEmpty(scan_fore_colors) && */ ! CRefIsInit(scan_fore_colors)) {
    /* memorized it as the stack for the next page */
    PageForeColor[LastForeChange].page = scanned_page + 1;
    PageForeColor[LastForeChange].color = CRefTop(scan_fore_colors);
    PageForeColor[LastForeChange].stack = CRefSave(scan_fore_colors);
#if DEBUG_COLOR
    fprintf(stderr, "Memorizing color stack at the end of page %d\n", scanned_page);
    CRefDump(PageForeColor[LastForeChange].stack, "PageForeColor[].stack");
#endif
    LastForeChange++;
  }

  {
	COLORREF new_back_Pixel = get_back_color(current_page);
	if (back_Pixel != new_back_Pixel) {
	  back_Pixel = new_back_Pixel;
      if (backBrush)
		DeleteObject(backBrush);
#if DEBUG_COLOR
      fprintf(stderr, "scan_get_fore_color() : Creating back brush %-8x\n", back_Pixel);
#endif
      backBrush = CreateSolidBrush(back_Pixel);
	}
  }
}

void
scan_colors(char *cp)
{
  char *p;
  COLORREF c = MYRGB(0,0,0);
  extern BOOL bColorPage;

#if DEBUG_COLOR
  fprintf(stderr, "Pre-scanning colors page %d : %s\n", scanned_page, cp);
#endif

  if (memicmp(cp, "background", 10) == 0) {
    p = cp + 11;
    /* background(p); */
    /* Store the background color */
    if (scanned_page > PageBackColor[LastBackChange].page) {
      ++LastBackChange;
#if DEBUG_COLOR
      fprintf(stderr, "adding new last back change (%d, %-8x)\n",
	      scanned_page, string_to_colorref(p));
#endif
      PageBackColor[LastBackChange].page = scanned_page;
      PageBackColor[LastBackChange].color = c = string_to_colorref(p);
      bColorPage = bColorPage || (c != string_to_colorref(resource.back_color));
    }
  }
  else if (memicmp(cp, "color", 5) == 0) {
    p = cp + 6;
    while ( *p <= ' ' ) p++ ;
    if (memicmp(p, "push", 4) == 0) {
      p += 5;
      c = string_to_colorref(p);
      if ((scanned_page == current_page) && (maneBitsPixel <= 8)) {
	c = add_color_palette(c);
      }
      CRefPush(scan_fore_colors, c);
#if DEBUG_COLOR
      fprintf(stderr, "Pushing scan color %-8x\n", c);
#endif
      bColorPage = bColorPage || (c != string_to_colorref(resource.fore_color));
    } 
    else if (memicmp(p, "pop", 3) == 0) {
      c = CRefPop(scan_fore_colors);
#if DEBUG_COLOR
      fprintf(stderr, "pop scan color %-8x\n", c);
#endif
    } 
    else {
      c = string_to_colorref(p);
#if DEBUG_COLOR
      fprintf(stderr, "reset color stack %-8x\n", c);
#endif
      if ((scanned_page == current_page) && (maneBitsPixel <= 8)) {
	c = add_color_palette(c);
      }
      CRefResetInit(scan_fore_colors, c);
    }
  }
#if DEBUG_COLOR
  dump_colors("scan specials");
#endif
}

COLORREF get_back_color(int page)
{
  COLORREF cret;
  int i;
  /* page background is a bit different : it is sticky through page changes. */
  cret = back_Pixel;
  /* get the color preceding */
  for (i = 0; i <= LastBackChange; i++)
    if (page < PageBackColor[i].page)
      break;
    else
      cret = PageBackColor[i].color;
  return cret;
}

void
init_colors()
{
  /* FIXME : this is called foar too often ! 
     Look at redraw() in winevt.c */

  static Boolean warn_done = False;
  Boolean warn = False;
  int i;
  RGBQUAD c;
  HPEN oldPen;
  HBRUSH oldBrush;
  COLORREF new_back_Pixel, new_fore_Pixel;
  extern int pen_size;

#if DEBUG_COLOR
  dump_colors("init_colors!!!\n");
#endif
  /* Looking for state to be restored */
  for (i = 0; i < LastForeChange; i++) {
   if (PageForeColor[i].page == current_page) {
      /* copy the color stack */
#if DEBUG_COLOR
      fprintf(stderr, "restoring stack for page %d from PageForeColor[%d]\n", 
	      current_page, i);
      CRefDump(PageForeColor[i].stack, "stack restored : ");
#endif
      CRefRestore(scan_fore_colors, PageForeColor[i].stack);
      CRefRestore(color_stack, PageForeColor[i].stack);
      new_fore_Pixel = CRefTop(color_stack);
      break;
    }
    else if (PageForeColor[i].page > current_page) {
      CRefResetInit(color_stack, string_to_colorref(resource.fore_color));
      CRefResetInit(scan_fore_colors, string_to_colorref(resource.fore_color));
      new_fore_Pixel = CRefTop(color_stack);
#if DEBUG_COLOR
      fprintf(stderr, "Resetted scan_fore_colors\n");
      CRefDump(scan_fore_colors, "stack resetted : ");
#endif
      break;
    }
  }

  if (i == LastForeChange) {
    CRefResetInit(color_stack, string_to_colorref(resource.fore_color));
    CRefResetInit(scan_fore_colors, string_to_colorref(resource.fore_color));
    new_fore_Pixel = CRefTop(color_stack);
#if DEBUG_COLOR
    fprintf(stderr, "Resetted scan_fore_colors\n");
    CRefDump(scan_fore_colors, "stack resetted : ");
#endif
  }

  /* page background is a bit different : it is sticky through page changes. */
  new_back_Pixel = get_back_color(current_page);
#if DEBUG_COLOR
  fprintf(stderr, "init_colors looking for bg color, found %-8x (previous %-8x)\n", 
		  new_back_Pixel, back_Pixel);
#endif

#if 0
  /* Unneeded ? */
  SetBkMode(foreGC, OPAQUE);
  SetBkColor(foreGC, back_Pixel);
#endif

  c.rgbReserved = 0;
  
  if (new_back_Pixel != back_Pixel) {
	back_Pixel = new_back_Pixel;

	/* Do not allow resource leaks ! */
	if (backBrush)
	  DeleteObject(backBrush);
	backBrush = CreateSolidBrush(back_Pixel);
#if DEBUG_COLOR
	fprintf(stderr, "init_colors(): Creating back brush %-8x\n", back_Pixel);
#endif
  }

  
  /* Setup pens and brushes */

  if (new_fore_Pixel != fore_Pixel) {
	fore_Pixel = new_fore_Pixel;

#if DEBUG_COLOR
	fprintf(stderr, "init_colors fore = %x\n", fore_Pixel);
#endif
	if (forePen)
	  if (!DeleteObject(forePen))
		Win32Error("init_colors/DeleteObject/forePen");
	if ((forePen = CreatePen(PS_SOLID, 1, fore_Pixel)) == NULL) {
	  Win32Error("init_colors/CreatePen(fore_pixel) failed.");
	}
	if (foreBrush)
	  if (!DeleteObject(foreBrush))
		Win32Error("init_colors/DeleteObject/foreBrush");
	if ((foreBrush = CreateSolidBrush(fore_Pixel)) == NULL) {
	  Win32Error("init_colors/CreateSolidBrush(fore_pixel) failed.");
	}
	
	if (foreTPicPen)
	  DeleteObject(foreTPicPen);
	foreTPicPen = CreatePen(PS_SOLID, pen_size, fore_Pixel);
	if (foreTPicBrush)
	  DeleteObject(foreTPicBrush);
	foreTPicBrush = CreateSolidBrush(fore_Pixel);
  }

	c.rgbBlue = GetBValue(back_Pixel);
	c.rgbRed =  GetRValue(back_Pixel);
	c.rgbGreen = GetGValue(back_Pixel);
	bmi1.bmiColors[0] = c;
	
	c.rgbBlue = GetBValue(fore_Pixel);
	c.rgbRed =  GetRValue(fore_Pixel);
	c.rgbGreen = GetGValue(fore_Pixel);
	bmi1.bmiColors[1] = c;

#ifdef	GREY
  if (use_grey) {
    warn = init_pix(new_back_Pixel, new_fore_Pixel);
  }
#endif

#if 0 
  if (warn && !warn_done) {
    Puts("Note:  overstrike characters may be incorrect.");
    warn_done = True; /* Print warning only once. */
  }
#endif
}

/*

  FIXME: create a pool of pens and brushes to avoid
  all this mess when moving the mag. glass on a 
  color page.
*/

void SetForeColor(COLORREF c)
{
  HBRUSH oldBrush;
  HPEN oldPen;
  extern int pen_size;

#if DEBUG_COLOR
  fprintf(stderr, "SetForeColor %8x\n", c);
#endif

  if (c != fore_Pixel)
    bColorPage = TRUE;
  
  fore_Pixel = c;

#if 1
  if (forePen)
    if (!DeleteObject(forePen))
      Win32Error("init_colors/DeleteObject/forePen");
  if ((forePen = CreatePen(PS_SOLID, 1, fore_Pixel)) == NULL) {
    Win32Error("init_colors/CreatePen(fore_pixel) failed.");
  }
  if (foreBrush)
    if (!DeleteObject(foreBrush))
      Win32Error("init_colors/DeleteObject/foreBrush");
  if ((foreBrush = CreateSolidBrush(fore_Pixel)) == NULL) {
    Win32Error("init_colors/CreateSolidBrush(fore_pixel) failed.");
  }
  if (foreTPicPen)
    DeleteObject(foreTPicPen);
  foreTPicPen = CreatePen(PS_SOLID, pen_size, fore_Pixel);
  if (foreTPicBrush)
    DeleteObject(foreTPicBrush);
  foreTPicBrush = CreateSolidBrush(fore_Pixel);

#else
  /* Setup pens and brushes */
  if ((forePen = CreatePen(PS_SOLID, 1, fore_Pixel)) == NULL) {
    Win32Error("SetForeColor/CreatePen failed.");
  }
  else {
    if ((oldPen = SelectObject(foreGC, forePen)) == NULL)
      Win32Error("SelectObject/foreGC forePen");
    else
      DeleteObject(oldPen);
  }
  if ((foreBrush = CreateSolidBrush(fore_Pixel)) == NULL) {
    Win32Error("SetForeColor/CreateSolidBrush failed.");
  }
  else {
    if ((oldBrush = SelectObject(foreGC, foreBrush)) == NULL)
      Win32Error("SelectObject/foreGC foreBrush");
    else
      DeleteObject(oldBrush);
  }
#endif
}

void SetBackColor(COLORREF c)
{
#if DEBUG_COLOR
  fprintf(stderr, "Setting back color to %-8x\nbefore :\n", c);
  dump_colors("SetBackColor");
  fprintf(stderr, "current_page %d, last change (%d, %-8x)\n",
	  current_page, PageBackColor[LastBackChange].page, 
	  PageBackColor[LastBackChange].color);
#endif

  if (c != back_Pixel)
    bColorPage = TRUE;

  back_Pixel = c;

  if (current_page > PageBackColor[LastBackChange].page) {
    LastBackChange++;
    fprintf(stderr, "adding new last change (%d, %-8x)\n",
	    current_page, back_Pixel);
    
    PageBackColor[LastBackChange].color = back_Pixel;
    PageBackColor[LastBackChange].page = current_page;
  }
  /* Do not allow resource leaks ! */
  if (backBrush)
    DeleteObject(backBrush);
  
  backBrush = CreateSolidBrush(back_Pixel);

#if DEBUG_COLOR
  fprintf(stderr, "SetBackColor: creating new back brush %-8x\n", back_Pixel);
#endif
#if 0
  if (! isPrinting)
    SendMessage(hWndDraw, WM_ERASEBKGND, GetDC(currwin.win), 0);
#endif

}

/* 
   This should be called only once at the very beginning of the
   current dvi file. 
   */

void initcolor()
{
  int i;
  extern int pen_size;
  /* Setup default color as black, background color as white */
  
  /* Free any used resources */
  CRefFree(scan_fore_colors);
  CRefFree(color_stack);

  if (LastForeChange > 0) {
    for (i = 0; i < LastForeChange; i++) {
      if (PageForeColor[i].stack)
	CRefFree(PageForeColor[i].stack);
    }
  }
  scanned_page = scanned_page_reset = -1;

  fore_Pixel = string_to_colorref(resource.fore_color);
  back_Pixel = string_to_colorref(resource.back_color);

  LastForeChange = 0;
  PageForeColor[0].color = fore_Pixel;
  PageForeColor[0].page = 0;

  if (forePen)
	if (!DeleteObject(forePen))
	  Win32Error("init_colors/DeleteObject/forePen");
  if ((forePen = CreatePen(PS_SOLID, 1, fore_Pixel)) == NULL) {
	Win32Error("init_colors/CreatePen(fore_pixel) failed.");
  }
  if (foreBrush)
	if (!DeleteObject(foreBrush))
	  Win32Error("init_colors/DeleteObject/foreBrush");
  if ((foreBrush = CreateSolidBrush(fore_Pixel)) == NULL) {
	Win32Error("init_colors/CreateSolidBrush(fore_pixel) failed.");
  }
  
  if (foreTPicPen)
	DeleteObject(foreTPicPen);
  foreTPicPen = CreatePen(PS_SOLID, pen_size, fore_Pixel);
  if (foreTPicBrush)
	DeleteObject(foreTPicBrush);
  foreTPicBrush = CreateSolidBrush(fore_Pixel);
  
  LastBackChange = 0;
  PageBackColor[0].color = back_Pixel;
  PageBackColor[0].page = -1;

  if (backBrush)
	DeleteObject(backBrush);
  backBrush = CreateSolidBrush(back_Pixel);


  scan_fore_colors = InitCRefStack(COLOR_STACK_DEPTH);
  color_stack = InitCRefStack(COLOR_STACK_DEPTH);

  CRefPush(scan_fore_colors, fore_Pixel );
  CRefPush(color_stack, fore_Pixel);

  if (maneBitsPixel <= 8)
    initialize_palette();

#if DEBUG_COLOR
  fprintf(stderr, "initcolor() called\n");
#endif
  init_colors();
}
