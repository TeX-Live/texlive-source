/*
 * Copyright (c) 2004-2013 the xdvik development team
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
 * Help window for xdvik, using the `topics' window framework.
 */

#include "xdvi-config.h"
#include "xdvi.h"
#include "version.h"

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#if MOTIF
# include <Xm/Xm.h>
# include <Xm/Form.h>
# include <Xm/Frame.h>
# include <Xm/Text.h>
# include <Xm/Protocols.h>
#else /* MOTIF */
# include <X11/Xaw/Paned.h>
# include <X11/Xaw/Form.h>
# include <X11/Xaw/AsciiText.h>
#endif /* MOTIF */

#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

#include "message-window.h"
#include "util.h"
#include "x_util.h"
#include "string-utils.h"
#include "topic-window.h"
#include "help-window.h"

#if HAVE_XKB_BELL_EXT
# include <X11/XKBlib.h>
# define XdviBell(display, window, percent)	\
	 XkbBell(display, window, percent, (Atom) None)
#else
# define XdviBell(display, window, percent)	XBell(display, percent)
#endif

/* missing features that will be listed in the help window */
#if !XDVI_XT_TIMER_HACK
#define HAVE_MISSING_FEATURES 1
#endif

#if !HAVE_REGEX_H
#define HAVE_MISSING_FEATURES 1
#endif

/*
 * The number of help topics - must larger or equal to actual number of items,
 * also keep in sync with elements resource.help_* !!!
 */
#define NUM_HELP_TOPICS 16

/*
 * helper routines
 */

static Widget
create_help_text(Widget parent, const char *name, const char *value)
{
    Widget text;
#if MOTIF
    Arg args[20];
    int n = 0;

    XtSetArg(args[n], XmNeditable, False);			n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False);		n++;
    XtSetArg(args[n], XmNvalue, value);				n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);		n++;
    XtSetArg(args[n], XmNwordWrap, True);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    /*     XtSetArg(args[n], XmNtopWidget, top_widget);		n++; */
    /*     XtSetArg(args[n], XmNtopOffset, 10);			n++; */
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC);		n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED);	n++;
    
    text = XmCreateScrolledText(parent, (char *)name, args, n);

    XtManageChild(text);
#else /* MOTIF */
    text = XtVaCreateManagedWidget(name, asciiTextWidgetClass, parent,
				   /* 				   XtNfromVert, top_widget, */
				   /* 				   XtNvertDistance, 10, */
				   XtNstring, value,
				   XtNheight, 400,
				   XtNwidth, 500,
				   /* resizing of pane by user isn't needed */
				   XtNshowGrip, False,
				   XtNscrollVertical, XawtextScrollAlways,
				   XtNscrollHorizontal, XawtextScrollNever,
				   XtNeditType, XawtextRead,
				   XtNleftMargin, 5,
				   NULL);
    XawTextDisplayCaret(text, False);
#endif /* MOTIF */
    return text;
}


static void
get_title_and_summary(const char *str, int len, char **title, char **summary)
{
    char *tmp = xmalloc(len + 1);
    char *ptr;

    memcpy(tmp, str, len);
    tmp[len] = '\0';

    if ((ptr = strchr(tmp, '\t')) == NULL) {
	XDVI_WARNING((stderr, "Help resource label `%s' doesn't contain a tab character - ignoring it.", tmp));
	*title = tmp;
	*summary = NULL;
    }
    else {
	*ptr++ = '\0';
	*title = tmp;
	*summary = ptr;
    }
    
    TRACE_GUI((stderr, "Title, Summary: |%s|%s|", *title, *summary));
}

static void
init_item(const char *resource, const char **resource_default,
	  struct topic_info *info, size_t idx, Dimension *width)
{
    const char *ptr = NULL;
    char *widget_text = NULL;
    Widget help_form;
    Widget text;
    struct topic_item *item = &(info->items[idx]);
    char *translation_str = get_string_va("#override \n"
					  "<Key>q:close-topic-window(%p)\n"
#ifdef MOTIF
					  "<Key>osfCancel:close-topic-window(%p)\n"
#else
					  "<Key>Escape:close-topic-window(%p)\n"
#endif
					  "<Key>Return:close-topic-window(%p)",
					  info, info, info);
    
    if (resource != NULL) {
	if ((ptr = strchr(resource, '\n')) == NULL) {
	    XDVI_WARNING((stderr, "Help resource text `%s' doesn't contain a newline character.", resource));
	    ptr = resource;
	}
	else
	    ptr++;
	get_title_and_summary(resource, ptr - resource - 1,
			      &(item->topic), &(item->title));
    }
    else { /* resource not set; copy resource_default into malloc()ed widget_text: */
	size_t size = 0, alloc_len = 0, offset;
	const size_t alloc_step = 1024;
	int i;

	for (i = 0; resource_default[i] != NULL; i++) {
	    if (i == 0) { /* special case */
		get_title_and_summary(resource_default[i], strlen(resource_default[i]) - 1,
				      &(item->topic), &(item->title));
	    }
	    else {
		offset = size;
		size += strlen(resource_default[i]);
		/*
		 * allocate chunks of `alloc_step' to avoid frequent calls to malloc.
		 * `alloc_len' is always 1 more than `size', for the terminating NULL character.
		 */
		while (size + 1 > alloc_len) {
		    alloc_len += alloc_step;
		    widget_text = xrealloc(widget_text, alloc_len);
		}
		memcpy(widget_text + offset, resource_default[i], size - offset);
	    }
	}
	/* null-terminate string */
	widget_text[size] = '\0';
    }

    help_form =  XtVaCreateManagedWidget("help_form",
#if MOTIF
					 xmFormWidgetClass, info->right_form,
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_FORM,
					 XmNrightAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
#else
					 formWidgetClass, info->right_form,
					 XtNborderWidth, 0,
					 XtNdefaultDistance, 0,
#endif
					 NULL);
    item->widget = help_form;
    if (ptr != NULL) {
	text = create_help_text(help_form, "help_text", ptr);
    }
    else {
	text = create_help_text(help_form, "help_text", widget_text);
	free(widget_text);
    }
    XtOverrideTranslations(text, XtParseTranslationTable(translation_str));
    free(translation_str);
#if !MOTIF
    {
	Dimension w;
	XtVaGetValues(text, XtNwidth, &w, NULL);
	if (w > *width)
	    *width = w;
    }
#else
    UNUSED(width);
#endif
}

static void
initialize_items(struct topic_info *info)
{
    size_t k;
    Dimension width;
    
    /*
     * Define fallbacks: default_xyz is used as fallback text if
     * X resource xyz isn't specified.
     *
     * We use arrays of strings rather than simple strings because of C's
     * limitations on maximum string length; but since the resource needs to
     * be a simple `char *', these have to be copied into larger buffers
     * later on (which is a bit wasteful to space). OTOH, splitting the
     * strings into smaller pieces would make them hard to deal with as
     * X resources. They are defined as static so that they are initialized
     * only once.
     *
     * Last elem of each array is NULL for ease of looping through it.
     *
     * Advantages of this method vs. putting the help texts into a file:
     * - couldn't use #ifdef's as easily
     * - would need to invent our own file format
     * - file searching is more error-prone (needs to be installed etc.)
     */

    static const char *default_help_general[] = {
	"Introduction\tAbout this version of xdvi\n",
	"This is xdvik, version ",
	XDVI_TERSE_VERSION_INFO
	".\nThe program's homepage is located at:\n",
	"http://sourceforge.net/projects/xdvi\n",
	"where you can find updates, report bugs and submit feature requests.\n",
	"\n",
	"\n",
	"Xdvi has many command-line options, too numerous to be listed here;\n",
	"see the man page for a full description.\n",
	"\n",
	"The most important key bindings are listed in the help sections shown\n",
	"in the left window.\n",
	"\n",
	"Note: Unless a key binding also has an uppercase version,\n",
	"all bindings are case-insensitive.\n\n",
	"\n",
	"The major parts of Xdvik are licensed under the X Consortium license.\n",
	"Parts (encoding.c) are licensed under the GNU General Public License.\n",
	"Xdvik uses the following libraries:\n",
	"- The kpathsea library, licensed in part under the GNU General Public\n",
	"  License, in part under the GNU Library General Public License.\n",
	"- freetype2, licensed under the GNU General Public License.\n",
	"There is NO WARRANTY of anything.\n",
	"\n",    
	"Built using these configure options:\n",
#if MOTIF
	"- Motif toolkit (",
	XmVERSION_STRING,
	")\n",
#else
	"- Athena toolkit\n",
#endif
#ifdef A4
	"- paper: a4, units cm\n",
#else
	"- paper: letter, units inches\n",
#endif
#ifdef GREY
	"- anti-aliasing (grey) enabled\n",
#endif
#if FREETYPE
	"- freetype2 (direct rendering of PS fonts) enabled\n",
#endif
#if HAVE_ICONV_H
	"- Iconv support compiled in\n",
#if USE_LANGINFO
	"- Langinfo support compiled in\n",
#else
	"- Langinfo support not compiled in\n",
#endif
#else
	"- Iconv/langinfo support not compiled in\n",
#endif
#ifdef TEXXET
	"- left-to-right typesetting (TeXXeT) support enabled\n",
#endif
#ifdef USE_GF
	"- gf file support enabled\n",
#endif
#if HAVE_MISSING_FEATURES
	"\n",
	"Features not available on this platform:\n",
#if !XDVI_XT_TIMER_HACK
	"- Could not redefine XtAppAddTimeOut(); some widgets may\n",
	"  not be updated until the mouse is moved.\n",
#endif
#if !HAVE_REGEX_H
	"- regex.h header not available, regular expression support\n",
	"  in string search is disabled.\n",
#endif
#endif /* HAVE_MISSING_FEATURES */
	NULL
    };

    static const char *default_help_hypertex[] = {
	"Hyperlinks\tNavigating links\n",
	"Whenever the mouse is positioned on a link, the cursor changes\n",
	"to a `hand' shape and the target of the link is displayed\n",
	"in the statusline at the bottom of the window.\n",
	"\n",
	"The following keybindings are pre-configured:\n",
	"\n",
        "Mouse-1\n",
	"	Follow the link at the cursor position.\n",
	"	If the link target is not a DVI file, try to launch\n",
	"	an application to view the file.\n",
        "Mouse-2\n",
	"	Open a new xdvi window displaying the link\n",
        "	at the cursor position if the link is a DVI file;\n",
	"	else, try to launch an application to view the file.\n",
        "B\n",
        "	Go back to the previous hyperlink in the history.\n",
        "F\n",
	"	Go forward to the next hyperlink in the history.\n",
	"\n",
	"By default, the hyperlinks are displayed in the colors \n",
	"`linkColor' and `visitedLinkColor' (for visited links) and \n",
	"underlined in the same colors. This can be customized \n",
	"by setting the resource or command-line option `linkstyle' \n",
	"to a value between 0 and 3, which have the following meaning:\n",
	"    0: no highlighting of links,\n",
	"    1: underline links,\n",
	"    2: color links,\n",
	"    3: color and underline links.\n\n",
	NULL
    };

    static const char *default_help_othercommands[] = {
	"Other Commands\tMiscellaneous other commands\n",
	"Ctrl-f",
#if MOTIF
	", toolbar button 12\n",
#else
	"\n",
#endif	
	"     Opens a dialog window to search for a text string\n",
	"     in the DVI file.\n",
	"\n",
	"Ctrl-g\n",
	"     Search for the next string match.\n",
	"\n",
	"Ctrl-l\n",
	"     Toggles fullscreen mode (which may not work with your\n",
	"     window manager/desktop).\n",
	"\n",
	"Ctrl-o",
#if MOTIF
	", toolbar button 1\n",
#else
	"\n",
#endif	
	"     Opens a popup window to select another DVI file.\n",
	"     With a prefix argument `n', the `n'th file from the file history\n",
	"     is opened instead.\n",
	"\n",
	"Ctrl-p",
#if MOTIF
	", toolbar button 11\n",
#else
	"\n",
#endif	
	"     Opens a popup window for printing the DVI file, or parts of it.\n",
	"\n",
	"Ctrl-r or Clear\n",
	"     Redisplays the current page.\n",
	"\n",
	"Ctrl-s\n",
	"     Opens a popup window for saving the DVI file, or parts of it.\n",
	"\n",
	"G\n",
	"     Toggles the use of greyscale anti-aliasing for\n",
	"     displaying shrunken bitmaps.  In addition, the key\n",
	"     sequences `0G' and `1G' clear and set this flag,\n",
	"     respectively.  See also the -nogrey option.\n",
	"\n",
	"k\n",
	"     Normally when xdvi switches pages, it moves to the home\n",
	"     position as well.  The `k' keystroke toggles a `keep-\n",
	"     position' flag which, when set, will keep the same\n",
	"     position when moving between pages.  Also `0k' and `1k'\n",
	"     clear and set this flag, respectively.  See also the\n",
	"     -keep option.\n",
	"\n",
	"M\n",
	"     Sets the margins so that the point currently under the\n",
	"     cursor is the upper left-hand corner of the text in the\n",
	"     page.  Note that this command itself does not move the\n",
	"     image at all.  For details on how the margins are used,\n",
	"     see the -margins option.\n",
	"\n",
	"P\n",
	"     ``This is page number n.''  This can be used to make\n",
	"     the `g' keystroke refer to a different page number.\n",
	"     (See also `Options->Use TeX Page Numbers' and the\n",
	"     `T' keystroke).\n",
	"\n",
	"R",
#if MOTIF
	", toolbar button 2\n",
#else
	"\n",
#endif
	"     Forces the DVI file to be reread.\n",
	"\n",
	"s\n",
	"     Changes the shrink factor to the given number.\n",
	"     If no number is given, the smallest factor that makes the\n",
	"     entire page fit in the window will be used.  (Margins\n",
	"     are ignored in this computation.)\n",
	"\n",
	"S\n",
	"     Sets the density factor to be used when shrinking\n",
	"     bitmaps.  This should be a number between 0 and 100;\n",
	"     higher numbers produce lighter characters.\n",
	"\n",
	"t\n",
	"     Switches to the next unit in a sorted list of TeX dimension\n",
	"     units for the popup magnifier ruler and `Ruler mode' (see the\n",
	"     section `Modes').\n"	
	"\n",
	"V\n",
	"     Toggles Ghostscript anti-aliasing.  Also `0V' and `1V' clear\n",
	"     and enables this mode, respectively.  See also the the\n",
	"     -gsalpha option.\n",
	"\n",
	"\n",
	"v\n",
	"     Toggles between several modes of displaying postscript specials:\n",
	"     Display specials, display specials with their bounding box\n",
	"     (if available), and display bounding boxes only (if available).\n",
	"     The prefix arguments 1, 2 and 0 also allow you to select one of\n"
	"     these states directly.\n",
	"\n",
	"x\n",
	"     Toggles expert mode (in which ",
#if MOTIF
	"the menu bar, the toolbar\n",
#else
	"the menu buttons,\n",
#endif
	"     the page list and the statusline do not appear).\n",
	"     `1x' toggles the display of the statusline at the bottom of the window.\n",
	"     `2x' toggles the scrollbars,\n",
#if MOTIF
	"     `3x' toggles the page list,\n",
	"     `4x' toggles the toolbar,\n",
	"     `5x' toggles the menu bar.\n",
#else
	"     `3x' toggles the page list and menu buttons.\n",
#endif
	"\n",
	"Ctrl-+",
#if MOTIF
	", toolbar button 9\n",
#else
	"\n",
#endif
	"     Makes the display of the page larger (zooms in).\n",
	"\n",
	"Ctrl--",
#if MOTIF
	", toolbar button 10\n",
#else
	"\n",
#endif
	"     Makes the display of the page smaller (zooms out).\n",
	"\n",
	"Alt-Ctrl-+",
#if MOTIF
	", toolbar button 17\n",
#else
	"\n",
#endif
	"     Makes the fonts darker (by adding to the gamma value).\n",
	"\n",
	"Alt-Ctrl--",
#if MOTIF
	", toolbar button 18\n",
#else
	"\n",
#endif
	"     Makes the fonts lighter (by subtracting from the gamma\n",
	"     value).\n",
	"\n",
	NULL
    };

    static const char *default_help_marking[] = {
	"Printing and Saving\tMarking, printing and saving pages\n",
	"The `Save' and `Print' dialogs allow you to save or print all,\n",
	"pages, a range of pages, or all marked pages from a DVI file.\n",
	"\n",
	"Note that the page numbers for the `From ... to ...' range\n",
	"refer to physical pages, not TeX pages (compare the option\n",
	"`Use TeX Page Numbers' and the `T' keystroke).\n",
	"\n",
	"To mark a page or a range of pages, use one of the folllowing\n",
	"methods:\n",
	"- Click on the page in the page list with Mouse Button 2 to mark\n",
	"  a single page, or drag the mouse while holding down Button 2\n",
	"  to mark a range of pages.\n",
	"- Use one of the following key combinations:\n",
	"     m:  toggle the mark of the current page,\n",
	"     1m  toggle the marks of all odd pages,\n",
	"     2m  toggle the marks of all even pages,\n",
	"     0m: unmark all pages,\n",
	"     Ctrl-n: toggle mark of current page, then move one page forward,\n",
	"     Ctrl-u: move one page back, then toggle mark of that page.\n",
#if MOTIF
        "- Use the toobar buttons 13 to 16 to toggle the marks\n",
        "  of odd pages, toggle the marks of even pages, toggle the mark\n",
	"  of the current page, or unmark all pages, respectively.\n",
#endif
	"\n",
	"If the X resource or command line option `paper' has been used,\n",
	"its value is inserted into the `Dvips Options' field of the printing\n",
	"dialog so that the appropriate options can be passed to dvips.\n",
	"This doesn't happen if the paper size has been specified explicitly\n",
	"in the DVI file (e.g. by using the LaTeX `geometry' package).\n",
	"Note that  not all of the paper options used by xdvi\n",
	"may be understood by dvips; dvips will ignore the option\n",
	"in that case, and will use its default paper setting.\n",
	NULL
    };
    
    static const char *default_help_pagemotion[] = {
	"Page Motion\tMoving around in the document\n",
	"\n",
        "[",
#if MOTIF
	", toolbar button 7\n",
#else
	"\n",
#endif
	"     Moves back one item in the page history. With a prefix\n",
	"     argument n, move back n history items.\n"
	"\n",
        "]",
#if MOTIF
	", toolbar button 8\n",
#else
	"\n",
#endif
	"     Moves forward one item in the page history. With a prefix\n",
	"     argument n, move forward n history items.\n"
	"\n",
        "Ctr-[\n",
	"     Deletes current item in the page history and move\n",
	"     to the history item before the deleted one. With a prefix\n",
	"     argument n, delete n previous history items.\n",
	"\n",
        "Ctr-]\n",
	"     Deletes current item in the page history and move\n",
	"     to the history item after the deleted one. With a prefix\n",
	"     argument n, delete n next history items.\n",
	"\n",
	"n or f or Return or LineFeed or PgDn",
#if MOTIF
	", toolbar button 5\n",
#else
	"\n",
#endif
	"     Moves to the next page (or to the nth next page if a\n",
	"     number is given).\n",
	"\n",
	"Space key\n",
	"     Moves down or to the next page.",
	"\n",
	"p or b or Ctrl-h or BackSpace or PgUp",
#if MOTIF
	", toolbar button 4\n",
#else
	"\n",
#endif
	"     Moves to the previous page (or back n pages).\n",
	"\n",
	"Del key\n",
	"     Moves up on the page or to the previous page.",
	"\n",
	"Up-arrow\n",
	"     Scrolls page up.\n",
	"\n",
	"Down-arrow\n",
	"     Scrolls page down.\n",
	"u\n",
	"     Moves page up two thirds of a window-full.\n",
	"\n",
	"d\n",
	"     Moves page down two thirds of a window-full.\n",
	"\n",
	"Left-arrow\n",
	"     Scrolls page left.\n",
	"\n",
	"Right-arrow\n",
	"     Scrolls page right.\n",
	"\n",
	"l\n",
	"     Moves page left two thirds of a window-full.\n",
	"\n",
	"r\n",
	"     Moves page right two thirds of a window-full.\n",
	"\n",
	"T\n",
	"     Toggle the use of TeX page numbers instead of physical\n",
	"     pages for the page list and the `g' command.\n",
	"     (See also the `Options -> Use TeX Pages' menu.)\n",
	"\n",
	"g\n",
	"     Moves to the page with the given number.  Initially,\n",
	"     the first page is assumed to be page number 1, but this\n",
	"     can be changed with the `P' keystroke, described in the\n",
	"     section `Other Commands'.  If no page number is given,\n",
	"     it moves to the last page.\n",
	"\n",
	"<, Ctrl-Home",
#if MOTIF
	", toolbar button 3\n",
#else
	"\n",
#endif
	"     Moves to first page in the document.\n",
	"\n",
	">, Ctrl-End",
#if MOTIF
	", toolbar button 6\n",
#else
	"\n",
#endif
	"     Moves to last page in the document.\n",
	"\n",
	"^\n",
	"     Move to the ``home'' position of the page.  This is\n",
	"     normally the upper left-hand corner of the page,\n",
	"     depending on the margins set via the -margins option.\n",
	"\n",
	"Home\n",
	"     Move to the ``home'' position of the page (the upper\n",
	"     left-hand corner), or to the top of the page if the `keep'\n",
	"     flag is set.\n",
	"\n",
	"End\n",
	"     Move to the end position of the page (the lower\n",
	"     right-hand corner), or to the bottom of the page if the\n",
	"     `keep' flag is set.\n",
	"\n",
	"c\n",
	"     Moves the page so that the point currently beneath the\n",
	"     cursor is moved to the middle of the window.  It also\n",
	"     warps the cursor to the same place.\n",
	"\n",
	NULL
    };


    static const char *default_help_mousebuttons[] = {
	"Mouse Buttons\tActions bound to the mouse buttons\n",
	"The mouse buttons can be customized just like the keys;\n",
	"however the bindings cannot be intermixed (since\n",
	"a mouse event always requires the cursor location\n",
	"to be present, which a key event doesn't).\n",
        "The default bindings are as follows:\n"
	"\n",
	"Buttons 1-3\n",
	"     Pops up magnifier windows of different sizes.\n",
	"     When the mouse is over a hyperlink, the link overrides\n",
	"     the magnifier. In that case, Button 1 jumps to the link\n",
	"     in the current xdvi window, Button 2 opens the link target\n",
	"     in a new instance of xdvi.\n",
	"     In `Ruler Mode', Button1 shows/drags the ruler instead;\n",
	"     in `Text Selection Mode', Button1 can be used to select\n",
	"     a rectangular region of text from the DVI file.\n",
	"\n",
	"Shift-Button1 to Shift-Button3\n",
	"     Drag the page in each direction (Button 1), vertically\n",
	"     only (Button 2) or horizontally only (Button 3).\n",
	"\n",
	"Ctrl-Button1\n",
	"     Invoke a reverse search for the text on the cursor\n",
	"     location (see the section SOURCE SPECIALS for more\n",
	"     information on this).\n",
	"\n",
	"The buttons 4 and 5 (wheel up and down for wheel mice)\n",
	"scroll the page up and down respectively, or jump to the\n",
	"next/previous page when the mouse is over the page list.",
	"\n",
	"In the page list, Button 2 toggles the mark a page (see\n",
	"section `Marking Pages'); moving the mouse while holding\n",
	"Button 2 lets you toggle a range of pages.\n",
	"\n",
	NULL
    };

    static const char *default_help_sourcespecials[] = {
	"Source Specials\tNavigating between the TeX and the DVI file\n",
	"Some TeX implementations have an option to automatically\n",
	"include so-called `source specials' into a DVI file. These\n",
	"contain the line number and the filename of the TeX source\n",
	"and make it possible to go from a DVI file to the\n",
	"(roughly) corresponding place in the TeX source and back\n",
	"(this is also called `reverse search' and `forward search').\n",
	"\n",
	"On the TeX side, you need a TeX version that supports the `-src'\n",
	"option (e.g. teTeX >= 2.0) or a macro package like srcltx.sty\n",
	"to insert the specials into the DVI file.\n",
	"\n",
	"Source special mode can be customized for various editors\n",
	"by using the command line option \"-editor\" or one of the\n",
	"environment variables \"XEDITOR\", \"VISUAL\" or \"EDITOR\".\n",
	"See the xdvi man page on the \"-editor\" option for details\n",
	"and examples.\n",
	"\n",
	"Forward search can be performed by a program (i.e. your editor)\n",
	"invoking xdvi with the \"-sourceposition\" option like this:\n",
	"xdvi -sourceposition \"<line> <filename>\" <main file>\n",
	"If there is already an instance of xdvi running that displays\n",
	"<main file>, it will try to open the page specified by\n",
	"<line> and <filename> an highlight this location on the page.\n",
	"Else, a new instance of xdvi will be started that will try to\n",
	"do the same.\n",
	"\n",
	"The following keybindings are pre-configured:\n",
	"\n",
	"Ctrl-Mouse1\n",
	"     [source-special()] Invoke the editor (the value\n",
	"     of the \"editor\" resource ) to display the line in the\n",
	"     TeX file corresponding to special at cursor position.\n",
	"\n",
	"Ctrl-v\n",
	"     [show-source-specials()]  Show bounding boxes for every\n",
	"     source special on the current page, and print the strings\n",
	"     contained in these specials to  stderr. With prefix 1,\n",
	"     show every bounding box on the page (for debugging purposes).\n",
	"\n",
	"Ctrl-x\n",
	"     [source-what-special()]  Display information about the\n",
	"     source special next to the cursor, similar to\n",
	"     \"source-special()\", but without actually invoking\n",
	"     the editor (for debugging purposes).\n",
	"\n",
	NULL
    };

    static const char *default_help_modes[] = {
	"Mouse Modes\tMagnifier Mode, Ruler Mode and Text Selection Mode\n",
	"The keystroke Ctrl-m [switch-mode()] switches between\n",
	"three different bindings for Mouse-1, which can also be\n",
	"activated via the Modes menu (in Motif, this is a submenu\n",
	"of the Options menu).  The default mode at startup can be\n",
	"customized via the X resource `mouseMode' or the command-line\n",
	"option `-mousemode'.  The default startup mode is Magnifier Mode.\n",
	"\n",
	"Note: The modes are activated by changing the magnifier()\n",
	"action. Switching the mode will not work if Mouse-1 has\n",
	"been customized to an action sequence that does not contain\n",
	"the magnifier() action.\n",
        "\n",
        "Magnifier Mode\n",
        "\n",
        "       In this mode, the mouse buttons 1 to 5 will pop up a\n",
        "       ``magnifying glass'' that shows an unshrunken image of\n",
	"       the page (i.e. an image at the resolution determined by\n",
	"       the option/X resource pixels or mfmode) at varying sizes.\n",
        "       When the magnifier is moved, small ruler-like tick marks\n",
	"       are displayed at the edges of the magnifier (unless\n",
        "       the X resource delayRulers is set to false, in which case\n",
	"       the tick marks are always displayed).\n",
        "\n",
        "       The unit of the marks is determined by the X resource\n",
        "       `tickUnits' (mm by default). This unit can be changed at\n",
        "       runtime via the action `switch-magnifier-units()', by\n",
        "       default bound to the keystroke `t' (see the description\n",
        "       of that key, and of `switch-magnifier-units()' for more\n",
        "       details on the units available).  The length of the tick\n",
        "       marks can be changed via the X resource `tickLength'\n",
        "       (4 by default). A zero or negative value suppresses the\n",
        "       tick marks.\n",
        "\n",
        "\n",
        "Text Selection Mode\n",
        "\n",
        "       This mode allows you to select a rectangular region of\n",
        "       text in the DVI file by holding down Mouse-1 and moving\n",
        "       the mouse. The text is put into the X primary selection\n",
        "       so that it can be pasted into other X applications with\n",
        "       Mouse-2.\n",
        "\n",
        "       If xdvi has been compiled with locale, nl_langinfo() and\n",
        "       iconv support, the selected text is converted into the\n",
        "       character set of the current locale (see the output of\n",
        "       `locale -a' for a list of locale settings available on\n",
        "       your system).  If nl_langinfo() is not available, but\n",
        "       iconv is, you can specify the input encoding for iconv\n",
        "       via the X resource `textEncoding' (see the output of\n",
        "       `iconv -l' for a list of valid encodings). If iconv support\n",
        "       is not available, only the encodings ISO-8859-1 and UTF-8\n",
        "       are supported (these names are case-insensitive).\n",
        "\n",
        "       Note that UTF-8 is the only encoding that can render all\n",
        "       characters (e.g. mathematical symbols). If ISO-8859-1 is\n",
        "       active, characters that cannot be displayed are replaced\n",
        "       by `\' followed by the hexadecimal character code.  If a\n",
        "       character is not recognized at all, it is replaced by\n",
        "       `?'.  For other encodings, such characters may trigger\n",
        "       iconv error messages.\n",
        "\n",
        "       If you want to extract larger portions of text, you\n",
        "       can also save selected pages or the entire file in\n",
        "       text format from the `File > Save as ...'  menu.\n",
        "\n",
        "\n",
        "Ruler Mode\n",
        "\n",
        "       This mode provides a simple way of measuring distances\n",
        "       on the page.  When this mode is activated, the mouse\n",
        "       cursor changes into a thin cross, and a larger, cross-\n",
        "       shaped ruler is drawn in the highlight color at the\n",
        "       mouse location. The ruler doesn't have units attached\n",
        "       to it; instead, the current distance between the ruler\n",
        "       and the mouse cursor is continously printed to the\n",
        "       statusline.\n",
        "\n",
        "       When activating Ruler Mode, the ruler is at first\n",
        "       attached to the mouse and can be moved around.  It can\n",
        "       then be positioned at a fixed place by clicking Mouse-1.\n",
        "       After that, the mouse cursor can be moved to measure the\n",
        "       horizontal (dx), vertical (dy) and direct (shortest)\n",
        "       (dr) distance between the ruler center point and the\n",
        "       mouse.\n",
        "\n",
        "       Clicking Mouse-1 again will move the ruler to the\n",
        "       current mouse position, and holding down Mouse-1 will\n",
        "       drag the ruler around.\n",
        "\n",
        "       In Ruler Mode, the following special keybindings extend\n",
        "       or replace the default bindings:\n",
        "\n",
        "       o      [ruler-snap-origin()] Snap the ruler back to\n",
        "              the origin coordinate (0,0).\n",
        "\n",
        "       t      [overrides switch-magnifier-units()] Toggle\n",
        "              between various ruler units,  which can be\n",
        "              specified by the X resource tickUnits (`mm'\n",
        "              by default).\n",
        "\n",
        "       P      [overrides declare-page-number()] Print the\n",
        "              distances shown in the statusline to standard\n",
        "              output.\n",
        NULL
    };

    static const char *default_help_search[] = {
	"String Search\tSearching for strings in the DVI file\n",
        "The keystroke Ctrl-f or the menu entry File->Find ...\n",
        "opens a dialog window to search for a text string or a\n",
        "regular expression in the DVI file. The keystroke Ctrl-g\n",
	"jumps to the next match.\n",
#ifdef MOTIF
	"(With Motif, you can also click on the `Binoculars' symbol\n",
	"in the toolbar.)\n",
#endif
        "\n",
#if HAVE_ICONV_H
#if USE_LANGINFO
	"The search term is converted from the character set specified\n",
	"by the current locale into UTF-8. (See the output of `locale -a'\n",
	"for a list of locale settings available on your system).\n",
#else /* USE_LANGINFO */
	"Since langinfo() support is not available on this platform,\n",
	"the character set of the search string should be specified\n",
	"via the X resource/command-line option textEncoding if the\n",
	"encoding is different from iso_8859-1.\n",
#endif /* USE_LANGINFO */
#else /* HAVE_ICONV_H */
        "Since iconv() support is not available on this platform,\n",
	"the search term should be a string in the encoding specified\n",
        "by the X resource/command-line option textEncoding;\n",
        "currently, only the values iso_8859-1 and utf-8 are suported.\n",
#endif
        "Internally, the text in the DVI file is represented in\n",
        "UTF-8 encoding (you can view the text by saving the DVI\n",
        "file to a text file in UTF-8 encoding via the `File -> Save As ...'\n",
        "dialog).\n",
	"\n",
	"Ideographic characters from CJKV fonts are treated specially:\n",
	"All white space (spaces and newlines) before and after such\n",
	"characters is ignored in the search string and in the DVI file.\n",
	"\n",
	"To match a newline character, use `\\n' in the search string;\n",
	"to match the string `\\n', use `\\\\n'.\n",
        "\n",
        "If the checkbox Regular Expression is activated, the\n",
        "string is teated as a regular expression in extended POSIX\n",
        "format, with the following properties:\n",
        "\n",
        "   a? matches a zero or one times\n",
        "\n",
        "   a* matches a zero or more times\n",
        "\n",
        "   a+ matches a one or more times. Note that * and + are\n",
        "   greedy, i.e. they match the longest possible\n",
        "   sub string.\n",
        "\n",
        "   a{n} matches a exactly n times\n",
        "\n",
        "   a{n,m} matches a at least n and no more than m times\n",
        "\n",
        "   a|b matches a or b. Brackets can be used for grouping,\n",
        "   e.g.: (a|b)|c.\n",
        "\n",
        "   The string matched by the nth group can be referenced\n",
        "   by \\n, e.g. \\1 refers to the first match.\n",
        "\n",
        "   The characters ^ and $ match the beginning and the end\n",
        "   of a line, respectively.\n",
        "\n",
        "   [abc] matches any of the letters a, b, c, and [a-z]\n",
        "   matches all characters from a to z.\n",
        "\n",
        "   The patterns . and [...] without an explicit newline\n",
        "   don't match a newline character.\n",
        "\n",
        "   Each item in a regular expression can also be one of\n",
        "   the following POSIX character classes:\n",
        "   [[:alnum:]] [[:alpha:]] [[:blank:]] [[:cntrl:]] [[:digit:]]\n",
        "   [[:graph:]] [[:lower:]] [[:print:]] [[:space:]] [[:upper:]]\n",
        "\n",
        "   These can be negated by inserting a ^ symbol after the\n",
        "   first bracket: [^[:alpha:]]\n",
        "\n",
        "   For more details on POSIX regular expressions, see\n",
        "   e.g. the IEEE Std 1003.1 available online from:\n",
        "\n",
        "   http://www.opengroup.org/onlinepubs/007904975/basedefs/xbd_chap09.html\n",
	"\n",
        "   As a non-standard extension, the following Perl-like\n",
        "   abbreviations can be used instead of the POSIX classes:\n",
        "\n",
        "\n",
        "      Symbol Meaning                       POSIX Class\n",
        "\n",
        "        \\w   an alphanumeric character     [[:alnum:]]\n",
        "        \\W   a non-alphanumeric character  [^[:alnum:]]\n",
        "        \\d   a digit character             [[:digit:]]\n",
        "        \\D   a non-digit character         [^[:digit:]]\n",
        "        \\s   a whitespace character        [[:space:]]\n",
        "        \\S   a non-whitespace character    [^[:space:]]\n",
        "\n",
        "   The following characters are special symbols; they\n",
        "   need to be escaped with \\ in order to match them\n",
        "   literally: ( ) [ ] . * ? + ^ $ \\.\n",
        "\n",
        "The dialog also provides checkboxes to search backwards,\n",
        "to match in a case-sensitive manner (the default is to\n",
        "ignore case, i.e. a search string Test will match both\n",
        "the strings test and TEST in the DVI file) and to ignore\n",
	"newlines and hyphens in the DVI file.\n",
	"\n",
	NULL
    };
    
    k = width = 0;

    init_item(resource.help_general, default_help_general, info, k++, &width);
    init_item(resource.help_pagemotion, default_help_pagemotion, info, k++, &width);
    init_item(resource.help_othercommands, default_help_othercommands, info, k++, &width);
    init_item(resource.help_hypertex, default_help_hypertex, info, k++, &width);
    init_item(resource.help_mousebuttons, default_help_mousebuttons, info, k++, &width);
    init_item(resource.help_modes, default_help_modes, info, k++, &width);
    init_item(resource.help_search, default_help_search, info, k++, &width);
    init_item(resource.help_pagemotion, default_help_marking, info, k++, &width);
    init_item(resource.help_sourcespecials, default_help_sourcespecials, info, k++, &width);

    ASSERT(k < NUM_HELP_TOPICS, "Too many elements in help topics!");

    /* NULL-terminate items info */
    info->items[k].widget = 0;
    info->items[k].topic = info->items[k].title = NULL;

    /* adjust width of topics label to longest text */
#if !MOTIF
    XtVaSetValues(info->topic_label, XtNwidth, width, NULL);
#endif
}



/*
 * Pops up the help window. If topic != NULL, also selects the topic.
 */
void
show_help(Widget toplevel, const char *topic)
{
    size_t i;
    static Widget help_shell = 0;
    static struct topic_info info;
    static struct topic_item items[NUM_HELP_TOPICS];
    static Boolean first_time = True;
    
    if (help_shell == 0) { /* called 1st time; create widget */

	/* no special callbacks for OK/Cancel buttons */
	info.ok_callback = NULL;
	info.cancel_callback = NULL;
	info.items = items;
	/* 	info.items_size = NUM_HELP_TOPICS; */
	    
	help_shell = create_topic_window(toplevel,
					 "xdvik: Help",
					 "help_window",
					 &info,
					 initialize_items,
					 "Close",
					 /* no Cancel button needed */
					 NULL);
	info.shell = help_shell;
	
	center_window(help_shell, globals.widgets.top_level);
    }
    
#if MOTIF
    { /* check if resources are set properly */
	Dimension w, h;
	XtVaGetValues(help_shell, XtNwidth, &w, XtNheight, &h, NULL);
	if (h < 200 || w < 400) {
	    XDVI_WARNING((stderr, "Initial help window size too small (%dx%d); overriding size.\n"
			  "Please check/update your application defaults file, and set both of\n"
			  "`XDvi*help_text.rows' and `XDvi*help_text.columns' to a realistic value.",
			  h, w));
	    XtVaSetValues(help_shell, XtNwidth, 620, XtNheight, 520, NULL);
	}
    }
#endif
    XtPopup(help_shell, XtGrabNone);

    if (topic != NULL) {
	Boolean matched = False;
	for (i = 0; info.items[i].topic != NULL; i++) {
	    if (strcmp(info.items[i].topic, topic) == 0) { /* match */
		select_topic(&info, i);
		matched = True;
	    }
	}
	if (!matched) {
	    XdviBell(DISP, XtWindow(help_shell), 0);
	    popup_message(help_shell,
			  MSG_WARN,
			  NULL,
			  "Shouldn't happen: Could not find topic `%s' in help list!\n"
			  REPORT_XDVI_BUG_TEMPLATE,
			  topic);
	}
    }
    else if (first_time) {
	first_time = False;
	select_topic(&info, 0);
    }
}
