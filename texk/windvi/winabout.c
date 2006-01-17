#include "wingui.h"

/*****************************************************************************
  About Box
  ****************************************************************************/
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgAboutInit(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgAboutCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT CmdAboutDone(HWND, WORD, WORD, HWND);

/* About dialog message table definition. */
MSD rgmsdAbout[] =
{
    {WM_COMMAND,    MsgAboutCommand},
    {WM_INITDIALOG, MsgAboutInit}
};

MSDI msdiAbout =
{
    sizeof(rgmsdAbout) / sizeof(MSD),
    rgmsdAbout,
    edwpNone
};

/* About dialog command table definition. */
CMD rgcmdAbout[] =
{
    {IDOK,     CmdAboutDone},
    {IDCANCEL, CmdAboutDone}
};

CMDI cmdiAbout =
{
    sizeof(rgcmdAbout) / sizeof(CMD),
    rgcmdAbout,
    edwpNone
};

/* Module specific "globals"  Used when a variable needs to be
   accessed in more than on handler function. */

HFONT hFontCopyright;

/*
  About Box
  */

/*
  Process the IDM_ABOUT message, opens a dialog box.
  */

LRESULT CmdAbout(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    DialogBox(hInst, "AboutBox", hwnd, (DLGPROC)About);
    return 0;
}

/*
  Process messages for the about box.
  */

LRESULT CALLBACK About(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiAbout, hdlg, uMessage, wparam, lparam);
}

/*
  Initializes the about box with version info from resources.
  */

LRESULT MsgAboutInit(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    #define POINTSIZE 8

    char  szFullPath[256];
    DWORD dwVerHnd;
    DWORD dwVerInfoSize;
    HDC   hDC;
    int   iLogPixelsY, iPointSize;

     /*  Center the dialog over the application window */
    CenterWindow(hdlg, GetWindow(hdlg, GW_OWNER));

     /*  Set the copyright font to something smaller than default */
    hDC = GetDC(hdlg);
    iLogPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
    ReleaseDC(hdlg, hDC);
    iPointSize = MulDiv(iLogPixelsY, POINTSIZE, 72);
    iPointSize *= -1;

    hFontCopyright = CreateFont(iPointSize,
                                0, 0, 0,
                                FW_BOLD,
                                0, 0, 0, 0,
                                0, 0, 0, 0,
                                "Arial");

    SendDlgItemMessage(hdlg, 
                       IDD_VERLAST, 
                       WM_SETFONT, 
                       (WPARAM)hFontCopyright,
                       0L);
    SendDlgItemMessage(hdlg, 
                       IDD_VERCOPY, 
                       WM_SETFONT, 
                       (WPARAM)hFontCopyright,
                       0L);

     /*  Get version information from the application */
    GetModuleFileName(hInst, szFullPath, sizeof(szFullPath));
    dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
    if (dwVerInfoSize)
    {
         /*  If we were able to get the information, process it: */
        HANDLE  hMem;
        LPVOID  lpvMem;
        char    szGetName[256];
        int     cchRoot;
        int     i;

        hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
        lpvMem = GlobalLock(hMem);
        GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);
        lstrcpy(szGetName, "\\StringFileInfo\\040904E4\\");
        cchRoot = lstrlen(szGetName);

         /*  Walk through the dialog items that we want to replace: */
        for (i = IDD_VERFIRST; i <= IDD_VERLAST; i++)
        {
            BOOL  fRet;
            UINT  cchVer = 0;
            LPSTR lszVer = NULL;
            char  szResult[256];

            GetDlgItemText(hdlg, i, szResult, sizeof(szResult));
            lstrcpy(&szGetName[cchRoot], szResult);
            fRet = VerQueryValue(lpvMem, szGetName, &lszVer, &cchVer);

            if (fRet && cchVer && lszVer)
            {
                 /*  Replace dialog item text with version info */
                lstrcpy(szResult, lszVer);
                SetDlgItemText(hdlg, i, szResult);
            }
        }
        GlobalUnlock(hMem);
        GlobalFree(hMem);
    }
    return TRUE;
}

/*
  Processes WM_COMMAND sent to the about box.
  */

LRESULT MsgAboutCommand(HWND   hwnd, 
                        UINT   uMessage, 
                        WPARAM wparam, 
                        LPARAM lparam)
{
    return DispCommand(&cmdiAbout, hwnd, wparam, lparam);
}

/*
  OK has been clicked on, free the box and related resources.
  */

LRESULT CmdAboutDone(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    if (hFontCopyright)
       DeleteObject(hFontCopyright);

    EndDialog(hdlg, TRUE);           /*  Exit the dialog */
    return TRUE;
}
