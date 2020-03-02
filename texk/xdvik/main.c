/*
 * Copyright (c) 2013-2014 the xdvik development team
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
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "xdvi-config.h"

#include <locale.h>

#include <X11/Intrinsic.h>

#include "kpathsea/proginit.h"
#include "kpathsea/expand.h"

#include "xdvi.h"
#include "util.h"
#include "x_util.h"
#include "sfSelFile.h"
#include "my-snprintf.h"
#include "dvi-init.h"
#include "filehist.h"
#include "mag.h"
#include "message-window.h"

#if FREETYPE
# include <ft2build.h>
# include FT_FREETYPE_H
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#ifndef XDVI_KPSE_PROG_NAME
#define XDVI_KPSE_PROG_NAME xdvi
#endif
static const char *xdvi_kpse_prog_name = TOSTRING(XDVI_KPSE_PROG_NAME);
#undef STRINGIFY
#undef TOSTRING

static char XtRBool3[] = "Bool3";	/* resource for Bool3 */

/* get these before setting `application_resources' */
static XtResource xdvirc_resources[] = {
    {"noInitFile", "NoInitFile", XtRBoolean, sizeof(Boolean),
     XtOffsetOf(struct x_resources, no_init_file), XtRString, "false"},
};


static XrmOptionDescRec options[] = {
    {"-q",		".noInitFile",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+q",		".noInitFile",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-geometry",       ".geometry",	        XrmoptionSepArg,	(XPointer)NULL	},
#ifdef MOTIF
    /* to make `-font' and `-fn' options work, make them an alias for `fontList' */
    { "-font",		"*fontList",		XrmoptionSepArg,	(XPointer)NULL	},
    { "-fn",		"*fontList",		XrmoptionSepArg,	(XPointer)NULL	},
#endif
    {"-s",		".shrinkFactor",	XrmoptionSepArg,	(XPointer)NULL	},
#ifndef	VMS
    {"-S",		".densityPercent",	XrmoptionSepArg,	(XPointer)NULL	},
#endif
    {"-density",	".densityPercent",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-noomega",	".omega",		XrmoptionNoArg,		(XPointer)"off"	},
    {"+noomega",	".omega",		XrmoptionNoArg,		(XPointer)"on"	},
#if COLOR
    {"-nocolor",	".color",		XrmoptionNoArg,		(XPointer)"off"	},
    {"+nocolor",	".color",		XrmoptionNoArg,		(XPointer)"on"	},
#endif
#ifdef GREY
    {"-nogrey",		".grey",		XrmoptionNoArg,		(XPointer)"off"	},
    {"+nogrey",		".grey",		XrmoptionNoArg,		(XPointer)"on"	},
    {"-gamma",		".gamma",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-nomatchinverted",".matchInverted",	XrmoptionNoArg,	(XPointer)"off"	},
    {"+nomatchinverted",".matchInverted",	XrmoptionNoArg,	(XPointer)"on"	},	
    /*     {"-invertedfactor", ".invertedFactor",	XrmoptionSepArg,	(XPointer)NULL	}, */
    {"-install",	".install",		XrmoptionNoArg,		(XPointer)"on"	},
    {"-noinstall",	".install",		XrmoptionNoArg,		(XPointer)"off"	},
#endif
    {"-rulecolor",	".ruleColor",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-p",		".pixelsPerInch",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-margins",	".Margin",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-sidemargin",	".sideMargin",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-topmargin",	".topMargin",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-offsets",	".Offset",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-xoffset",	".xOffset",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-yoffset",	".yOffset",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-paper",		".paper",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-altfont",	".altFont",		XrmoptionSepArg,	(XPointer)NULL	},
#ifdef MKTEXPK
    {"-nomakepk",	".makePk",		XrmoptionNoArg,		(XPointer)"off"	},
    {"+nomakepk",	".makePk",		XrmoptionNoArg,		(XPointer)"on"	},
#endif
    {"-mfmode",		".mfMode",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-editor",		".editor",		XrmoptionSepArg,	(XPointer)NULL	},
#if FREETYPE
    {"-notype1fonts",	".type1",		XrmoptionNoArg,		(XPointer)"off"	},
    {"+notype1fonts",	".type1",		XrmoptionNoArg,		(XPointer)"on"	},
#endif
#if HAVE_XI21
    {"-noxi2scrolling",	".xi2Scrolling",	XrmoptionNoArg,		(XPointer)"off"},
    {"+noxi2scrolling",	".xi2Scrolling",	XrmoptionNoArg,		(XPointer)"on"},
#endif
    {"-sourceposition",	".sourcePosition",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-findstring",	".findString",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-text-encoding",	".textEncoding",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-unique",		".unique",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+unique",		".unique",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-nofork",		".fork",		XrmoptionNoArg,		(XPointer)"off" },
    {"+nofork",		".fork",		XrmoptionNoArg,		(XPointer)"on"  },
#ifdef RGB_ANTI_ALIASING
    {"-subpixels",	".subPixels",		XrmoptionSepArg,	(XPointer)NULL	},
#endif
    {"-l",		".listFonts",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+l",		".listFonts",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-watchfile",	".watchFile",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-expertmode",	".expertMode",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-expert",		".expert",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+expert",		".expert",		XrmoptionNoArg,		(XPointer)"off"	},
    {"+statusline",	".statusline",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-statusline",	".statusline",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+useTeXpages",	".useTeXPages",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-useTeXpages",	".useTeXPages",		XrmoptionNoArg,		(XPointer)"on"	},
    {"-mgs",		".magnifierSize1",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-mgs1",		".magnifierSize1",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-mgs2",		".magnifierSize2",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-mgs3",		".magnifierSize3",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-mgs4",		".magnifierSize4",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-mgs5",		".magnifierSize5",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-warnspecials",	".warnSpecials",	XrmoptionNoArg,		(XPointer)"on"	},
    {"+warnspecials",	".warnSpecials",	XrmoptionNoArg,		(XPointer)"off"	},
    {"-hush",		".Hush",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+hush",		".Hush",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-hushchars",	".hushLostChars",	XrmoptionNoArg,		(XPointer)"on"	},
    {"+hushchars",	".hushLostChars",	XrmoptionNoArg,		(XPointer)"off"	},
    {"-hushchecksums",	".hushChecksums",	XrmoptionNoArg,		(XPointer)"on"	},
    {"+hushchecksums",	".hushChecksums",	XrmoptionNoArg,		(XPointer)"off"	},
    {"-hushstdout",	".hushStdout",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+hushstdout",	".hushStdout",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-hushbell",	".hushBell",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+hushbell",	".hushBell",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-safer",		".safer",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+safer",		".safer",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-fg",		".foreground",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-foreground",	".foreground",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-bg",		".background",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-background",	".background",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-hl",		".highlight",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-cr",		".cursorColor",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-icongeometry",	".iconGeometry",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-keep",		".keepPosition",	XrmoptionNoArg,		(XPointer)"on"	},
    {"+keep",		".keepPosition",	XrmoptionNoArg,		(XPointer)"off"	},
    {"-copy",		".copy",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+copy",		".copy",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-thorough",	".thorough",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+thorough",	".thorough",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-fullscreen",	".fullscreen",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+fullscreen",	".fullscreen",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-pause",		".pause",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+pause",		".pause",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-pausespecial",	".pauseSpecial",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-wheelunit",	".wheelUnit",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-mousemode",	".mouseMode",		XrmoptionSepArg,	(XPointer)NULL	},
#ifdef PS
    {"-postscript",	".postscript",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-allowshell",	".allowShell",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+allowshell",	".allowShell",		XrmoptionNoArg,		(XPointer)"off"	},
# ifdef	PS_DPS
    {"-nodps",		".dps",			XrmoptionNoArg,		(XPointer)"off"	},
    {"+nodps",		".dps",			XrmoptionNoArg,		(XPointer)"on"	},
# endif
# ifdef	PS_NEWS
    {"-nonews",		".news",		XrmoptionNoArg,		(XPointer)"off"	},
    {"+nonews",		".news",		XrmoptionNoArg,		(XPointer)"on"	},
# endif
# ifdef	PS_GS
    {"-noghostscript",	".ghostscript",		XrmoptionNoArg,		(XPointer)"off"	},
    {"+noghostscript",	".ghostscript",		XrmoptionNoArg,		(XPointer)"on"	},
    {"-nogssafer",	".gsSafer",		XrmoptionNoArg,		(XPointer)"off"	},
    {"+nogssafer",	".gsSafer",		XrmoptionNoArg,		(XPointer)"on"	},
    {"-gsalpha",	".gsAlpha",		XrmoptionNoArg,		(XPointer)"on"	},
    {"+gsalpha",	".gsAlpha",		XrmoptionNoArg,		(XPointer)"off"	},
    {"-interpreter",	".interpreter",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-gspalette",	".palette",		XrmoptionSepArg,	(XPointer)NULL	},
# endif
# ifdef	MAGICK
    {"-magick",		".ImageMagick",		XrmoptionNoArg,		(XPointer)"on"  },
    {"+magick",		".ImageMagick",		XrmoptionNoArg,		(XPointer)"off" },
    {"-magick_cache",	".MagickCache",		XrmoptionSepArg,	(XPointer)NULL	},
# endif
#endif /* PS */
    {"-noscan",		".prescan",		XrmoptionNoArg,		(XPointer)"off"	},
    {"+noscan",		".prescan",		XrmoptionNoArg,		(XPointer)"on"	},
    {"-notempfile",	".tempFile",		XrmoptionNoArg,		(XPointer)"off"	},
    {"+notempfile",	".tempFile",		XrmoptionNoArg,		(XPointer)"on"	},
    {"-dvipspath",	".dvipsPath",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-ps2pdfpath",	".ps2pdfPath",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-debug",		".debugLevel",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-linkstyle",	".linkStyle",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-linkcolor",	".linkColor",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-visitedlinkcolor",".visitedLinkColor",	XrmoptionSepArg,	(XPointer)NULL	},
    {"-browser",	".wwwBrowser",		XrmoptionSepArg,	(XPointer)NULL	},
    {"-anchorposition",	".anchorPosition",	XrmoptionSepArg,	(XPointer)NULL	},
};

/*
 * Data for options processing.
 */
static const char SILENT[] = " ";	/* flag value for usage() */
static const char SUBST[] = "x";	/* another flag value */
static const char USAGESTR_END_MARKER[] = "__USAGE_END_MARKER__"; /* end marker */

/* Here, list usage values for options that have `XrmoptionSepArg' set.
   TODO: what does the `^' stand for?
*/
static const char *usagestr[] = {
    /* geometry		*/ SILENT,
#ifdef MOTIF
    /* font		*/ SILENT,
    /* f		*/ SILENT,
#endif
    /* shrinkFactor	*/ "shrink",
#ifndef	VMS
    /* S		*/ "density",
    /* density		*/ SILENT,
#else
    /* density		*/ "density",
#endif
#ifdef	GREY
    /* gamma		*/ "float",
#endif
    /* rulecolor	*/ "color",
    /* p		*/ "pixels",
    /* margins		*/ "dimen",
    /* sidemargin	*/ "dimen",
    /* topmargin	*/ "dimen",
    /* offsets		*/ "dimen",
    /* xoffset		*/ "dimen",
    /* yoffset		*/ "dimen",
    /* paper		*/ "papertype",
    /* altfont		*/ "font",
    /* mfmode		*/ "mode-def",
    /* editor		*/ "editor",
    /* sourceposition	*/ "linenumber[ ]*filename",
    /* findstring	*/ "string",
    /* textencoding	*/ "charset",
#ifdef RGB_ANTI_ALIASING
    /* subpixels	*/ "{rgb,bgr}[ i1 i2 i3]",
#endif
    /* rv		*/ "^-l", "-rv",
    /* watchfile	*/ "secs",
    /* expertmode	*/ "flag",
    /* mgs		*/ SUBST,
    /* mgs1		*/ SILENT,
    /* mgs2		*/ SILENT,
    /* mgs3		*/ SILENT,
    /* mgs4		*/ SILENT,
    /* mgs5		*/ SILENT,
    /* fg		*/ "color",
    /* foreground	*/ SILENT,
    /* bg		*/ "color",
    /* background	*/ SILENT,
    /* hl		*/ "color",
    /* cr		*/ "color",
#ifndef VMS
    /* display		*/ "^-cr", "-display <host:display>",
#else
    /* display		*/ "^-cr", "-display <host::display>",
#endif
    /* geometry		*/ "^-cr", "-geometry <geometry>",
    /* icongeometry	*/ "geometry",
    /* iconic		*/ "^-icongeometry", "-iconic",
    /* font		*/ "^-icongeometry", "-font <font>",
    /* pausespecial	*/ "string",
    /* wheelunit	*/ "pixels",
    /* mousemode	*/ "0|1|2",
#ifdef PS
    /* postscript	*/ "0|1|2",
# ifdef PS_GS
    /* interpreter	*/ "path",
    /* gspalette	*/ "monochrome|grayscale|color",
# endif
# ifdef MAGICK
    /* magick_cache	*/ "size[k|K|m|M|g|G]",
# endif
#endif
    /* dvipspath	*/ "path",
    /* ps2pdfpath	*/ "path",
    /* debug		*/ "bitmask|string[,string ...]",
    /* linkstyle	*/ "0|1|2|3",
    /* linkcolor	*/ "color",
    /* visitedlinkcolor	*/ "color",
    /* browser		*/ "WWWbrowser",
    /* anchorposition	*/ "anchor",
    /* [end marker]	*/ USAGESTR_END_MARKER
};

static const char *SUBST_VAL[] = { "-mgs[n] <size>" };

static int
compare_strings(const void *s, const void *t)
{
    const char *const *ss = (const char *const *)s;
    const char *const *tt = (const char *const *)t;

    return memicmp(*ss, *tt, strlen(*tt) + 1); /* also check for final 0 */
}

static void
usage(int exitval)
{
    XrmOptionDescRec *opt;
    const char **usageptr = usagestr;
    const char **sv = SUBST_VAL;
    const char *str1;
    const char *str2;
    const char *sorted_options[XtNumber(options)];
    char buf[256];
    char *s;
    int col, n;
    size_t nopt = 0, k;
    
    for (opt = options; opt < options + XtNumber(options); ++opt) {
	str1 = opt->option;
	if (*str1 != '-')
	    continue;

	ASSERT(*usageptr != USAGESTR_END_MARKER, "Too few elements in usageptr[]");

	str2 = NULL;
	if (opt->argKind != XrmoptionNoArg) {
	    str2 = *usageptr++;
	    if (str2 == SILENT)
		continue;
	    if (str2 == SUBST) {
		str1 = *sv++;
		str2 = NULL;
	    }
	}
#if 0
	fprintf(stderr, "str1: %s, str2: %s\n", str1, str2);
#endif
	for (;;) {
	    if (str2 == NULL)
		sprintf(buf, "[%.80s]", str1);
	    else
		sprintf(buf, "[%.80s <%.80s>]", str1, str2);

	    /* 	    fprintf(stderr, "number of options: %d; len of usagestr: %d\n", */
	    /* 		    XtNumber(options), XtNumber(usagestr)); */
	    ASSERT(nopt < XtNumber(options), "index out of range");
	    /* 	    fprintf(stderr, "sorted: %d=%s\n", nopt, buf); */
	    sorted_options[nopt++] = xstrdup(buf);
	    
	    if (**usageptr != '^' || strcmp(*usageptr + 1, opt->option) != 0)
		break;
	    ++usageptr;
	    str1 = *usageptr++;
	    str2 = NULL;
	}
    }

    ASSERT(*usageptr == USAGESTR_END_MARKER, "Too many elements in usageptr[]");

    /*     fprintf(stderr, "elems in sorted options: %d\n", nopt); */
    qsort((void*)sorted_options,
	  nopt,
	  sizeof(sorted_options[0]),
	  compare_strings);
    
    s = xstrdup("Usage: ");
    s = xstrcat(s, XDVI_PROGNAME); /* use `xdvi' here, not `xdvik' or `xdvi-xaw.bin' or ... */
    s = xstrcat(s, " [+[<page>]] [-h | --help] [-v | --version] [-license]");

    col = strlen(s);
    fputs(s, stdout);
    
    for (k = 0; k < nopt; ++k) {
	n = strlen(sorted_options[k]);
	if (col + n < 80)
	    putc(' ', stdout);
	else {
	    fputs("\n\t", stdout);
	    col = 8 - 1;
	}
	fputs(sorted_options[k], stdout);
	col += n + 1;
    }

    /* put this in an extra line, to emphasize that it must come last */
    fputs("\n\t[dvi_file]\n", stdout);

    xdvi_exit(exitval);
}


static void
display_bug_reporting_info(void)
{
    printf("Please send bug reports, feature requests etc. to one of:\n"
	   "   http://sourceforge.net/tracker/?group_id=23164&atid=377580\n"
	   "   tex-k@tug.org (http://tug.org/mailman/listinfo/tex-k)\n\n"
	   "\n");
}

static void
display_licensing_info(void)
{
    fputs("Licenses: X Consortium license, GNU Library General Public\n"
	  "License, GNU General Public License (use option `-license'\n"
	  "for more details). There is NO WARRANTY of anything.\n\n", stdout);
}

static void
display_long_licensing_info(void)
{
    fputs("The major parts of Xdvik are licensed under the X Consortium license.\n"
	  "Parts (encoding.c) are licensed under the GNU General Public License.\n"
	  "Xdvik uses the following libraries:\n"
	  "- The kpathsea library, licensed in part under the GNU General Public\n"
	  "  License, in part under the GNU Library General Public License.\n"
	  "- FreeType2, licensed under the GNU General Public License.\n"
	  "There is NO WARRANTY of anything.\n\n", stdout);
}

static char *
is_good_dvi_file(const char *filename, Boolean from_history)
{
    static char canonical_path[MAXPATHLEN + 1];
    Boolean tried_dvi_extension = False;
    /* following allocates real_filename */
    char *real_filename = find_dvi_file(filename, &tried_dvi_extension, from_history);
    char *ret;
    FILE *f = NULL;
    dviErrFlagT errflag;
	
    if (real_filename == NULL)
	return NULL;

    if ((ret = REALPATH(real_filename, canonical_path)) == NULL) {
	/* REALPATH failed, use real_filename */
	size_t n = strlen(real_filename);
	if (n >= MAXPATHLEN) {
	  n = MAXPATHLEN - 1;
	}

	memcpy(canonical_path, real_filename, n);
	canonical_path[n] = '\0';
	ret = canonical_path;
    }
    free(real_filename);

    /* check for correct DVI files */
    if ((f = XFOPEN(ret, OPEN_MODE)) != NULL) {
	TRACE_EVENTS((stderr, "watching: new file opened successfully."));
	if (process_preamble(f, &errflag)
	    && find_postamble(f, &errflag)
	    && read_postamble(f, &errflag, False
#if DELAYED_MKTEXPK
			      , False
#endif
			      )) {
	    fclose(f);
	    return ret;
	}
	fclose(f);
	if (!from_history)
	    XDVI_FATAL((stderr, "%s: %s.", filename, get_dvi_error(errflag)));
	return NULL;
    }
    else {
	if (!from_history)
	    XDVI_FATAL((stderr, "Could not open `%s': %s.", filename, strerror(errno)));
	return NULL;
    }
}


static char *
get_filename_from_history(int *pageno)
{
    size_t i;
    /* loop through history, trying to get a good file */
    for (i = 0; i < file_history_size(); i++) {
	char *ret, *test;
	
	if ((test = file_history_get_elem(i, pageno)) == NULL)
	    return NULL;
	TRACE_FILES((stderr, "HISTORY %lu: |%s|", (unsigned long)i, test));
	if ((ret = is_good_dvi_file(test, True)) != NULL) {
	    TRACE_FILES((stderr, "SUCCESS: |%s|", test));
	    return ret;
	}
    }
    return NULL;
}

static void
warn_about_prerelease_versions(void)
{
    int unstable_version = 0;
    if (strstr(XDVI_VERSION_INFO, "-cvs") != NULL)
	unstable_version = 1;
    else if (strstr(XDVI_VERSION_INFO, "-beta") != NULL)
	unstable_version = 2;

    if (unstable_version > 0) {
	printf("\n**********************************************************************\n");
	printf("%s version %s,\n%s version.\n\n", XDVIK_PROGNAME, XDVI_VERSION_INFO,
	       unstable_version == 1 ? "an unstable development" : "a beta testing");
	printf("Want a stable version instead?\n"
	       " -> please visit one of:\n"
	       "    http://xdvi.sourceforge.net/cvs-upgrade.html\n"
	       "    http://sourceforge.net/project/showfiles.php?group_id=23164\n\n"
	       "Found a bug?\n"
	       " -> please report it to:\n"
	       "    http://sourceforge.net/tracker/?group_id=23164&atid=377580\n\n"
	       "Thanks for your support!\n");
	printf("**********************************************************************\n");
    }
}

/*
  Initialize internal data (most of them global ...) according to the values
  of resources/command-line arguments, warning user about illegal values
  etc.
*/
static void
init_check_resources(void)
{
    size_t i;
    
    if (resource.mfmode != NULL) {
	char *p;

	p = strrchr(resource.mfmode, ':');
	if (p != NULL) {
	    unsigned int len;
	    char *p1;

	    ++p;
	    len = p - resource.mfmode;
	    p1 = xmalloc(len);
	    memcpy(p1, resource.mfmode, len - 1);
	    p1[len - 1] = '\0';
	    resource.mfmode = p1;
	    resource.pixels_per_inch = atoi(p);
	}
    }
    if (currwin.shrinkfactor < 0) {
	XDVI_ERROR((stderr, "Invalid shrink factor: %d.", currwin.shrinkfactor));
	usage(EXIT_FAILURE);
    }
    if (resource.density <= 0) {
	XDVI_ERROR((stderr, "Invalid shrink density: %d.", resource.density));
	usage(EXIT_FAILURE);
    }
    if (resource.pixels_per_inch <= 0) {
	XDVI_ERROR((stderr, "Invalid dpi value: %d.", resource.pixels_per_inch));
	usage(EXIT_FAILURE);
    }
    if (resource.link_style < 0 || resource.link_style > 3) {
	XDVI_ERROR((stderr, "Unrecognized value %d for resource \"linkstyle\" (valid range is 0 - 3); assuming 3.",
		    resource.link_style));
	resource.link_style = 3;
    }
    if (currwin.shrinkfactor > 1) {
	mane.shrinkfactor = currwin.shrinkfactor;	/* otherwise it's 1 */
    }

#ifdef RGB_ANTI_ALIASING
#warning Note: RGB Anti-aliasing enabled
    /* subpixel rendering */
    resource.subpixel_order = SUBPIXEL_NONE;
    if (resource.sub_pixels != NULL) {
	int sum;
	
	if (memicmp(resource.sub_pixels, "rgb", 3) == 0)
	    resource.subpixel_order = SUBPIXEL_RGB;
	else if (memicmp(resource.sub_pixels, "bgr", 3) == 0)
	    resource.subpixel_order = SUBPIXEL_BGR;
	else if (memicmp(resource.sub_pixels, "none", 3) == 0)
	    resource.subpixel_order = SUBPIXEL_NONE;
	else {
	    XDVI_ERROR((stderr,
			"Unrecognized value \"%s\" for resource subpixels\n"
			"(possible values are: \"rgb\" or \"bgr\").",
			resource.sub_pixels));
	    xdvi_exit(EXIT_FAILURE);
	}
	/* get the energy distribution */
	if (resource.subpixel_order == SUBPIXEL_RGB || resource.subpixel_order == SUBPIXEL_BGR) {
	    const char *ptr = resource.sub_pixels + 3;
	    while (isspace(*ptr))
		ptr++;
	    fprintf(stderr, "ptr: |%s|\n", ptr);
	    resource.subpixel_energy[0] = 33.333;
	    resource.subpixel_energy[1] = 33.333;
	    resource.subpixel_energy[2] = 0.0;
	    if (*ptr != '\0') {
		if (sscanf(ptr, "%f %f %f",
			   &(resource.subpixel_energy[0]),
			   &(resource.subpixel_energy[1]),
			   &(resource.subpixel_energy[2]))
		    != 3) {
		    XDVI_ERROR((stderr,
				"Illegal color mask `%s' for resource subpixels (should be: `n n n')\n",
				ptr));
		}
	    }

	    sum = (int)(resource.subpixel_energy[0] +
			2 * resource.subpixel_energy[1] +
			2 * resource.subpixel_energy[2]);
	    if (sum < 99 || sum > 100) {
		XDVI_WARNING((stderr, "energy values %f + 2 * %f + 2 * %f don't sum up to 100%%!\n",
			      resource.subpixel_energy[0], resource.subpixel_energy[1], resource.subpixel_energy[2]));
		exit(1);
	    }
	    
	    resource.subpixel_energy[0] /= 100.0;
	    resource.subpixel_energy[1] /= 100.0;
	    resource.subpixel_energy[2] /= 100.0;
	    fprintf(stderr, "subpixel order: %s = %d; [%f %f %f]\n",
		    resource.sub_pixels, resource.subpixel_order,
		    resource.subpixel_energy[0], resource.subpixel_energy[1], resource.subpixel_energy[2]);
	}
    }
#endif

    /* margins */
    if (resource.sidemargin)
	resource.sidemargin_int = atopix(resource.sidemargin);
    if (resource.topmargin)
	resource.topmargin_int = atopix(resource.topmargin);
    resource.xoffset_int = resource.xoffset ? atopix_signed(resource.xoffset)
	: resource.pixels_per_inch;
    resource.yoffset_int = resource.yoffset ? atopix_signed(resource.yoffset)
	: resource.pixels_per_inch;

    /* paper type */
    if (!set_paper_type(resource.paper)) {
	const char **p;
	char *helpmsg = xstrdup("Possible paper types are:\n    ");
	const char **paper_types = get_paper_types();
	for (p = paper_types; p < paper_types + get_paper_types_size(); p += 2) {
	    if (**p == '\0') { /* next line of list */
		helpmsg = xstrcat(helpmsg, "\n    ");
	    }
	    else {
		helpmsg = xstrcat(helpmsg, *p);
		helpmsg = xstrcat(helpmsg, " ");
	    }
	}
	helpmsg = xstrcat(helpmsg,
			  "\n(the names ending with `r' are `rotated' or `landscape' variants).\n"
			  "Alternatively, you can specify the dimensions as `WIDTHxHEIGHT', followed "
			  "by a dimension unit (one of: pt pc in bp cm mm dd cc sp).");
	/* also dump it to stderr ... */
	fprintf(stderr,
		"Unrecognized value `%s' for paper type option; using %s instead.\n%s\n",
		resource.paper, DEFAULT_PAPER, helpmsg);
	
	popup_message(globals.widgets.top_level,
		      MSG_WARN, helpmsg,
		      "Unrecognized value `%s' for paper type option; using a4 instead.", resource.paper);
	set_paper_type(DEFAULT_PAPER);
    }

    /* magnifier sizes */
    for (i = 0; i < get_magglass_items(); ++i) {
	if (resource.mg_arg[i] != NULL) {
	    char *s;

	    int n = atoi(resource.mg_arg[i]);
	    set_magglass_widht(i, n);
	    set_magglass_height(i, n);
	    s = strchr(resource.mg_arg[i], 'x');
	    if (s != NULL) {
		set_magglass_height(i, atoi(s + 1));
		if (get_magglass_height(i) <= 0)
		    set_magglass_widht(i, 0);
	    }
	}
    }

#ifdef PS
    if (resource.safer) {
	resource.allow_shell = False;
# ifdef PS_GS
	resource.gs_safer = True;
# endif /* PS_GS */
    }
# ifdef	PS_GS
    {
	const char *CGMcgm = "CGMcgm";
	const char *cgmp;

	cgmp = strchr(CGMcgm, resource.gs_palette[0]);
	if (cgmp == NULL)
	    XDVI_FATAL((stderr, "Invalid value %s for gs palette option", resource.gs_palette));
	if (cgmp >= CGMcgm + 3) {
	    static char gsp[] = "x";

	    gsp[0] = *(cgmp - 3);
	    resource.gs_palette = gsp;
	}
    }
# endif /* PS_GS */
#endif /* PS */

    /* The old `-expert' flag overrides resource.expert_mode, `+expert' (or not
       setting it) just uses expert_mode.
    */
    if (resource.expert)
	resource.expert_mode = XPRT_SHOW_NONE;
    /*      fprintf(stderr, "++++++++ initializing resource.expert_mode: %d\n", resource.expert_mode); */
    update_expert_mode();

    if (resource.hush) {
	resource.hush_chars = resource.hush_chk = resource.hush_stdout = resource.hush_bell = True;
    }    
}

/*
  a custom error handler that makes it easier to catch X errors in the debugger
  (by setting a breakpoint to this function, plus using the -sync option).
  Also, Motif sometimes gives non-fatal errors that we want to ignore ...
*/ 
static int
x_error_handler(Display *display, XErrorEvent *error)
{
    char buf[1024], req_buf[1024];

    if (error->request_code < 128) {
	char num[LENGTH_OF_INT];
	sprintf(num, "%d", error->request_code);
	XGetErrorDatabaseText(display, "XRequest", num, "", req_buf, 1024);
    }
    else {
	req_buf[0] = '\0';
    }
    
    XGetErrorText(display, error->error_code, buf, sizeof buf);
    /*     XtCloseDisplay(DISP); */
    if (error->error_code == BadWindow
	|| error->error_code == BadPixmap
	|| error->error_code == BadCursor
	|| error->error_code == BadFont
	|| error->error_code == BadDrawable
	|| error->error_code == BadColor
	|| error->error_code == BadGC
	|| error->error_code == BadIDChoice
	|| error->error_code == BadValue
	|| error->error_code == BadAtom) {
	XDVI_WARNING((stderr, "X protocol error: %s\n    X Request %d (%s), Value=0x%x.",
		      buf, error->request_code, req_buf, (unsigned int)error->resourceid));
    }
    else {
	XDVI_WARNING((stderr, "X protocol error: %s\n    X Request %d (%s).",
		      buf, error->request_code, req_buf));
    }
    return 0;
}

static void
display_version_info(void)
{
    printf("%s version %s ", XDVIK_PROGNAME, XDVI_VERSION);
#ifdef MOTIF
    printf("(%s, runtime version %d.%d)\n",
	   /* 	   XmVERSION, XmREVISION, XmUPDATE_LEVEL, */
	   XmVERSION_STRING,
	   xmUseVersion / 1000, xmUseVersion % 1000);
#else
    printf("%s\n", XDVI_GUI);
#endif
#if FREETYPE
    printf("Libraries: %s, freetype version %d.%d.%d\n",
      kpathsea_version_string, FREETYPE_MAJOR, FREETYPE_MINOR, FREETYPE_PATCH);
#else
    printf("Libraries: %s\n", kpathsea_version_string);
#endif
}


static void
check_early_arguments(int argc, char **argv)
{
    /* This checks arguments that need to work before the X machinery is
     * started (e.g. if no display is available, or for information that is
     * needed before the X defaults are evaluated), so the `options' structure
     * can't be used for them.
     *
     * We need to loop through all arguments in case xdvi is aliased,
     * or called via a shell script (like in teTeX) that adds things
     * like `-name' at the beginning of the arglist.
     */
    int i;
    Boolean install_err_handler = True;
    
    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-help") == 0
	    || strcmp(argv[i], "-h") == 0
	    || strcmp(argv[i], "+help") == 0
	    || strcmp(argv[i], "--help") == 0) {
	    printf("%s version %s\n", XDVIK_PROGNAME, XDVI_VERSION_INFO);
	    printf("A DVI file previewer for the X window system.\n\n");
	    display_licensing_info();
	    display_bug_reporting_info();
	    usage(0);
	}
	else if (strcmp(argv[i], "-license") == 0) {
	    display_long_licensing_info();
	    xdvi_exit(EXIT_SUCCESS);
	}
	else if (strcmp(argv[i], "--version") == 0
		 || strcmp(argv[i], "-version") == 0
		 || strcmp(argv[i], "-v") == 0) {
	    display_version_info();
	    xdvi_exit(EXIT_SUCCESS);
	}
	else if (strcmp(argv[i], "--default-xerr-handler") == 0) { /* hook to disable custom handler */
	    install_err_handler = False;
	}
    }

    if (install_err_handler) {
	(void)XSetErrorHandler(x_error_handler);
    }
}

/* initialize global variables with default values. */
static void
init_globals(void) {
    globals.program_name = NULL;
    globals.dvi_name = NULL;
    globals.cwd = xgetcwd();
    globals.orig_locale = NULL;
    globals.debug = 0L;
    globals.pageno_correct = 1;

    globals.curr_paper = NULL;
    globals.curr_editor = NULL;
    globals.curr_browser = NULL;
    
    globals.ev.flags = EV_IDLE;
    globals.ev.ctr = 0;
    
    globals.pausing.num = 0;
    globals.pausing.num_save = NULL;
    globals.pausing.flag = False;

    globals.win_expose.min_x = 0;
    globals.win_expose.max_x = 0;
    globals.win_expose.min_y = 0;
    globals.win_expose.max_y = 0;

    globals.gc.rule = NULL;
    globals.gc.fore = NULL;
    globals.gc.inverted = NULL;
    globals.gc.high = NULL;
    globals.gc.linkcolor = NULL;
    globals.gc.visited_linkcolor = NULL;
    globals.gc.fore2 = NULL;
    globals.gc.fore2_bak = NULL;
    globals.gc.fore2_bak1 = NULL;
    globals.gc.copy = NULL;
    globals.gc.ruler = NULL;
    
    globals.gc.do_copy = False;
    
    globals.cursor.flags = 0;

    globals.src.fwd_box_page = -1; /* -1 means no box */
    globals.src.fwd_string = NULL;

    globals.widgets.top_level = 0;
    globals.widgets.draw_widget = 0;
    globals.widgets.draw_background = 0;
    globals.widgets.clip_widget = 0;
    globals.widgets.x_bar = 0;
    globals.widgets.y_bar = 0;
#ifdef MOTIF
    globals.widgets.main_window = 0;
    globals.widgets.main_row = 0;
    globals.widgets.tool_bar = 0;
    globals.widgets.top_row = 0;
    globals.widgets.menu_bar = 0;
#else
    globals.widgets.vport_widget = 0;
    globals.widgets.form_widget = 0;
    globals.widgets.paned = 0;
#endif

    globals.page.w = 0;
    globals.page.h = 0;
    globals.page.unshrunk_w = 0;
    globals.page.unshrunk_h = 0;
    
    globals.dvi_file.dirname = NULL;
    globals.dvi_file.dirlen = 0;
    globals.dvi_file.bak_fp = NULL;
    globals.dvi_file.time = 0;

#if defined(LESSTIF_VERSION)
    globals.broken_motif_event_handling = True;
#elif defined(MOTIF) /* was: && XmVersion <= 1002 - better use runtime information:*/
    if (xmUseVersion <= 1002)
	globals.broken_motif_event_handling = True;
    else
	globals.broken_motif_event_handling = False;
#else
    globals.broken_motif_event_handling = False;
#endif
    
}


#ifdef GREY
/*
 * Convert string to yes/no/maybe.  Adapted from the X toolkit.
 */

static Boolean
XdviCvtStringToBool3(Display *dpy,
		     XrmValuePtr args, Cardinal *num_args,
		     XrmValuePtr fromVal, XrmValuePtr toVal,
		     XtPointer *closure_ret)
{
    String str = (String) fromVal->addr;
    static Bool3 value;

    UNUSED(args);
    UNUSED(num_args);
    UNUSED(closure_ret);
    
    if (memicmp(str, "true", 5) == 0
	|| memicmp(str, "yes", 4) == 0
	|| memicmp(str, "on", 3) == 0 || memicmp(str, "1", 2) == 0)
	value = True;

    else if (memicmp(str, "false", 6) == 0
	     || memicmp(str, "no", 3) == 0
	     || memicmp(str, "off", 4) == 0 || memicmp(str, "0", 2) == 0)
	value = False;

    else if (memicmp(str, "maybe", 6) == 0)
	value = Maybe;

    else {
	XtDisplayStringConversionWarning(dpy, str, XtRBoolean);
	return False;
    }

    if (toVal->addr != NULL) {
	if (toVal->size < sizeof(Bool3)) {
	    toVal->size = sizeof(Bool3);
	    return False;
	}
	*(Bool3 *) (toVal->addr) = value;
    }
    else
	toVal->addr = (XPointer) & value;

    toVal->size = sizeof(Bool3);
    return True;
}
#endif

/*
************************************************************
************************************************************
main routine
************************************************************
************************************************************
*/

int
main(int argc, char **argv)
{
    int i;
    static struct startup_info info;
    const char *file_name = NULL;
    const char *file_name2 = NULL;
    
    /* Hack to have command-line options override ~/.xdvirc stuff:
     * Parse and merge them again from argv_bak, a copy of the command-line options,
     * via XrmParseCommand(). I think the only alternative would be to merge in all
     * resources manually instead of using XtInitialize(), similar to what's done in gv,
     * but that looks like too much trouble.
     */
#define COMMANDLINE_OVERRIDE_HACK 1
    
#if COMMANDLINE_OVERRIDE_HACK
    int argc_bak;
    char **argv_bak;
#endif

    setup_signal_handlers(True); /* catch USR1 early */
    
    info.file_idx = 0;
    info.page_arg = NULL;
    
    /* BEGIN_TIMER_LOOP; */
    
    init_globals();
    
    /*
     *      Step 1:  Process command-line options and resources.
     */

    globals.program_name = xstrdup(argv[0]);
    {   /* get filename from program_name if it contains a path */
	char sep;
	char *ptr;
#ifdef	VMS
	sep = ']';
#else
	sep = '/';
#endif
	if ((ptr = strrchr(globals.program_name, sep)) != NULL) {
	    globals.program_name = ++ptr;
	}
#ifdef	VMS
	if ((ptr = strchr(globals.program_name, '.')) != NULL)
	    *ptr = '\0';
#endif
    }

#if COMMANDLINE_OVERRIDE_HACK
    /* create a copy of argv[] */
    argc_bak = argc;
    argv_bak = xmalloc((1 + argc_bak) * sizeof *argv_bak);
    for (i = 0; i < argc_bak; i++) {
	argv_bak[i] = xstrdup(argv[i]);
    }
    argv_bak[i] = NULL;
#endif

    /*
      Arguments that need to be checked early, like `help', `version' and `sync'.
      The former don't even require an X connection.
    */
    check_early_arguments(argc, argv);
    
    warn_about_prerelease_versions();

    /* We need to set up SIGALRM before calling XtAppAddTimeOut() (inside sfSelFile, or
       called from XtInitialize() in Motif), otherwise we'll die with the error message
       `Alarm clock'. However, we mustn't install SIGPOLL before the forking is done
       below, otherwise xdvi may hang forever waiting for input. So the signal handler setup
       is split in 2 parts: setup_sigalarm(), and setup_signal_handlers() below.
    */
    setup_sigalarm();

    /* get the debug value (if any) from the environment
       (it's too early to get it from the command line) */
    globals.debug = parse_debugging_option(getenv("XDVIDEBUG"));

    /* to make input of non-latin characters work */
    XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);
    
    globals.widgets.top_level = XtInitialize(globals.program_name, "XDvi", options, XtNumber(options),
					     &argc, argv);

    globals.app = XtWidgetToApplicationContext(globals.widgets.top_level);
    
    XtAppAddActions(globals.app, get_actions(), get_num_actions());

    /*     create_magnifier(); */

    if ((globals.orig_locale = setlocale(LC_ALL, "")) != NULL)
	globals.orig_locale = xstrdup(globals.orig_locale); /* note: never free'd */
    else /* fallback */
	globals.orig_locale = xstrdup(setlocale(LC_ALL, NULL));
    
    /* Override LC_NUMERIC, otherwise sscanf(), strtod() etc. won't work for floats
       inside specials for locales like LANG=de_DE which uses `,' instead of `.' as
       decimal separator. (Strangely, using
       LC_CTYPE | LC_COLLATE 
       doesn't work?)
       Regression:
       xdvik/regression/special-pics/pictest.dvi
    */
    setlocale(LC_NUMERIC, "C");

    /* at this point, all known options will have been consumed; so the
       only ones remaining should be: +pageno, and the dvi name. Exit with
       usage message if this is not the case. The convention is that the last
       argument is always treated as filename (even if it starts with '-'),
       so we throw an error if there's more than one unknown argument at the end.
    */
    for (i = 1; i < argc; i++) { /* skip argv[0] */
	if (*(argv[i]) == '+') {
	    if (info.page_arg != NULL) {
		XDVI_ERROR((stderr, "Unrecognized option `%s'.", argv[i]));
		usage(EXIT_FAILURE);
	    }
	    else {
		info.page_arg = argv[i] + 1;
	    }
	}
	else if (file_name == NULL) {
	    file_name = xstrdup(argv[i]); /* leaks, but never mind ... */
	}
	else {
	    /* we had already seen a filename */
	    XDVI_ERROR((stderr, "Unrecognized option `%s'.", argv[i - 1])); /* safe because we started at argv[1] */
	    usage(EXIT_FAILURE);
	}
    }

    DISP = XtDisplay(globals.widgets.top_level);
    SCRN = XtScreen(globals.widgets.top_level);

#ifdef GREY
    XtSetTypeConverter(XtRString, XtRBool3, XdviCvtStringToBool3,
		       NULL, 0, XtCacheNone, NULL);
#endif

#ifdef MOTIF
    /* we'll take care of that ourselves */
    XtVaSetValues(globals.widgets.top_level, XmNdeleteResponse, XmDO_NOTHING, NULL);
    
    {
	/* Hack to work around #884290 (drag&drop freezes file selector, see comment
	   in xm_filesel.c): Disable drag&drop altogether (we don't need it).
	   Could also be done via Xdefaults as follows:
	   XDvi*dragInitiatorProtocolStyle: XmDRAG_NONE
	   XDvi*dragReceiverProtocolStyle:  XmDRAG_NONE
	*/
	Widget display = XmGetXmDisplay(DISP);
	XtVaSetValues(display,
		      XmNdragInitiatorProtocolStyle, XmDRAG_NONE,
		      XmNdragReceiverProtocolStyle,  XmDRAG_NONE,
		      NULL);
    }
#else
    G_accels_cr = XtParseAcceleratorTable("<Key>Return:set()notify()unset()\n"
					  "<Key>q:set()notify()unset()\n"
					  "<Key>Escape: set()notify()unset()");
#endif

    /* get the no_init_file resource first: This needs to be done
     * before the call to XtGetApplicationResources() below, which populates
     * the `resource' struct with the actual application resources (which
     * may be merged from ~/.xdvirc). */
    XtGetApplicationResources(globals.widgets.top_level, (XtPointer)&resource,
			      xdvirc_resources, XtNumber(xdvirc_resources),
			      (ArgList)NULL, 0);

    if (!resource.no_init_file) { /* Read user preferences from ~/.xdvirc. */
	read_user_preferences(globals.widgets.top_level, ".xdvirc");
    }

#if COMMANDLINE_OVERRIDE_HACK /* see above */
    {
	XrmDatabase cmdline_db = XrmGetDatabase(DISP);
	XrmParseCommand(&cmdline_db, options, XtNumber(options),
			"xdvi", &argc_bak, argv_bak);

	for (i = 0; i < argc_bak; i++) {
	    free(argv_bak[i]);
	}
	free(argv_bak);
	argc_bak = 0;
    }
#endif /* COMMANDLINE_OVERRIDE_HACK */

    load_app_resources(False);

    /* musn't do this, 0 is 'fit to window' (bug #1454648) */
    /*     if (resource.shrinkfactor == 0) /\* protect against division by 0 *\/ */
    /* 	resource.shrinkfactor = 1; */
    currwin.shrinkfactor = resource.shrinkfactor;
    globals.curr_use_color = resource.use_color;
    globals.curr_gamma = resource.gamma;
    globals.curr_paper = xstrdup(resource.paper); /* never free()d */
    globals.curr_editor = NULL;
    globals.curr_browser = NULL;
    globals.curr_mode = NO_MODE_ACTIVE;
    
    /* Initialize `globals.debug' as early as possible.  Note: earlier
     * calls to TRACE_* or tests for `if (globals.debug)' will only work if the
     * XDVIDEBUG environment variable is set!
     */
    globals.debug |= parse_debugging_option(resource.debug_arg);
    kpathsea_debug = globals.debug / DBG_STAT;

    if (globals.debug)
	fprintf(stderr, "KPATHSEA_DEBUG = %d\n", kpathsea_debug);

    kpse_init_prog("XDVI", resource.pixels_per_inch, resource.mfmode, resource.alt_font);

    TRACE_FILES((stderr, "Initializing kpathsearch with program name '%s'",
		 xdvi_kpse_prog_name));
    kpse_set_program_name(argv[0], xdvi_kpse_prog_name);
    
    if (globals.debug & DBG_EXPAND) {
	const char *texmfcnf = kpse_path_expand("$TEXMFCNF");
	const char *texmfmain = kpse_path_expand("$TEXMFMAIN");
	fprintf(stderr, "\n%s:%d: KPATHSEA variables:\n", __FILE__, __LINE__);
	fprintf(stderr, "%s:%d: SELFAUTOLOC=\"%s\"\n", __FILE__, __LINE__, getenv("SELFAUTOLOC"));
	fprintf(stderr, "%s:%d: SELFAUTODIR=\"%s\"\n", __FILE__, __LINE__, getenv("SELFAUTODIR"));
	fprintf(stderr, "%s:%d: SELFAUTOPARENT=\"%s\"\n", __FILE__, __LINE__, getenv("SELFAUTOPARENT"));
 	fprintf(stderr, "%s:%d: TEXMFCNF=\"%s\"\n", __FILE__, __LINE__, texmfcnf);
 	fprintf(stderr, "%s:%d: TEXMFMAIN=\"%s\"\n\n", __FILE__, __LINE__, texmfmain);
    }
    
    if (resource.regression) {
	/* currently it just turns on everything; what we'd like here is
	   output that's usable for automatic diffs (e.g. independent
	   of window manager state) */
	globals.debug = DBG_ALL;
    }

    /* Check early for whether to pass off to a different xdvi process
     * (-sourceposition argument for reverse source special lookup).
     */
    property_initialize();

#if 0 /*  def RGB_ANTI_ALIASING */
    /* error checking and setting of resources according to command line arguments */
    if (resource.sub_pixels != NULL && memicmp(resource.sub_pixels, "unknown", 4) == 0) {
#ifdef __GNUC__
#warning TODO: implement callbacks
#endif
	choice_dialog_sized(globals.widgets.top_level,
			    MSG_QUESTION,
			    SIZE_MEDIUM,
			    NULL,
#ifndef MOTIF
			    NULL, /* TODO: xaw ret_action_str */
#endif
			    NULL, NULL, /* pre callback */
			    "Enable", NULL, NULL,
			    "Disable", NULL, NULL,
			    "This version of xdvi can optimize the anti-aliased font display "
			    "when running on an LCD monitor, such as a notebook screen or a TFT flat screen."
			    "\n\n"
			    "If you are using such a monitor, click `Enable' to enable optimized display; otherwise click `Disable'."
			    "\n\n"
			    "You can change this setting later via the Menu `Options -> Anti-Aliasing'.");
	/* enter event loop */
	do_pages();
    }
    else {
#endif
	init_check_resources();

	TRACE_FILES((stderr, "file history: |%s|", resource.file_history));
	file_history_init();

#if !DELAYED_MKTEXPK
    /* Notify users that fonts might be created. This is just a hack
       and no replacement for true asynchronous font creation since it
       doesn't give details (is just invoked if startup takes somewhat
       longer) and freezes during font creation.
    */
/* 	register_font_popup(); */
#endif
	
	if (file_name == NULL) { /* no filename argument */

	    if (resource.no_file_arg_use_history) {
		static char buf[LENGTH_OF_INT]; /* so that we can pass its address */
		int pageno;
		if ((file_name = get_filename_from_history(&pageno)) != NULL) {
		    SNPRINTF(buf, LENGTH_OF_INT, "%d", pageno + 1); /* pageno is 0-based */
		    info.page_arg = buf;
		}
	    }

	    TRACE_FILES((stderr, "got from history: |%s|", file_name));
	
	    if (file_name != NULL) {
		run_dvi_file(file_name, &info);
	    }
	    else { /* get filename from file selector */
		/* static so that we can pass its address */
		static struct filesel_callback cb = {
		    NULL, NULL, "Xdvi: Open file", "Open file:", "OK", "Cancel",
		    NULL, "*.dvi", True, True, NULL, NULL
		};
		cb.func_ptr = run_dvi_file;
		cb.data = &info;
		cb.browse_fname = xt_strdup(globals.cwd);

		if (cb.shell == NULL)
		    cb.shell = XsraSelFile(globals.widgets.top_level, &cb);
		XsraSelFilePopup(&cb);
		/* enter event loop */
		do_pages();
	    }
	}
	else if ((file_name2 = is_good_dvi_file(file_name, False)) != NULL) {
	    run_dvi_file(file_name2, &info);
	}
#if 0 /*  def RGB_ANTI_ALIASING */
    }
#endif
    /* notreached */
    return 0;
}
