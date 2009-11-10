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
 *
 */

/*
 * Panel 4 (Paper size) for xdvik preferences dialog.
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include "x_util.h"
#include "xm_colorsel.h"
#include "topic-window.h"
#include "util.h"
#include "events.h"
#include "statusline.h"

#include "xm_prefsP.h"
#include "xm_prefs_page.h"
#include "my-snprintf.h"

#ifdef MOTIF /* entire file */

#include <X11/Xatom.h>

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/DialogS.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/PanedW.h>
#include <Xm/DrawnB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ToggleBG.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/CascadeBG.h>

#if USE_SPINBOX
#include <Xm/SpinB.h>
#endif

/*
 * Handy defaults
 */
static Arg one_of_many = { XmNindicatorType, XmONE_OF_MANY };
static Arg n_of_many = { XmNindicatorType, XmN_OF_MANY };
static Arg frame_title = { XmNchildType, XmFRAME_TITLE_CHILD };
static Arg one_col[] = {
    { XmNpacking, XmPACK_TIGHT },
    { XmNnumColumns, 1 },
    { XmNorientation, XmVERTICAL }
};


/* this is a small selection from the paper types in xdvi.c
 * (only the more frequenlty used ones)
 */
static struct paper_info {
    const char *format;
    const char *resource_format;
    const char *size;
    const char *landscape_size;
} papersizes[] = {
    { "US (Letter)",	"us",		"8.5x11in",	"11x8.5in"	},
    { "Tabloid",	"tabloid",	"11x17in",	"17x11in"	},	/* dvips compatibility */
    { "Legal",		"legal",	"8.5x14in",	"14x8.5in"	},
    { "A0",		"a0",		"841x1189mm",	"1189x841mm"	},
    { "A1",		"a1",		"594x841mm",	"841x594mm"	},
    { "A2",		"a2",		"420x594mm",	"594x420mm"	},
    { "A3",		"a3",		"297x420mm",	"420x297mm"	},
    { "A4",		"a4",		"210x297mm",	"297x210mm"	},
    { "A5",		"a5",		"148x210mm",	"210x148mm"	},
    { "B5",		"b5",		"176x250mm",	"250x176mm"	},
    { "B6",		"b6",		"125x176mm",	"176x125mm"	},
    { "B7",		"b7",		"88x125mm",	"125x88mm"	},
    { NULL,		NULL,		NULL,		NULL		} /* terminate */
};

void update_preferences_windowsize(void)
{
    Widget shell, button;

    /* it's not an error if the prefs dialog doesn't exist yet */
    if (get_widget_by_name(&shell, globals.widgets.top_level, Xdvi_PREFS_DIALOG_NAME, False)
	&& get_widget_by_name(&button, shell, Xdvi_REMEMBER_WINDOWSIZE_STR, True)) {
	XmToggleButtonGadgetSetState(button, resource.remember_windowsize, False);
    }
}

void update_preferences_shrink(void)
{
    Widget shell, text;

    if (get_widget_by_name(&shell, globals.widgets.top_level, Xdvi_PREFS_DIALOG_NAME, False)
	&& get_widget_by_name(&text, shell, Xdvi_SHRINK_TEXT, True)) {
	char buf[LENGTH_OF_INT];
	SNPRINTF(buf, LENGTH_OF_INT, "%d", resource.shrinkfactor);
#if USE_SPINBOX
	XmTextFieldSetString(text, buf);
	XtVaSetValues(text, XmNposition, resource.shrinkfactor, NULL);
#else
	XtVaSetValues(text, XmNvalue, buf, NULL);
#endif
    }
}

static void
landscape_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *tinfo  = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)tinfo->data;
    Widget portrait_b, landscape_b;
    Boolean landscape = False, is_set;
    static char *paper_option = NULL;
    Boolean match = False;
    int i;

    UNUSED(call_data);
    
    for (i = 0; papersizes[i].format != NULL; i++) {
	/* ignore final `r' when comparing */
	if (strncmp(resource.paper,
		    papersizes[i].resource_format,
		    strlen(papersizes[i].resource_format)) == 0) {
	    const char *ptr;
	    match = True;
	    if (strcmp(resource.paper, "letter") == 0) /* special case */
		ptr = "us";
	    else
		ptr = papersizes[i].resource_format;
	    free(paper_option);
	    paper_option = xstrdup(ptr);
	    break;
	}
    }

    if (!match) {
	XDVI_ERROR((stderr, "paper resource `%s' not found in list!\n", resource.paper));
	return;
    }

    if (get_widget_by_name(&portrait_b, tinfo->shell, Xdvi_PAPER_PORTRAIT_STR, True)
	&& get_widget_by_name(&landscape_b, tinfo->shell, Xdvi_PAPER_LANDSCAPE_STR, True)) {

	if (w == portrait_b) {
	    XtVaGetValues(portrait_b, XmNset, &is_set, NULL);
	    landscape = !is_set;
	}
	else {
	    XtVaGetValues(landscape_b, XmNset, &is_set, NULL);
	    landscape = is_set;
	}

	/* 	fprintf(stderr, "++++++++ CALLBACK: setting portrait to %s, landscape to %s\n", */
	/* 		landscape ? "False" : "True", */
	/* 		landscape ? "True" : "False"); */
	
	XmToggleButtonGadgetSetState(portrait_b, !landscape, False);
	XmToggleButtonGadgetSetState(landscape_b, landscape, False);
    }

    if (landscape) {
	paper_option = xstrcat(paper_option, "r");
    }

    resource.paper = paper_option;
    store_preference(&(prefs->db), "paper", "%s", paper_option);
}

static void
select_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *tinfo  = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)tinfo->data;

    XmString str;
    char *choice;
    int i;
    static char *curr_choice= NULL;
    Boolean landscape = False;
    
    UNUSED(call_data);
    
    XtVaGetValues(w, XmNlabelString, &str, NULL);
    XmStringGetLtoR(str, G_charset, &choice);

    if (strlen(resource.paper) > 0 &&
	strcmp(resource.paper, "letter") != 0 &&
	resource.paper[strlen(resource.paper) - 1] == 'r')
	landscape = True;
    
    free(curr_choice);
    curr_choice = NULL;
    for (i = 0; papersizes[i].format != NULL; i++) {
	if (strcmp(choice, papersizes[i].format) == 0) {
	    curr_choice = xstrdup(papersizes[i].resource_format);
	}
    }
    if (curr_choice == NULL) {
	XDVI_ERROR((stderr, "choice`%s' not found in list!\n", choice));
	return;
    }

    if (landscape) {
	curr_choice = xstrcat(curr_choice, "r");
    }

    resource.paper = curr_choice;
    store_preference(&(prefs->db), "paper", "%s", curr_choice);
}

void
update_preferences_paper(void)
{
    Widget shell, portrait_button, landscape_button, sizes_menu, sizes_cascade;
    Boolean landscape = False;

    /* it's not an error if the prefs dialog doesn't exist yet */
    if (get_widget_by_name(&shell, globals.widgets.top_level, Xdvi_PREFS_DIALOG_NAME, False)
	&& get_widget_by_name(&portrait_button, shell, Xdvi_PAPER_PORTRAIT_STR, True)
	&& get_widget_by_name(&landscape_button, shell, Xdvi_PAPER_LANDSCAPE_STR, True)
	&& get_widget_by_name(&sizes_menu, shell, Xdvi_PAPER_MENU, True)
	&& get_widget_by_name(&sizes_cascade, shell, Xdvi_PAPER_CASCADE, True)) {

	int i;
	Widget sizes_button;
	
	if (strlen(resource.paper) > 0 &&
	    strcmp(resource.paper, "letter") != 0 &&
	    resource.paper[strlen(resource.paper) - 1] == 'r') {

	    landscape = True;
	}

#if 0
	XtVaGetValues(portrait_button, XmNset, &test1, NULL);
	XtVaGetValues(landscape_button, XmNset, &test2, NULL);
	
	fprintf(stderr, "+++++++ old state of portrait: %d, landscape: %d\n",
		test1, test2);
	
	fprintf(stderr, "+++++++ setting portrait to %s, landscape to %s\n",
		landscape ? "False" : "True",
		landscape ? "True" : "False");
#endif /* 0 */

	/*
	  Apparently there's a bug in Motif (OpenMotif 2.1) in that the button
	  is not properly updated in all cases.

	  To reproduce:

	  Open Preferences dialog, change `landscape' option, click
	  `OK'.  Now open the dialog again, change the option again,
	  but click `Cancel'.  This correctly reverts the visible
	  state of the buttons, but when opening the dialog again,
	  the button's internal state is not consistent with the
	  visual appearance: when clicking on the unset button, its
	  ValueChanged callback is not invoked, so nothing happens;
	  but after clicking on another(!)  button in the preferences
	  dialog, it works again.

	  As a workaround, we destroy the buttons and re-create them from scratch - yuck!
	*/
	{
	    Widget form = XtParent(portrait_button);
	    Widget cascade;
	    struct topic_info *info;
	    
	    XtVaGetValues(portrait_button,
			  XmNleftWidget, &cascade,
			  XmNuserData, &info,
			  NULL);
	    XtDestroyWidget(portrait_button);
	    XtDestroyWidget(landscape_button);

	    portrait_button = XmCreateToggleButtonGadget(form, Xdvi_PAPER_PORTRAIT_STR, &one_of_many, 1);
	    XmToggleButtonGadgetSetState(portrait_button, !landscape, False);
	    XtVaSetValues(portrait_button,
			  XmNtopAttachment, XmATTACH_FORM,
			  XmNbottomAttachment, XmATTACH_FORM,
			  XmNleftAttachment, XmATTACH_WIDGET,
			  XmNleftWidget, cascade,
			  XmNleftOffset, 10,
			  XmNuserData, (XtPointer)info,
			  NULL);
	    landscape_button = XmCreateToggleButtonGadget(form, Xdvi_PAPER_LANDSCAPE_STR, &one_of_many, 1);
	    XmToggleButtonGadgetSetState(landscape_button, landscape, False);
	    XtVaSetValues(landscape_button,
			  XmNtopAttachment, XmATTACH_FORM,
			  XmNbottomAttachment, XmATTACH_FORM,
			  XmNleftAttachment, XmATTACH_WIDGET,
			  XmNleftWidget, portrait_button,
			  XmNleftOffset, 10,
			  XmNuserData, (XtPointer)info,
			  NULL);
	    XtManageChild(portrait_button);
	    XtManageChild(landscape_button);
	    XtAddCallback(portrait_button, XmNvalueChangedCallback, landscape_cb, (XtPointer)info);
	    XtAddCallback(landscape_button, XmNvalueChangedCallback, landscape_cb, (XtPointer)info);
	}
#if 0
	XtVaGetValues(portrait_button, XmNset, &test1, NULL);
	XtVaGetValues(landscape_button, XmNset, &test2, NULL);
	
	fprintf(stderr, "+++++++ new state of portrait: %d, landscape: %d\n",
		test1, test2);
#endif /* 0 */
	
	for (i = 0; papersizes[i].format != NULL; i++) {
	    if (strncmp(resource.paper,
			papersizes[i].resource_format,
			strlen(papersizes[i].resource_format)) == 0
		&& get_widget_by_name(&sizes_button, sizes_menu, papersizes[i].format, True)) {

		XtVaSetValues(sizes_cascade, XmNmenuHistory, sizes_button, NULL);
	    }
	}
    }
}

#if 0 /* currently unused */
static void
shrinkfactor_spinbox_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    int val;
    /*      static Boolean first_time = True; */
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);
    UNUSED(call_data);
    
    XtVaGetValues(w, XmNposition, &val, NULL);

    /* return if value not changed yet; else the first invocation of the
       window may actually reset the shrink factor to the default.
    */
    /*      if (val != resource.shrinkfactor) { */
    /*  	first_time = False; */
    /*      } */

    if (XtIsRealized(w)) {
	/* don't set the resource.shrinkfactor here so that we can revert it from there if needed */
	do_set_shrinkfactor(val, False);
	store_preference(&(prefs->db), "shrinkFactor", "%d", val);
    }
}
#endif /* currently unused */

static void
set_shrinkfactor_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    char *text;
    int val;
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);

#if USE_SPINBOX
    XmTextPosition pos;
    UNUSED(call_data);
    text = XmTextFieldGetString(w);
    val = strtol(text, (char **)NULL, 10);
#else
    Widget text_field;
    UNUSED(call_data);
    XtVaGetValues(w, XmNuserData, &text_field, NULL);
    ASSERT(text_field != 0, "Expected text field in XmNuserData, set_shrinkfactor_cb()");
    XtVaGetValues(text_field, XmNvalue, &text, NULL);
    val = strtol(text, (char **)NULL, 10);
    /*     fprintf(stderr, "spinbox value: |%s|\n", text); */
#endif

    if (!XtIsRealized(w))
	return;
    
    /* verify values in case of direct text input */
    if (val > 100 || val <= 0) {
	if (val > 100)
	    statusline_info(STATUS_MEDIUM, "Shrink factor larger than maximum 100");
	else if (val <= 0)
	    statusline_info(STATUS_MEDIUM, "Shrink factor smaller than minimum 1");
	return;
    }
#if USE_SPINBOX
    /* synchronize internal spinbox value */
    pos = XmTextFieldGetInsertionPosition(w);
    XtVaSetValues(w, XmNposition, val, NULL);
    XmTextFieldSetInsertionPosition(w, pos);
#endif
    /* don't set the resource.shrinkfactor here so that we can revert it from there if needed */
    do_set_shrinkfactor(val, False);
    store_preference(&(prefs->db), "shrinkFactor", "%d", val);
    
    XtFree(text);
}

static Widget
h_create_shrink_frame(Widget top, struct topic_info *info)
{
    Widget shrink_form, shrink_label;

    UNUSED(info);
    
    shrink_form = XmCreateForm(top, "shrink_form", NULL, 0);
    XtVaSetValues(shrink_form,
		  /*  		  XmNverticalSpacing, 10, */
		  /*  		  XmNhorizontalSpacing, 10, */
		  NULL);

    
    shrink_label = XmCreateLabelGadget(shrink_form, "Default Shrink Factor: ", NULL, 0);
    XtVaSetValues(shrink_label,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  /* TODO  XmNleftOffset, 10, */
		  NULL);
    XtManageChild(shrink_label);
    {
	Widget shrink_text;
#if USE_SPINBOX
	Widget shrink_spinbox =
	    XtVaCreateManagedWidget(Xdvi_SHRINK_SPINBOX, xmSpinBoxWidgetClass, shrink_form,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNleftAttachment, XmATTACH_WIDGET,
				    XmNleftWidget, shrink_label,
				    XmNinitialDelay, 200,
				    XmNrepeatDelay, 50,
				    NULL);
	shrink_text =
	    XtVaCreateManagedWidget(Xdvi_SHRINK_TEXT, xmTextFieldWidgetClass, shrink_spinbox,
				    XmNspinBoxChildType, XmNUMERIC,
				    XmNminimumValue, 1,
				    XmNmaximumValue, 100,
				    XmNeditable, True,
				    XmNcolumns, 3,
				    XmNincrementValue, 1,
				    XmNwrap, False, /* too confusing */
				    /* NOTE: use resource.shrinkfactor here, not current setting,
				       otherwise the current setting will sneak into ~/.xdvirc */
				    XmNposition, (int)(resource.shrinkfactor),
				    NULL);

	XtAddCallback(shrink_text, XmNactivateCallback, set_shrinkfactor_cb, (XtPointer)info);
	XtAddCallback(shrink_text, XmNvalueChangedCallback, set_shrinkfactor_cb, (XtPointer)info);

	adjust_heights(shrink_spinbox, shrink_text, shrink_label, NULL);
	
#else
	char buf[LENGTH_OF_INT];
	Widget shrink_button;
	shrink_text = XmCreateTextField(shrink_form, Xdvi_SHRINK_TEXT, NULL, 0);
	SNPRINTF(buf, LENGTH_OF_INT, "%d", resource.shrinkfactor);
	XtVaSetValues(shrink_text,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_WIDGET,
		      XmNleftWidget, shrink_label,
		      XmNcolumns, 4,
		      XmNvalue, buf,
		      XmNuserData, shrink_text,
		      NULL);
	XtOverrideTranslations(shrink_text, XtParseTranslationTable("<Key>Return:activate()"));
	XtAddCallback(shrink_text, XmNactivateCallback, set_shrinkfactor_cb, (XtPointer)info);

	shrink_button = XmCreatePushButton(shrink_form, Xdvi_APPLY_STR, NULL, 0);
	XtVaSetValues(shrink_button,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_WIDGET,
		      XmNleftOffset, 10,
		      XmNleftWidget, shrink_text,
		      XmNuserData, shrink_text,
		      NULL);
	XtAddCallback(shrink_button, XmNactivateCallback, set_shrinkfactor_cb, (XtPointer)info);
	
	adjust_heights(shrink_label, shrink_text, shrink_button, NULL);
	XtManageChild(shrink_text);
	XtManageChild(shrink_button);
#endif
    }
    
    /*     XtManageChild(shrink_form); */

    return shrink_form;
}

static void
remember_windowsize_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);

    UNUSED(w);
    UNUSED(call_data);
    
    resource.remember_windowsize = !resource.remember_windowsize;
    
    store_preference(&(prefs->db), "rememberWindowSize", "%s", resource.remember_windowsize ? "True" : "False");
}



static Widget
h_create_papersize_form(Widget parent, struct topic_info *info)
{
    Widget form, menu, cascade, portrait_option, landscape_option;
    XmString str;
    Arg args[8];
    int n;
    size_t i;
    Boolean landscape = False;
    
    form = XmCreateForm(parent, "paper_form", NULL, 0);
    
    menu = XmCreatePulldownMenu(form, Xdvi_PAPER_MENU, NULL, 0);

    if (strcmp(resource.paper, "letter") != 0 && /* exceptions: these have no landscape format */
	strcmp(resource.paper, "ledger") != 0 &&
	strlen(resource.paper) > 0 &&
	resource.paper[strlen(resource.paper) - 1] == 'r') { /* landscape format */
	landscape = True;
    }

    str = XmStringCreateLocalized("Paper Size:");

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, menu); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

    cascade = XmCreateOptionMenu(form, Xdvi_PAPER_CASCADE, args, n);
    XmStringFree(str);

    portrait_option = XmCreateToggleButtonGadget(form, Xdvi_PAPER_PORTRAIT_STR, &one_of_many, 1);
    XmToggleButtonGadgetSetState(portrait_option, !landscape, False);
    XtVaSetValues(portrait_option,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, cascade,
		  XmNleftOffset, 10,
		  XmNuserData, (XtPointer)info,
		  NULL);
    
    landscape_option = XmCreateToggleButtonGadget(form, Xdvi_PAPER_LANDSCAPE_STR, &one_of_many, 1);
    XmToggleButtonGadgetSetState(landscape_option, landscape, False);
    XtVaSetValues(landscape_option,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, portrait_option,
		  XmNleftOffset, 10,
		  XmNuserData, (XtPointer)info,
		  NULL);
    /*     adjust_heights(landscape_option, menu, cascade, NULL); */
    XtAddCallback(portrait_option, XmNvalueChangedCallback, landscape_cb, (XtPointer)info);
    XtAddCallback(landscape_option, XmNvalueChangedCallback, landscape_cb, (XtPointer)info);
    /*      XtAddCallback(portrait_option, XmNarmCallback, test_cb, (XtPointer)info); */
    /*      XtAddCallback(landscape_option, XmNarmCallback, test_cb, (XtPointer)info); */
    
    XtManageChild(portrait_option);
    XtManageChild(landscape_option);
    
    for (i = 0; papersizes[i].format != NULL; i++) {
	Widget w = XtVaCreateManagedWidget(papersizes[i].format, xmPushButtonGadgetClass, menu,
					   XmNuserData, landscape_option,
					   NULL);
	/*  	fprintf(stderr, "Created button: %p for %s\n", w, papersizes[i].format); */

	/* select default value */
	if (strncmp(resource.paper,
		    papersizes[i].resource_format,
		    strlen(papersizes[i].resource_format)) == 0) {
	    XtVaSetValues(cascade, XmNmenuHistory, w, NULL);
	}
	XtAddCallback(w, XmNactivateCallback, select_cb, (XtPointer)info);
    }
    XtManageChild(cascade);

    return form;
}

Widget
prefs_paper(struct topic_info *info)
{
    Widget form;
    Widget geometry_frame, geometry_label, geometry_rowcol, shrink_frame, geometry_button;
    Widget paper_frame, paper_label;
    Arg args[10];
    int n = 0;
    Widget rowcol;
#if !defined(LESSTIF_VERSION)
    Widget paper_text;
#endif
    Widget paper_size_form;
    
    form = XtVaCreateWidget("form", xmFormWidgetClass,
			    info->right_form,
			    XmNtopAttachment, XmATTACH_FORM,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNbottomAttachment, XmATTACH_FORM,
			    NULL);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 8); n++;
    XtSetArg(args[n], XmNmarginHeight, 4); n++;
    geometry_frame = XmCreateFrame(form, "geometry_frame", args, n);
    h_attach_below(geometry_frame,  NULL);

    geometry_label = XmCreateLabelGadget(geometry_frame, "Window Size and Shrink Factor", &frame_title, 1);
    XtManageChild(geometry_label);
    
    geometry_rowcol = XmCreateRowColumn(geometry_frame, "geometry_rowcol", one_col, XtNumber(one_col));
    
    geometry_button = XmCreateToggleButtonGadget(geometry_rowcol, Xdvi_REMEMBER_WINDOWSIZE_STR, &n_of_many, 1);
    XmToggleButtonGadgetSetState(geometry_button, resource.remember_windowsize, False);
    XtAddCallback(geometry_button, XmNvalueChangedCallback, remember_windowsize_cb, (XtPointer)info);
    XtManageChild(geometry_button);
    
    shrink_frame = h_create_shrink_frame(geometry_rowcol, info);
    XtManageChild(shrink_frame);

    XtManageChild(geometry_rowcol);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 8); n++;
    XtSetArg(args[n], XmNmarginHeight, 4); n++;
    XtSetArg(args[n], XmNtopOffset, 10); n++;
    paper_frame = XmCreateFrame(form, "paper_frame", args, n);
    h_attach_below(paper_frame, geometry_frame);

    paper_label = XmCreateLabelGadget(paper_frame, "Default Paper Size", &frame_title, 1);
    XtManageChild(paper_label);
    
    rowcol = XmCreateRowColumn(paper_frame, "papersize_rowcol", NULL, 0);

#if !defined(LESSTIF_VERSION) /* stupid LessTif doesn't wrap Labels at '\n' */
    paper_text = XmCreateLabelGadget(rowcol,
				     "Used if the DVI file does not specify the paper size, e.g. via\n"
				     "\\usepackage[dvips]{geometry}.  This setting will only take effect\n"
				     "after restarting xdvi.", NULL, 0);
    XtVaSetValues(paper_text,
		  XmNalignment, XmALIGNMENT_BEGINNING,
		  NULL);
    XtManageChild(paper_text);
#endif
    
    paper_size_form = h_create_papersize_form(rowcol, info);
    XtManageChild(paper_size_form);

    XtManageChild(rowcol);
    
    XtManageChild(geometry_frame);
    XtManageChild(paper_frame);
    
    XtManageChild(form);

    return form;
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* MOTIF */

