#include "wingui.h"
#include "xdvi-config.h"

/*****************************************************************************
  GotoPage Box
  ****************************************************************************/
LRESULT CALLBACK DlgGotoPage(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgGotoPageInit(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgGotoPageCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT CmdGotoPageOK(HWND, WORD, WORD, HWND);
LRESULT CmdGotoPageCancel(HWND, WORD, WORD, HWND);
LRESULT CmdGotoPageSPUp(HWND, WORD, WORD, HWND);
LRESULT CmdGotoPageSPDown(HWND, WORD, WORD, HWND);

/* GotoPage dialog message table definition. */
MSD rgmsdGotoPage[] =
{
    {WM_COMMAND,    MsgGotoPageCommand},
    {WM_INITDIALOG, MsgGotoPageInit}
};

MSDI msdiGotoPage =
{
    sizeof(rgmsdGotoPage) / sizeof(MSD),
    rgmsdGotoPage,
    edwpNone
};

/* GotoPage dialog command table definition. */
CMD rgcmdGotoPage[] =
{
    {IDOK,     CmdGotoPageOK},
    {IDCANCEL, CmdGotoPageCancel}
};

CMDI cmdiGotoPage =
{
    sizeof(rgcmdGotoPage) / sizeof(CMD),
    rgcmdGotoPage,
    edwpNone
};

/*
  Goto page dialog box
  */

/*
  Processes WM_COMMAND sent to the GotoPage box.
  */

LRESULT MsgGotoPageCommand(HWND   hwnd, 
			   UINT   uMessage, 
			   WPARAM wparam, 
			   LPARAM lparam)
{
  return DispCommand(&cmdiGotoPage, hwnd, wparam, lparam);
}

LRESULT MsgGotoPageInit(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  /*  Center the dialog over the application window */
  CenterWindow(hdlg, GetWindow(hdlg, GW_OWNER));

  /* Put the current page number into the edit */
  SendDlgItemMessage(hdlg, IDC_SPIN_PAGE_NUMBER, UDM_SETPOS,
		     0, (LPARAM) MAKELONG((short) current_page + 1, 0));
  /* Put the page range into the spinner */
  SendDlgItemMessage(hdlg, IDC_SPIN_PAGE_NUMBER, UDM_SETRANGE,
		     0, (LPARAM) MAKELONG((short) total_pages, 1));
  /* set the selection */
  SendDlgItemMessage(hdlg, IDC_PAGE_NUMBER, EM_SETSEL,
		     0, -1);
  SetFocus(GetDlgItem(hdlg, IDC_PAGE_NUMBER));
  return 0;
}

/*
  OK has been clicked on, free the box and related resources.
  */

LRESULT CmdGotoPageOK(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  char szBuf[80];
  int new_page;
  /* Get the current page number */
  *((LPDWORD)szBuf) = sizeof(szBuf); 
  SendDlgItemMessage(hdlg, IDC_PAGE_NUMBER, EM_GETLINE,
		     0, (LPARAM)szBuf);
  new_page = atoi(szBuf) - 1;
  wsprintf(szBuf, "Goto page %d", new_page+1);
  UpdateStatusBar(szBuf, 0, 0);
  ChangePage(new_page - current_page);
  EndDialog(hdlg, TRUE);           /*  Exit the dialog */
  return 0;
}

/*
  Cancel has been clicked on, free the box and related resources.
  */

LRESULT CmdGotoPageCancel(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    EndDialog(hdlg, TRUE);           /*  Exit the dialog */
    return 0;
}

/*
  Process messages for the GotoPage box.
  */

LRESULT CALLBACK DlgGotoPage(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiGotoPage, hdlg, uMessage, wparam, lparam);
}

