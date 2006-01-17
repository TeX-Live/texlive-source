#include "wingui.h"

/* C RunTime Header Files */
#ifndef DEBUG_GUI
#include <kpathsea/c-minmax.h>
#endif


LRESULT DispDefault(EDWP, HWND, UINT, WPARAM, LPARAM);

/* Functions defined in winevt.c */
#include <stdio.h>
#include <setjmp.h>
#include "xdvi-config.h"

/*
  System type
*/
int iSystemType = -1;

int GetSystemType(void);

/****************************************************************************
  Global Variables
  ***************************************************************************/

HINSTANCE hInst;		/* Current instance */
char szAppName[100];		/* Name of the app */
char szTitle[100];		/* The title bar text */
HWND hWndMain;			/* Application main window handle */
HWND hWndDraw;			/* The window with the dvi page */
HWND hViewLog = NULL;		/* Logging window */
HDC maneDC;			/* DC for the screen window */
HBITMAP maneDIB;		/* In memory bitmap for the dvi page */
HBITMAP magDIB;			/* In memory bitmap for the magnifying glass */
HMENU hMenuMain;		/* Main menu */

char *szLogFileName;		/* Temporary log file for kpathsea */
HANDLE hCrtIn, hCrtOut, hCrtErr; /* Standard handles */
/* hCrtOut and hCrtErr are the same handle, duplicated
   from the writable part of a pipe.
   */
HANDLE hLogIn;			/* One side of the pipe for logging */
/*
  Logging needs 2 threads :
  - one sentinel for reading on the other (readable) end of the pipe
  - one loop message for the hViewLog window.
  With these threads, anybody writing on stdout/stderr will
  make hViewLog pop up, and the writings will be displayed 
  almost synchronously.
  */
HANDLE hViewLogThread = 0, 
  hLogLoopThread = 0;
BOOL bLogShown;			/* Log window is shown */

int tbHeight, sbHeight;		/* Toolbar and Status bar heights */
int maneHeight, maneWidth;	/* hWndDraw height and width */
RECT maneRect;			/* hWndDraw Rect */
RECT rectWndPrev;               /* Previous instance window rectangle */
BOOL bPrevInstance = FALSE;	/* Is there a previous instance ? */
BOOL bSkipFirstClick = FALSE;	/* Avoid mag. glass at click to gain focus.  */

/* Scrollbars' variables */
SCROLLINFO si;
int xMinScroll;
int xMaxScroll;
int xCurrentScroll;
int yMinScroll;
int yMaxScroll;
int yCurrentScroll;
BOOL fScroll;
BOOL fSize;

BOOL bInitComplete = FALSE;		/* Initialization phase completed */

/* Mouse Position */
int xMousePos;
int yMousePos;

/* 
   Last used files.
   This is a queue. It is addressed in thae arry, indexes modulo number of 
   entries. Two values tell the head and tail of the queue.
*/
char **lpLastUsedFiles;
int iLastUsedFilesNum;
int iLastCurrentUsed, iLastLatestUsed;

/*****************************************************************************
  Main window message table definition. 
  ****************************************************************************/
MSD rgmsd[] =
{
    {WM_CREATE,     MsgCreate    },
    {WM_SIZE,       MsgSize      },
    {WM_MOVE,       MsgMove      },
    {WM_DROPFILES,  MsgDropFiles },
    {WM_COMMAND,    MsgCommand   },
    {WM_NOTIFY,     MsgNotify    },
    {WM_MENUSELECT, MsgMenuSelect},
    {WM_DESTROY,    MsgDestroy   },
    {WM_CHAR,       MsgChar },
    {WM_KEYDOWN,    MsgKeyDown },
    {WM_ACTIVATE,   MsgActivate },
    {WM_COPYDATA,   MsgCopyData }
    /*    {WM_PAINT,      MsgPaint     } */
};

MSDI msdiMain =
{
    sizeof(rgmsd) / sizeof(MSD),
    rgmsd,
    edwpWindow
};


/* Main window command table definition. */
CMD rgcmd[] =
{
    {IDM_FILEOPEN,    CmdOpen},
    {IDM_FILECLOSE,   CmdClose},
    {IDM_FILEPRINT,   CmdFilePrint},
    {IDM_FILEPRINTDVIPS,   CmdFilePrint},
    {IDM_FILEPAGESU,  CmdStub},
    {IDM_FILEPRINTSU, CmdFilePrSetup},
    {IDM_EXIT,        CmdExit},
    
    {IDM_FILE_RECENT,  CmdOpenRecentFile},
    {IDM_FILE_RECENT1, CmdOpenRecentFile},
    {IDM_FILE_RECENT2, CmdOpenRecentFile},
    {IDM_FILE_RECENT3, CmdOpenRecentFile},
    {IDM_FILE_RECENT4, CmdOpenRecentFile},
    {IDM_FILE_RECENT5, CmdOpenRecentFile},
    {IDM_FILE_RECENT6, CmdOpenRecentFile},
    {IDM_FILE_RECENT7, CmdOpenRecentFile},
    {IDM_FILE_RECENT8, CmdOpenRecentFile},
    {IDM_FILE_RECENT9, CmdOpenRecentFile},

    {IDM_ZOOMIN,      CmdZoomIn},
    {IDM_ZOOMOUT,     CmdZoomOut},
    {IDM_REDRAWPAGE,  CmdRedrawPage},
    {IDM_KEEPPOS,     CmdKeepPosition},
    {IDM_TOGGLEPS,    CmdTogglePS},
    {IDM_TOGGLEGRID,  CmdToggleGrid},

    {IDM_NEXTPAGE,    CmdNextPage},
    {IDM_PREVIOUSPAGE,CmdPreviousPage},
    {IDM_NEXT5,       CmdNext5},
    {IDM_PREVIOUS5,   CmdPrevious5},
    {IDM_NEXT10,      CmdNext10},
    {IDM_PREVIOUS10,  CmdPrevious10},
    {IDM_GOTOPAGE,    CmdGotoPage},
	{IDM_SRCSPECIALS, CmdSrcSpecials},
    {IDM_FIRSTPAGE,   CmdFirstPage},
    {IDM_LASTPAGE,    CmdLastPage},

#ifdef HTEX
    {IDM_URLBACK,     CmdUrlBack},
#endif
    {ID_OPTIONS_WINDVI,    CmdWindviConfig},
    {ID_OPTIONS_TEXCONFIG, CmdTexConfig},

    {IDM_HELPTOPICS,  CmdHelpTopics},
    /*    {IDM_HELPCONTENTS, CmdHelpContents}, */
    /*     {IDM_HELPSEARCH,  CmdHelpSearch}, */
    /*     {IDM_HELPHELP,    CmdHelpHelp}, */
    {IDM_VIEW_LOG,    CmdViewLog},
    {IDM_ABOUT,       CmdAbout},
};

CMDI cmdiMain =
{
    sizeof(rgcmd) / sizeof(CMD),
    rgcmd,
    edwpWindow
};

/*****************************************************************************
  Toolbar window 
  ****************************************************************************/
HWND hWndToolbar;

int bVertToolbar = 0;
/*
**TODO**  Change the following values to match your toolbar bitmap

NUMIMAGES    = Number of images in toolbar.bmp.  Note that this is not
the same as the number of elements on the toolbar.
IMAGEWIDTH   = Width of a single button image in toolbar.bmp
IMAGEHEIGHT  = Height of a single button image in toolbar.bmp
BUTTONWIDTH  = Width of a button on the toolbar (zero = default)
BUTTONHEIGHT = Height of a button on the toolbar (zero = default)
*/

#define NUMIMAGES       17

#define IMAGEWIDTH      18
#define IMAGEHEIGHT     17
#define BUTTONWIDTH     0
#define BUTTONHEIGHT    0

/*
**TODO**  Add/remove entries in the following array to define the 
toolbar buttons (see documentation for TBBUTTON).
*/

TBBUTTON tbButton[] =
{
    {0, IDM_FILEOPEN,   TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {1, IDM_FILEPRINT,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0},
    {2, IDM_ZOOMIN,     TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {3, IDM_ZOOMOUT,    TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0},
    {4, IDM_PREVIOUS10, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {6, IDM_PREVIOUS5,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {8, IDM_PREVIOUSPAGE,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {11, IDM_REDRAWPAGE,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {9, IDM_NEXTPAGE,   TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {7, IDM_NEXT5,      TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {5, IDM_NEXT10,     TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0},
    {13,IDM_GOTOPAGE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {16,IDM_SRCSPECIALS, 
#ifdef SRC_SPECIALS
	 TBSTATE_ENABLED,
#else
	 TBSTATE_HIDDEN,
#endif
	 TBSTYLE_BUTTON | TBSTYLE_CHECK, 0, 0},
    {0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0},
    {10,IDM_TOGGLEGRID, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {12,IDM_TOGGLEPS, 
#ifdef PS_GS
	 TBSTATE_ENABLED,
#else
	 TBSTATE_HIDDEN,
#endif
	 TBSTYLE_BUTTON | TBSTYLE_CHECK, 0, 0},
    {15,IDM_BOOKMODE, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK, 0, 0},
    {0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0},
    {14,IDM_URLBACK, 
#ifdef HTEX
	 TBSTATE_ENABLED,
#else
	 TBSTATE_HIDDEN,
#endif
	 TBSTYLE_BUTTON, 0, 0},
};

/****************************************************************************
 Status Bar Window
 ****************************************************************************/
HWND hWndStatusbar;

/*  **TODO**  Add entries to this array for each popup menu in the same
              positions as they appear in the main menu.  Remember to define
              the ID's in globals.h and add the strings to windvi.rc. */

UINT idPopup[] =
{
  IDS_FILEMENU,
  IDS_MOVEMENU,
  IDS_VIEWMENU,
  IDS_HELPMENU,
};

/*
  6 -> 123,123
  5 -> Cursor Pos:
  4 -> 999
  3 -> Scaling:
  2 -> 999999
  1 -> Page:
  */

/*****************************************************************************/

/*
  FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

  PURPOSE: Entry point for the application.

  COMMENTS:

 This function initializes the application and processes the
 message loop.
*/
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
  MSG msg;
  HANDLE hAccelTable;

  /* Initialize global strings */
  lstrcpy (szAppName, SZAPPNAME);
  LoadString (hInstance, IDS_APPNAME, szTitle, 100);

  /* Setup standard io, real-time logging ... */
  SetupEnv();

  /* are we nt, w9x ? */
  iSystemType = GetSystemType();

  /* Get all information from command line */
  ParseCmdLine(GetCommandLine());

  if (!hPrevInstance) {
    /* Perform instance initialization: */
      if (!InitApplication(hInstance)) {
	CleanUp();
	return (FALSE);
      }
  }

  /* Perform application initialization: */
  if (!InitInstance(hInstance, nCmdShow)) {
    CleanUp();
    return (FALSE);
  }

#if 0
  hAccelTable = LoadAccelerators (hInstance, szAppName);
#endif

  bInitComplete = TRUE;
  SetForegroundWindow(hWndMain);
  /* Main message loop: */
  while (GetMessage(&msg, NULL, 0, 0)) {
#if 0
    if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg)) {
#endif
      TranslateMessage(&msg);
      DispatchMessage(&msg);
#if 0
    }
#endif
  }
  CleanUp();
#if 0
    DestroyWindow(hWndMain);
#endif
  return (msg.wParam);
}

/*
  FUNCTION: SetupEnv()
  PURPOSE: initializing stdi, stdout, stderr, redirecting output to the
           view log window in real-time.
*/
void SetupEnv()
{
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
  HANDLE current_pid, hTmpOut;
  DWORD idLogThread;
  FILE *hf;
  int fdIn, fdOut, fdErr;
  int i;
  extern char tick_tmp[];

  /* if _DEBUG is not defined, these macros will result in nothing. */
   SETUP_CRTDBG;
   /* Set the debug-heap flag so that freed blocks are kept on the
    linked list, to catch any inadvertent use of freed memory */
   /*   SET_CRT_DEBUG_FIELD( _CRTDBG_DELAY_FREE_MEM_DF );
	SET_CRT_DEBUG_FIELD( _CRTDBG_CHECK_ALWAYS_DF ); */

#if 0
  /* maybe we could still have an option to write log to some temp file ? */
  szLogFileName = xmalloc(260);
  if (GetTempFileName(szTempPath, "xdvi", 0, szLogFileName) == 0)
    Win32Error("GetTempFileName");
#endif

  if (GetTempPath(PATH_MAX, tick_tmp) == 0)
    Win32Error("GetTempPath/tick_tmp");

  hCrtIn = CreateFile("NUL", GENERIC_READ, FILE_SHARE_READ, 
		      &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (SetStdHandle(STD_INPUT_HANDLE, hCrtIn) == FALSE)
    Win32Error("SetStdHandle/hCrtIn");

  if (CreatePipe(&hLogIn, &hTmpOut, &sa, 0) == FALSE) {
    Win32Error("Init/CreatePipe");
  }

  current_pid = GetCurrentProcess();

  if (DuplicateHandle(current_pid, hTmpOut, 
		      current_pid, &hCrtOut, 
		      0, TRUE, DUPLICATE_SAME_ACCESS) == FALSE)
    Win32Error("DuplicateHandle/OutErr");

  if (DuplicateHandle(current_pid, hTmpOut, 
		      current_pid, &hCrtErr, 
		      0, TRUE, DUPLICATE_SAME_ACCESS) == FALSE)
    Win32Error("DuplicateHandle/OutErr");


  CloseHandleAndClear(&hTmpOut);

  if (SetStdHandle(STD_OUTPUT_HANDLE, hCrtOut) == FALSE)
    Win32Error("SetStdHandle/hCrtOut");

  if (SetStdHandle(STD_ERROR_HANDLE, hCrtErr) == FALSE)
    Win32Error("SetStdHandle/hCrtErr");

  fdIn = _open_osfhandle((long) hCrtIn, 
			 // (long) GetStdHandle(STD_INPUT_HANDLE),
			 _O_TEXT
			 );
  hf = _fdopen( fdIn, "r" );
  *stdin = *hf;
  i = setvbuf( stdin, NULL, _IONBF, 0 );

  fdOut = _open_osfhandle((long) hCrtOut, 
			  // (long) GetStdHandle(STD_OUTPUT_HANDLE),
			  _O_TEXT
			  );
  hf = _fdopen( fdOut, "w" );
  *stdout = *hf;
  i = setvbuf( stdout, NULL, _IONBF, 0 );

  fdErr = _open_osfhandle((long) hCrtErr,
			  // (long) GetStdHandle(STD_ERROR_HANDLE),
			  _O_TEXT
			  );
  hf = _fdopen( fdErr, "w" );
  *stderr = *hf;
  i = setvbuf( stderr, NULL, _IONBF, 0 );

  /* Run a thread for the ViewLog dialog box. The thread will
   wait for something to read on hLogIn. */
  if ((hViewLogThread = CreateThread(&sa, /* security attributes */
				     0,	/* default stack size */
				     ViewLogSentinel, /* start address of thread */
				     0,	/* parameter */
				     0,	/* creation flags */
				     &idLogThread /* thread id */
				     )) == NULL)
    Win32Error("Log/CreateThread");
#if 0
  fprintf(stderr, "Setupenv done\n");
#endif
}

/*
  FUNCTION: InitApplication(HANDLE)

  PURPOSE: Initializes window data and registers window class

  COMMENTS:

       In this function, we initialize a window class by filling out a data
       structure of type WNDCLASS and calling either RegisterClass or
       the internal MyRegisterClass.
*/
BOOL InitApplication(HINSTANCE hInstance)
{
  WNDCLASSEX  wc;
  HWND      hWndPrev;
  HANDLE    hMutex;

#if 0
  /* Win32 will always set hPrevInstance to NULL, so lets check
     things a little closer. This is because we only want a single
     version of this app to run at a time */
  hwnd = FindWindow (szAppName, szTitle);
  if (hwnd) {
    /* We found another version of ourself. Lets defer to it: */
    if (IsIconic(hwnd)) {
      ShowWindow(hwnd, SW_RESTORE);
    }
    SetForegroundWindow (hwnd);

    /* If this app actually had any functionality, we would
       also want to communicate any action that our 'twin'
       should now perform based on how the user tried to
       execute us. */
    return FALSE;
  }
#endif

  hWndPrev = FindWindow(szAppName, NULL);
  if (hWndPrev) {
    GetWindowRect(hWndPrev, &rectWndPrev);
    bPrevInstance = TRUE;
  }

  /* hPrevInstance is always NULL under Win32. We have been asked to run only one instance
   of windvi. Next instance will have to wake up the first one, and send it :
  - at least new working directory and file name
  - other params ? With the restriction that we cannot send pointers. */
  if (resource.single_flag) {
    COPYDATASTRUCT CopyData;
    SECURITY_ATTRIBUTES sa =  { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; 
    struct data_exchange DataEx;

    /* We create a mutex to be sure to be alone, but only if the option
       single_instance has been given. */

    if ((hMutex = CreateMutex(&sa, TRUE, "WindviMutex")) == NULL) {
      Win32Error("WinMain/CreateMutex");
      return FALSE;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
      /* send the right message to the existing instance */
      if (!hWndPrev) {
		MessageBox(hWndMain, "Can't find the previous instance.", NULL, MB_APPLMODAL | MB_ICONHAND | MB_OK);
		return FALSE;
      }
      /* We found another version of ourself. Lets defer to it: */
      if (IsIconic(hWndPrev)) {
	ShowWindow(hWndPrev, SW_RESTORE);
      }
      if (!SetForegroundWindow(hWndPrev))
	Win32Error("WinMain/SetForegroundWindow");
      /* We send the data_exchange structure ! */
      GetCurrentDirectory(sizeof(DataEx.cwd), DataEx.cwd);
      lstrcpy(DataEx.dviname, dvi_name);

      if (curr_page) {
	current_page = (*curr_page ? atoi(curr_page) : 1) - 1;
      }

      DataEx.currentpage = current_page;
      DataEx.shrinkfactor = mane.shrinkfactor;
      CopyData.dwData = 0;
      CopyData.cbData = sizeof(DataEx);
      CopyData.lpData = &DataEx;
      SendMessage(hWndPrev, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&CopyData);
      /* and exit */
      return FALSE;
    }
  }


  /* Fill in window class structure with parameters that describe
     the main window. */
  /* CS_OWNDC : un DC pour chaque fenêtre de la classe */
  wc.cbSize        = sizeof(WNDCLASSEX);
  wc.lpszClassName = szAppName;
  wc.hInstance     = hInstance;
  wc.lpfnWndProc   = (WNDPROC)WndProc;
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_APPICON));
  wc.lpszMenuName  = szAppName;
  wc.style         = CS_OWNDC;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
  wc.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
  /*  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);*/


  if (!RegisterClassEx(&wc)) {
    /* Assume we are running on NT where RegisterClassEx() is
       not implemented, so let's try calling RegisterClass(). */
    
    if (!RegisterClass((LPWNDCLASS)&wc.style)) {
      Win32Error("RegisterClassEx");
      return FALSE;
    }
  }

  wc.lpszClassName = "ClientDrawClass";
  wc.hInstance     = hInstance;
  wc.lpfnWndProc   = (WNDPROC)DrawProc;
  wc.hCursor       = NULL;	/* Different cursors may be loaded */
  wc.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_APPICON));
  wc.lpszMenuName  = NULL;
  wc.hbrBackground = GetStockObject(LTGRAY_BRUSH);
  wc.style         = CS_OWNDC /* 0| CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT */;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_APPICON));
  

  if (!RegisterClassEx(&wc)) {
    /* Assume we are running on NT where RegisterClassEx() is
       not implemented, so let's try calling RegisterClass(). */
    
    if (!RegisterClass((LPWNDCLASS)&wc.style)) {
      Win32Error("RegisterClassEx");
      return FALSE;
    }
  }

  wc.lpszClassName = "MagnifyGlass";
  wc.hInstance     = hInstance;
  wc.lpfnWndProc   = (WNDPROC)MagnifyProc;
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_APPICON));
  wc.lpszMenuName  = NULL;
  wc.hbrBackground = GetStockObject(LTGRAY_BRUSH);
  wc.style         = CS_OWNDC /* | CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT */;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_APPICON));
  

  if (!RegisterClassEx(&wc)) {
    /* Assume we are running on NT where RegisterClassEx() is
       not implemented, so let's try calling RegisterClass(). */
    
    if (!RegisterClass((LPWNDCLASS)&wc.style)) {
      Win32Error("RegisterClassEx");
      return FALSE;
    }
  }

  return TRUE;
}

/*
  FUNCTION: InitInstance(HANDLE, int)
  
  PURPOSE: Saves instance handle and creates main window
  
  COMMENTS:
  
  In this function, we save the instance handle in a global variable and
  create and display the main program window.
  */
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  extern void UpdateMainMenuUsedFiles(void);
  hInst = hInstance; /* Store instance handle in our global variable */

  magDIB = maneDIB = NULL;
  
  /* Setup bswap function */
  bswap = (check_386() ? bswap_c : bswap_asm);
  
  /* setup units */
  pixel_to_unit();
  
#ifdef _TRACE
  fprintf(stderr, "w = %d, h = %d\n", page_w, page_h);
#endif

  hCursWait = LoadCursor(NULL, IDC_WAIT);
  hCursArrow = LoadCursor(NULL, IDC_ARROW);
  hCursCross = LoadCursor(NULL, IDC_CROSS);
/* BEGIN CHUNK xdvi.c 2 */
#ifdef SRC_SPECIALS
  hCursSrc = LoadCursor(NULL, IDC_IBEAM);
#endif
/* END CHUNK xdvi.c 2 */

  hWndMain = CreateWindowEx(WS_EX_ACCEPTFILES, szAppName, szTitle, 
			    WS_OVERLAPPEDWINDOW /*| WS_CLIPCHILDREN */,
			    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
			    NULL, NULL, hInstance, NULL);

  if (!hWndMain) {
    Win32Error("CreateWindow");
    return (FALSE);
  }
  if (!CreateTBar(hWndMain))
    return FALSE;
  if (!CreateSBar(hWndMain))
    return FALSE;
  if (!CreateDraw(hWndMain))
    return FALSE;
  if (!CreateMagnify(hWndDraw))
    return FALSE;

  hMenuMain = GetMenu(hWndMain);
  CheckMenuItem(hMenuMain, IDM_TOGGLEPS, (resource._postscript ? MF_CHECKED : MF_UNCHECKED));
  CheckMenuItem(hMenuMain, IDM_TOGGLEGRID, (resource._grid_mode ? MF_CHECKED : MF_UNCHECKED));
  CheckMenuItem(hMenuMain, IDM_KEEPPOS, (resource.keep_flag ? MF_CHECKED : MF_UNCHECKED));

  UpdateMainMenuUsedFiles();

  ShowWindow(hWndMain, nCmdShow);
  return (TRUE);
}

/*
  FUNCTION: DispMessage(LPMSDI, HWND, UINT, WPARAM, LPARAM)
  
  PURPOSE: Call the function associated with a message.
  
  PARAMETERS:
  lpmsdi - Structure containing the message dispatch information.
  hwnd - The window handle
  uMessage - The message number
  wparam - Message specific data
  lparam - Message specific data
  
  RETURN VALUE:
  The value returned by the message function that was called.
  
  COMMENTS:
  Runs the table of messages stored in lpmsdi->rgmsd searching
  for a message number that matches uMessage.  If a match is found,
  call the associated function.  Otherwise, call DispDefault to
  call the default function, if any, associated with the message
  structure.  In either case, return the value recieved from the
  message or default function.
  */
  
LRESULT DispMessage(LPMSDI lpmsdi, 
                    HWND   hwnd, 
                    UINT   uMessage, 
                    WPARAM wparam, 
                    LPARAM lparam)
{
    int  imsd = 0;

    MSD *rgmsd = lpmsdi->rgmsd;
    int  cmsd  = lpmsdi->cmsd;

    for (imsd = 0; imsd < cmsd; imsd++)
    {
        if (rgmsd[imsd].uMessage == uMessage)
            return rgmsd[imsd].pfnmsg(hwnd, uMessage, wparam, lparam);
    }

    return DispDefault(lpmsdi->edwp, hwnd, uMessage, wparam, lparam);
}

/*
  FUNCTION: DispCommand(LPCMDI, HWND, WPARAM, LPARAM)
  
  PURPOSE: Call the function associated with a command.
  
  PARAMETERS:
  lpcmdi - Structure containing the command dispatch information.
  hwnd - The window handle
  GET_WM_COMMAND_ID(wparam, lparam) - Identifier of the menu item,
  control, or accelerator.
  GET_WM_COMMAND_CMD(wparam, lparam) - Notification code.
  GET_WM_COMMAND_HWND(wparam, lparam) - The control handle or NULL.
  
  RETURN VALUE:
  The value returned by the command function that was called.
  
  COMMENTS:
  Runs the table of commands stored in lpcmdi->rgcmd searching
  for a command number that matches wCommand.  If a match is found,
  call the associated function.  Otherwise, call DispDefault to
  call the default function, if any, associated with the command
  structure.  In either case, return the value recieved from the
  command or default function.
  */


LRESULT DispCommand(LPCMDI lpcmdi, 
                    HWND   hwnd, 
                    WPARAM wparam, 
                    LPARAM lparam)
{
  LRESULT lRet = 0;
  WORD    wCommand = GET_WM_COMMAND_ID(wparam, lparam);
  int     icmd;
  
  CMD    *rgcmd = lpcmdi->rgcmd;
  int     ccmd  = lpcmdi->ccmd;
  
  /*  Message packing of wparam and lparam have changed for Win32,
      so use the GET_WM_COMMAND macro to unpack the commnad */
  
  for (icmd = 0; icmd < ccmd; icmd++) {
    if (rgcmd[icmd].wCommand == wCommand) {
      return rgcmd[icmd].pfncmd(hwnd,
				wCommand,
				GET_WM_COMMAND_CMD(wparam, lparam),
				GET_WM_COMMAND_HWND(wparam, lparam));
    }
  }
  
  return DispDefault(lpcmdi->edwp, hwnd, WM_COMMAND, wparam, lparam);
}



/*
  FUNCTION: DispDefault(EDWP, HWND, UINT, WPARAM, LPARAM)
  
  PURPOSE: Call the appropriate default window procedure.
  
  PARAMETERS:
  edwp - Enumerate specifying the appropriate default winow procedure.
  hwnd - The window handle
  uMessage - The message number
  wparam - Message specific data
  lparam - Message specific data
  
  RETURN VALUE:
  If there is a default proc, return the value returned by the
  default proc.  Otherwise, return 0.
  
  COMMENTS:
  Calls the default procedure associated with edwp using the specified
  parameters.
  */

LRESULT DispDefault(EDWP   edwp, 
                    HWND   hwnd, 
                    UINT   uMessage, 
                    WPARAM wparam, 
                    LPARAM lparam)
{
    switch (edwp)
    {
        case edwpNone:
            return 0;
        case edwpWindow:
            return DefWindowProc(hwnd, uMessage, wparam, lparam);
        case edwpDialog:
            return DefDlgProc(hwnd, uMessage, wparam, lparam);
        case edwpMDIFrame:
            return DefFrameProc(hwnd, NULL, uMessage, wparam, lparam);
        case edwpMDIChild:
            return DefMDIChildProc(hwnd, uMessage, wparam, lparam);
    }
    return 0;
}

/*****************************************************************************
  Main Window messages
  ****************************************************************************/

/*
  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
  
  PURPOSE:  Processes messages for the main window.
  
  PARAMETERS:
  hwnd     - window handle
  uMessage - message number
  wparam   - additional information (dependant on message number)
  lparam   - additional information (dependant on message number)
  
  RETURN VALUE:
  The return value depends on the message number.  If the message
  is implemented in the message dispatch table, the return value is
  the value returned by the message handling function.  Otherwise,
  the return value is the value returned by the default window procedure.
  
  COMMENTS:
  Call the DispMessage() function with the main window's message dispatch
  information (msdiMain) and the message specific information.
  */

LRESULT CALLBACK WndProc(HWND   hwnd, 
                         UINT   uMessage, 
                         WPARAM wparam, 
                         LPARAM lparam)
{
    return DispMessage(&msdiMain, hwnd, uMessage, wparam, lparam);
}

/*
  FUNCTION: MsgCommand(HWND, UINT, WPARAM, LPARAM)
  
  PURPOSE: Handle the WM_COMMAND messages for the main window.
  
  PARAMETERS:
  hwnd     - window handle
  uMessage - WM_COMMAND (Unused)
  GET_WM_COMMAND_ID(wparam, lparam)   - Command identifier
  GET_WM_COMMAND_HWND(wparam, lparam) - Control handle
  
  RETURN VALUE:
  The return value depends on the message number.  If the message
  is implemented in the message dispatch table, the return value is
  the value returned by the message handling function.  Otherwise,
  the return value is the value returned by the default window procedure.
  
  COMMENTS:
  Call the DispCommand() function with the main window's command dispatch
  information (cmdiMain) and the command specific information.
  */

LRESULT MsgCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispCommand(&cmdiMain, hwnd, wparam, lparam);
}


/*
  FUNCTION: MsgCreate(HWND, UINT, WPARAM, LPARAM)
  
  PURPOSE: Handle the WM_CREATE messages for the main window.
  and call InitCommonControls() API to initialize the
  common control library. 
  
  PARAMETERS:
  hwnd     - window handle
  
  RETURN VALUE:
  Return 0 if the StatusBar and ToolBar Windows could be created
  successfully. Otherwise, returns -1 to abort the main window
  creation.
  
  COMMENTS:
  Call the CreateTSBars function with the main window's window handle
  information (msdiMain). 
  */  
  
LRESULT MsgCreate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  int nRet = -1;
  FARPROC icce;

  top_level = hwnd;

  if (!(icce = GetProcAddress(GetModuleHandle("comctl32.dll"),
			      "InitCommonControlsEx"))) {
#if 0
    fprintf(stderr, "Not running the latest comctl32.dll\n");
#endif
    bVertToolbar = 0;
    InitCommonControls() ; /*  Initialize the common control library. */
  }
  else {
    INITCOMMONCONTROLSEX icc = { sizeof(INITCOMMONCONTROLSEX),
				 ICC_BAR_CLASSES };
    if ((*icce)(&icc) == FALSE)
      Win32Error("InitCommonControlsEx");;
    bVertToolbar = 1;
  }

   DragAcceptFiles(hwnd, TRUE);

   return 0;
}

void UpdateGeometry()
{
  RECT r;
  char buf[256];
  
  /* Avoid updating when in iconic form */
  if (IsIconic(hWndMain)) return;

  /* Update the geometry string */
  GetWindowRect(hWndMain, &r);

  sprintf(buf, "%ux%u%+d%+d", 
	  r.right - r.left, 
	  r.bottom - r.top,
	  /* be safe, do not allow huge numbers */
	  (r.left < 0 ? 0 : r.left) % maneHorzRes,	
	  (r.top < 0 ? 0 : r.top) % maneVertRes
	  );
  if (geometry) free(geometry);
  geometry = strdup(buf);
}

/*
  FUNCTION: MsgSize(HWND, UINT, WPARAM, LPARAM)
  
  PURPOSE:  This function resizes the toolbar and statusbar controls. 
  
  
  PARAMETERS:
  
  hwnd      - Window handle  (Used)
  uMessage  - Message number (Used)
  wparam    - Extra data     (Used)
  lparam    - Extra data     (Used)
  
  RETURN VALUE:
  
  Always returns 0 - Message handled
  
  COMMENTS:
  
  When the window procdure that has the status and tool bar controls
  receive the WM_SIZE message, it has to pass the message on to these 
  controls so that these controls can adjust their size accordingly. 
  */
  

LRESULT MsgSize(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  fSize = TRUE;

  SendMessage(hWndStatusbar,  uMessage, wparam, lparam);
  SendMessage(hWndToolbar, uMessage, wparam, lparam);

  /*  Re-position the panes in the status bar */
  InitializeStatusBar(hwnd);

  /* Re-size client window relative to the tool/status bars */
  if (wparam != SIZE_MINIMIZED)
    SizeClientWindow(hwnd);

  UpdateGeometry();

  return 0;
}
/*
  We need to update geometry in case of moving.
  */
LRESULT MsgMove(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  UpdateGeometry();
  return 0;
}

void SetScrollBars(HWND hwnd)
{
  RECT r;

  GetClientRect(hwnd, &r);

  /* Scrollbars */
  xMinScroll = 0;
  xMaxScroll = max((unsigned)page_w, r.right) - 1;
  /* mane.base_x =  */xCurrentScroll = min(xCurrentScroll, xMaxScroll - max(r.right-1, 0));

  si.cbSize = sizeof(si);
  si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
  si.nMin = xMinScroll;
  si.nMax = xMaxScroll;
  si.nPage = r.right;
  si.nPos = xCurrentScroll;
  SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

  yMinScroll = 0;
  yMaxScroll = max((unsigned)page_h, r.bottom) - 1;
  /* mane.base_y =  */yCurrentScroll = min(yCurrentScroll, yMaxScroll - max(r.bottom-1, 0));

  si.cbSize = sizeof(si);
  si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
  si.nMin = yMinScroll;
  si.nMax = yMaxScroll;
  si.nPage = r.bottom;
  si.nPos = yCurrentScroll;
  SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

#ifdef _TRACE
  fprintf(stderr, "Page %d x %d\nScrollbars to [%d, %d, %d] and [%d, %d, %d]\n",
	  r.right, r.bottom,
	  xMinScroll, xCurrentScroll, xMaxScroll,
	  yMinScroll, yCurrentScroll, yMaxScroll);
#endif	  
}

void SizeClientWindow(HWND hwnd)
{
  int xNewSize, yNewSize;
  POINT ptOldOrg;
  int xorg, yorg;
  RECT r;
  int parentWidth, parentHeight;
  /* Re-position the child window */

  GetClientRect(hwnd, &r);
  parentHeight = r.bottom;
  parentWidth = r.right;
  GetClientRect(hWndToolbar, &r);
  tbHeight = r.bottom;
  GetClientRect(hWndStatusbar, &r);
  sbHeight = r.bottom;

  maneRect.left = 0;
  maneRect.right = parentWidth;
  maneRect.top = 0;
  maneRect.bottom = parentHeight - sbHeight -tbHeight - 1;

  xNewSize = parentWidth;
  yNewSize = parentHeight - sbHeight -tbHeight;

  /* FIXME: try to SetWindowPos() instead */
  SetWindowPos(hWndDraw, HWND_TOPMOST, 0, tbHeight+1,
	       xNewSize, yNewSize,
	       SWP_NOACTIVATE |  SWP_DEFERERASE | SWP_SHOWWINDOW
	       | SWP_NOOWNERZORDER | SWP_NOZORDER);
  
  GetClientRect(hWndDraw, &r);

  maneWidth = r.right;
  maneHeight = r.bottom;

  SetScrollBars(hWndDraw);

#if 1
/*   GdiFlush(); */
/*   UpdateWindow(hWndDraw); */
  /* FIXME : adjust the position of the page */
  if (maneDrawDC && hWndDraw && bInitComplete) {
	GetWindowOrgEx(maneDrawDC, &ptOldOrg);
	if (mane.win == hWndDraw && GetClientRect(hWndDraw, &maneRect)) {
	  int xorg = (int)(page_w - maneRect.right)/2;
	  int yorg = (int)(page_h - maneRect.bottom)/2;
	  xorg = min(0, xorg);
	  yorg = min(0, yorg);
#if 0
	  fprintf(stderr, "Old org (%ld, %ld) New org (%ld, %ld) page %d %d rect %d %d\n", 
			  ptOldOrg.x, ptOldOrg.y,
			  xorg, yorg, page_w, page_h, maneRect.right, maneRect.bottom);
#endif
	  ScrollWindowEx(hWndDraw, 
					 - xorg + ptOldOrg.x, - yorg + ptOldOrg.y,
					 NULL, NULL, (HRGN)NULL, (LPRECT)NULL,
					 SW_INVALIDATE | SW_ERASE); 
/* 	  fprintf(stderr, "scrolled window by %d, %d\n",  */
/* 			  - xorg + ptOldOrg.x, - yorg + ptOldOrg.y); */
/* 	  GdiFlush(); */
/* 	  UpdateWindow(hWndDraw); */
	  if (SetWindowOrgEx(maneDrawDC, xorg, yorg, NULL) == 0) {
		Win32Error("MsgDrawPaint/SetWindowOrgEx(x,y)");
	  }
/* 	  fprintf(stderr, "Set new org @ (%ld, %ld)\n", xorg, yorg); */
/* 	  GdiFlush(); */
/* 	  UpdateWindow(hWndDraw); */
	}
	else {
	  Win32Error("MsgDrawPaint/GetClientRect(hwnd)");
	}
  }
#endif
}

/*
  FUNCTION: MsgDestroy(HWND, UINT, WPARAM, LPARAM)
  
  PURPOSE: Calls PostQuitMessage().
  
  PARAMETERS:
  
  hwnd      - Window handle  (Unused)
  uMessage  - Message number (Unused)
  wparam    - Extra data     (Unused)
  lparam    - Extra data     (Unused)
  
  RETURN VALUE:
  
  Always returns 0 - Message handled
  
  COMMENTS:
  */


LRESULT MsgDestroy(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  DragAcceptFiles(hwnd, FALSE); 
  SaveOptions();

  PostQuitMessage(0);
  return 0;
}

/*
  FUNCTION: CmdExit(HWND, WORD, WORD, HWND)
  
  PURPOSE: Exit the application.
  
  PARAMETERS:
  hwnd     - The window.
  wCommand - IDM_EXIT (unused)
  wNotify  - Notification number (unused)
  hwndCtrl - NULL (unused)
  
  RETURN VALUE:
  Always returns 0 - command handled.
  
  COMMENTS:
  */


LRESULT CmdExit(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  DragAcceptFiles(hwnd, FALSE); 
  SaveOptions();

  PostQuitMessage(0);
  return 0;
}

/*
  Status Bar functions
  */

LRESULT MsgDropFiles(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  WORD cFiles; 
  char lpszFile[MAX_PATH]; 

  cFiles = DragQueryFile((HANDLE) wparam, 0xFFFFFFFF, (LPSTR) NULL, 0);
  if (cFiles != 1) {
    MessageBox(hWndMain, "Only one dvi file open.", NULL, MB_APPLMODAL | MB_ICONHAND | MB_OK);
  }
  DragQueryFile((HANDLE) wparam, 0, lpszFile, sizeof(lpszFile)); 
  dvi_name = xstrdup(lpszFile);
  DragFinish((HANDLE) wparam); 

  open_dvi_file();
  if (reconfig() == FALSE) {
    char buf[40];
    mane.shrinkfactor +=3;
    wsprintf(buf, "New shrink factor : %d\n", mane.shrinkfactor);
    UpdateStatusBar(buf, 0, 0);
    if (reconfig() == FALSE) {
      MessageBox(hWndMain, "Can't allocate page bitmap !\r\nPlease report this error.", NULL, MB_APPLMODAL | MB_ICONERROR | MB_OK);
      CleanExit(1);
      }
  }
  redraw_page();
  SetForegroundWindow(hwnd);

  return 0;

}
#if 0
/*
    FUNCTION: MsgTimer(HWND, UINT, WPARAM, LPARAM)
  
    PURPOSE: Calls GetLocalTime() to set the time on the status bar
  
  
    PARAMETERS:
  
      hwnd      - Window handle  (Unused)
      uMessage  - Message number (Unused)
      wparam    - Extra data     (Unused)
      lparam    - Extra data     (Unused)
  
    RETURN VALUE:
  
      Always returns 0 - Message handled
  
    COMMENTS:
  
      Every time the window procedure receives a Timer message, it calls
      GetLocalTime() to obtain the time and then formats the time into
      a string. The time sting is then displayed on the status bar.
      */  

LRESULT MsgTimer(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    char        szBuf[16];         /* Temp buffer. */
    SYSTEMTIME  sysTime;


    GetLocalTime(&sysTime);

    wsprintf(szBuf,
             "%2d:%02d:%02d %s",
             (sysTime.wHour == 0 ? 12 :
             (sysTime.wHour <= 12 ? sysTime.wHour : sysTime.wHour -12)),
             sysTime.wMinute,
             sysTime.wSecond,
             (sysTime.wHour < 12 ? "AM":"PM"));

    UpdateStatusBar(szBuf, 8, 0);
    return 0;
}

/*  
    FUNCTION: MsgMousemove(HWND, UINT, WPARAM, LPARAM)
  
    PURPOSE:  Obtains the Cursor position to display coordinates on
              the status bar.
  
    PARAMETERS:
  
      hwnd      - Window handle  (Unused)
      uMessage  - Message number (Unused)
      wparam    - Extra data     (Unused)
      lparam    - Extra data     (Used)
  
    RETURN VALUE:
  
      Always returns 0 - Message handled
  
    COMMENTS:
  
      The mouse coordinates (x and y) are in the HI And LO words
      of LPARAM
      */  
  

LRESULT MsgMousemove(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    char szBuf[20];		/* Array for formatting mouse coordinates */

    wsprintf(szBuf, "%d,%d", LOWORD(lparam), HIWORD(lparam));
    UpdateStatusBar(szBuf, 6, 0);
    return 0;
}
#endif

/*  
    FUNCTION: MsgMenuSelect(HWND, UINT, WPARAM, LPARAM)
  
    PURPOSE:  Upadates menu selections on the status bar.
  
  
    PARAMETERS:
  
      hwnd      - Window handle  (Used)
      uMessage  - Message number (Used)
      wparam    - Extra data     (Used)
      lparam    - Extra data     (Used)
  
    RETURN VALUE:
  
      Always returns 0 - Message handled
  
    COMMENTS:
      This message is sent when the user selects menu items by
      by pulling down  a popup menu move the mouse around to highlite
      different menu items.
      */  

LRESULT MsgMenuSelect(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    static char szBuffer[128];
    UINT   nStringID = 0;
    UINT   fuFlags = GET_WM_MENUSELECT_FLAGS(wparam, lparam) & 0xffff;
    UINT   uCmd    = GET_WM_MENUSELECT_CMD(wparam, lparam);
    HMENU  hMenu   = GET_WM_MENUSELECT_HMENU(wparam, lparam);


    szBuffer[0] = 0;		/* First reset the buffer */


    if (fuFlags == 0xffff && hMenu == NULL) /* Menu has been closed */
        nStringID = IDS_DESCRIPTION;

    else if (fuFlags & MFT_SEPARATOR) /* Ignore separators */
        nStringID = 0;

    else if (fuFlags & MF_POPUP) /* Popup menu */
    {
        if (fuFlags & MF_SYSMENU) /* System menu */
            nStringID = IDS_SYSMENU;

        else
	  /* Get string ID for popup menu from idPopup array. */
            nStringID = ((uCmd < sizeof(idPopup)/sizeof(idPopup[0])) ?
                            idPopup[uCmd] : 0);
    }   /*  for MF_POPUP */

    else			/* Must be a command item */
        nStringID = uCmd;	/* String ID == Command ID */

    /* Load the string if we have an ID */
    if (0 != nStringID)
        LoadString(hInst, nStringID, szBuffer, sizeof(szBuffer));

    /* Finally... send the string to the status bar */
    UpdateStatusBar(szBuffer, 0, 0);

    return 0;
}


/*  
    FUNCTION: InitializeStatusBar(HWND)
  
    PURPOSE:  Initialize statusbar control with time and mouse positions.
  
  
    PARAMETERS:
  
    hwndParent - Window handle of the status bar's parent
  
  
    RETURN VALUE:  NONE
  
  
    COMMENTS:
  
     This function initializes the time  and mouse positions sections of
     the statubar window. The Date for the time section is obtained by
     calling SetTimer API. When the timer messages start comming in,
     GetSytemTime() to fill the time section.
     The WPARAM of SB_SETTEXT is divided into 2 parameters. The LOWORD
     determines which section/part the text goes into, and the HIWORD
     tells how the bar is drawn (popin or popout).
     */

void InitializeStatusBar(HWND hwndParent)
{
    const cSpaceInBetween = 8;
    char szBuf[20];
    int   ptArray[7];      /* Array defining the number of parts/sections */
    SIZE  size;            /* the Status bar will display. */
    RECT  rect;
    HDC   hDC;

   /*
    * Fill in the ptArray...
    */

    hDC = GetDC(hwndParent);
    GetClientRect(hwndParent, &rect);

    ptArray[6] = rect.right;

    if (GetTextExtentPoint(hDC, "999999.999xx x 999999.999xx", 27, &size))
        ptArray[5] = ptArray[6] - (size.cx) - cSpaceInBetween;
    else
        ptArray[5] = 0;

    if (GetTextExtentPoint(hDC, "Cursor Pos:", 12, &size))
        ptArray[4] = ptArray[5] - (size.cx) - cSpaceInBetween;
    else
        ptArray[4] = 0;

    if (GetTextExtentPoint(hDC, "999", 2, &size))
        ptArray[3] = ptArray[4] - (size.cx) - cSpaceInBetween;
    else
        ptArray[3] = 0;

    if (GetTextExtentPoint(hDC, "Scaling:", 9, &size))
        ptArray[2] = ptArray[3] - (size.cx) - cSpaceInBetween;
    else
        ptArray[2] = 0;

    if (GetTextExtentPoint(hDC, "99999 of 99999", 14, &size))
        ptArray[1] = ptArray[2] - (size.cx) - cSpaceInBetween;
    else
        ptArray[1] = 0;

    if (GetTextExtentPoint(hDC, "Page:", 6, &size))
        ptArray[0] = ptArray[1] - (size.cx) - cSpaceInBetween;
    else
        ptArray[0] = 0;

    ReleaseDC(hwndParent, hDC);

    SendMessage(hWndStatusbar,
                SB_SETPARTS,
                sizeof(ptArray)/sizeof(ptArray[0]),
                (LPARAM)(LPINT)ptArray);

    UpdateStatusBar(SZDESCRIPTION, 0, 0);
    UpdateStatusBar("Page:", 1, SBT_POPOUT);
    wsprintf(szBuf, "%5d of %5d", current_page+1, total_pages);
    UpdateStatusBar(szBuf, 2, 0);
    UpdateStatusBar("Page:", 1, SBT_POPOUT);
    UpdateStatusBar("Scaling:", 3, SBT_POPOUT);    
    wsprintf(szBuf, "%2d", mane.shrinkfactor);
    UpdateStatusBar(szBuf, 4, 0);
    UpdateStatusBar("Cursor Pos:", 5, SBT_POPOUT);
}

/*  
    FUNCTION: CreateSBar(HWND, UINT, WPARAM, LPARAM)
  
    PURPOSE:  Calls CreateStatusWindow() to create the status bar
  
  
    PARAMETERS:
  
    hwndParent - Window handle of the status bar's parent
  
    RETURN VALUE:
  
    If both controls were created successfully Return TRUE,
    else returns FALSE.
  
    COMMENTS:
    */  

BOOL CreateSBar(HWND hwndParent)
{
    hWndStatusbar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_BORDER,
                                       SZDESCRIPTION,
                                       hwndParent,
                                       IDM_STATUSBAR);
    if(hWndStatusbar)
    {
        InitializeStatusBar(hwndParent);
        return TRUE;
    }

    return FALSE;
}


/*  
    FUNCTION: UpdateStatusBar(HWND)
  
    PURPOSE:  Updates the statusbar control with appropriate text
  
  
    PARAMETERS:
  
    lpszStatusString - text to be displayed
    partNumber       - which part of the status bar to display text in
    displayFlags     - display flags
  
  
    RETURN VALUE: NONE
  
  
    COMMENTS:
        None
	*/  
  

void UpdateStatusBar(LPSTR lpszStatusString, WORD partNumber, WORD displayFlags)
{

    SendMessage(hWndStatusbar,
                SB_SETTEXT,
                partNumber | displayFlags,
                (LPARAM)lpszStatusString);
}

/*
  FUNCTION: CreateTBar(HWND)
  
  PURPOSE:  Calls CreateToolBarEx()
  
  
  PARAMETERS:

  hwnd - Window handle : Used for the hWndParent parameter of the control.
  
  RETURN VALUE:
  
  If toolbar control was created successfully Return TRUE,
  else returns FALSE.
  
  COMMENTS:
  */


BOOL CreateTBar(HWND hwnd)
{
  DWORD style = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS;

  hWndToolbar = CreateToolbarEx(hwnd,
				style,
				IDR_TOOLBAR1,
				NUMIMAGES,
				hInst,
				IDR_TOOLBAR1,
				tbButton,
				sizeof(tbButton)/sizeof(TBBUTTON),
				BUTTONWIDTH,
				BUTTONHEIGHT,
				IMAGEWIDTH,
				IMAGEHEIGHT,
				sizeof(TBBUTTON));
  if (hWndToolbar != NULL) {
    ShowWindow(hWndToolbar, SW_SHOW); 
    return TRUE;
  }

  return FALSE;
}

/*
  FUNCTION: MsgNotify(HWND, UINT, WPARAM, LPARAM)
  
  PURPOSE:  WM_NOTIFY is sent to the parent window to get the
  tooltip text assoc'd with that toolbar button.
  
  PARAMETERS:
  
  hwnd      - Window handle  (Unused)
  uMessage  - Message number (Unused)
  wparam    - Extra data     (Unused)
  lparam    - TOOLTIPTEXT FAR*
  
  RETURN VALUE:
  Always returns 0 - Message handled
  
  
  COMMENTS:
  This message fills in the lpszText field of the TOOLTIPTEXT
  structure if code == TTN_NEEDTEXT
  */
  
LRESULT MsgNotify(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    LPTOOLTIPTEXT lpToolTipText;
    static char   szBuffer[64];

    lpToolTipText = (LPTOOLTIPTEXT)lparam;
    if (lpToolTipText->hdr.code == TTN_NEEDTEXT)
    {
        LoadString(hInst,
                   lpToolTipText->hdr.idFrom,    /*  string ID == command ID */
                   szBuffer,
                   sizeof(szBuffer));

        lpToolTipText->lpszText = szBuffer;
    }
    return 0;
}



/*
  Miscellaneous functions
  */
/*
  FUNCTION: CenterWindow(HWND, HWND)
  
  PURPOSE:  Center one window over another.
  
  PARAMETERS:
  hwndChild - The handle of the window to be centered.
  hwndParent- The handle of the window to center on.
  
  RETURN VALUE:

  TRUE  - Success
  FALSE - Failure
  
  COMMENTS:
  
  Dialog boxes take on the screen position that they were designed
  at, which is not always appropriate. Centering the dialog over a
  particular window usually results in a better position.
  */

BOOL CenterWindow(HWND hwndChild, HWND hwndParent)
{
    RECT    rcChild, rcParent;
    int     cxChild, cyChild, cxParent, cyParent;
    int     cxScreen, cyScreen, xNew, yNew;
    HDC     hdc;

     /*  Get the Height and Width of the child window */
    GetWindowRect(hwndChild, &rcChild);
    cxChild = rcChild.right - rcChild.left;
    cyChild = rcChild.bottom - rcChild.top;

     /*  Get the Height and Width of the parent window */
    GetWindowRect(hwndParent, &rcParent);
    cxParent = rcParent.right - rcParent.left;
    cyParent = rcParent.bottom - rcParent.top;

     /*  Get the display limits */
    hdc = GetDC(hwndChild);
    cxScreen = GetDeviceCaps(hdc, HORZRES);
    cyScreen = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(hwndChild, hdc);

     /*  Calculate new X position, then adjust for screen */
    xNew = rcParent.left + ((cxParent - cxChild) / 2);
    if (xNew < 0)
    {
        xNew = 0;
    }
    else if ((xNew + cxChild) > cxScreen)
    {
        xNew = cxScreen - cxChild;
    }

     /*  Calculate new Y position, then adjust for screen */
    yNew = rcParent.top  + ((cyParent - cyChild) / 2);
    if (yNew < 0)
    {
        yNew = 0;
    }
    else if ((yNew + cyChild) > cyScreen)
    {
        yNew = cyScreen - cyChild;
    }

     /*  Set it, and return */
    return SetWindowPos(hwndChild,
                        NULL,
                        xNew, yNew,
                        0, 0,
                        SWP_NOSIZE | SWP_NOZORDER);
}


/*
  FUNCTION: CmdStub(HWND, WORD, WORD, HWND)
  
  PURPOSE:  Display statusbar updates by calling UpdateStatusBar
  
  PARAMETERS:
  hwnd     - The window.
  wCommand - Menu command ID
  wNotify  - Notification number (unused)
  hwndCtrl - NULL (unused)
  
  RETURN VALUE:
  Always returns 0 - command handled.
  
  COMMENTS:
  Assumes there is a resource string describing this command with the
  same ID as the command ID.  Loads the string and calls UpdateStatusBar
  to put the string into main pane of the status bar.
  */

LRESULT CmdStub(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl) 
{
    char szBuffer[50];   
    int  cbWritten = 0;

    cbWritten = LoadString(hInst, wCommand, szBuffer, sizeof(szBuffer)); 
    if(cbWritten == 0) 
    {
        lstrcpy(szBuffer, "Unknown Command");
        UpdateStatusBar(szBuffer, 0, 0);
    }
    else
    { 
        UpdateStatusBar(szBuffer, 0, 0);
        MessageBox (hwnd, 
                    "Command Not Yet Implemented.\r\n", 
                    "Windvi",
                    MB_OK | MB_ICONEXCLAMATION);
    }
     /*
      * Once the command is executed, set the statusbar text to 
      * original text.
      */
    UpdateStatusBar(SZDESCRIPTION, 0, 0);

    return 0;
}
/*
  Utility functions 
*/

#if 0

/**************************************************************************** 
 *                                                                          * 
 *  FUNCTION   :  PaletteSize(VOID FAR * pv)                                * 
 *                                                                          * 
 *  PURPOSE    :  Calculates the palette size in bytes. If the info. block  * 
 *                is of the BITMAPCOREHEADER type, the number of colors is  * 
 *                multiplied by 3 to give the palette size, otherwise the   * 
 *                number of colors is multiplied by 4.                                                          * 
 *                                                                          * 
 *  RETURNS    :  Palette size in number of bytes.                          * 
 *                                                                          * 
 ****************************************************************************/ 
WORD PaletteSize (VOID FAR * pv) 
{     
  LPBITMAPINFOHEADER lpbi; 
  WORD               NumColors; 
  lpbi      = (LPBITMAPINFOHEADER)pv; 
  /*  NumColors = DibNumColors(lpbi);  */
  NumColors = lpbi->biBitCount;
  if (lpbi->biSize == sizeof(BITMAPCOREHEADER)) 
    return (WORD)(NumColors * sizeof(RGBTRIPLE));
  else 
    return (WORD)(NumColors * sizeof(RGBQUAD));
}  

/**************************************************************************** 
 *                                                                          * 
 *  FUNCTION   : DibBlt( HDC hdc,                                           * 
 *                       int x0, int y0,                                    * 
 *                       int dx, int dy,                                    * 
 *                       HANDLE hdib,                                       * 
 *                       int x1, int y1,                                    * 
 *                       LONG rop)                                          * 
 *                                                                          * 

*  PURPOSE    : Draws a bitmap in CF_DIB format, using SetDIBits to device.* 
 *               taking the same parameters as BitBlt().                    * 
 *                                                                          * 
 *  RETURNS    : TRUE  - if function succeeds.                              * 
 *               FALSE - otherwise.                                         * 
 *                                                                          * 
 ****************************************************************************/ 
BOOL DibBlt ( 
    HDC    hdc, 
    INT    x0, 
    INT    y0, 
    INT    dx, 
    INT    dy, 
    HANDLE hdib, 
    INT    x1, 
    INT    y1, 
    LONG   rop) 
{ 
    LPBITMAPINFOHEADER   lpbi; 
    LPSTR                pBuf; 
    DIBSECTION ds;

    if (!hdib) 
        return PatBlt(hdc,x0,y0,dx,dy,rop); 

    /* our dibs are created with CreateDIBsection(),
       so are DIBSECTION. We need to retrieve them with GetObject().
       FIXME : this does not work as-is.
       */

    if (GetObject(hdib, sizeof(DIBSECTION), &ds) == 0)
      Win32Error("DibBlt/GetObject");

    /*    lpbi = (VOID FAR *)GlobalLock(hdib);  */
    lpbi = &(ds.dsBmih);
 
    if (!lpbi) 
        return FALSE; 
 
    /*    pBuf = (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi);  */
    pBuf = (LPSTR)lpbi + (WORD)lpbi->biSize +
      lpbi->biBitCount * sizeof(RGBQUAD);
    if (SetDIBitsToDevice (hdc, x0, y0, dx, dy, 
			   x1,y1, 
			   x1, 
			   dy, 
			   pBuf, (LPBITMAPINFO)lpbi, 
			   DIB_RGB_COLORS ) == 0)
      Win32Error("DibBlt/SetDIBitsToDevice"); 
 
    /* GlobalUnlock(hdib); */
    return TRUE; 
} 

#endif

static char full_dviname[512];

void CloseDviFile()
{
  char *fp, *name;

  /* Nothing to close ? */
  if (!dvi_file)
	return;

  if (dvistate != SAVED) {
    /* Just in case we have tried to change directory : retain
       the full name. */

  /* Get the full file name */
#if 1
    /* Beware : might have added file: 
       FIXME: there should be a copy of original dvi_name
       rather that parsing it back ! 
       In fact, dvi_name must be made absolute first.
    */
    if (memicmp(dvi_name, "file:", 5) == 0) {
      if (GetFullPathName(dvi_name+5, sizeof(full_dviname), full_dviname, &fp) == 0)
	Win32Error("CloseDviFile/GetFullPathName");
    }
    else
#endif
      if (GetFullPathName(dvi_name, sizeof(full_dviname), full_dviname, &fp) == 0) {
	Win32Error("CloseDviFile/GetFullPathName");
      }

    dvistate = SAVED;
    dvipos = ftell(dvi_file);
    fclose(dvi_file);
    dvi_file = NULL;
#if 0
    fprintf(stderr, "Dvi file %s is closed\n", dvi_name);
#endif
  }
}

void ReopenDviFile()
{
  if (dvistate == SAVED && dvi_file == NULL) {
    dvi_file = fopen(full_dviname, OPEN_MODE);
    if (dvi_file) {
      /* user may have destroyed it !
	 FIXME: this needs testing */
      fseek(dvi_file, dvipos, SEEK_SET);
    }
    dvistate = RESTORED;
#if 0
    fprintf(stderr, "Dvi file %s is reopened\n", dvi_name);
#endif
  }
}

BOOL IsOpenedDviFile()
{
  return dvistate == RESTORED;
}

LRESULT MsgActivate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  extern Boolean check_dvi_file();
  int fActive = LOWORD(wparam);

  if (debug & DBG_EVENT) {
    fprintf(stderr, "Activate Draw: mag %s, set_home %s, init %scomplete, activated by %s\n",
	    (bMagDisp ? "on" : "off"),
	    (bSetHome ? "on" : "off"),
	    (bInitComplete ? "" : "in"),
	    (fActive == WA_CLICKACTIVE ? "click" : "other than click"));
  }
  
  if (!bInitComplete || !dvi_file)
	return;

  if (/* !bMagDisp && !bSetHome && */ bInitComplete) {
    if (fActive == WA_ACTIVE)
      bSkipFirstClick = FALSE;
    else if (fActive == WA_CLICKACTIVE)
      bSkipFirstClick = TRUE;
    if (fActive == WA_ACTIVE || fActive == WA_CLICKACTIVE) {
      /* We are being activated */
      /* Reopen dvi file */
	  ReopenDviFile();
      if (resource.scan_flag) {
		if (dvi_file) check_dvi_file();
		/*	ChangePage(0); */
      }
    }
    else if (fActive == WA_INACTIVE) {
      /* We are being deactivated */
      if (bMagDisp) {
		/* Remove the mag glass !
		   Look at windraw.c for more precisions. 
		*/
		foreGC = ruleGC = highGC = maneDrawDC;
		fprintf(stderr, "hiding mag glass\n");
		ShowWindow(hWndMagnify, SW_HIDE);
		bMagDisp = FALSE;
		/* FIXME : is this needed ? */
		bDrawKeep = FALSE;
		UpdateWindow(hWndDraw);
		/* restores the old shrink factor and redisplay page */
		ClipCursor(NULL);
		ReleaseCapture();
      }
	  else  if (bSetHome) {
		/* Restore cursor */
		bSetHome = FALSE;
		ClipCursor(NULL);
		ReleaseCapture();
		SetCursor(hCursArrow);
	  }
      /* Save dvi file params & close it if possible */
      if (dvi_file) {
		CloseDviFile();
      }
    }
  }

  return 0;
}


LRESULT MsgCopyData(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  char oldDir[256];
  int fa;
  struct data_exchange *lpDataEx = (struct data_exchange *)(((COPYDATASTRUCT *)lparam)->lpData);

#if 0
  MessageBox(hWndMain, lpDataEx->cwd, lpDataEx->dviname, 
	       MB_APPLMODAL | MB_ICONINFORMATION);
#endif
  /* Should we check for new file existence ? */
  GetCurrentDirectory(sizeof(oldDir), oldDir);
  SetCurrentDirectory(lpDataEx->cwd);
  NormalizeDviName(sizeof(lpDataEx->dviname), lpDataEx->dviname);
  if (((fa = GetFileAttributes(lpDataEx->dviname)) != 0xFFFFFFFF)
      && ((fa & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)) {
    CloseCurrentFile();
    mane.shrinkfactor = lpDataEx->shrinkfactor;
    current_page = lpDataEx->currentpage;
    wsprintf(oldDir, "page %d shrink %d", current_page, mane.shrinkfactor);
#if 0
    MessageBox(hWndMain, oldDir, lpDataEx->dviname, 
	       MB_APPLMODAL | MB_ICONINFORMATION);
#endif
    OpenCurrentFile(lpDataEx->dviname);
    SetForegroundWindow(hwnd);
  }
  else {
    MessageBox(hWndMain, "Can't find file !", lpDataEx->dviname, 
	       MB_APPLMODAL | MB_ICONERROR);
    SetCurrentDirectory(oldDir);
  }
  return TRUE;
}

#define TRSIZE 100

/* BEGIN CHUNK events.c 1 */
#ifdef SRC_SPECIALS
#define src_jumpButton resource._src_jumpButton
#endif
/* END CHUNK events.c 1 */

LRESULT MsgChar(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  char szBuf[80];
  static	BOOL	has_arg		= FALSE;
  static	int	number		= 0;
  static	int	sign		= 1;
  char	ch;
  BOOL	arg0;
  int	number0;
/* BEGIN CHUNK events.c 3 */
	int src_x, src_y;
/* END CHUNK events.c 3 */

  ch = wparam;

#if 0
  printf("char %c (%d) (%x) \n", ch, ch, ch);
#endif
  if (ch >= '0' && ch <= '9') {
    has_arg = TRUE;
    number = number * 10 + sign * (ch - '0');
    return 0;
  }
  else if (ch == '-') {
    has_arg = TRUE;
    sign = -1;
    number = 0;
    return 0;
  }
  number0 = number;
  number = 0;
  sign = 1;
  arg0 = has_arg;
  has_arg = FALSE;
#if 0
  printf("arg0 %d number0 %d\n", arg0, number0);
#endif
  switch (wparam) {

  case 'Z':
    debug = (arg0 ? number0 : -1);
    break;

  case 'q':
  case 'Q':
  case '\003':	/* control-C */
  case '\004':	/* control-D */
#ifdef	VMS
  case '\032':	/* control-Z */
    if (MessageBox(hWndMain, "Really want to quit ?", "Exit", 
		   MB_APPLMODAL | MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
      PostQuitMessage((ch == 'Q') ? 2 : 0);
    break;
#endif
#if	PS
		ps_destroy();
#endif

  case 'n':
  case 'f':
  case ' ':
    /* case '\r': */
    /* case '\n': */
    /* scroll forward; i.e. go to relative page */
    ChangePage(arg0 ? number0 : 1);
    break;
#ifdef HTEX
  case 'F': /* Follow link forward! */
    {
      int x, y;
      if (pointerlocate(&x, &y)) {
	/* screen_to_page(&mane,s_x,s_y,&page,&px,&py); */
	(void) htex_handleref(current_page, x, y);
#if 0
	redraw_page();
#endif
      }
    }
  return 0; /* Should goto bad if problem arises? */
  case 'B': /* Go back to previous anchor. */
    htex_goback(); /* Should goto bad if problem arises? */
#if 0
    redraw_page();
#endif
    return 0;
#endif

  case 'p':
  case 'b':
    /* case '\b':
  case '\177':  Del */
    /* scroll backward */
    ChangePage( - (arg0 ? number0 : 1));
    break;

  case '<':
    ChangePage(-current_page);
    break;

  case 'g':
  case 'j':
  case '>':
    /* go to absolute page (last by default) */
    ChangePage((arg0 ? number0 - pageno_correct :
	     total_pages - 1) - current_page);
    break;

  case '?':
  case 'h':
  case 'H':			/* Help */
    show_help();
    return 0;

  case 'P':		/* declare current page */
    pageno_correct = arg0 * number0 - current_page;
    return 0;

  case 'k':
    resource.keep_flag = (arg0 ? number0 : !resource.keep_flag);
    CheckMenuItem(hMenuMain, IDM_KEEPPOS, (resource.keep_flag ? MF_CHECKED : MF_UNCHECKED));
    wsprintf(szBuf, "Home position %skept.", (resource.keep_flag ? "" : "not "));
    UpdateStatusBar(szBuf, 0, 0);
    break;

  case '\f':
    /* redisplay current page */
    ChangePage(0);
    break;

  case '^':
    home(TRUE);
    break;

  case 'l':
    SendMessage(hWndDraw, WM_HSCROLL, MAKELONG(SB_PAGEUP, 0), 0L); 
    break;

  case 'r':
    SendMessage(hWndDraw, WM_HSCROLL, MAKELONG(SB_PAGEDOWN, 0), 0L); 
    break;

  case 'u':
    SendMessage(hWndDraw, WM_VSCROLL, MAKELONG(SB_PAGEUP, 0), 0L); 
    break;

  case 'd':
    SendMessage(hWndDraw, WM_VSCROLL, MAKELONG(SB_PAGEDOWN, 0), 0L); 
    break;

  case 'c':
#ifndef WIN32
		scrollwindow(&mane, mane.base_x + eventp->xkey.x - clip_w/2,
		    mane.base_y + eventp->xkey.y - clip_h/2);
		if (x_bar) paint_x_bar();
		if (y_bar) paint_y_bar();
		XWarpPointer(DISP, None, None, 0, 0, 0, 0,
		    clip_w/2 - eventp->xkey.x, clip_h/2 - eventp->xkey.y);
		return;
#else
		break;
#endif
  case 'M':
    {
	  POINT ptOrg = { 0, 0};
	  GetWindowOrgEx(maneDrawDC, &ptOrg);
      home_x = min(xMousePos + xCurrentScroll + ptOrg.x, (unsigned) page_w) * mane.shrinkfactor;
      home_y = min(yMousePos + yCurrentScroll + ptOrg.y, (unsigned) page_h) * mane.shrinkfactor;
      if (resource.sidemargin) free(resource.sidemargin);
      resource.sidemargin = pixtoa(home_x);
      if (resource.topmargin) free(resource.topmargin);
      resource.topmargin = pixtoa(home_y);

      wsprintf(szBuf, "Setting home to %5d, %5d", home_x, home_y);
      UpdateStatusBar(szBuf, 0, 0);
    }
  break;
/* BEGIN CHUNK events.c 4 */
#ifdef SRC_SPECIALS
  case 'X':
	{	  
	  POINT ptOrg = { 0, 0};
	  GetWindowOrgEx(maneDrawDC, &ptOrg);
	  src_x = min(xMousePos + ptOrg.x + xCurrentScroll, (unsigned) page_w) /* * mane.shrinkfactor */;
	  src_y = min(yMousePos + ptOrg.y + yCurrentScroll, (unsigned) page_h) /* * mane.shrinkfactor */;
	  /* just highlight next special without calling editor for it */
	  src_find_special(0, src_x, src_y);
	}
	return;
#endif		
/* END CHUNK events.c 4 */

  case 's':
    if (!arg0) {
      int temp;
      
      number0 = ROUNDUP(unshrunk_page_w, window_w - 2);
      temp = ROUNDUP(unshrunk_page_h, window_h - 2);
      if (number0 < temp) number0 = temp;
    }
    if (number0 <= 0) goto bad;
    if (number0 == mane.shrinkfactor) return 0;

    ChangeZoom(number0);
    break;

/* BEGIN CHUNK events.c 6 */
#ifdef SRC_SPECIALS
	/*
	 * Control-S toggles visibility of src specials
	 * (mnemonic for isearch in Emacs ;-)
	 * Also changes the cursor to emphasize the new mode.
	 */
  case '\023': /* Control-S */
	if (src_evalMode) {
	  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
		Fprintf(stdout, "SRC specials OFF\n");
	  }
	  /* free src_arr */
	  src_cleanup();
	  src_evalMode = False;
	  SetCursor(hCursArrow);
	}
	else {
	  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
		Fprintf(stdout, "SRC specials ON\n");
	  }
	  src_evalMode = True;
	  /* used to have this in addition to emphasize the mode,
	   * but usage seems more coherent without it: */
	  /*	src_tickVisibility = True; */
	  SetCursor(hCursSrc);
	}
	redraw_page();
	break;
  case 'T':
    /*
     * change shape of specials, but only when they're visible;
     * this makes the key usable for other purposes in ordinary mode.
     * However, it seems that in ordinary mode `T' already does the same
     * as Ctrl-p: print the Unit/bitord/byteord stuff; what's
     * the reason for this ???
     */
	if (src_evalMode) {
	  src_tickShape++;
	  if (src_tickShape > SPECIAL_SHAPE_MAX_NUM) {
		src_tickShape = 0;
	  }
	  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
		Fprintf(stdout, "changing shape to \"%d\"\n", src_tickShape);
	  }
	  if (src_tickVisibility) {
		redraw_page();
	  }
	  break;
	}
#ifndef PS_GS
	case 'V':
	  if (src_evalMode) {
		/*
		 * toggle visibility of src specials
		 */
		if (src_tickVisibility) {
		  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
			Fprintf(stdout, "SRC special visibility OFF\n");
		  }
		  src_tickVisibility = False;
		}
		else {
		  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
			Fprintf(stdout, "SRC special visibility ON\n");
		  }
		  src_tickVisibility = True;
		}
		redraw_page();
		return;
	  }
#endif  /* PS_GS */		  
#endif  /* SRC_SPECIALS */
/* END CHUNK events.c 6 */
  case 'S':
    if (!arg0) goto bad;
#ifdef	GREY
    if (use_grey) {
      float newgamma = number0 != 0 ? number0 / 100.0 : 1.0;
      
      if (newgamma == gamma) return 0;
      gamma = newgamma;
      init_colors();
      ChangePage(0);
      return 0;
    }
#endif
    if (number0 < 0) goto bad;
    if (number0 == density) return 0;
    density = number0;
    reset_fonts();
    if (mane.shrinkfactor == 1) return 0;
    ChangePage(0);
    break;

  case 't':
    {		/* toggle through magnifier ruler tick units */
      extern void pixel_to_unit(void);
      extern char * pos_format;
      extern double p2u_factor;
      char szBuf[80];
      int k = 0;
      static char *TeX_units[] = {
	"bp", "cc", "cm", "dd", "in", "mm", "pc", "pt", "sp",
      };
	  POINT ptOrg = { 0, 0};
	  GetWindowOrgEx(maneDrawDC, &ptOrg);
      
      for (k = 0; k < sizeof(TeX_units)/sizeof(TeX_units[0]); ++k)
	if (strcmp(resource._tick_units,TeX_units[k]) == 0)
	  break;
      k++;
      if (k >= sizeof(TeX_units)/sizeof(TeX_units[0]))
	k = 0;
      resource._tick_units = TeX_units[k];
      pixel_to_unit();
      wsprintf(szBuf, "Ruler units = %.2s\n", resource._tick_units);
      UpdateStatusBar(szBuf, 0, 0);
      sprintf(szBuf, pos_format, 
	      (xMousePos + xCurrentScroll + ptOrg.x) * mane.shrinkfactor * p2u_factor,
	      (yMousePos + yCurrentScroll + ptOrg.y) * mane.shrinkfactor * p2u_factor);
      UpdateStatusBar(szBuf, 6, 0);
    }
  break;

  case 'G':
    use_grey = (arg0 ? number0 : !use_grey);
    if (use_grey) init_colors();
    reset_fonts();
    ChangePage(0);
    break;

  case 'D':
    grid_mode = (arg0 ? number0 : !grid_mode );
    init_page();
    reconfig();
    ChangePage(0);
    break;


#if	PS
	    case 'v':
		if (!arg0 || resource._postscript != !number0) {
		    resource._postscript = !resource._postscript;
		    if (resource._postscript) scanned_page = scanned_page_bak;
		    psp.toggle();
		}
		ChangePage(0);
		break;
#endif

#ifdef SELFILE
  case '\006': /* control-f */
    ++dvi_time ; /* notice we want a new file in check_dvi_file */
    ChangePage(0);
    break ;
#endif  /* SELFILE */
    
#if PS_GS
	    case 'V':
/* BEGIN CHUNK events.c 7 */
#ifdef SRC_SPECIALS	  
	  if (src_evalMode) {
		/*
		 * toggle visibility of src specials
		 */
		if (src_tickVisibility) {
		  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
			Fprintf(stdout, "SRC special visibility OFF\n");
		  }
		  src_tickVisibility = False;
		}
		else {
		  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
			Fprintf(stdout, "SRC special visibility ON\n");
		  }
		  src_tickVisibility = True;
		}
		redraw_page();
		return;
	  }
	  else {
#endif		
/* END CHUNK events.c 7 */
		if (!arg0 || resource.gs_alpha != !number0)
		    resource.gs_alpha = !resource.gs_alpha;
		break;
/* BEGIN CHUNK events.c 8 */
#ifdef SRC_SPECIALS		
	  }
#endif
/* END CHUNK events.c 8 */
#endif

  case 'R':
    /* reread DVI file */
    --dvi_time;	/* then it will notice a change */
    ChangePage(0);
    break;
  default:
    break;
  }

  goto good;

 bad:
  if (MessageBeep(0xFFFFFFFF) == 0)
    Win32Error("HandleKey/MessageBeep");
 good:
  return 0;
}

LRESULT MsgKeyDown(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
  WORD wScrollNotify = 0xFFFF;
  UINT msg = WM_VSCROLL;
  int new_zoom = shrink_factor, 
    rel_page = 0;

#define SHIFTED 0x8000
#define KEYCOUNT 0x000F

  int ctrl  = (GetKeyState(VK_CONTROL) & SHIFTED);
  int shift = (GetKeyState(VK_SHIFT) & SHIFTED);
  int l_alt = (GetKeyState(VK_LMENU) & SHIFTED);
  int r_alt = (GetKeyState(VK_RMENU) & SHIFTED);
  int count = lparam & KEYCOUNT;

#if 0
  fprintf(stderr, "ctrl = %x, shift = %x, wparam = %x, count = %x\n",
	  ctrl, shift, wparam, count);
#endif

  switch (wparam) { 
  case VK_LEFT:
    msg = WM_HSCROLL;
    if (ctrl) 
      wScrollNotify = SB_LEFT;
    else if (shift)
      wScrollNotify = SB_PAGELEFT;
    else
      wScrollNotify = SB_LINELEFT;
    break;
  
  case VK_RIGHT:
    msg = WM_HSCROLL;
    if (ctrl) 
      wScrollNotify = SB_RIGHT;
    else if (shift)
      wScrollNotify = SB_PAGERIGHT;
    else
      wScrollNotify = SB_LINERIGHT;
    break;

  case VK_UP: 
    msg = WM_VSCROLL;
    if (ctrl) 
      wScrollNotify = SB_TOP;
    else if (shift)
      wScrollNotify = SB_PAGEUP;
    else
      wScrollNotify = SB_LINEUP;
    break; 
    
  case VK_DOWN: 
    msg = WM_VSCROLL;
    if (ctrl) 
      wScrollNotify = SB_BOTTOM;
    else if (shift)
      wScrollNotify = SB_PAGEDOWN;
    else
      wScrollNotify = SB_LINEDOWN;
    break; 
    
  case VK_PRIOR: 
    if (ctrl)
      ChangePage(-current_page);
    wScrollNotify = SB_PAGEUP; 
    break; 
    
  case VK_NEXT: 
    if (ctrl)
      ChangePage(total_pages - 1 - current_page);
    wScrollNotify = SB_PAGEDOWN; 
    break; 
	    
  case VK_END: 
    /* Only vertical end */
    wScrollNotify = SB_BOTTOM; 
    break; 

  case VK_HOME:
    home(TRUE);
    break;

  case VK_ADD:
    new_zoom -= count;
    break;

  case VK_SUBTRACT:
    new_zoom += count;
    break;

  case VK_RETURN:
    rel_page = count;
    break;

  case VK_BACK:
    rel_page = -count;
    break;

  case 'L':
    if (ctrl) {
      ChangePage(0);
    }
    break;

  } 
#if 0
  fprintf(stderr, "wScrollNotify = %x msg = %x\n", wScrollNotify, msg);
#endif
  if (new_zoom != shrink_factor)
    ChangeZoom(new_zoom);
  if (rel_page != 0)
    ChangePage(rel_page);
  if (wScrollNotify != 0xFFFF) {
    int i;
    for (i = 0; i < count; i++)
      SendMessage(hWndDraw, msg, MAKELONG(wScrollNotify, 0), 0L); 
  }
  return 0;
}


LRESULT CmdOpen (HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  extern FILE *select_filename(int, int);
  extern void open_dvi_file();

  if (dvi_file = select_filename(TRUE, TRUE)) {
    open_dvi_file();
    if (reconfig() == FALSE) {
      char buf[40];
      mane.shrinkfactor +=3;
      wsprintf(buf, "New shrink factor : %d\n", mane.shrinkfactor);
      UpdateStatusBar(buf, 0, 0);
      if (reconfig() == FALSE) {
	MessageBox(hWndMain, "Can't allocate page bitmap !\r\nPlease report this error.", NULL, MB_APPLMODAL | MB_ICONERROR | MB_OK);
	CleanExit(1);
      }
    };
#if 0
    redraw_page();
#else
    ChangeZoom(resource.shrinkfactor);
#endif
    ChangePage(0);
    SetForegroundWindow(hwnd);
  }
  return 0;
}

LRESULT CmdOpenRecentFile (HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  extern void UpdateMainMenuUsedFiles(void);

  int nCount = wCommand - IDM_FILE_RECENT;
  char *new_name, *new_cwd, *last_sep, *p;
  char oldDir[260];
  int fa, i;

  new_name = strdup(p = lpLastUsedFiles[nCount]);

  /* shift the first files down to this one */
  for (i = nCount; i >= 1; i--)
    lpLastUsedFiles[i] = lpLastUsedFiles[i-1];
  lpLastUsedFiles[0] = p;
  UpdateMainMenuUsedFiles();

  if ((last_sep = strrchr(new_name, '/')) == NULL)
    last_sep = strrchr(new_name, '\\');

  if (!last_sep)
    return 0;

  *last_sep = '\0';
  new_cwd = new_name;
  new_name = last_sep+1;

  /* Should we check for new file existence ? */
  GetCurrentDirectory(sizeof(oldDir), oldDir);
  SetCurrentDirectory(new_cwd);
  if (((fa = GetFileAttributes(new_name)) != 0xFFFFFFFF)
      && ((fa & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)) {
    CloseCurrentFile();
    current_page = 0;
#if 0
    MessageBox(hWndMain, oldDir, lpDataEx->dviname, 
	       MB_APPLMODAL | MB_ICONINFORMATION);
#endif
    OpenCurrentFile(new_name);
    if (reconfig() == FALSE) {
      char buf[40];
      mane.shrinkfactor +=3;
      wsprintf(buf, "New shrink factor : %d\n", mane.shrinkfactor);
      UpdateStatusBar(buf, 0, 0);
      if (reconfig() == FALSE) {
	MessageBox(hWndMain, "Can't allocate page bitmap !\r\nPlease report this error.", NULL, MB_APPLMODAL | MB_ICONERROR | MB_OK);
	CleanExit(1);
      }
    };
#if 0
    redraw_page();
#endif
    ChangePage(0);
    SetForegroundWindow(hwnd);
  }
  else {
    MessageBox(hWndMain, "Can't find file !", new_name, 
	       MB_APPLMODAL | MB_ICONERROR);
    SetCurrentDirectory(oldDir);
  }
  free(new_cwd);
  return 0;
}

LRESULT CmdClose (HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  if (dvi_file)
    CloseDviFile();
  if (dvi_name) {
    free(dvi_name);
    dvi_name = NULL;
	set_icon_and_title("", NULL, NULL, 1);
  }
  redraw_page();
  return 0;
}

/* Choose a decent shrink factor, given the page size 
   and the screen params */
int ChooseShrink()
{
  extern HWND hWndMain, hWndDraw;
  RECT rcTotal, rcDraw;
  int total_width, total_height, draw_width, draw_height;
  int usable_screen_width, usable_screen_height;
  int xNewSize, yNewSize;
  int shrink_x, shrink_y;
  char szBuf[80];

 /*
    Principle : get the size of the non-drawing area 
    remove it from the size of the screen
    and calculate the best shrink factor of this size
    next resize the frame window
    */
  GetWindowRect(hWndMain, &rcTotal);
  GetWindowRect(hWndDraw, &rcDraw);
  total_width = rcTotal.right - rcTotal.left;
  total_height = rcTotal.bottom - rcTotal.top;
  draw_width = rcDraw.right - rcDraw.left;
  draw_height = rcDraw.bottom - rcDraw.top;
  usable_screen_width = maneHorzRes - (total_width - draw_width);
  usable_screen_height = maneVertRes - (total_height - draw_height);

  shrink_x = ROUNDUP(unshrunk_page_w, draw_width);
  shrink_y = ROUNDUP(unshrunk_page_h, draw_height);
  if (shrink_x > shrink_y) shrink_x = shrink_y;

  xNewSize = ROUNDUP(unshrunk_page_w,shrink_x) 
    + (total_width - draw_width) + 1;
  yNewSize = ROUNDUP(unshrunk_page_h,shrink_x) 
    + (total_height - draw_height) + 1;

  wsprintf(szBuf, "Shrink choosen %d (%d x %d)", shrink_x,
	   xNewSize, yNewSize);
  UpdateStatusBar(szBuf, 0, 0);
  return shrink_x;
}

void ChangeZoom(int new_shrink)
{
  char szBuf[20];

  if (new_shrink <= 0 || new_shrink == mane.shrinkfactor)
    return;

  mane.shrinkfactor = new_shrink;
  init_page();
#if 0
  fprintf(stderr, "new_shrink (%d) != bak_shrink (%d)\n", 
	  new_shrink, bak_shrink);
#endif
  if (new_shrink != 1 && new_shrink != bak_shrink) {
    bak_shrink = new_shrink;
#ifdef	GREY
    /* if (use_grey) init_pix(RGB(0,0,0), RGB(255,255,255)); */
    if (use_grey) 
      init_pix(string_to_colorref(resource.fore_color), 
	       string_to_colorref(resource.back_color));
#endif
  }
  if (reconfig() == FALSE) {
    char buf[40];
    mane.shrinkfactor += 1;
    wsprintf(buf, "Can't allocate bitmap for this shrink factor.");
    UpdateStatusBar(buf, 0, 0);
    init_page();
    if (reconfig() == FALSE) {
      MessageBox(hWndMain, "Not enough storage for page bitmap\r\n", 
		 NULL, MB_OK | MB_APPLMODAL | MB_ICONERROR);
      CleanExit(1);
    }
  }
  reset_fonts();
  /* In case the ScrollBars will disappear */
  SetScrollBars(hWndDraw);
  redraw_page();
  resource.shrinkfactor = mane.shrinkfactor;
  wsprintf(szBuf, "%2d", mane.shrinkfactor);
  UpdateStatusBar(szBuf, 4, 0);
}

LRESULT CmdZoomIn(HWND hwnd, WORD wCommand, 
		  WORD wNotify, HWND hwndCtrl)
{
  ChangeZoom(mane.shrinkfactor - 1);
  return 0;
}

LRESULT CmdZoomOut(HWND hwnd, WORD wCommand, 
		   WORD wNotify, HWND hwndCtrl)
{
  ChangeZoom(mane.shrinkfactor + 1);
  return 0;
}

LRESULT CmdTogglePS(HWND hwnd, WORD wCommand, 
		    WORD wNotify, HWND hwndCtrl)
{
  resource._postscript = (resource._postscript ? 0 : 1);
  CheckMenuItem(hMenuMain, IDM_TOGGLEPS, (resource._postscript ? MF_CHECKED : MF_UNCHECKED));
  if (resource._postscript) scanned_page = scanned_page_bak;
  psp.toggle();
  redraw_page();
  return 0;
}

LRESULT CmdToggleGrid(HWND hwnd, WORD wCommand, 
		    WORD wNotify, HWND hwndCtrl)
{
  char szBuf[80];
  float sep = 0.0;

  resource._grid_mode = (resource._grid_mode + 1) % 4;
  if (resource._grid_mode == 3) {
    sep = (float)ROUNDUP(unshrunk_paper_unit, shrink_factor) / 4.0; 
    sprintf(szBuf, "Grid mode %4.2f pixels", sep);
  }
  else if (resource._grid_mode == 2) {
    sep = (float)ROUNDUP(unshrunk_paper_unit, shrink_factor) / 2.0; 
    sprintf(szBuf, "Grid mode %4.2f pixels", sep);
  }
  else if (resource._grid_mode == 1) {
    sep = (float)ROUNDUP(unshrunk_paper_unit, shrink_factor); 
    sprintf(szBuf, "Grid mode %4.2f pixels", sep);
  }
  else
    sprintf(szBuf, "Grid mode off");

  UpdateStatusBar(szBuf, 0, 0);
  CheckMenuItem(hMenuMain, IDM_TOGGLEGRID, (resource._grid_mode ? MF_CHECKED : MF_UNCHECKED));
  init_page();
  redraw_page();
  return 0;
}

/*
  Warning : ChangePage takes a relative count
  */
void ChangePage(int count)
{
  extern int total_pages;
  char szBuf[20];
  if (count == 0) {
    /* Explicit call to redraw the current page */
    redraw_page();
  }
  else {
    int next_page;
#ifdef BOOK_MODE
    if (resource.book_mode) {
      if (count == 1 || count == -1) {
	/* in this case, should be 2 */
	next_page = current_page + 2*count;
      }
      /* ensure that the left page is even */
      current_page = (current_page % 2 ? current_page - 1 : current_page);
    }
    else {
      next_page = current_page + count;
    }
#else
      next_page = current_page + count;
#endif
#if 1
      next_page = min(next_page, total_pages - 1);
      next_page = max(next_page, 0);
      if (current_page != next_page) {
		  /* BEGIN CHUNK events.c 0 */
#ifdef SRC_SPECIALS
		  src_delete_all_specials();
#endif
/* END CHUNK events.c 0 */
	current_page = next_page;
	warn_spec_now = warn_spec;
	redraw_page();
      }
#else
      if (0 <= next_page && next_page < total_pages) {
	current_page = next_page;
	warn_spec_now = warn_spec;
	redraw_page();
      }
#endif
  }
#ifdef BOOK_MODE
  if (resource.book_mode) {
    wsprintf(szBuf, "%5d-%5d of %5d", current_page+1, current_page+2, total_pages);    
  }
  else {
    wsprintf(szBuf, "%5d of %5d", current_page+1, total_pages);
  }
#else
    wsprintf(szBuf, "%5d of %5d", current_page+1, total_pages);
#endif
  UpdateStatusBar(szBuf, 2, 0);
}

LRESULT CmdNextPage(HWND hwnd, WORD wCommand, 
		    WORD wNotify, HWND hwndCtrl)
{
  ChangePage(+1);
  return 0;
}

LRESULT CmdPreviousPage(HWND hwnd, WORD wCommand, 
			WORD wNotify, HWND hwndCtrl)
{
  ChangePage(-1);
  return 0;
}

LRESULT CmdNext5(HWND hwnd, WORD wCommand, 
		 WORD wNotify, HWND hwndCtrl)
{
  ChangePage(+5);
  return 0;
}

LRESULT CmdPrevious5(HWND hwnd, WORD wCommand, 
		     WORD wNotify, HWND hwndCtrl)
{
  ChangePage(-5);
  return 0;
}

LRESULT CmdNext10(HWND hwnd, WORD wCommand, 
		  WORD wNotify, HWND hwndCtrl)
{
  ChangePage(+10);
  return 0;
}

LRESULT CmdPrevious10(HWND hwnd, WORD wCommand, 
		      WORD wNotify, HWND hwndCtrl)
{
  ChangePage(-10);
  return 0;
}

LRESULT CmdFirstPage(HWND hwnd, WORD wCommand, 
		      WORD wNotify, HWND hwndCtrl)
{
  ChangePage(- current_page);
  return 0;
}

LRESULT CmdLastPage(HWND hwnd, WORD wCommand, 
		      WORD wNotify, HWND hwndCtrl)
{
  ChangePage(total_pages - current_page - 1);
  return 0;
}

LRESULT CmdRedrawPage(HWND hwnd, WORD wCommand, 
		      WORD wNotify, HWND hwndCtrl)
{
  warn_spec_now = warn_spec;
  redraw_page();
  return 0;
}

LRESULT CmdKeepPosition(HWND hwnd, WORD wCommand, 
			WORD wNotify, HWND hwndCtrl)
{
  char szBuf[80];

  resource.keep_flag = !resource.keep_flag;
  CheckMenuItem(hMenuMain, IDM_KEEPPOS, (resource.keep_flag ? MF_CHECKED : MF_UNCHECKED));
  wsprintf(szBuf, "Home position %s kept.", (resource.keep_flag ? "" : "not"));
  UpdateStatusBar(szBuf, 0, 0);
  return 0;
}

LRESULT CmdGotoPage(HWND hwnd, WORD wCommand, 
		    WORD wNotify, HWND hwndCtrl)
{
  DialogBox(hInst, "DlgGotoPage", hwnd, (DLGPROC)DlgGotoPage);
  return 0;
}

LRESULT CmdSrcSpecials(HWND hwnd, WORD wCommand, 
					   WORD wNotify, HWND hwndCtrl)
{
  /* BEGIN CHUNK events.c 2 */
#ifdef SRC_SPECIALS	
	if (src_evalMode) {
	  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
		Fprintf(stdout, "SRC specials OFF\n");
	  }
	  /* free src_arr */
	  src_cleanup();
	  src_evalMode = False;
	  SetCursor(hCursArrow);
	}
	else {
	  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
		Fprintf(stdout, "SRC specials ON\n");
	  }
	  src_evalMode = True;
	  /* used to have this too to make the mode clearer, but usage seems more coherent without it: */
	  /*	src_tickVisibility = True; */
	  SetCursor(hCursSrc);
	}
	redraw_page();
#endif
/* END CHUNK events.c 2 */
  return 0;
}

#ifdef HTEX
LRESULT CmdUrlBack(HWND hwnd, WORD wCommand, 
		   WORD wNotify, HWND hwndCtrl)
{
  htex_goback();
#if 0
  redraw_page();
#endif
  return 0;
}
#endif

LRESULT CmdViewLog(HWND hwnd, WORD wCommand, 
		    WORD wNotify, HWND hwndCtrl)
{
  ShowWindow(hViewLog, SW_SHOW);
  bLogShown = TRUE;
  return 0;
}

void CleanUp()
{
#ifdef TRANSFORM
  extern HRGN hClipRgn;
#endif
  
#ifdef PS
#ifdef TRANSFORM
  extern HANDLE hGsEvent;
#endif
#endif

  extern HDC hdcDrawSave;
  
#if 0
  __asm int 3;
#endif

  _fcloseall();

#ifdef PS
  if (hGsDll) {
    ps_destroy();
	gs_dll_release();
  }
#endif

#ifdef HTEX
  htex_cleanup(0);
#endif

  remove_temporary_dir();

#if 0
  /* resources that should be freed */
  /*
    Colors
  */
  CRefFree(scan_fore_colors);
#endif

  FreeOptions();

#if 0
  MessageBox( NULL, "LogLoopThread finished", "", MB_OK|MB_ICONINFORMATION );
#endif

  /* close any handle */
  fprintf(stderr, "\n");
  _flushall();

  fclose(stdin);
  fclose(stdout);
  fclose(stderr);

  if (dvi_file)
    Fclose(dvi_file);

  /* terminate thread */
  if (WaitForSingleObject(hViewLogThread, 2000) == WAIT_TIMEOUT) {
    /* This is really unclean. From 20/04/99, it seems that closing
       stdout and stderr is not enough to make the ReadFile()
       call return broken_pipe. So we have to make it the hard way. */
    // MessageBox( NULL, "ViewLog thread does not want to shut down...", "", MB_OK|MB_ICONINFORMATION );
    TerminateThread(hViewLogThread, 1);
    CloseHandleAndClear(&hViewLogThread);
    PostMessage(hViewLog, WM_QUIT, 0, 0);
    if (WaitForSingleObject(hLogLoopThread, 250) == WAIT_TIMEOUT) {
      TerminateThread(hLogLoopThread, 1);
    }
    CloseHandleAndClear(&hLogLoopThread);
  }

  CloseHandleAndClear(&hLogIn);
  CloseHandleAndClear(&hViewLogThread);

    
  /* Deallocate gdi resources */
  if (forePen && !DeleteObject(forePen)) 
    Win32Error("CleanUp/DeleteObject/forePen");
  if (foreBrush && !DeleteObject(foreBrush))
    Win32Error("CleanUp/DeleteObject/foreBrush");
  if (backBrush && !DeleteObject(backBrush)) 
    Win32Error("CleanUp/DeleteObject/backBrush");
  if (backTPicPen && !DeleteObject(backTPicPen)) 
    Win32Error("CleanUp/DeleteObject/backTPicPen");
  if (foreTPicPen && !DeleteObject(foreTPicPen)) 
    Win32Error("CleanUp/DeleteObject/foreTPicPen");
  if (foreTPicBrush && !DeleteObject(foreTPicBrush))
    Win32Error("CleanUp/DeleteObject/foreTPicBrush");

  if (resource.in_memory) {
    if (oldmaneDIB) {
      /* There is an old maneDIB, put it back in the DC
	 and delete the current one */
      if ((maneDIB = SelectObject(maneDrawDC, oldmaneDIB)) == NULL)
	Win32Error("reconfig/SelectObject");
      if (DeleteObject(maneDIB) == FALSE)
	Win32Error("DeleteObject/maneDIB");
    }
    
    if (oldmagDIB) {
      /* There is an old magDIB, put it back in the DC
	 and delete the current one */
      if ((magDIB = SelectObject(magMemDC, oldmagDIB)) == NULL)
	Win32Error("SelectObject/oldmagDIB");
      if (DeleteObject(magDIB) == FALSE)
	Win32Error("DeleteObject/magDIB");
    }

    if (maneDrawDC && !DeleteDC(maneDrawDC)) Win32Error("CleanUp/DeleteDC/maneDrawDC");
    if (magMemDC && !DeleteDC(magMemDC)) Win32Error("CleanUp/DeleteDC/magMemDC");
  }

  if (hdcDrawSave) DeleteDC(hdcDrawSave);

#ifdef TRANSFORM
  if (hClipRgn)
    DeleteObject(hClipRgn);
#endif
}

void CleanExit(int code)
{
  CleanUp();
  ExitProcess(code);
}
/*
  Be warned: the string has to be used before this function is called 
  a second time.
  */
LPTSTR   GetStringRes (int id)
{
  static TCHAR buffer[MAX_PATH];

  buffer[0]=0;
  LoadString (GetModuleHandle (NULL), id, buffer, MAX_PATH);
  return buffer;
}


LRESULT CmdHelpTopics(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  ReadHelp (hwnd, SZAPPNAME".HTML");
  return 0;
}

#if 0
LRESULT CmdHelpContents(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  BOOL bGotHelp;
  /* Not called in Windows 95 */
  bGotHelp = WinHelp (hwnd, SZAPPNAME".HLP", HELP_CONTENTS,(DWORD)0);
  if (!bGotHelp) {
    MessageBox (GetFocus(), GetStringRes(IDS_NO_HELP),
		szAppName, MB_OK|MB_ICONHAND);
  }
  return 0;
}  

LRESULT CmdHelpSearch(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  /* Not called in Windows 95 */
  if (!WinHelp(hwnd, SZAPPNAME".HLP", HELP_PARTIALKEY, (DWORD)(LPSTR)"")) {
    MessageBox (GetFocus(), GetStringRes(IDS_NO_HELP),
		szAppName, MB_OK|MB_ICONHAND);
  }
  return 0;
}  

LRESULT CmdHelpHelp(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
  /* Not called in Windows 95 */
  if(!WinHelp(hwnd, (LPSTR)NULL, HELP_HELPONHELP, 0)) {
    MessageBox (GetFocus(), GetStringRes(IDS_NO_HELP),
		szAppName, MB_OK|MB_ICONHAND);
  }
  return 0;
}  
#endif

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F 
  Function: GetErrorMsg  
  Summary:  Accepts a Win32 error code and retrieves a human readable 
            system message for it.    Args:     HRESULT hr 
              SCODE error code.             LPTSTR pszMsg 
              Pointer string where message will be placed. 
            UINT uiSize               Max size of the msg string.  
  Returns:  BOOL               TRUE if hr was error; FALSE if not. 
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/ 
BOOL GetErrorMsg(HRESULT hr, LPTSTR pszMsg, UINT uiSize) 
{   
  BOOL bErr = FAILED(hr);
  DWORD dwSize;
  if (bErr) { 
    memset(pszMsg, 0, uiSize * sizeof(TCHAR));
    dwSize = FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,       
			   NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),       
			   pszMsg, uiSize, NULL);     
    if (dwSize>2)     { 
      /* Take out the trailing CRLF. */
      pszMsg[--dwSize] = 0; 
      pszMsg[--dwSize] = 0;     
    }   
  }    
  return bErr; 
}   

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F 
  Function: HrMsg  
  Summary:  HRESULT Error Message box. Takes standard result code, 
            looks it up in the system tables, and shows a message 
            box with the error code (in hex) and the associated 
            system message.    Args:     HWND hWndOwner, 
              Handle to owner parent window.             LPTSTR pszTitle 
              User message string (eg, designating the attempted function). 
              Appears in dialog title bar.             HRESULT hr, 
              Standard result code.    Returns:  void 
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/ 
void HrMsg(HWND hWndOwner, LPTSTR pszTitle, HRESULT hr) 
{ 
  TCHAR szMsg[MAX_PATH];   
  TCHAR szErrMsg[MAX_PATH];   
  int iResult;  
  wsprintf(szMsg, TEXT("Error=0x%X:\r\n"), hr); 
  GetErrorMsg(hr, szErrMsg, MAX_PATH);   
  lstrcat(szMsg, szErrMsg); 
  iResult = MessageBox(hWndOwner, szMsg, pszTitle, MB_OK | MB_ICONEXCLAMATION);    
  return; 
} 
  
/*
  We can replace the WinHelp() system by html files using this.
  */
void ReadHelp(HWND hWndOwner, LPTSTR pszHelpFile) 
{   
#define NOBROWSE_ERROR_STR "Can't run browser." 
#define NOHTM_ERROR_STR "Can't find .HTM file." 
#define ERROR_TITLE_STR "-Error-" 
  int iRes;  
  LPSTR lpFullHelpFile = NULL;
  if (NULL != pszHelpFile) { 
    /* First check if the .HTM help file is there at all. */
    lpFullHelpFile = kpse_var_expand("$TEXMFMAIN/doc/html/windvi/windvi.html");
    MessageBox(hWndOwner, lpFullHelpFile, "Help File", 
			MB_OK | MB_ICONEXCLAMATION);     
    if (!lpFullHelpFile || GetFileAttributes(lpFullHelpFile) == -1)
      lpFullHelpFile = kpse_find_file(pszHelpFile, kpse_texdoc_format, TRUE);
    if (lpFullHelpFile) {
      /* Use shell to invoke web browser on the HTML help file. */
      iRes = (int) ShellExecute(hWndOwner, TEXT("open"), lpFullHelpFile,
				NULL, NULL, SW_SHOWNORMAL);       
      if (iRes <= 32) { 
        /* If unable to browse then put up an error box. */
        Win32Error(TEXT(NOBROWSE_ERROR_STR));       
      }     
    } 
    else {       
      /* If the .HTM file doesn't exist then put up an error box. */
      iRes = MessageBox(hWndOwner, TEXT(NOHTM_ERROR_STR), TEXT(ERROR_TITLE_STR), 
			MB_OK | MB_ICONEXCLAMATION);     
    }   
  }    
  return; 
} 

/*
  Add Last Used Files to main menu
*/

#define RECENT_POSITION 8

void UpdateMainMenuUsedFiles()
{  
  int nCount; 
  TCHAR szTemp[MAX_PATH + 6]; 
  HMENU hMenu; 
  MENUITEMINFO ItemInfo; 
  
  /* Validate parameters. */ 
  if (!IsWindow(hWndMain)) { 
    return; 
  } 

  if ((hMenu = GetSubMenu(GetMenu(hWndMain), 0)) == NULL)
    Win32Error("UpdateMainMenuUsedFiled/GetSubMenu");
    
  for (nCount = 0; nCount < iLastUsedFilesNum; nCount++) { 
    /* Only add strings that are not null or zero length.  */
    if ((lpLastUsedFiles[nCount] != NULL) && (lstrlen(lpLastUsedFiles[nCount]) != 0)) { 
      /* Build recent file menu string. */ 
      wsprintf(szTemp, __TEXT("&%d %s"), nCount +1, lpLastUsedFiles[nCount]); 
 
      /* Determine if replacing item or inserting.*/
      memset(&ItemInfo, 0, sizeof(MENUITEMINFO)); 
      ItemInfo.cbSize = sizeof(MENUITEMINFO); 
      ItemInfo.fMask = MIIM_TYPE; 
      GetMenuItemInfo(hMenu, RECENT_POSITION + nCount, TRUE, &ItemInfo); 
      if (MFT_SEPARATOR == ItemInfo.fType) { 
	/* Insert item. MIIM_ID */
	ItemInfo.fMask = MIIM_TYPE | MIIM_ID; 
	ItemInfo.wID = IDM_FILE_RECENT + nCount; 
	ItemInfo.fType = MFT_STRING; 
	ItemInfo.dwTypeData = szTemp; 
	ItemInfo.cch = lstrlen(ItemInfo.dwTypeData); 
	InsertMenuItem(hMenu, RECENT_POSITION + nCount, TRUE, &ItemInfo); 
      } 
      else { 
	/* Replace menu item. */
	ItemInfo.fMask = MIIM_TYPE | MIIM_STATE; 
	ItemInfo.fState = MFS_ENABLED; 
	ItemInfo.fType = MFT_STRING; 
	ItemInfo.dwTypeData = szTemp; 
	ItemInfo.cch = lstrlen(ItemInfo.dwTypeData); 
	SetMenuItemInfo(hMenu, RECENT_POSITION + nCount, TRUE, &ItemInfo); 
      } 
    } 
  } 
  DrawMenuBar(hWndMain);
}

int GetSystemType()
{
  OSVERSIONINFOEX osvi;
  BOOL bOsVersionInfoEx;
  int result = -1;

  /* Try calling GetVersionEx using the OSVERSIONINFOEX structure,
     which is supported on Windows NT versions 5.0 and later.
     If that fails, try using the OSVERSIONINFO structure,
     which is supported on earlier versions of Windows and Windows NT */

  ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

  if (! (bOsVersionInfoEx = GetVersionEx ( (OSVERSIONINFO *) &osvi) ) ) {
    /* If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO. */
    osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) {
      fprintf(stderr, "GetVersion() failed \n");
	  MessageBox(NULL, "GetVersion() failed.", NULL, MB_APPLMODAL | MB_ICONHAND | MB_OK);
      return -1;
    }
  }
  
  switch (osvi.dwPlatformId) {
  case VER_PLATFORM_WIN32_NT:
    if (osvi.dwMajorVersion == 3)
      result = WINNT3;
    else if (osvi.dwMajorVersion == 4)
      result = WINNT4;
    else
      result = WINNT5;
    break;
  case VER_PLATFORM_WIN32_WINDOWS:
    if ((osvi.dwMajorVersion > 4) || 
	((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0)))
      result = WIN98;
    else
      result = WIN95;
    break;
  case VER_PLATFORM_WIN32s:
    result = WIN31;
    break;
  }
#if 0
  fprintf (stderr, "version %d.%d (Build %d)\n",
	   osvi.dwMajorVersion, 
	   osvi.dwMinorVersion, 
	   osvi.dwBuildNumber & 0xFFFF);
  if (bOsVersionInfoEx)
    fprintf (stderr, "Service Pack %d.%d\n", 
	     osvi.wServicePackMajor, 
	     osvi.wServicePackMinor);
#endif  
  return result;
}

void CloseHandleAndClear(HANDLE *h)
{
  if (h && *h != INVALID_HANDLE_VALUE) {
    if (CloseHandle(*h) == FALSE) {
      Win32Error("CloseHandle");
    }
    *h = INVALID_HANDLE_VALUE;
  }
}
