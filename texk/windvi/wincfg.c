/*
  Desc: This has to manage the property sheet for WinDvi configuration.
  */

#include "wingui.h"
#include <prsht.h>
#include "xdvi-config.h"
#include "mfmodes.h"

HWND hwndPSheet;

/*
  DVI File Configuration Dialog.
  */
LRESULT CALLBACK DviConfigDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDviConfigInit(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDviConfigCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDviConfigNotify(HWND, UINT, WPARAM, LPARAM);
LRESULT CmdDviConfigOK(HWND, WORD, WORD, HWND);
LRESULT CmdDviConfigCancel(HWND, WORD, WORD, HWND);

/* DviConfig dialog message table definition. */
MSD rgmsdDviConfig[] =
{
    {WM_COMMAND,    MsgDviConfigCommand},
    {WM_INITDIALOG, MsgDviConfigInit},
    {WM_NOTIFY,     MsgDviConfigNotify}
};

MSDI msdiDviConfig =
{
    sizeof(rgmsdDviConfig) / sizeof(MSD),
    rgmsdDviConfig,
    edwpNone
};

/* DviConfig dialog command table definition. */
CMD rgcmdDviConfig[] =
{
    {IDOK,     CmdDviConfigOK},
    {IDCANCEL, CmdDviConfigCancel}
};

CMDI cmdiDviConfig =
{
    sizeof(rgcmdDviConfig) / sizeof(CMD),
    rgcmdDviConfig,
    edwpNone
};

/*
  Page View Configuration Dialog.
  */
LRESULT CALLBACK ViewConfigDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgViewConfigInit(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgViewConfigCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgViewConfigNotify(HWND, UINT, WPARAM, LPARAM);
LRESULT CmdViewConfigOK(HWND, WORD, WORD, HWND);
LRESULT CmdViewConfigCancel(HWND, WORD, WORD, HWND);

/* ViewConfig dialog message table definition. */
MSD rgmsdViewConfig[] =
{
    {WM_COMMAND,    MsgViewConfigCommand},
    {WM_INITDIALOG, MsgViewConfigInit},
    {WM_NOTIFY,     MsgViewConfigNotify}
};

MSDI msdiViewConfig =
{
    sizeof(rgmsdViewConfig) / sizeof(MSD),
    rgmsdViewConfig,
    edwpNone
};

/* ViewConfig dialog command table definition. */
CMD rgcmdViewConfig[] =
{
    {IDOK,     CmdViewConfigOK},
    {IDCANCEL, CmdViewConfigCancel}
};

CMDI cmdiViewConfig =
{
    sizeof(rgcmdViewConfig) / sizeof(CMD),
    rgcmdViewConfig,
    edwpNone
};

/*
  Dvi File Configuration dialog box.
  */

/* 
   List of controls in this dialog :
   Pixels-per-inches : IDC_EDITBDPI          : edit
   Alternate font    : IDC_EDITALTFONT       : edit
   MF mode           : IDC_COMBOMFMODE       : combo box
   Paper type        : IDC_COMBOPAPER        : combo box
   Make pk           : IDC_CHECKMAKEPK       : check box
   List fonts        : IDC_CHECKLISTFONTS    : check box
   Auto scan         : IDC_CHECKSCAN         : check box
   Single instance   : IDC_CHECKSINGLE       : check box
   Alllow Shell      : IDC_CHECKALLOWSHELL   : check box
   Hush specials     : IDC_CHECKHUSHSPECIALS : boolean
   Hush chars        : IDC_CHECKHUSHCHARS    : boolean
   Hush checksums    : IDC_CHECKHUSHCHECKSUMS: boolean
   */
void DviConfigExport(HWND hwnd)
{
  char szBuf[256];
  int i;
  extern char * paper_types[];
  extern int paper_types_number;


  /* Put every current value in its control */
  wsprintf(szBuf, "%d", resource._pixels_per_inch);
  SendDlgItemMessage(hwnd, IDC_EDITBDPI, WM_SETTEXT, (WPARAM)0, (LPARAM)szBuf);

  SendDlgItemMessage(hwnd, IDC_EDITALTFONT, WM_SETTEXT, (WPARAM)0, (LPARAM)resource._alt_font);

  /* populate combo boxes */
  /* MF modes */
  for (i = 0; i < sizeof(avail_modes)/sizeof(mfmode) - 1; i++) {
    SendDlgItemMessage(hwnd, IDC_COMBOMFMODE, CB_ADDSTRING, 
		       (WPARAM) 0, (LPARAM)avail_modes[i].desc);
  }
  for (i = 0; i < sizeof(avail_modes)/sizeof(mfmode) - 1; i++) {
    if (resource.mfmode && (strcmp(resource.mfmode, avail_modes[i].name) == 0)) {
      SendDlgItemMessage(hwnd, IDC_COMBOMFMODE, CB_SELECTSTRING, 
			 (WPARAM) i, (LPARAM)avail_modes[i].desc);
      /* SendDlgItemMessage(hwnd, IDC_COMBOMFMODE, CB_SETCURSEL, 
			 (WPARAM) i, (LPARAM)0);  */
    }
  }
  /* Paper types */
  for (i = 0; i < paper_types_number; i+=2) {
    wsprintf(szBuf, "%-10s%s", paper_types[i], paper_types[i+1]);
    SendDlgItemMessage(hwnd, IDC_COMBOPAPER, CB_ADDSTRING, 
		       (WPARAM) 0, (LPARAM)szBuf);
  }

  if (resource.paper)
    SendDlgItemMessage(hwnd, IDC_COMBOPAPER, CB_SELECTSTRING, 
		       (WPARAM) 0, (LPARAM)resource.paper);
  
  SendDlgItemMessage(hwnd, IDC_CHECKMAKEPK, BM_SETCHECK, 
		     (WPARAM) ( resource.makepk ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKLISTFONTS, BM_SETCHECK,
		     (WPARAM)( resource._list_fonts ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKSCAN, BM_SETCHECK,
		     (WPARAM)( resource.scan_flag ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKLOG, BM_SETCHECK,
		     (WPARAM)( resource.log_flag ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKSINGLE, BM_SETCHECK,
		     (WPARAM)( resource.single_flag ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKHUSHSPECIALS, BM_SETCHECK,
		     (WPARAM)( resource._warn_spec ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKHUSHCHARS, BM_SETCHECK,
		     (WPARAM)( resource._hush_chars ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKHUSHCHECKSUMS, BM_SETCHECK,
		     (WPARAM)( resource._hush_chk ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKALLOWSHELL, BM_SETCHECK,
		     (WPARAM)( resource.allow_shell ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
}

BOOL DviConfigImport(HWND hwnd)
{
  char szBuf[256], *p;
  int i;

  SendDlgItemMessage(hwnd, IDC_EDITBDPI, WM_GETTEXT, (WPARAM)sizeof(szBuf), (LPARAM)szBuf);
  if (resource._pixels_per_inch = atoi(szBuf))
    xputenv("BDPI", szBuf);

  SendDlgItemMessage(hwnd, IDC_EDITALTFONT, WM_GETTEXT, (WPARAM)sizeof(szBuf), (LPARAM)szBuf);
  if (resource._alt_font)
    free(resource._alt_font);
  resource._alt_font = strdup(szBuf);

  SendDlgItemMessage(hwnd, IDC_COMBOMFMODE, WM_GETTEXT, (WPARAM)sizeof(szBuf), (LPARAM)szBuf);
  if (resource.mfmode)
    free(resource.mfmode);

  for (i = 0; i < sizeof(avail_modes)/sizeof(mfmode) - 1; i++) {
    if (strcmp(szBuf, avail_modes[i].desc) == 0) {
      resource.mfmode = strdup(avail_modes[i].name);
      xputenv("MODE", resource.mfmode);
      break;
    }
  }
#if 0
  fprintf(stderr, "Dvi config import putenv MODE\n");
#endif
  SendDlgItemMessage(hwnd, IDC_COMBOPAPER, WM_GETTEXT, (WPARAM)sizeof(szBuf), (LPARAM)szBuf);
  if (resource.paper)
    free(resource.paper);

  if (*szBuf) {
    for (p = szBuf; *p && p < szBuf+sizeof(szBuf); p++)
      if (*p == ' ') {
	*p = '\0';
	break;
      }
    resource.paper = strdup(szBuf);
    set_paper_type();
    init_page();
  }

  resource.makepk = (SendDlgItemMessage(hwnd, IDC_CHECKMAKEPK, BM_GETCHECK, 
					(WPARAM) 0, (LPARAM)0) == BST_CHECKED);

  resource._list_fonts = (SendDlgItemMessage(hwnd, IDC_CHECKLISTFONTS, 
					     BM_GETCHECK, (WPARAM) 0, (LPARAM)0) == BST_CHECKED);
  resource.scan_flag = (SendDlgItemMessage(hwnd, IDC_CHECKSCAN, 
					   BM_GETCHECK, (WPARAM) 0, (LPARAM)0) == BST_CHECKED);
  resource.log_flag = (SendDlgItemMessage(hwnd, IDC_CHECKLOG, 
					   BM_GETCHECK, (WPARAM) 0, (LPARAM)0) == BST_CHECKED);
  resource.single_flag = (SendDlgItemMessage(hwnd, IDC_CHECKSINGLE, 
					     BM_GETCHECK, (WPARAM) 0, (LPARAM)0) == BST_CHECKED);
  resource._warn_spec = (SendDlgItemMessage(hwnd, IDC_CHECKHUSHSPECIALS, 
					    BM_GETCHECK, (WPARAM) 0, (LPARAM)0) == BST_CHECKED);
  resource._hush_chars = (SendDlgItemMessage(hwnd, IDC_CHECKHUSHCHARS,
					     BM_GETCHECK,(WPARAM) 0, (LPARAM)0) == BST_CHECKED);
  resource._hush_chk = (SendDlgItemMessage(hwnd, IDC_CHECKHUSHCHECKSUMS, 
					   BM_GETCHECK, (WPARAM) 0, (LPARAM)0) == BST_CHECKED);
  resource.allow_shell = (SendDlgItemMessage(hwnd, IDC_CHECKALLOWSHELL, 
					     BM_GETCHECK, (WPARAM) 0, (LPARAM)0) == BST_CHECKED);

  return TRUE;
}

BOOL DviConfigValidate(HWND hwnd)
{

  return TRUE;
}

void UpdateConfiguration()
{
  extern Boolean init_dvi_file(void);
  extern void ReopenDviFile(void);

  initcolor();
  /* FIXME : this should happen only if mode or bdpi changed 
   or shrink factor has changed. */
  reset_fonts();
  reconfig();
  SendMessage(hWndDraw, WM_ERASEBKGND, (WPARAM)GetDC(hWndDraw), 0);
  ChangePage(0);
}

/*
  Processes WM_COMMAND sent to the DviConfig box.
  */

LRESULT MsgDviConfigCommand(HWND   hwnd, 
			   UINT   uMessage, 
			   WPARAM wparam, 
			   LPARAM lparam)
{
  return DispCommand(&cmdiDviConfig, hwnd, wparam, lparam);
}

LRESULT MsgDviConfigInit(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  DviConfigExport(hdlg);
  return 0;
}

LRESULT MsgDviConfigNotify(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  HWND hwndPSheet = ((NMHDR FAR *) lparam)->hwndFrom;
  switch (((NMHDR FAR *) lparam)->code) {
  case PSN_APPLY:
    if (DviConfigImport(hdlg)) {
      SendMessage(hwndPSheet, PSM_UNCHANGED, (WPARAM)hdlg, 0);
      SetWindowLong(hwndPSheet, DWL_MSGRESULT, PSNRET_NOERROR);
      UpdateConfiguration();
    } else
      SetWindowLong(hwndPSheet, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
    break;
  case PSN_HELP:            
    {
      /* char szBuf[PATH_MAX];	/* buffer for name of help file */
      /* Display help for the Dvi File options. */
    }
    return TRUE;
  break;
  case PSN_KILLACTIVE:
    if (DviConfigValidate(hdlg)) {
      SetWindowLong(hwndPSheet, DWL_MSGRESULT, FALSE);
    }
    else {
      SetWindowLong(hwndPSheet, DWL_MSGRESULT, TRUE);
    }
    break;
  case PSN_QUERYCANCEL:
    /* Accept cancelation */
    return FALSE;
    break;
  default:
    break;
  }
  return 0;
}

/*
  OK has been clicked on, free the box and related resources.
  */

LRESULT CmdDviConfigOK(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  /* EndDialog(hdlg, TRUE);           /*  Exit the dialog */
  return 0;
}

/*
  Cancel has been clicked on, free the box and related resources.
  */

LRESULT CmdDviConfigCancel(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  /* EndDialog(hdlg, TRUE);           /*  Exit the dialog */
    return 0;
}

/*
  Process messages for the DviConfig box.
  */

LRESULT CALLBACK DviConfigDlgProc(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiDviConfig, hdlg, uMessage, wparam, lparam);
}

/*
  View Page configuration dialog box.
  */

/*
  List of controls in this dialog box.
  Postscript          : IDC_CHECKPOSTSCRIPT
  Antialiasing        : IDC_CHECKALIASING
  GS Antialiasing     : IDC_CHECKGSALPHA
  Reverse video       : IDC_CHECKREVIDEO
  Density             : IDC_EDITDENSITY
  Gamma               : IDC_EDITGAMMA
  Grid 1 color        : 
  Grid 2 color        :
  Grid 3 color        :
  Mag size 1          : IDC_EDITMGS1
  Mag size 2          : IDC_EDITMGS2
  Mag size 3          : IDC_EDITMGS3
  */ 

void ViewConfigExport(HWND hwnd)
{
  int i;
  char szBuf[256];

  /* Put every current value in its control */
  sprintf(szBuf, "%2.4f", resource._gamma);
  SendDlgItemMessage(hwnd, IDC_EDITGAMMA, WM_SETTEXT, (WPARAM)0, (LPARAM)szBuf);
  wsprintf(szBuf, "%d", resource._density);
  SendDlgItemMessage(hwnd, IDC_EDITDENSITY, WM_SETTEXT, (WPARAM)0, (LPARAM)szBuf);

  wsprintf(szBuf, "%dx%d", mg_size[0].w, mg_size[0].h);
  SendDlgItemMessage(hwnd, IDC_EDITMGS1, WM_SETTEXT, (WPARAM)0, (LPARAM)szBuf);
  wsprintf(szBuf, "%dx%d", mg_size[1].w, mg_size[1].h);
  SendDlgItemMessage(hwnd, IDC_EDITMGS2, WM_SETTEXT, (WPARAM)0, (LPARAM)szBuf);
  wsprintf(szBuf, "%dx%d", mg_size[2].w, mg_size[2].h);
  SendDlgItemMessage(hwnd, IDC_EDITMGS3, WM_SETTEXT, (WPARAM)0, (LPARAM)szBuf);

  SendDlgItemMessage(hwnd, IDC_CHECKGSALPHA, BM_SETCHECK, 
		     (WPARAM) ( resource.gs_alpha ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKINMEMORY, BM_SETCHECK, 
		     (WPARAM) ( resource.in_memory ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKPOSTSCRIPT, BM_SETCHECK, 
		     (WPARAM) ( resource._postscript ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKALIASING, BM_SETCHECK, 
		     (WPARAM) ( resource._use_grey ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  SendDlgItemMessage(hwnd, IDC_CHECKREVIDEO, BM_SETCHECK, 
		     (WPARAM) ( resource.reverse ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);

#ifdef TRANSFORM
  if (IS_WIN95 || IS_WIN98) {
    SendDlgItemMessage(hwnd, IDC_CHECKXFORM, BN_DISABLE, 0, 0);
    resource.use_xform = False;
  }
  else {
    fprintf(stderr, "set use_xform is %s\n", (resource.use_xform ? "true" : "false"));
    SendDlgItemMessage(hwnd, IDC_CHECKXFORM, BM_SETCHECK,
		       (WPARAM)( resource.use_xform ? BST_CHECKED : BST_UNCHECKED), (LPARAM)0);
  }
#else
  SendDlgItemMessage(hwnd, IDC_CHECKXFORM, BN_DISABLE, 0, 0);
#endif

  /* Colors */
  for (i = 0; i < win32_color_map_size; i++) {
    wsprintf(szBuf, "%s", win32_color_map[i].name);
    SendDlgItemMessage(hwnd, IDC_COMBO_FORE_COLOR, CB_ADDSTRING, 
		       (WPARAM) 0, (LPARAM)szBuf);
    SendDlgItemMessage(hwnd, IDC_COMBO_BACK_COLOR, CB_ADDSTRING, 
		       (WPARAM) 0, (LPARAM)szBuf);
  }
  if (resource.fore_color)
    SendDlgItemMessage(hwnd, IDC_COMBO_FORE_COLOR, CB_SELECTSTRING, 
		       (WPARAM) 0, (LPARAM)resource.fore_color);
  if (resource.back_color)
    SendDlgItemMessage(hwnd, IDC_COMBO_BACK_COLOR, CB_SELECTSTRING, 
		       (WPARAM) 0, (LPARAM)resource.back_color);

}

BOOL ViewConfigImport(HWND hwnd)
{
  char szBuf[256];
  int i;

  SendDlgItemMessage(hwnd, IDC_EDITGAMMA, WM_GETTEXT, (WPARAM)sizeof(szBuf), (LPARAM)szBuf);
  resource._gamma = atof(szBuf);

  SendDlgItemMessage(hwnd, IDC_EDITDENSITY, WM_GETTEXT, (WPARAM)sizeof(szBuf), (LPARAM)szBuf);
  resource._density = atoi(szBuf);

  SendDlgItemMessage(hwnd, IDC_EDITMGS1, WM_GETTEXT, (WPARAM)sizeof(szBuf), (LPARAM)szBuf);
  if (resource.mg_arg[0])
    free(resource.mg_arg[0]);
  resource.mg_arg[0] = strdup(szBuf);
  SendDlgItemMessage(hwnd, IDC_EDITMGS2, WM_GETTEXT, (WPARAM)sizeof(szBuf), (LPARAM)szBuf);
  if (resource.mg_arg[1])
    free(resource.mg_arg[1]);
  resource.mg_arg[1] = strdup(szBuf);
  SendDlgItemMessage(hwnd, IDC_EDITMGS3, WM_GETTEXT, (WPARAM)sizeof(szBuf), (LPARAM)szBuf);
  if (resource.mg_arg[2])
    free(resource.mg_arg[2]);
  resource.mg_arg[2] = strdup(szBuf);

  for (i = 0; i < 3; ++i)
    if (resource.mg_arg[i] != NULL) {
      char    *s;

      mg_size[i].w = mg_size[i].h = atoi(resource.mg_arg[i]);
      s = strchr(resource.mg_arg[i], 'x');
      if (s != NULL) {
	mg_size[i].h = atoi(s + 1);
	if (mg_size[i].h <= 0) mg_size[i].w = 0;
      }
    }

  resource.gs_alpha = (SendDlgItemMessage(hwnd, IDC_CHECKGSALPHA, BM_GETCHECK, 
					(WPARAM) 0, (LPARAM)0) == BST_CHECKED);
  resource.in_memory = (SendDlgItemMessage(hwnd, IDC_CHECKINMEMORY, BM_GETCHECK, 
					(WPARAM) 0, (LPARAM)0) == BST_CHECKED);
  resource._use_grey = (SendDlgItemMessage(hwnd, IDC_CHECKALIASING, BM_GETCHECK, 
					(WPARAM) 0, (LPARAM)0) == BST_CHECKED);
  resource._postscript = (SendDlgItemMessage(hwnd, IDC_CHECKPOSTSCRIPT, BM_GETCHECK, 
					(WPARAM) 0, (LPARAM)0) == BST_CHECKED);
  resource.reverse = (SendDlgItemMessage(hwnd, IDC_CHECKREVIDEO, BM_GETCHECK, 
					(WPARAM) 0, (LPARAM)0) == BST_CHECKED);
#ifdef TRANSFORM
  if (IS_WIN95 || IS_WIN98) {
    resource.use_xform = False;
  }
  else {
    resource.use_xform =  (SendDlgItemMessage(hwnd, IDC_CHECKXFORM,
					      BM_GETCHECK, (WPARAM) 0, (LPARAM) 0) == BST_CHECKED);
    fprintf(stderr, "get use_xform is %s\n", (resource.use_xform ? "true" : "false"));
    if (resource.use_xform) {
      extern void init_xfrm_stack(void);
      init_xfrm_stack();
      ChangePage(0);
    }
  }
#else
  resource.use_xform = False;
#endif

  SendDlgItemMessage(hwnd, IDC_COMBO_FORE_COLOR, 
		     WM_GETTEXT, (WPARAM)sizeof(szBuf), (LPARAM)szBuf);
  if (resource.fore_color)
    free(resource.fore_color);
  resource.fore_color = xstrdup(szBuf);

  SendDlgItemMessage(hwnd, IDC_COMBO_BACK_COLOR, 
		     WM_GETTEXT, (WPARAM)sizeof(szBuf), (LPARAM)szBuf);
  if (resource.back_color)
    free(resource.back_color);
  resource.back_color = xstrdup(szBuf);

  return TRUE;
}

BOOL ViewConfigValidate(HWND hwnd)
{

  return TRUE;
}

/*
  Processes WM_COMMAND sent to the ViewConfig box.
  */

LRESULT MsgViewConfigCommand(HWND   hwnd, 
			   UINT   uMessage, 
			   WPARAM wparam, 
			   LPARAM lparam)
{
  return DispCommand(&cmdiViewConfig, hwnd, wparam, lparam);
}

LRESULT MsgViewConfigInit(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  /* Setup current values into the dialog */
  ViewConfigExport(hdlg);
  return 0;
}

LRESULT MsgViewConfigNotify(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  HWND hwndPSheet = ((NMHDR FAR *) lparam)->hwndFrom;

  switch (((NMHDR FAR *) lparam)->code) {
  case PSN_APPLY:
    if (ViewConfigImport(hdlg)) {
      SendMessage(hwndPSheet, PSM_UNCHANGED, (WPARAM)hdlg, 0);
      SetWindowLong(hwndPSheet, DWL_MSGRESULT, PSNRET_NOERROR);
      UpdateConfiguration();
    } else
      SetWindowLong(hwndPSheet, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
    break;
  case PSN_HELP:            
    {
      /* char szBuf[PATH_MAX];	/* buffer for name of help file */
      /* Display help for the View Page options. */
    }
    break;
  case PSN_KILLACTIVE:
    if (ViewConfigValidate(hdlg)) {
      SetWindowLong(hwndPSheet, DWL_MSGRESULT, TRUE);
    }
    else {
      SetWindowLong(hwndPSheet, DWL_MSGRESULT, FALSE);
    }
    break;
  case PSN_QUERYCANCEL:
    /* Accept cancelation */
    break;
  default:
    break;
  }
  return 0;
}

/*
  OK has been clicked on, free the box and related resources.
  */

LRESULT CmdViewConfigOK(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  /* Retrieve the values in the dialog and set them as current */

  /*  EndDialog(hdlg, TRUE);           /*  Exit the dialog */
#if	PS
  ps_destroy();
#endif
  
  initcolor();
  reconfig();
  redraw_page();

  return 0;
}

/*
  Cancel has been clicked on, free the box and related resources.
  */

LRESULT CmdViewConfigCancel(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  /* EndDialog(hdlg, TRUE);           /*  Exit the dialog */
    return 0;
}

/*
  Process messages for the ViewConfig box.
  */

LRESULT CALLBACK ViewConfigDlgProc(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiViewConfig, hdlg, uMessage, wparam, lparam);
}

int CALLBACK ConfigPSheetCallback(HWND hwnd, UINT uMsg, LPARAM lParam)
{
  if (uMsg == PSCB_INITIALIZED)
    hwndPSheet = hwnd;
  return 0;
}

/* DoConfigSheet - creates a property sheet that contains two pages.
   hwndOwner - handle to the owner window of the property sheet. */
   
void DoConfigPSheet(HWND hwndOwner)	
{    
  PROPSHEETPAGE psp[2];
  PROPSHEETHEADER psh;    
  int num_sheets = sizeof(psp) / sizeof(PROPSHEETPAGE);

  ZeroMemory(psp, num_sheets*sizeof(PROPSHEETPAGE));
  psp[0].dwSize = sizeof(PROPSHEETPAGE);
  /*  psp[0].dwFlags = PSP_USEICONID | PSP_USETITLE; */
  psp[0].hInstance = hInst;
  psp[0].pszTemplate = "DLGCONFIGDVI";
  /* psp[0].pszIcon = MAKEINTRESOURCE(IDI_FONT); */
  psp[0].pfnDlgProc = DviConfigDlgProc;
  /* psp[0].pszTitle = MAKEINTRESOURCE(IDS_FONT); */
  psp[0].lParam = 0;
  psp[0].pfnCallback = NULL;

  psp[1].dwSize = sizeof(PROPSHEETPAGE);
  /*  psp[1].dwFlags = PSP_USEICONID | PSP_USETITLE; */
  psp[1].hInstance = hInst;
  psp[1].pszTemplate = "DLGCONFIGVIEW";
  /* psp[1].pszIcon = MAKEINTRESOURCE(IDI_BORDER); */
  psp[1].pfnDlgProc = ViewConfigDlgProc;
  /* psp[1].pszTitle = MAKEINTRESOURCE(IDS_BORDER); */
  psp[1].lParam = 0;
  psp[1].pfnCallback = NULL;
  
  ZeroMemory(&psh, sizeof(PROPSHEETHEADER));
  psh.dwSize = sizeof(PROPSHEETHEADER);
  psh.dwFlags = /* PSH_USEICONID |*/ PSH_PROPSHEETPAGE;
  psh.hwndParent = hwndOwner;    
  psh.hInstance = hInst;
  /*  psh.pszIcon = MAKEINTRESOURCE(IDI_CELL_PROPERTIES); */
  psh.pszCaption = (LPSTR) "Windvi Configuration";
  psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);    
  psh.nStartPage = 0;
  psh.ppsp = (LPCPROPSHEETPAGE) &psp;    
  psh.pfnCallback = ConfigPSheetCallback;
  PropertySheet(&psh);    
  return;
}

LRESULT CmdWindviConfig (HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
#if 0
   SET_CRT_DEBUG_FIELD( _CRTDBG_DELAY_FREE_MEM_DF );
   SET_CRT_DEBUG_FIELD( _CRTDBG_CHECK_ALWAYS_DF );
#endif 
  DoConfigPSheet(hwnd);
  return 0;
}

LRESULT CmdTexConfig (HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  MessageBox (hwnd, 
	      "Command Not Yet Implemented.\r\n", 
	      "Windvi",
	      MB_OK | MB_ICONEXCLAMATION);
  return 0;
}

