/* Xserver debugging functions, lifted from X11 R6.6's xdpyinfo.c.
   Original copyright follows:
*/

/*
  Copyright 1988, 1998  The Open Group

  Permission to use, copy, modify, distribute, and sell this software and its
  documentation for any purpose is hereby granted without fee, provided that
  the above copyright notice appear in all copies and that both that
  copyright notice and this permission notice appear in supporting
  documentation.

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
  OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
  AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  Except as contained in this notice, the name of The Open Group shall not be
  used in advertising or otherwise to promote the sale, use or other dealings
  in this Software without prior written authorization from The Open Group.
  *
  * Author:  Jim Fulton, MIT X Consortium
  */

#include "xdvi-config.h"
#include "xdvi.h"
#include "xserver-info.h"

#ifdef XSERVER_INFO

static void
print_display_info (Display *dpy)
{
    char dummybuf[40];
    char *cp;
    int minkeycode, maxkeycode;
    int i, n;
    /*     long req_size; */
    XPixmapFormatValues *pmf;
    Window focuswin;
    int focusrevert;

    printf("name of display:    %s\n", DisplayString (dpy));
    printf("version number:    %d.%d\n",
	   ProtocolVersion (dpy), ProtocolRevision (dpy));
    printf("vendor string:    %s\n", ServerVendor (dpy));
    printf("vendor release number:    %d\n", VendorRelease (dpy));
    /* apparently AIX 3 does not have XExtendedMaxRequestSize.
       We don't really need it either.
    */
    /*     req_size = XExtendedMaxRequestSize (dpy); */
    /*     if (!req_size) req_size = XMaxRequestSize (dpy); */
    /*     printf("maximum request size:  %ld bytes\n", req_size * 4); */
    /*     printf("motion buffer size:  %lu\n", XDisplayMotionBufferSize (dpy)); */

    switch (BitmapBitOrder (dpy)) {
    case LSBFirst:    cp = "LSBFirst"; break;
    case MSBFirst:    cp = "MSBFirst"; break;
    default:    
	sprintf(dummybuf, "unknown order %d", BitmapBitOrder (dpy));
	cp = dummybuf;
	break;
    }
    printf("bitmap unit: %d, bit order: %d (%s), padding: %d\n",
	   BitmapUnit (dpy), BitmapBitOrder(dpy), cp, BitmapPad (dpy));

    switch (ImageByteOrder (dpy)) {
    case LSBFirst:    cp = "LSBFirst"; break;
    case MSBFirst:    cp = "MSBFirst"; break;
    default:    
	sprintf(dummybuf, "unknown order %d", ImageByteOrder (dpy));
	cp = dummybuf;
	break;
    }
    printf("image byte order: %d (%s)\n", ImageByteOrder(dpy), cp);

    pmf = XListPixmapFormats (dpy, &n);
    printf("number of supported pixmap formats:    %d\n", n);
    if (pmf) {
	printf("supported pixmap formats:\n");
	for (i = 0; i < n; i++) {
	    printf("    depth %d, bits_per_pixel %d, scanline_pad %d\n",
		   pmf[i].depth, pmf[i].bits_per_pixel, pmf[i].scanline_pad);
	}
	XFree ((char *) pmf);
    }


    /*
     * when we get interfaces to the PixmapFormat stuff, insert code here
     */

    XDisplayKeycodes (dpy, &minkeycode, &maxkeycode);
    printf("keycode range:    minimum %d, maximum %d\n",
	   minkeycode, maxkeycode);

    XGetInputFocus (dpy, &focuswin, &focusrevert);
    printf("focus:  ");
    switch (focuswin) {
    case PointerRoot:
	printf("PointerRoot\n");
	break;
    case None:
	printf("None\n");
	break;
    default:
	printf("window 0x%lx, revert to ", focuswin);
	switch (focusrevert) {
	case RevertToParent:
	    printf("Parent\n");
	    break;
	case RevertToNone:
	    printf("None\n");
	    break;
	case RevertToPointerRoot:
	    printf("PointerRoot\n");
	    break;
	default:			/* should not happen */
	    printf("%d\n", focusrevert);
	    break;
	}
	break;
    }

    /*      print_extension_info (dpy); */

    printf("default screen number:    %d\n", DefaultScreen (dpy));
    printf("number of screens:    %d\n", ScreenCount (dpy));
}

static void
print_visual_info (XVisualInfo *vip)
{
    char errorbuf[40];			/* for sprintfing into */
    char *class = NULL;			/* for printing */

    switch (vip->class) {
    case StaticGray:    class = "StaticGray"; break;
    case GrayScale:    class = "GrayScale"; break;
    case StaticColor:    class = "StaticColor"; break;
    case PseudoColor:    class = "PseudoColor"; break;
    case TrueColor:    class = "TrueColor"; break;
    case DirectColor:    class = "DirectColor"; break;
    default:    
	sprintf(errorbuf, "unknown class %d", vip->class);
	class = errorbuf;
	break;
    }

    printf("  visual:\n");
    printf("    visual id:    0x%lx\n", vip->visualid);
    printf("    class:    %s\n", class);
    printf("    depth:    %d plane%s\n", vip->depth, 
	   vip->depth == 1 ? "" : "s");
    if (vip->class == TrueColor || vip->class == DirectColor)
	printf("    available colormap entries:    %d per subfield\n",
	       vip->colormap_size);
    else
	printf("    available colormap entries:    %d\n",
	       vip->colormap_size);
    printf("    red, green, blue masks:    0x%lx, 0x%lx, 0x%lx\n",
	   vip->red_mask, vip->green_mask, vip->blue_mask);
    printf("    significant bits in color specification:    %d bits\n",
	   vip->bits_per_rgb);
}

static void
print_screen_info (Display *dpy, int scr)
{
    Screen *s = ScreenOfDisplay (dpy, scr);  /* opaque structure */
    XVisualInfo viproto;		/* fill in for getting info */
    XVisualInfo *vip;			/* retured info */
    int nvi;				/* number of elements returned */
    int i;				/* temp variable: iterator */
    /*      char eventbuf[80]; */			/* want 79 chars per line + nul */
    static char *yes = "YES", *no = "NO", *when = "WHEN MAPPED";
    double xres, yres;
    int ndepths = 0, *depths = NULL;
    unsigned int width, height;


    /*
     * there are 2.54 centimeters to an inch; so there are 25.4 millimeters.
     *
     *     dpi = N pixels / (M millimeters / (25.4 millimeters / 1 inch))
     *         = N pixels / (M inch / 25.4)
     *         = N * 25.4 pixels / M inch
     */

    xres = ((((double) DisplayWidth(dpy,scr)) * 25.4) / 
	    ((double) DisplayWidthMM(dpy,scr)));
    yres = ((((double) DisplayHeight(dpy,scr)) * 25.4) / 
	    ((double) DisplayHeightMM(dpy,scr)));

    printf("\n");
    printf("screen #%d:\n", scr);
    printf("  dimensions:    %dx%d pixels (%dx%d millimeters)\n",
	   DisplayWidth (dpy, scr), DisplayHeight (dpy, scr),
	   DisplayWidthMM(dpy, scr), DisplayHeightMM (dpy, scr));
    printf("  resolution:    %dx%d dots per inch\n", 
	   (int) (xres + 0.5), (int) (yres + 0.5));
    depths = XListDepths (dpy, scr, &ndepths);
    if (!depths) ndepths = 0;
    printf("  depths (%d):    ", ndepths);
    for (i = 0; i < ndepths; i++) {
	printf("%d", depths[i]);
	if (i < ndepths - 1) { 
	    putchar (',');
	    putchar (' ');
	}
    }
    putchar ('\n');
    if (depths) XFree ((char *) depths);
    printf("  root window id:    0x%lx\n", RootWindow (dpy, scr));
    printf("  depth of root window:    %d plane%s\n",
	   DisplayPlanes (dpy, scr),
	   DisplayPlanes (dpy, scr) == 1 ? "" : "s");
    printf("  number of colormaps:    minimum %d, maximum %d\n",
	   MinCmapsOfScreen(s), MaxCmapsOfScreen(s));
    printf("  default colormap:    0x%lx\n", DefaultColormap (dpy, scr));
    printf("  default number of colormap cells:    %d\n",
	   DisplayCells (dpy, scr));
    printf("  preallocated pixels:    black %lu, white %lu\n",
	   BlackPixel (dpy, scr), WhitePixel (dpy, scr));
    printf("  options:    backing-store %s, save-unders %s\n",
	   (DoesBackingStore (s) == NotUseful) ? no :
	   ((DoesBackingStore (s) == Always) ? yes : when),
	   DoesSaveUnders (s) ? yes : no);
    XQueryBestSize (dpy, CursorShape, RootWindow (dpy, scr), 65535, 65535,
		    &width, &height);
    if (width == 65535 && height == 65535)
	printf("  largest cursor:    unlimited\n");
    else
	printf("  largest cursor:    %dx%d\n", width, height);
    printf("  current input event mask:    0x%lx\n", EventMaskOfScreen (s));
    /*      (void) print_event_mask (eventbuf, 79, 4, EventMaskOfScreen (s)); */
		      

    nvi = 0;
    viproto.screen = scr;
    vip = XGetVisualInfo (dpy, VisualScreenMask, &viproto, &nvi);
    printf("  number of visuals:    %d\n", nvi);
    printf("  default visual id:  0x%lx\n", 
	   XVisualIDFromVisual (DefaultVisual (dpy, scr)));
    for (i = 0; i < nvi; i++) {
	print_visual_info (vip+i);
    }
    if (vip) XFree ((char *) vip);
}


/*
  public function: when DBG_ALL is enabled, dump info on Xserver
  to stdout.
*/
void
print_xserver_info(void)
{
    int i;
    if (globals.debug & DBG_PK) {
	puts("========== Xserver properties info begin ==========\n");
	print_display_info(DISP);
	for (i = 0; i < ScreenCount(DISP); i++) {
	    print_screen_info(DISP, i);
	}
	puts("========== Xserver properties info end ==========\n");
    }
}


#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* XSERVER_INFO */
