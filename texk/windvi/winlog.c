#include "wingui.h"
#include "xdvi-config.h"

/*****************************************************************************
  View Log File Box
  ****************************************************************************/
LRESULT CALLBACK DlgViewLog(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgViewLogInit(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgViewLogCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT CmdViewLogOK(HWND, WORD, WORD, HWND);
/* LRESULT CmdViewLogCancel(HWND, WORD, WORD, HWND); */

/* ViewLog dialog message table definition. */
MSD rgmsdViewLog[] =
{
    {WM_COMMAND,    MsgViewLogCommand},
    {WM_INITDIALOG, MsgViewLogInit}
};

MSDI msdiViewLog =
{
    sizeof(rgmsdViewLog) / sizeof(MSD),
    rgmsdViewLog,
    edwpNone
};

/* ViewLog dialog command table definition. */
CMD rgcmdViewLog[] =
{
    {IDOK,     CmdViewLogOK}
    /*    {IDCANCEL, CmdViewLogCancel} */
};

CMDI cmdiViewLog =
{
    sizeof(rgcmdViewLog) / sizeof(CMD),
    rgcmdViewLog,
    edwpNone
};

/* Global Variables */
HANDLE hLogEdit;
HANDLE hLogEvent;
char *szText;
BOOL abortRequest;		/* Used to wait the user has acknowledged 
				 the log view. */
BOOL somethingToRead;
/*****************************************************************************
  View Log File Box
  ****************************************************************************/
LRESULT CALLBACK DlgViewLog(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiViewLog, hdlg, uMessage, wparam, lparam);
}

BOOL CreateViewLog(HWND hwndParent)
{
  abortRequest = FALSE;
  somethingToRead = FALSE;
  hViewLog = CreateDialog(hInst,
                          "DlgViewLog",
			  hwndParent, 
			  (DLGPROC) DlgViewLog); 

  if (hViewLog == NULL) {
    Win32Error("ViewLog/CreateDialog");
    return FALSE;
  }

  hLogEdit = GetDlgItem(hViewLog, IDC_LOG_EDIT);

  SendDlgItemMessage(hViewLog, IDC_LOG_EDIT, 
			   EM_LIMITTEXT, (WPARAM)0, 0);

  ShowWindow(hViewLog, SW_HIDE);
  bLogShown = False;
  return TRUE;
}

LRESULT MsgViewLogInit(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  return 0;
}


LRESULT MsgViewLogCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  return DispCommand(&cmdiViewLog, hwnd, wparam, lparam);
}

LRESULT CmdViewLogOK(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  somethingToRead = FALSE;	/* The user has read */
#if 0
  if (abortRequest) {
    DestroyWindow(hViewLog);
    hViewLog = NULL;
  }
  else {
#endif
    ShowWindow(hViewLog, SW_HIDE);
    bLogShown = False;
    if (hWndMain) SetForegroundWindow(hWndMain);
#if 0
  }
#endif
  return 0;
}

#if 0
LRESULT CmdViewLogCancel(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  /* If there is nothing to read, destroy the window immediately */
  if (somethingToRead) {
    abortRequest = TRUE;
    ShowWindow(hViewLog, SW_SHOW);
    bLogShown=TRUE;
  }
  else {
    DestroyWindow(hViewLog);
    hViewLog = NULL;
  }
    
  return 0;
}
#endif

/*
  Log Loop function
  */

DWORD WINAPI LogLoop(LPVOID lpParam)
{
  MSG msg;
  int rc;

  if (CreateViewLog(hWndMain) == FALSE) {
    return 1;
  }

  if (SetEvent(hLogEvent) == 0) {
    return 1;
  }

  while (TRUE) {
    rc = GetMessage(&msg, hViewLog, 0, 0);
    switch (rc) {
    case -1:
      break;
    case 0:
      goto end1;
    default:
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      break;
    }
  }
 end1:
  if (hViewLog) DestroyWindow(hViewLog);
  hViewLog = NULL;

#if 0
  MessageBox(NULL, "Exit ViewLog thread", NULL, MB_APPLMODAL | MB_ICONHAND | MB_OK);
#endif
  ExitThread(0);
  /* unreachable */
  return 0;
}

/*
  Sentinel function. It runs in its own thread and sends messages 
  to the dialog box.
*/

DWORD WINAPI ViewLogSentinel(LPVOID lpParam)
{
  char data[64];
  char line[128];
  int nbRead, count, text_count;
  BOOL bResult;
  DWORD idLoopThread;
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

  count = 0;			/* count chars in line */
  ZeroMemory(line, sizeof(line));
#if 0
  if ((szText = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1)) == NULL) {
      MessageBox(NULL, "HeapAlloc failed", "ViewLog", 
		 MB_OK|MB_ICONERROR );
      return 1;
  }
#endif
  text_count = 1;

  if ((hLogEvent = CreateEvent(&sa, /* security attributes */
			       FALSE, /* Automatic reset */
			       FALSE, /* Initial state is reset */
			       "WinLogEvent")) == NULL) {
    Win32Error("ViewLogSentinel/WinLogEvent");
    return 0;
  }

  if ((hLogLoopThread = CreateThread(&sa, /* security attributes */
				  0,	/* default stack size */
				  LogLoop, /* start address of thread */
				  0,	/* parameter */
				  0,	/* creation flags */
				  &idLoopThread /* thread id */
				  )) == NULL) {
    Win32Error("ViewLogSentinel/LogLoopThread");
    return 0;
  }

  if (WaitForSingleObject(hLogEvent, 4000) != WAIT_OBJECT_0) {
    Win32Error("Log Window not created !");
    return 0;
  }
  
  CloseHandleAndClear(&hLogEvent);

  do {
    nbRead = 0;
    bResult = ReadFile(hLogIn, data, 1, &nbRead, NULL /* &gOverLapped */);
    if (!bResult || !nbRead) {
      /* EOF on hCrtOut */
      goto exit_loop;
    }

    if (!bResult) {	
      /* deal with the error code */
      switch (GetLastError()) {
      case ERROR_BROKEN_PIPE:
	/* program exiting */
#if 1
	MessageBox(NULL, "Broken Pipe", NULL, MB_APPLMODAL | MB_ICONHAND | MB_OK);
#endif
	goto exit_loop;
      default:
	Win32Error("ViewLogSentinel/ReadFile");
      }
    }

    /* first, accumulate into line[]
s       until 80 chars or eol */

    if (data[0] == '\n') {
      line[count++] = '\r';
      line[count++] = '\n';
    }
    else
      line[count++] = data[0];

    if (data[0] == '\n' || count >= 80) {
      if (hViewLog) {
	/* remove any selection */
		SendDlgItemMessage(hViewLog, IDC_LOG_EDIT, EM_SETSEL,
						   -1, 0);
		SendDlgItemMessage(hViewLog, IDC_LOG_EDIT, EM_REPLACESEL,
						   FALSE, (LPARAM)line);
		if (! bLogShown) {
		  /* FIXME : really needed ? */
		  GdiFlush();
		  somethingToRead = TRUE;
		  if (resource.log_flag) {
		    ShowWindow(hViewLog, SW_SHOW);
		    /* SetFocus(hViewLog); */
		    SetForegroundWindow(hViewLog);
		    /* There might be some fight to get the focus. */
		    SetForegroundWindow(hWndMain);
		    if (bMagDisp) {
		      SetForegroundWindow(hWndMagnify);
		    }
		    bLogShown = TRUE;
		  }
		}
		UpdateWindow(hViewLog);
      }
      ZeroMemory(line, sizeof(line));
      count = 0;
    }
	
  }  while(1);

exit_loop:
  if (PostMessage(hViewLog, WM_QUIT, 0, 0) == 0)
    Win32Error("ViewLogSentinel/PostMessage/WM_QUIT");
  if (WaitForSingleObject(hLogLoopThread, INFINITE) == WAIT_FAILED) {
    // MessageBox( NULL, "LogLoop thread does not want to shut down...", "", MB_OK|MB_ICONINFORMATION );
    Win32Error("ViewLogSentinel/WaitForSingleObject/hLogLoopThread");
  }
  CloseHandleAndClear(&hLogLoopThread);

  return 0;
}
