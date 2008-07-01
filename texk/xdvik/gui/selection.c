/*
 * Copyright (c) 2004 Stefan Ulrich
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
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include <string.h>

#include <X11/Xatom.h>
#include <X11/StringDefs.h>

#if HAVE_X11_XMU_XMU_H
#include <X11/Xmu/Atoms.h>
#include <X11/Xmu/Xmu.h>
#endif

#include "events.h"
#include "util.h"
#include "encodings.h"
#include "message-window.h"
#include "selection.h"
#include "statusline.h"

/*
 * Set the current X selection (i.e. the primary selection AKA XA_PRIMARY).
 * Most of this is copied from Asente/Converse/Swick: X Window System Toolkit Manual
 */

/* This is file scope since deliver_selection_cb() needs access to it.
 * I guess we could also store it in an Atom, but why bother ...
 */
static char *m_selection_text = NULL;
static char *m_cvt_selection_text = NULL;
static size_t m_selection_size = 0;

/* helper routine */
static Atom
fetch_atom(Widget w, const char *name)
{
    Atom a;
    XrmValue source, dest;

    source.size = strlen(name) + 1;
    source.addr = (char *)name;
    dest.size = sizeof a;
    dest.addr = (caddr_t)&a;

    (void)XtConvertAndStore(w, XtRString, &source, XtRAtom, &dest);
    return a;
}

/* Lose the selection */
static void
lose_selection_cb(Widget w, Atom *selection)
{
    UNUSED(w);
    UNUSED(selection);
    
    text_change_region(TEXT_SEL_CLEAR, NULL);
}

static char *
utf8_to_native_encoding(const char *utf8)
{
    static const char *text_encoding = NULL;

    /* convert to user encoding, similar to search-internal.c */
    if (text_encoding == NULL) {
	text_encoding = get_text_encoding();
    }

    if (memicmp(text_encoding, "iso-8859-1", strlen("iso-8859-1")) == 0
	|| memicmp(text_encoding, "iso8859-1", strlen("iso8859-1")) == 0) {
	return str_utf8_to_iso_8859_1(utf8);
    }
    else if (memicmp(text_encoding, "utf-8", strlen("utf-8")) != 0
	     && memicmp(text_encoding, "utf8", strlen("utf8")) != 0) {
	/* some other encoding */
	return iconv_convert_string("utf-8", text_encoding, utf8);
    }
    /* fallback */
    return xstrdup(utf8);
}

/* Deliver the selection. Only supports XA_STRING. */
static Boolean
deliver_selection_cb(Widget w,
		     Atom *selection,
		     Atom *target,
		     Atom *type,
		     XtPointer *value,
		     unsigned long *length,
		     int *format)
{
    Atom targets = fetch_atom(w, "TARGETS");
    Atom utf8_string = fetch_atom(w, "UTF8_STRING");
    
    TRACE_GUI((stderr, "selection target = %lu (%s)", 
	       *target, XGetAtomName(DISP, *target)));

    if (m_selection_text == NULL) /* paranoia */
	return False;
    
#if HAVE_X11_XMU_XMU_H
    if (*target == targets) {
	/* TARGETS handling copied from xclipboard.c */
	Atom* targetP;
	XPointer std_targets; /* was: Atom* */
	unsigned long std_length;
	XSelectionRequestEvent* req = XtGetSelectionRequest(w, *selection, (XtRequestId)NULL);

	TRACE_GUI((stderr, "Selection type: targets"));
	XmuConvertStandardSelection(w, req->time, selection,
				    target, type, &std_targets, &std_length, format);
	*value = XtMalloc(sizeof(Atom)*(std_length + 2));
	targetP = *(Atom**)value;
	*length = std_length + 2;
	*targetP++ = XA_COMPOUND_TEXT(DISP);
	*targetP++ = XA_STRING;
	memcpy((char*)targetP, (char*)std_targets, sizeof(Atom)*std_length);
	XtFree((char*)std_targets);
	*type = XA_ATOM;
	*format = sizeof(Atom) * 8;
	return True;
    }
    else
#endif
    {
	if (*target == utf8_string) {
	    TRACE_GUI((stderr, "Selection type: UTF8_STRING"));
	    *type = *target;
	    *value = (XtPointer)XtNewString(m_selection_text);
	    /* *value = (XtPointer)m_selection_text; */
	    *length = strlen(m_selection_text);
	    *format = 8;
	    return True;
	}
	else if (*target == XA_STRING) {
	    char *ptr = utf8_to_native_encoding(m_selection_text);
	    TRACE_GUI((stderr, "Selection type: XA_STRING"));
	    strncpy(m_cvt_selection_text, ptr, m_selection_size);
	    m_cvt_selection_text[m_selection_size - 1] = '\0'; /* ensure termination */
	    free(ptr);
	    *type = *target;
	    *value = (XtPointer)XtNewString(m_cvt_selection_text);
	    /* *value = (XtPointer)m_selection_text; */
	    *length = strlen(m_cvt_selection_text);
	    *format = 8;
	    return True;
	}
#if HAVE_X11_XMU_XMU_H
	else if (*target == XA_COMPOUND_TEXT(DISP) || *target == XA_TEXT(DISP)) {
	    const char *cl[1];
	    char *ptr;
	    int retval;
	
	    XTextProperty ct;
	    XICCEncodingStyle style = XStdICCTextStyle;
	
	    TRACE_GUI((stderr, "Selection type: XA_COMPOUND_TEXT"));
	    ptr = utf8_to_native_encoding(m_selection_text);
	    strncpy(m_cvt_selection_text, ptr, m_selection_size);
	    m_cvt_selection_text[m_selection_size - 1] = '\0'; /* ensure termination */
	    cl[0] = m_cvt_selection_text;
	
	    *type = *target;
	    retval = XmbTextListToTextProperty(DISP, (char **)cl, 1, style, &ct);

	    if (retval == XNoMemory || retval == XLocaleNotSupported || retval == XConverterNotFound) {
		statusline_info(STATUS_MEDIUM, "XmbTextListToTextProperty failed: %d", retval);
		return False;
	    }		
	
	    *value = ct.value;
	    *length = ct.nitems;
	    *format = 8;
	    return True;
	}
#endif
	else {
#if HAVE_X11_XMU_XMU_H
	    TRACE_GUI((stderr, "Selection type: standard selection"));
	    if (XmuConvertStandardSelection(w, CurrentTime, selection,
					    target, type, (XPointer *)value, length, format))
		return True;
	    else {
#endif
		TRACE_GUI((stderr, "Selection type unsupported: %lu (%s)",
			   (unsigned long)*target, XGetAtomName(DISP, *target)));
		statusline_error(STATUS_MEDIUM,
				 "X client asked for an unsupported selection target type: %lu (%s)",
				 (unsigned long)*target, XGetAtomName(DISP, *target));
		return False ;
#if HAVE_X11_XMU_XMU_H
	    }
#endif
	}
    }
}

/* do it */
Boolean
set_selection(const char *text, Widget w)
{
    /* caller should make sure that text is never longer than 4 * XMaxRequestSize(DISP) - 32 */
    if (m_selection_text == NULL) {
	m_selection_size = 4 * XMaxRequestSize(DISP);
	m_selection_text = xmalloc(m_selection_size);
	m_cvt_selection_text = xmalloc(m_selection_size);
    }
    strncpy(m_selection_text, text, m_selection_size);
    m_selection_text[m_selection_size - 1] = '\0'; /* ensure termination */

    return XtOwnSelection(w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)),
			  deliver_selection_cb,
			  lose_selection_cb,
			  (XtSelectionDoneProc)NULL);
}

void
unset_selection(Widget w)
{
    XtDisownSelection(w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)));
}
 
