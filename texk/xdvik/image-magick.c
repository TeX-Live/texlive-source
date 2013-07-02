/*
  Imagemagick support for xdvik, written by Zhang Linbo <zlb@lsec.cc.ac.cn>
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to
  deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/

/*
  TODO:
  
  1. Specials other than 'psfile=' and 'em:graph' which include
  image files.

  [ NOTE SU: I consider this more or less done now with the backtick
  parsing ;-) ]

  2. Use an XtWorkProc or similar to do some background pre-processing
  of image files before they are displayed.

  3. Replace popen() by fork and explicit pipes, and make the
  use of xdvi_temp_fd safer.
     
  4. PS figs are always clipped to the bbox defined in the special
  string, which is not what we want when clip=false.
  The problem can be partly solved by increasing the value of
  the PSBW macro, or we may get the correct bbox by reading the
  '%%BoundingBox' or '%%HiresBoundingBox' lines in the PS file
  and set the image geometry accordingly.
  
  5. The code only(?) works with true color displays.
  
  6. There's also work to do to respond to window events in 
  load_image() and display_image() functions.
*/
#include "xdvi-config.h"

#include <ctype.h>

#include "xdvi.h"
#include "dvi-init.h"
#include "events.h"
#include "dvi-draw.h"
#include "util.h"
#include "special.h"
#include "image-magick.h"

#ifdef MAGICK	/* entire file */

#ifndef MAGICK_RENDER_PS
#  define MAGICK_RENDER_PS 0
#endif

#include "magick/api.h"
#if MAGICK_VER_MAJOR > 5 || MAGICK_VER_MINOR >= 4
#  include "magick/xwindow.h"	/* IM >= 5.4 */
#else
#  include "magick/xwindows.h"	/* IM <= 5.3 */
#endif

#if MAGICK_VER_MAJOR > 5 || MAGICK_VER_MINOR >= 4	/* >= 5.4 */
#  define CATCH_EXCEPTION(e) do { \
	if (e.severity != UndefinedException) CatchException(&e); } while (0)
#elif MAGICK_VER_MINOR >= 3	/* 5.3 */
#  define CATCH_EXCEPTION(e) if (image == NULL) \
	MagickWarning(e.severity, e.reason, e.description);
#else /* <= 5.2 (no check) */
#  define CATCH_EXCEPTION(e) if (image == NULL) \
	MagickWarning(e.severity, e.message, e.qualifier);
#endif

#if 0
#define CATCH_EXCEPTION(e)	/* do nothing */
#endif

#ifndef False
#  define False 0
#endif

#ifndef True
#  define True 1
#endif

#define TWOPI (2*3.141592653589793)

static double ROUND_tmp;
#define ROUND(x) ( (ROUND_tmp=(x)) >= 0 ? (long)(ROUND_tmp + 0.5) : \
					- (long)(- ROUND_tmp + 0.5) )
	
static void
showtime(const char *msg)
{
    if (globals.debug & DBG_PS) {
	static double time0 = -1;
	double time;
	struct timeval tv;

	gettimeofday(&tv, (struct timezone *) 0);
	time = tv.tv_sec + (double) tv.tv_usec * 1e-6;
	if (time0 < 0)
	    time0 = time;
	fprintf(stderr, "        %-40s %0.4f\n", msg, time - time0);
    }
}

#define DPI ( resource.pixels_per_inch*0.001*magnification/mane.shrinkfactor )

typedef struct XDviImageInfo {
    Boolean clip;
    int rwi, rhi, angle;
    double llx, lly, urx, ury;
    char filename[MaxTextExtent];
} XDviImageInfo;

typedef struct {
    XDviImageInfo info;
    XImage *ximage;	/* ximage */
    unsigned long bgpixel;	/* background pixel */
    int xoff, yoff;	/* offsets in pixels */
} XDviImage;

static size_t cache_size = 0;
/* default value for cache_limit (16MB), it can be specified with the
 * "-magick_cache size" option */
static size_t cache_limit = 16 * 1024 * 1024;
static int ncached = 0;
static XDviImage *cache = NULL;

static char *magick_tmp;
static double current_dpi = -1;

/* Input image types */
enum {
    TYPE_UNKNOWN,	/* unknown */
    TYPE_CONVERT,	/* `convert ... */
};
static int image_type;	/* type of current input image */

static void
cleanup(void)
{
    if (magick_tmp != NULL)
	unlink(magick_tmp);
}

static void
free_ximage(XImage * ximage)
{
    if (ximage == NULL)
	return;
    if (ximage->data != NULL) {
	free(ximage->data);
	ximage->data = NULL;
    }
    XDestroyImage(ximage);
}

static void
clear_cache(void)
{
    XDviImage *c;
    if (!ncached)
	return;
    for (c = cache; c < cache + ncached; c++)
	free_ximage(c->ximage);
    free(cache);
    ncached = 0;
    cache = NULL;
    cache_size = 0;
}

static void
parse_backtick(char *fn)
{
    char *p, *q, *r, c;

    if (fn[0] != '`')
	return;

    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: parsing backtick command |%s|\n", fn);

#ifdef __GNUC__
#warning FIXME: add code to execute the cmd then load the output file, \
    		if '-allowshell' is effective and the user requests it \
		somehow (slow but reliable).
#endif

    fn[0] = '\0';
    p = fn + 1;
    /* Skip 'env' and 'VAR=value' */
    do {
	while (isspace((int)*p))
	    ++p;
	q = p;
	while (*q != '\0' && !isspace((int)*q))
	    ++q;
	if (*q == '\0' || q <= p)
	    return;
	if (((r = strchr(p, '=')) == NULL || r >= q) &&
	    (!isspace((int)*q) || strncmp(p, "env", 3)))
	    break;
	p = q;
    } while (1);

    /* Now [p ... q-1] is the command name */
    if (q - p == 7 && !memcmp(p, "convert", 7)) 
	image_type = TYPE_CONVERT;
    else
	image_type = TYPE_UNKNOWN;
    p = q;
    do {
	while (isspace((int)*p))
	    ++p;
	q = p;
	if (*q == '`' || *q == '\'' || *q == '"') {
	    c = *p;
	    ++q;
	    while (*q != '\0' && *q != c)
		q++;
	    if (*q != '\0')
		q++;
	}
	else {
	    while (*q != '\0' && !isspace((int)*q))
		q++;
	}
#ifdef __GNUC__
#warning FIXME: check for some cmdline options
	break;
#endif
    } while (1);
    *q = '\0';

    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: filename=|%s|\n", p);

    /* check for the case `kpsewhich -n ... #1` */
    if (*p == '`') {
#if 0
	/* WARNING: unsafe! */
	FILE *pipe;

	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: executing %s\n", p);
	if (*(--q) == '`')
	    *q = '\0';
	if ((pipe = popen(p + 1, "r")) == NULL)
	    return;
	fscanf(pipe, "%s", fn);
	pclose(pipe);
	return;
#else
	if (memcmp(++p, "kpsewhich", sizeof("kpsewhich") - 1))
	    return;
#ifdef __GNUC__
#warning FIXME: better make KPSE library call here.
#endif
	if (*(--q) == '`')
	    --q;
	while (q >= p && isspace((int)*q))
	    --q;
	if (q < p)
	    return;
	*(q + 1) = '\0';
	while (q >= p && !isspace((int)*q))
	    --q;
	p = q + 1;
#endif
    }

    strcpy(fn, p);
}

static Boolean
imagick_eps_to_jpeg(const char *fullpath,
		    unsigned long *columns, unsigned long *rows,
		    XDviImage *img, PixelPacket *bgcolor,
		    double xdpi, double ydpi)
{
    size_t size;
#ifdef __GNUC__
#warning FIXME: security (check 'allowshell'?), backticks, pathname canonicalization,
    /* note SU: better use fork()/exec() here, popen() is too unsafe. */
#endif
    static char *cmdline = NULL;
    static size_t cmdline_size = 0;
    static char *fmt = "%s -q -dBATCH -dMaxBitmap=300000000 "
	"-dNOPAUSE -dNOSAFER -sDEVICE=jpeg "
	"-dTextAlphaBits=4 -dGraphicsAlphaBits=4 "
	"-g%ldx%ld -r%0.4fx%0.4f -sOutputFile=%s - -c quit";
    static const char strsafe[] =
	"{ << /PermitFileReading [ (*) ] /PermitFileWriting [ ] /PermitFileControl [ ] "
	">> setuserparams .locksafe "
	"} stopped pop\n";
    FILE *pipe;

    showtime("PS/EPS -> JPEG:");
    /* Add a border to the drawing box (PSBW=border width in bp) */
#define PSBW 1
    *columns += ROUND(2 * PSBW / 72.0 * xdpi);
    *rows += ROUND(2 * PSBW / 72.0 * ydpi);
    img->xoff = ROUND(PSBW / 72.0 * xdpi);
    img->yoff = -ROUND(PSBW / 72.0 * ydpi);

    size = strlen(fmt) + 25 * 2 + 10 * 2 + strlen(magick_tmp) + strlen(resource.gs_path) + 1;
    if (size > cmdline_size) {
	cmdline_size = size;
	cmdline = xrealloc(cmdline, cmdline_size);
    }
    unlink(magick_tmp);
    sprintf(cmdline, fmt, resource.gs_path, *columns, *rows, xdpi, ydpi,
	    magick_tmp);
    if ((pipe = popen(cmdline, "w")) == NULL) {
	fprintf(stderr, "%s: Cannot execute %s, ignoring image %s.\n",
		globals.program_name, resource.gs_path, fullpath);
	return False;
    }

#define ColorToPixelPacket(c)	((c*((1L<<(QuantumDepth))-1L)+32768L)/65535L)
#define UnifiedColorValue(c)	((c)/(double)((1L<<(QuantumDepth))-1L))
#ifdef __GNUC__
#warning FIXME: how to control roundoff error which may produce a \
	 different backgroung pixel in the output image?
#endif
#if COLOR
    bgcolor->red = ColorToPixelPacket(bg_current->color.r);
    bgcolor->green = ColorToPixelPacket(bg_current->color.g);
    bgcolor->blue = ColorToPixelPacket(bg_current->color.b);
#else
    bgcolor->red = bgcolor->green = bgcolor->blue = ColorToPixelPacket(65535);
#endif
    bgcolor->opacity = (1L << QuantumDepth) - 1L;

#ifdef __GNUC__
#warning FIXME: how to set bg color within gs (instead of filling the bbox) ?
#endif
    if (resource.gs_safer) {
	fprintf(pipe, strsafe);
    }
    /* fill PS paper with background color */
    fprintf(pipe,
	    "newpath -5 -5 moveto %f 0 rlineto "
	    "0 %f rlineto %f 0 rlineto 0 %f rlineto "
	    "closepath %f %f %f setrgbcolor fill\n",
	    img->info.urx - img->info.llx + 2 * (PSBW + 5),
	    img->info.ury - img->info.lly + 2 * (PSBW + 5),
	    -(img->info.urx - img->info.llx + 2 * (PSBW + 5)),
	    -(img->info.ury - img->info.lly + 2 * (PSBW + 5)),
	    UnifiedColorValue(bgcolor->red),
	    UnifiedColorValue(bgcolor->green),
	    UnifiedColorValue(bgcolor->blue));

    /* shift PS coordinates */
    fprintf(pipe, "%f %f translate\n", -img->info.llx + PSBW, -img->info.lly + PSBW);
    if (globals.debug & DBG_PS)
	fprintf(stderr, "    clip=%s\n", img->info.clip ? "yes" : "no");
    if (img->info.clip) {
	fprintf(pipe, "newpath %f %f moveto %f 0 rlineto 0 %f rlineto "
		"%f 0 rlineto 0 %f rlineto closepath clip\n",
		img->info.llx, img->info.lly, img->info.urx - img->info.llx,
		img->info.ury - img->info.lly, -(img->info.urx - img->info.llx),
		-(img->info.ury - img->info.lly));
    }
    fprintf(pipe, "%f %f moveto\n", img->info.llx, img->info.lly);
    fprintf(pipe, "(%s) run showpage\n", fullpath);
    pclose(pipe);
    return True;
}


static Image *
rotate_image(XDviImage *img, Image *image, ExceptionInfo exception)
{
    Image *tmp_image;
    unsigned long w = image->columns, h = image->rows;
    double sin_a = sin(img->info.angle * (TWOPI / 360));
    double cos_a = cos(img->info.angle * (TWOPI / 360));

    if (globals.debug & DBG_PS)
	fprintf(stderr, "\t    rotating image by %d degrees\n",
		img->info.angle);

    /* adjust xoff, yoff, assuming (0,0) the origin of rotation */
    if (img->xoff || img->yoff) {
	double a, b;
	a = img->xoff * cos_a + img->yoff * sin_a;
	b = -img->xoff * sin_a + img->yoff * cos_a;
	img->xoff = ROUND(a);
	img->yoff = ROUND(b);
    }
	
    showtime("RotateImage:");
    tmp_image = RotateImage(image, -(double)img->info.angle, &exception);
    DestroyImage(image);
    image = tmp_image;
    CATCH_EXCEPTION(exception);
    if (image == NULL)
	return NULL;
    /* 
     *	 _______________________________________
     *   |		    *		       |
     *   |		  *   *		       |
     *   |		*	*	       |
     *   |	      *		  *	       |
     *	 |	    *		    *	       |
     *	 |	  *		      *	       |
     *	 |	*			*      |
     *	 |    *				  *    |
     *	 |  *				    *  |
     *	 |*				      *|
     *	 |  *				    *  |
     *	 |    *				  *    |
     *	 |	*			*      |
     *	 |	  *		      *	       |
     *	 |	    *		    *	       |
     *   |	      *		  *	       |
     *   |		*	*	       |
     *   |		  *   *		       |
     *   |		    O (origin)	       |
     *	 O--------------------------------------
     *	(lower-left corner of image box after rotation)
     */
    if (img->info.angle <= 90) {
	img->xoff += ROUND(h * sin_a);
    } 
    else if (img->info.angle <= 180) {
	img->xoff += ROUND(h * sin_a - w * cos_a);
	img->yoff += ROUND(h * cos_a);
    }
    else if (img->info.angle <= 270) {
	img->xoff -= ROUND(w * cos_a);
	img->yoff += ROUND(w * sin_a + h * cos_a);
    }
    else {
	img->yoff += ROUND(w * sin_a);
    }
    return image;
}

static Image *
crop_shift_image(XDviImage *img, Image *image,
		 unsigned long *columns, unsigned long *rows,
		 double xscale, double yscale,
		 ExceptionInfo exception)
{
    /* ZLB: the code below works for the case where the BoundingBox
     * of the PS figure generated by the shell command is (0 0 w h), 
     * where w and h are the pixel width and height of the input image.
     * (this is true for the program 'convert' without options).
     * The image should then be cropped or/and shifted depending
     * on the value of 'clip' */
    int w = image->columns, h = image->rows;
    
    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: type of input image = CONVERT\n");
    
    if (!img->info.clip) {
	/* We only need to shift the image */
	img->xoff = +ROUND(xscale * DPI * img->info.llx / 72.0);
	img->yoff = -ROUND(yscale * DPI * img->info.lly / 72.0);
	*columns = ROUND(xscale * DPI * w / 72.0);
	*rows = ROUND(yscale * DPI * h / 72.0);
    }
    else {
	int llx = ROUND(img->info.llx);
	int lly = ROUND(img->info.lly);
	int urx = ROUND(img->info.urx);
	int ury = ROUND(img->info.ury);
	
	if (llx < 0) {
	    img->xoff = +ROUND(xscale * DPI * llx / 72.0);
	    llx = 0;
	}
	if (lly < 0) {
	    img->yoff = -ROUND(yscale * DPI * lly / 72.0);
	    lly = 0;
	}
	if (urx > w)
	    urx = w;
	if (ury > h)
	    ury = h;

	if ((urx - llx) < 1 || (urx - llx) < 1) {
	    DestroyImage(image);
	    return NULL;
	}

	if (llx != 0 || lly != 0 || urx != w || ury != h) {
	    /* Crop the image */
	    Image *tmp_image;
	    RectangleInfo geo;
	    geo.width = urx - llx;
	    geo.height = ury - lly;
	    geo.x = llx;
	    geo.y = h - ury;
	    if (globals.debug & DBG_PS)
		fprintf(stderr, "\t    cropping image (%d %d %d %d)\n",
			llx, lly, urx, ury);
	    showtime("CropImage:");
	    tmp_image = CropImage(image, &geo, &exception);
	    DestroyImage(image);
	    image = tmp_image;
	    CATCH_EXCEPTION(exception);
	    if (image == NULL)
		return NULL;	/* error cropping image */
	}
	*columns = ROUND(xscale * DPI * (urx - llx) / 72.0);
	*rows = ROUND(yscale * DPI * (ury - lly) / 72.0);
    }
    return image;
}


/* parse and return the `magick_cache' resource size_str */
static long
parse_cache_setting(const char *size_str)
{
    char *p;
    long size = strtol(size_str, &p, 0);
    switch (*p) {
    case 'g':
    case 'G':
	size *= 1024;
	/* fall through */
    case 'm':
    case 'M':
	size *= 1024;
	/* fall through */
    case 'k':
    case 'K':
	size *= 1024;
	break;
    default:
	if (*p != '\0') {
	    fprintf(stderr, "Magick: invalid suffix (%c) in "
		    "magick_cache option\n", *p);
	}
    }
    return size;
}

static XDviImage *
load_image(XDviImageInfo *info)
{
    static ExceptionInfo exception;
    static ImageInfo *image_info;
    static XVisualInfo visual_info;
    static XStandardColormap map_info;
    static Boolean initialized = False;
    static Boolean disabled = False;

    static XDviImage img = { {False, 0, 0, 0, 0, 0, 0, 0, ""}, NULL, 0, 0, 0 };

    Image *image, *tmp_image;
    PixelPacket *pp;
    double xscale, yscale, xdpi, ydpi;
    int n;
    unsigned int x, y;
    unsigned long columns, rows;
    char density[40];
    size_t size;
    char *fn = info->filename;
    XDviImage *c;
    PixelPacket bgcolor;

    struct stat statbuf;
    char *path, *fullpath;
    char canonical_path[MAXPATHLEN + 1];
    
    if (disabled)
	return NULL;

    parse_backtick(info->filename);
    if (info->filename[0] == '\0') {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: skipping malformed backtick command.\n");
	return NULL;
    }
    else {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: filename=|%s|\n", info->filename);
    }

    img.info = *info;
    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: load_image: fn=%s\n", fn);

    /* check for changes in mag/shrink/resolution */
    if (fabs(current_dpi - DPI) > 1e-4) {
	current_dpi = DPI;
	clear_cache();
    }

    /* look it up in the cache */
    if (ncached) {
	for (c = cache; c < cache + ncached; c++) {
	    if (memcmp(&c->info, info, sizeof *info))
		continue;
	    if (globals.debug & DBG_PS)
		fprintf(stderr, "    Image found in cache.\n");
	    return c;
	}
    }

    if (!initialized) {
	magick_tmp = NULL;
	if ((n = xdvi_temp_fd(&magick_tmp)) == -1) {
	    fprintf(stderr, "Magick: cannot create tmp filename, disabled.\n");
	    disabled = True;
	    return NULL;
	}
	/* FIXME: mustn't close filedescriptor returned by xdvi_temp_fd(),
	   else we have a race condition again; see e.g.
	   http://www.dwheeler.com/secure-programs/Secure-Programs-HOWTO/avoid-race.html
	*/
	close(n);
	unlink(magick_tmp);
	atexit(cleanup);	/* remove magick_tmp at exit. */
	
	showtime("Initializing load_image:");
	InitializeMagick("xdvi.bin");
	GetExceptionInfo(&exception);
	image_info = CloneImageInfo((ImageInfo *) NULL);
	
	if (!XMatchVisualInfo(DISP, XScreenNumberOfScreen(SCRN),
			      G_depth, G_visual->class, &visual_info)) {
	    fprintf(stderr, "Magick: can't get visual info, image disabled.\n");
	    disabled = True;
	    return NULL;
	}

	if (resource.magick_cache != NULL) {
	    cache_limit = parse_cache_setting(resource.magick_cache);
	    if (globals.debug & DBG_PS)
		fprintf(stderr, "    Setting magick_cache to %lu\n",
			(unsigned long)cache_limit);
	}
	initialized = True;
    }

    xscale = info->rwi / (double) ((int) (10 * (info->urx - info->llx) + .5));
    yscale = info->rhi / (double) ((int) (10 * (info->ury - info->lly) + .5));
    xdpi = xscale * DPI;
    ydpi = yscale * DPI;
    /* Compute scaled size of the image */
    columns = ROUND(DPI * info->rwi * 0.1 / 72);
    rows = ROUND(DPI * info->rhi * 0.1 / 72);
    if (globals.debug & DBG_PS)
	fprintf(stderr, "\tscale=%0.2fx%0.2f, dpi=%0.2fx%0.2f, size=%ldx%ld\n",
		xscale, yscale, xdpi, ydpi, columns, rows);

    /* expand and canonicalize path name */
    path = find_file(fn, &statbuf, kpse_pict_format);
    if (path != NULL) {
	fullpath = REALPATH(path, canonical_path);
	if (fullpath == NULL) {
	    XDVI_WARNING((stderr, "Couldn't canonicalize %s to full path - returning unexpanded.",
			  path));
	    fullpath = path;
	}
	else {
	    free(path);
	}
    }
    else {
	fprintf(stderr, "%s: %s:%d: Can't find image file %s!\n",
		globals.program_name, __FILE__, __LINE__, fn);
	return NULL;
    }
    
    /* If the file is PS/EPS transform it to JPEG first (faster) */
    n = strlen(fullpath);
    if ((n >= 3 && !memicmp(fullpath + n - 3, ".ps", 3)) ||
	(n >= 4 && !memicmp(fullpath + n - 4, ".eps", 4))) {
	if (!imagick_eps_to_jpeg(fullpath, &columns, &rows, &img, &bgcolor, xdpi, ydpi)) {
	    return NULL;
	}
	/* Should we append the 'JPEG:' prefix to the filename to help
	 * ImageMagick to identify the file format? */
	fullpath = magick_tmp;
    }
    else {
	img.xoff = img.yoff = 0;
    }

    strcpy(image_info->filename, fullpath);
    showtime("ReadImage:");
    if (fullpath == magick_tmp) {
	/* seems 'density' only affects image size of PS/EPS files */
	sprintf(density, "%0.2fx%0.2f", xdpi, ydpi);
	image_info->density = density;
	image_info->units = PixelsPerInchResolution;
    }
    image = ReadImage(image_info, &exception);
    CATCH_EXCEPTION(exception);
    unlink(magick_tmp);
    if (image == NULL)	/* error loading image */
	return NULL;

    /* Set background color for PS/EPS fig */
    if (fullpath == magick_tmp)	/* input is PS */
	image->background_color = bgcolor;

    if (globals.debug & DBG_PS) {
	int i = image->units;
	fprintf(stderr, "\t    image size = %ldx%ld\n", 
		image->columns, image->rows);
	fprintf(stderr, "\t    image resolution = %0.2fx%0.2f (units=%s)\n",
		image->x_resolution, image->y_resolution,
		i == UndefinedResolution ? "???" :
		i == PixelsPerInchResolution ? "PPI" : "PPCM");
    }

    if (fullpath != magick_tmp) {	/* non PS image */
	/* Note: (info->llx info->lly info->urx info->ury) should be
	 * regarded as the viewport */
	if (image_type == TYPE_CONVERT) {
	    image = crop_shift_image(&img, image, &columns, &rows, xscale, yscale, exception);
	}
	else {
	    /* Do nothing. The image will be scaled to the size of the bbox.
	     * ZLB: the output will be identical to dvips iff the bbox of
	     * the PS figure generated by the shell command is equal to
	     * (info->llx info->lly info->urx info->ury).
	     * (this is true if the bbox set via 'bb', 'viewport', etc.,
	     * is equal to the real bbox of the PS figure) */
	}

	if (columns != image->columns || rows != image->rows) {
	    /* Scale image */
	    if (globals.debug & DBG_PS) {
		fprintf(stderr, "\t    scaling image %ldx%ld -> %ldx%ld\n",
			image->columns, image->rows, columns, rows);
	    }
	    showtime("ScaleImage:");
	    tmp_image = ScaleImage(image, columns, rows, &exception);
	    DestroyImage(image);
	    image = tmp_image;
	    CATCH_EXCEPTION(exception);
	    if (image == NULL)
		return NULL;	/* error resizing image */
	}
    }

    /* rotate image */
    if (info->angle) {
	image = rotate_image(&img, image, exception);
    }

#if 0
    DescribeImage(image, stdout, 1 /* verbosity */ );
    DisplayImages(image_info, image);
#endif

#define XStandardPixel(map,color,dx)  (unsigned long) (map.base_pixel + \
  (((color).red   * map.red_max   + (1L << (dx - 1L))) / ((1L << dx) - 1L)) * map.red_mult + \
  (((color).green * map.green_max + (1L << (dx - 1L))) / ((1L << dx) - 1L)) * map.green_mult + \
  (((color).blue  * map.blue_max  + (1L << (dx - 1L))) / ((1L << dx) - 1L)) * map.blue_mult)

    img.bgpixel = XStandardPixel(map_info, image->background_color, 16);
#undef XStandardPixel
    if (img.ximage != NULL) {
	free_ximage(img.ximage);
	img.ximage = NULL;
    }

    /* Transform to XImage */
    pp = GetImagePixels(image, 0, 0, image->columns, image->rows);
    img.ximage = XCreateImage(DISP, G_visual, G_depth, ZPixmap, 0, NULL,
			      image->columns, image->rows, BMBITS, 0);
    if (img.ximage == NULL)
	return NULL;
    size = img.ximage->bytes_per_line * img.ximage->height;
    img.ximage->data = malloc(size ? size : 1); /* NOTE: no xmalloc! */
    if (img.ximage->data == NULL) {
	fprintf(stderr, "Magick: cannot allocate memory for ximage data.\n");
	XDestroyImage(img.ximage);
	img.ximage = NULL;
	return NULL;
    }
    for (y = 0; y < image->rows; y++) {
	for (x = 0; x < image->columns; x++, pp++) {
	    struct rgb color;
            color.r = USHRT_MAX * pp->red / MaxRGB;
            color.g = USHRT_MAX * pp->green / MaxRGB;
            color.b = USHRT_MAX * pp->blue / MaxRGB;
	    XPutPixel(img.ximage, x, y, alloc_color(&color, img.bgpixel));
	}
    }
    DestroyImage(image);

    size += sizeof(XDviImage) + sizeof(XImage);
    if (cache_size + size > cache_limit) {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "    image not cached (cache limit).\n");
	return &img;
    }

    c = realloc(cache, sizeof(XDviImage) * (ncached + 1)); /* NOTE: no xrealloc! */
    if (c == NULL) {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "    image not cached (cache alloc).\n");
	return &img;
    }
    cache_size += size;
    cache = c;
    c = cache + (ncached++);
    *c = img;
    img.ximage = NULL;	/* so next call won't delete this ximage */

    if (globals.debug & DBG_PS)
	fprintf(stderr, "    image cached (cache_size=%uKB, limit=%uKB).\n",
		cache_size / 1024, cache_limit / 1024);
#if 0
    DestroyImageInfo(image_info);
    DestroyExceptionInfo(&exception);
    DestroyMagick();
#endif
    
    return c;
}

static void
display_image(XDviImage * img, int xul, int yul,
	      const struct window_expose_info *expose,
	      const GC gc)
{
    if (globals.debug & DBG_PS)
	fprintf(stderr, "    display_image: pos=%d, %d\n", xul, yul);

    xul -= img->xoff;
    yul -= img->yoff;

#if 0
    XPutImage(DISP, mane.win, globals.gc.copy, img->ximage, 0, 0,
	      xul, yul - img->ximage.height + 1,
	      img->ximage->width, img->ximage->height);
#else
    /* Try to make background pixels transparent */
    {
	XImage *scr;
	int x0 = xul;
	int y0 = yul - img->ximage->height + 1;
	int w = img->ximage->width;
	int h = img->ximage->height;
	int xoff = 0;
	int yoff = 0;
	int x, y;

	if (x0 + w <= expose->min_x || x0 >= expose->max_x ||
	    y0 + h <= expose->min_y || y0 >= expose->max_y)
	    return;
	if (x0 < expose->min_x) {
	    xoff = expose->min_x - x0;
	    x0 = expose->min_x;
	    w -= xoff;
	}
	if (x0 + w > expose->max_x)
	    w = expose->max_x - x0 + 0;
	if (y0 < expose->min_y) {
	    yoff = expose->min_y - y0;
	    y0 = expose->min_y;
	    h -= yoff;
	}
	if (y0 + h > expose->max_y)
	    h = expose->max_y - y0 + 0;

	{
	    /* Need to double check the bounds here, because redraw_page() in
	     * events.c may set larger bounding box (expose->min_x, expose->min_y, etc.)
	     * than actual viewport causing 'BadMatch' error in XGetImage().
	     *
	     * Here we clip the rectangle w*h+x0+y0 with the root window. */
	    XWindowAttributes a;
	    Status status;
	    Window child;

	    status = XGetWindowAttributes(DISP, mane.win, &a);
	    if (!status || a.map_state != IsViewable)
		return;
	    XTranslateCoordinates(DISP, mane.win, a.root, x0, y0, &x, &y,
				  &child);
	    XGetWindowAttributes(DISP, a.root, &a);
	    if (x + w <= a.x || x >= a.x + a.width || y + h <= a.y
		|| y >= a.y + a.height) return;
	    if (x < a.x) {
		x0 += a.x - x;
		xoff += a.x - x;
		w -= a.x - x;
		x = a.x;
	    }
	    if (x + w > a.x + a.width)
		w = a.x + a.width - x;
	    if (y < a.y) {
		y0 += a.y - y;
		yoff += a.y - y;
		h -= a.y - y;
		y = a.y;
	    }
	    if (y + h > a.y + a.height)
		h = a.y + a.height - y;
#if 0
	    XDrawLine(DISP, mane.win, gc, x0, y0, x0 + w - 1, y0);
	    XDrawLine(DISP, mane.win, gc, x0 + w - 1, y0, x0 + w - 1, y0 + h - 1);
	    XDrawLine(DISP, mane.win, gc, x0 + w - 1, y0 + h - 1, x0, y0 + h - 1);
	    XDrawLine(DISP, mane.win, gc, x0, y0 + h - 1, x0, y0);
	    XDrawLine(DISP, mane.win, gc, x0, y0, x0 + w - 1, y0 + h - 1);
	    XDrawLine(DISP, mane.win, gc, x0 + w - 1, y0, x0, y0 + h - 1);

	    XDrawLine(DISP, mane.win, gc, expose->min_x,
		      expose->min_y, expose->max_x, expose->min_y);
	    XDrawLine(DISP, mane.win, gc, expose->max_x,
		      expose->min_y, expose->max_x, expose->max_y);
	    XDrawLine(DISP, mane.win, gc, expose->max_x,
		      expose->max_y, expose->min_x, expose->max_y);
	    XDrawLine(DISP, mane.win, gc, expose->min_x,
		      expose->max_y, expose->min_x, expose->min_y);
	    XDrawLine(DISP, mane.win, gc, expose->max_x,
		      expose->max_y, expose->min_x, expose->min_y);
	    XDrawLine(DISP, mane.win, gc, expose->min_x,
		      expose->max_y, expose->max_x, expose->min_y);
#endif
	}

#ifdef __GNUC__
#warning FIXME: GetImage may still produce 'BadMatch' when dragging \
	the window around (toward outside of the desktop).
#endif
	scr = XGetImage(DISP, mane.win, x0, y0, w, h, AllPlanes, ZPixmap);

#warning FIXME: the loops are only a quick hack and are slow.
	for (y = 0; y < h; y++)
	    for (x = 0; x < w; x++) {
		unsigned long pixel = 0;
		pixel = XGetPixel(img->ximage, x + xoff, y + yoff);
		if (pixel == img->bgpixel)
		    continue;
		XPutPixel(scr, x, y, pixel);
	    }
	XPutImage(DISP, mane.win, globals.gc.copy, scr, 0, 0, x0, y0, w, h);
	XDestroyImage(scr);
    }
#endif
    showtime("    Done.");
}

static void
render_image_file(XDviImageInfo *info, int x, int y)
{
    XDviImage *im = NULL;
    int w, h;

    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: render_image_file: pos=(%d,%d)\n", x, y);

#ifdef __GNUC__
#warning FIXME: immediate return if image not exposed.
#endif

    if (!INSIDE_MANE_WIN)
	return;

    info->angle = bbox_angle >= 0 ? bbox_angle % 360 : 360 - (-bbox_angle) % 360;
    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: render_image_file: angle=%d\n", info->angle);

    if (resource.postscript && (im = load_image(info)) != NULL)
	display_image(im, x, y, &globals.win_expose, globals.gc.rule);

    if (im != NULL && resource.postscript == 1)
	return;

    /* Draw bbox of the image */
    w = ROUND(DPI * info->rwi / 720.0);
    h = ROUND(DPI * info->rhi / 720.0);

    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: render_image_file: box=%dx%d+%d+%d\n",
		w, h, x, y - h + 1);

    if (!info->angle) {
	XDrawRectangle(DISP, currwin.win, globals.gc.high, x, y - h + 1, w, h);
    }
    else
    {
#if 1
	bbox_valid = True;
	bbox_width = w;
	bbox_height = bbox_voffset = h;
	save_bbox();
	bbox_valid = False;
#else
	/* code borrowed from special.c:draw_bbox() */
	double sin_a = sin(info->angle * (TWOPI / 360));
	double cos_a = cos(info->angle * (TWOPI / 360));
	double a, b, c, d;

	a = cos_a * (w-1);
	b = -sin_a * (w-1);
	c = -sin_a * (h-1);
	d = -cos_a * (h-1);

	XDrawLine(DISP, currwin.win, globals.gc.high,
		  x, y, x + ROUND(a), y + ROUND(b));
	XDrawLine(DISP, currwin.win, globals.gc.high,
		  x + ROUND(a), y + ROUND(b),
		  x + ROUND(a + c), y + ROUND(b + d));
	XDrawLine(DISP, currwin.win, globals.gc.high,
		  x + ROUND(a + c), y + ROUND(b + d),
		  x + ROUND(c), y + ROUND(d));
	XDrawLine(DISP, currwin.win, globals.gc.high,
		  x + ROUND(c), y + ROUND(d), x, y);
#endif
    }
}

#if !MAGICK_RENDER_PS
static Boolean
is_ps(char *fn)
{
    int l;
    char *p;
    
    if (strchr(fn, '`') != NULL)
	return False;

    l = strlen(fn);
    p = fn + l - 1;
    while (p > fn && *p != '.' && *p != '/')
	p--;
    l -= p - fn;
    if (*p == '.' && ((l >= 3 && !memicmp(p, ".ps", 3)) ||
		      (l >= 4 && !memicmp(p, ".eps", 4)))) {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: passing PS/EPS file to psgs.c\n");
	return True;
    }
    return False;
}
#endif

/* Process EPSF specials */

static Boolean
epsf_special(char *cp)
{
    char *filename;
    unsigned filename_len;
    char *p;
    int n;
    unsigned long flags = 0, u;
    double rwi, rhi;
    enum { LLX, LLY,
	   URX, URY,
	   RWI, RHI,
	   HSIZE, VSIZE,
	   HOFFSET, VOFFSET,
	   HSCALE, VSCALE,
	   ANGLE
    };
    XDviImageInfo info;

    struct {
	double val;
	const char *key;
	long mask;
    } key_info[] = {
	{ 0, "llx",	0x0001 },
	{ 0, "lly",	0x0002 },
	{ 0, "urx",	0x0004 },
	{ 0, "ury",	0x0008 },
	{ 0, "rwi",	0x0010 },
	{ 0, "rhi",	0x0020 },
	{ 0, "hsize",	0x0040 },
	{ 0, "vsize",	0x0080 },
	{ 0, "hoffset",	0x0100 },
	{ 0, "voffset",	0x0200 },
	{ 0, "hscale",	0x0400 },
	{ 0, "vscale",	0x0800 },
	{ 0, "angle",	0x1000 },
    };

    memset(&info, 0, sizeof info);	/* for validating memcmp(&info, ... */

    filename = cp;
    if (*cp == '\'' || *cp == '"') {
	do
	    ++cp;
	while (*cp != '\0' && *cp != *filename);
	filename_len = cp - filename - 1;
	if (*cp == *filename)
	    ++cp;
	++filename;
    }
    else {
	while (*cp != '\0' && !isspace((int)*cp))
	    ++cp;
	filename_len = cp - filename;
    }

    if (filename_len + 1 > MaxTextExtent) {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: filename too long, passing to psgs.c\n");
	return False;
    }

    memcpy(info.filename, filename, filename_len); /* no need to terminate because of memset(...) above */

    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: epsf_special: filename = '%s'\n",
		info.filename);

#if !MAGICK_RENDER_PS
    if (is_ps(info.filename))
	return False;
#endif

    /* Scan for EPSF keywords */
    while (*cp != '\0') {
	while (isspace((int)*cp))
	    ++cp;
	p = cp;
	while (*p != '=' && !isspace((int)*p) && *p != '\0')
	    p++;
	n = p - cp;
	if (!n)
	    break;
	if (*p == '=')
	    ++p;
	while (isspace((int)*p))
	    ++p;

	if (!memcmp(cp, "clip", n)) {
	    /* this is the only key without value */
	    info.clip = True;
	}
	else {
	    size_t i;
	    Boolean found = False;
	    for (i = 0; !found && i < XtNumber(key_info); i++) {
		if (memicmp(cp, key_info[i].key, n) == 0) {
		    if (sscanf(p, "%lf", &(key_info[i].val)) == 1) {
			flags |= key_info[i].mask;
			while (!isspace((int)*p) && *p != '\0')
			    ++p;
			found = True;
		    }
		    else {
			fprintf(stderr, "Magick: invalid value for %s\n", key_info[i].key);
			return False;
		    }
		}
	    }

	    if (!found) {
		i = *(cp + n);
		*(cp + n) = '\0';
		fprintf(stderr,"Magick: ignoring unknown EPSF key \"%s\".\n", cp);
		*(cp + n) = i;
		/* skip '=...' */
		while (*p != '\0' && !isspace((int)*p))
		    ++p;
	    }
	}
	cp = p;
    }

    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: epsf_special: flags = 0x%lx\n", flags);

    /* only accept the special when llx, lly, urx, ury are properly defined */
    u = key_info[LLX].mask | key_info[LLY].mask |
	key_info[URX].mask | key_info[URY].mask;
    if ((flags & u) != u)
	return False;

#ifdef __GNUC__
#warning keywords to handle: [hv]size, [hv]offset, [hv]scale, angle \
		(ZLB: not used by the graphicx/graphics packages)
#endif
    if (flags & (key_info[HSIZE].mask | key_info[VSIZE].mask |
		 key_info[HOFFSET].mask | key_info[VOFFSET].mask |
		 key_info[HSCALE].mask | key_info[VSCALE].mask |
		 key_info[ANGLE].mask))
	fprintf(stderr, "Magick: warning: EPSF keywords '[hv]size', "
		"'[hv]offset', '[hv]scale', and 'angle' are not "
		"implemented.\n");

    info.urx = key_info[URX].val;
    info.ury = key_info[URY].val;
    info.llx = key_info[LLX].val;
    info.lly = key_info[LLY].val;
    rwi = key_info[RWI].val;
    rhi = key_info[RHI].val;

    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: epsf_special: llx=%0.2f, lly=%0.2f, "
		"urx=%0.2f, ury=%0.2f\n",
		info.llx, info.lly, info.urx, info.ury);

    if (info.urx - info.llx < 1 || info.ury - info.lly < 1)
	return False;
    if (info.urx - info.llx > 1e+4 || info.ury - info.lly > 1e+4)
	return False;
    if (fabs(info.llx) > 1e+10 || fabs(info.lly) > 1e+10)
	return False;

    if (!(flags & (key_info[RWI].mask | key_info[RHI].mask))) {
	/* both rwi and rhi undefined */
	rwi = 10 * (info.urx - info.llx);
	rhi = 10 * (info.ury - info.lly);
    }
    else if (!(flags & key_info[RWI].mask)) {
	/* rwi undefined, rhi defined */
	rwi = rhi * (info.urx - info.llx) / (info.ury - info.lly);
    }
    else if (!(flags & key_info[RHI].mask)) {
	/* rhi undefined, rwi defined */
	rhi = rwi * (info.ury - info.lly) / (info.urx - info.llx);
    }

    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: epsf_special: rwi=%f, rhi=%f\n", rwi, rhi);

    if (rwi < 1 || rhi < 1)
	return False;

    /* We don't want IM to process large images (>16MB) */
    if (DPI * rwi / 720 * DPI * rhi / 720 * G_depth / 8 / 1024 / 1024 > 16) {
	p = filename + (n = filename_len);
	if ((n >= 3 && !memicmp(p - 3, ".ps", 3)) ||
	    (n >= 4 && !memicmp(p - 4, ".eps", 4))) {
	    if (globals.debug & DBG_PS)
		fprintf(stderr, "Magick: image too large, passing to psgs.c\n");
	    return False;
	}
	else {
	    /* don't pass non-PS files to psgs.c */
	    if (globals.debug & DBG_PS)
		fprintf(stderr, "Magick: image too large, ignored.\n");
	    return True;
	}
    }

    info.rwi = ROUND(rwi);
    info.rhi = ROUND(rhi);

    render_image_file(&info, PXL_H - currwin.base_x, PXL_V - currwin.base_y);

    return True;
}

static char *
emgraph_get_dimen(char *cp, double *v)
{
    char *p;
    typedef struct {
	int name_len;
	char *name;
	double factor;
    } Unit;
    Unit *u;
    /* table for converting an arbitrary unit to bp */
#define UNIT(u)	sizeof(u)-1, u
    static Unit units[] = {
	{ UNIT("bp"), 1.0 },
	{ UNIT("in"), 72.0 },
	{ UNIT("cm"), 72.0 / 2.54 },
	{ UNIT("mm"), 72.0 / 2.54 * 0.1 },
	{ UNIT("pt"), 72.0 / 72.27 },
	{ UNIT("sp"), 72.0 / 72.27 / 65536.0 },
	{ UNIT("pc"), 12.0 * 72.0 / 72.27 },
	{ UNIT("dd"), 1238.0 / 1157.0 * 72.0 / 72.27 },
	{ UNIT("cc"), 12.0 * 1238.0 / 1157.0 * 72.0 / 72.27 }
    };

    *v = strtod(cp, &p);
    if (p == NULL) {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: can't get dimension, image ignored.\n");
	return NULL;
    }

    while (isspace((int)*p))
	++p;
    for (u = units; u < units + XtNumber(units); u++)
	if (!memicmp(p, u->name, u->name_len))
	    break;
    if (u >= units + XtNumber(units)) {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: unknown unit, image ignored.\n");
	return NULL;
    }
    *v *= u->factor;

    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: unit=%s, len=%d, factor=%0.4f, value=%0.4f\n",
		u->name, u->name_len, u->factor, *v);

    if (*v < 0.01 || *v > 10000) {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: invalid dimension, image ignored.\n");
	return NULL;
    }

    return p + u->name_len;
}

static Boolean
emgraph_special(char *cp)
{
    char *p;
    double w, h;
    XDviImageInfo info;

    memset(&info, 0, sizeof info);

    /* get filename */
    while (isspace((int)*cp))
	++cp;
    p = cp;
    while (!isspace((int)*p) && *p != '\0' && *p != ',')
	++p;
    if ((unsigned)(p - cp) >= sizeof info.filename) {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: filename too long, image ignored.\n");
	return False;
    }
    if (p <= cp) {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: empty filename, image ignored.\n");
	return False;
    }
    memcpy(info.filename, cp, p - cp); /* no need to terminate because of memset(...) above */

    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: emgraph_special: filename = '%s'\n",
		info.filename);

#if !MAGICK_RENDER_PS
    if (is_ps(info.filename))
	return False;
#endif

    cp = p;

    /* get width */
    if ((cp = strchr(cp, ',')) == NULL) {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: width undefined, image ignored.\n");
	return False;
    }
    if ((cp = emgraph_get_dimen(++cp, &w)) == NULL)
	return False;

    /* get height */
    if ((cp = strchr(cp, ',')) == NULL) {
	if (globals.debug & DBG_PS)
	    fprintf(stderr, "Magick: height undefined, image ignored.\n");
	return False;
    }
    if ((cp = emgraph_get_dimen(++cp, &h)) == NULL)
	return False;

    if (globals.debug & DBG_PS)
	fprintf(stderr, "Magick: filename=\"%s\", width=%0.2f, height=%0.2f\n",
		info.filename, w, h);

    info.llx = info.lly = 0;
    info.urx = w;
    info.ury = h;
    info.rwi = ROUND(10 * w);
    info.rhi = ROUND(10 * h);

    if (info.rwi < 1 || info.rhi < 1)
	return False;

    render_image_file(&info, PXL_H - currwin.base_x,
		      PXL_V - currwin.base_y + ROUND(h / 72 * DPI) - 1);

    return True;
}

/* Filter specials. Returns True if the special has been processed
 * by ImageMagick, False otherwise. */
Boolean
Magick_parse_special(char *cp)
{
    switch (*cp) {
    case 'p':
    case 'P':
	if (!memicmp(cp, "psfile", 6)) {
	    if (globals.debug & DBG_PS)
		fprintf(stderr, "Magick: parsing string |%s|\n", cp);
	    cp += 6;
	    while (isspace((int)*cp))
		++cp;
	    if (*cp != '=')
		return False;
	    do
		++cp;
	    while (isspace((int)*cp));
	    return epsf_special(cp);
	}
	break;
    case 'e':
    case 'E':
	if (!memicmp(cp, "em:", 3)) {
	    cp += 3;
	    while (isspace((int)*cp))
		++cp;
	    if (memicmp(cp, "graph", 5))
		return False;
	    cp += 5;
	    if (globals.debug & DBG_PS)
		fprintf(stderr, "Magick: parsing string |em:graph %s|\n", cp);
	    return emgraph_special(cp);
	}
	break;
    }

    return False;
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* MAGICK */
