/*
 * Copyright (c) 2001 Marcin Dalecki
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*
 * Implement a nice and well behaved application icon.
 */

#include "xdvi-config.h"
#include "xdvi.h"
#include "xicon.h"
#include "util.h"
#include "xdvi-debug.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xfuncs.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#ifdef MOTIF
# include <Xm/XmStrDefs.h>
#endif

#if defined(HAVE_X11_XPM_H)
# include <X11/xpm.h>
#elif defined(HAVE_XPM_H)
# include <xpm.h>
#elif defined(HAVE_XM_XPMP_H)
# include <Xm/XpmP.h>
#endif

#include "xdvi.icon"
#include "pixmaps/xdvi32x32.xpm"
#include "pixmaps/xdvi16x16.xpm"
#include "pixmaps/xdvi48x48.xpm"
void
add_icon(Widget top_level,
	 String title_name,
	 String icon_name)
{
#if !HAVE_XPM

    static Arg args[] = {
	{XtNiconX, (XtArgVal) 0},
	{XtNiconY, (XtArgVal) 0},
    };
    static Pixmap icon_pm;
    static Arg temp_args4 = { XtNiconPixmap, (XtArgVal) &icon_pm };

    UNUSED(xdvi16x16_xpm);
    UNUSED(xdvi32x32_xpm);
    UNUSED(xdvi48x48_xpm);
    
    if (resource.icon_geometry != NULL) {
	int junk;

	(void)XGeometry(DISP, XScreenNumberOfScreen(SCRN),
			resource.icon_geometry, "",
			0, 0, 0, 0, 0,
			(int *)&args[0].value,
			(int *)&args[1].value,
			&junk,
			&junk);
	XtSetValues(top_level, args, XtNumber(args));
    }
    /* Set icon pixmap */
    XtGetValues(top_level, &temp_args4, 1);
    if (icon_pm == (Pixmap) 0) {
	temp_args4.value =
	    (XtArgVal)XCreateBitmapFromData(DISP,
					    RootWindowOfScreen(SCRN),
					    (const char *)xdvi_bits,
					    xdvi_width,
					    xdvi_height);
	XtSetValues(top_level, &temp_args4, 1);
    }
#else /* HAVE_XPM */
    
    /* Use Pixmaps, looking much nicer. */

    static Pixmap   icon = 0;
    static Pixmap   icon_mask = 0;
    static const char **pixmap_data = xdvi32x32_xpm;
    XIconSize	    *size;
    int		    number_sizes;
    Display	    *dsp;
    XpmAttributes   attr;

    UNUSED(xdvi_bits);
    /*
     * get the icon size preferred by the window manager
     */
    if (XGetIconSizes(XtDisplay(top_level), RootWindowOfScreen(SCRN),
		      &size, &number_sizes) != 0) {
	if (number_sizes > 0) {
	    if (size->max_height >= 48 && size->max_height >= 48)
		pixmap_data = xdvi48x48_xpm;
	    else if (size->max_height >= 32 && size->max_height >= 32)
		pixmap_data = xdvi32x32_xpm;
	    else if (size->max_height >= 16 && size->max_height >= 16)
		pixmap_data = xdvi16x16_xpm;
	}
    }

    dsp = XtDisplay(top_level);

    attr.valuemask = 0L;
    attr.valuemask = XpmCloseness | XpmReturnPixels | XpmColormap | XpmDepth | XpmVisual;
    attr.closeness = 65535;	/* accuracy isn't crucial */
    /* use the same visual/colormap/depth as main window, else BadMatch ... */
    attr.visual = G_visual;
    attr.colormap = G_colormap;
    attr.depth = G_depth;

    if (!icon) {
	Window rootWindow = XtWindow(globals.widgets.top_level);
	ASSERT(rootWindow != 0, "");
	XpmCreatePixmapFromData(dsp, rootWindow, (char **)pixmap_data,
				&icon, &icon_mask, &attr);
    }

# ifdef MOTIF
    XtVaSetValues(top_level, XmNiconPixmap, icon, XmNiconMask, icon_mask, NULL);
# else
    XtVaSetValues(top_level, XtNiconPixmap, icon, XtNiconMask, icon_mask, NULL);
# endif

    XpmFreeAttributes(&attr);
#endif

    {
	/* code locality ... */
	static Arg args[] = {
	    { XtNtitle, (XtArgVal) 0 },
	    { XtNiconName, (XtArgVal) 0 },
	    { XtNinput, (XtArgVal) True },
	};

	args[0].value = (XtArgVal)title_name;
	args[1].value = (XtArgVal)icon_name;
	XtSetValues(top_level, args, XtNumber(args));
    }
}
