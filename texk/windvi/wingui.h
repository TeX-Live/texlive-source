#define HAVE_BOOLEAN 1

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <tchar.h>

typedef long HRESULT;

#define APPNAME  WINDVI
#define SZAPPNAME "Windvi"
#define SZDESCRIPTION "Dvi viewer for Win32"
#define SZABOUT "About Windvi"
#define SZVERSION "4.0"

/*
  Resources
  */
#include "resource.h"

/****************************************************************************
  Global Variables
  ***************************************************************************/

extern HINSTANCE hInst;		/* Current instance */
extern char szAppName[100];	/* Name of the app */
extern char szTitle[100];	/* The title bar text */
extern HWND hWndMain, hWndDraw, hWndMagnify;
extern HWND hWndToolbar, hWndStatusBar;
extern HWND hViewLog;
extern HDC maneDC, maneDrawDC;		/* DC for the screen window */
extern HDC magDC, magMemDC;		/* DC for the magnifying glass */
extern HBITMAP maneDIB, magDIB;
extern HMENU hMenuMain;		/* Main Menu */
extern char *szLogFileName;	/* Temporary log file for kpathsea */
extern HANDLE hCrtIn, hCrtOut, hCrtErr; /* Standard handles */
extern HANDLE hLogIn;
extern HANDLE hViewLogThread, hLogLoopThread;
extern int tbHeight, sbHeight, maneHeight, maneWidth;
extern RECT maneRect;
/* Scrollbars' variables */
extern SCROLLINFO si;
extern int xMinScroll;
extern int xMaxScroll;
extern int xCurrentScroll;
extern int yMinScroll;
extern int yMaxScroll;
extern int yCurrentScroll;
extern BOOL fScroll;
extern BOOL fSize;
extern BOOL bLogShown;

/* Mouse Position */
extern int xMousePos;
extern int yMousePos;


void SetupEnv();
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
BOOL CreateDraw(HWND hwndParent);
BOOL CreateMagnify(HWND hwndParent);
BOOL CreateViewLog(HWND hwndParent);
extern void InitGlobalsDraw();

/* Used by the WM_COPYDATA mechanism */
struct data_exchange {
  char cwd[256];
  char dviname[256];
  int currentpage;
  int shrinkfactor;
};


/*-------------------------------------------------------------------------
   Functions for handling main window messages.  The message-dispatching
   mechanism expects all message-handling functions to have the following
   prototype:
  
       LRESULT FunctionName(HWND, UINT, WPARAM, LPARAM);

   **TODO**  Add message-handling function prototypes here.  Be sure to
             add the function names to the main window message table in
             windvi.c.
*/

LRESULT MsgCommand   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgCreate    (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDestroy   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgSize      (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMove      (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgHScroll   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgVScroll   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgTimer     (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMousemove (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMenuSelect(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgNotify    (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgPaint     (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgChar      (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgKeyDown   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgActivate  (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDropFiles (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgCopyData  (HWND, UINT, WPARAM, LPARAM);

LRESULT MsgDrawCommand   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawCreate    (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawSize      (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawHScroll   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawVScroll   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawMousemove (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawPaint     (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawEraseBkgnd(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawNCPaint   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawEnterMagL (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawEnterMagM (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawEnterMagR (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDrawQuitMagL  (HWND, UINT, WPARAM, LPARAM);

LRESULT MsgMagnifyCommand   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMagnifyCreate    (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMagnifySize      (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMagnifyHScroll   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMagnifyVScroll   (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMagnifyMousemove (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMagnifyEraseBkgnd(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgMagnifyPaint     (HWND, UINT, WPARAM, LPARAM);

/*-------------------------------------------------------------------------
   Functions for handling main window commands--ie. functions for
   processing WM_COMMAND messages based on the wParam value.
   The message-dispatching mechanism expects all command-handling
   functions to have the following prototype:
  
       LRESULT FunctionName(HWND, WORD, WORD, HWND);

   **TODO**  Add message-handling function prototypes here.  Be sure to
             add the function names to the main window command table in
             windvi.c.
*/

LRESULT CmdExit         (HWND, WORD, WORD, HWND);
LRESULT CmdFilePrint    (HWND, WORD, WORD, HWND);
LRESULT CmdFilePrSetup  (HWND, WORD, WORD, HWND);
LRESULT CmdAbout        (HWND, WORD, WORD, HWND);
LRESULT CmdStub         (HWND, WORD, WORD, HWND);
LRESULT CmdOpen         (HWND, WORD, WORD, HWND);
LRESULT CmdOpenRecentFile (HWND, WORD, WORD, HWND);
LRESULT CmdClose        (HWND, WORD, WORD, HWND);
LRESULT CmdZoomIn       (HWND, WORD, WORD, HWND);
LRESULT CmdZoomOut      (HWND, WORD, WORD, HWND);
LRESULT CmdToggleGrid   (HWND, WORD, WORD, HWND);
LRESULT CmdTogglePS     (HWND, WORD, WORD, HWND);
LRESULT CmdPreviousPage (HWND, WORD, WORD, HWND);
LRESULT CmdNextPage     (HWND, WORD, WORD, HWND);
LRESULT CmdPreviousPage (HWND, WORD, WORD, HWND);
LRESULT CmdNext5        (HWND, WORD, WORD, HWND);
LRESULT CmdPrevious5    (HWND, WORD, WORD, HWND);
LRESULT CmdNext10       (HWND, WORD, WORD, HWND);
LRESULT CmdPrevious10   (HWND, WORD, WORD, HWND);
LRESULT CmdGotoPage     (HWND, WORD, WORD, HWND);
LRESULT CmdSrcSpecials  (HWND, WORD, WORD, HWND);
LRESULT CmdUrlBack      (HWND, WORD, WORD, HWND);
LRESULT CmdViewLog      (HWND, WORD, WORD, HWND);
LRESULT CmdRedrawPage   (HWND, WORD, WORD, HWND);
LRESULT CmdKeepPosition (HWND, WORD, WORD, HWND);
LRESULT CmdLastPage     (HWND, WORD, WORD, HWND);
LRESULT CmdFirstPage    (HWND, WORD, WORD, HWND);
LRESULT CmdHelpTopics   (HWND, WORD, WORD, HWND);
LRESULT CmdHelpContents (HWND, WORD, WORD, HWND);
LRESULT CmdHelpSearch   (HWND, WORD, WORD, HWND);
LRESULT CmdHelpHelp     (HWND, WORD, WORD, HWND);
LRESULT CmdWindviConfig (HWND, WORD, WORD, HWND);
LRESULT CmdTexConfig    (HWND, WORD, WORD, HWND);

BOOL InitApplication(HINSTANCE);
BOOL CenterWindow(HWND, HWND);

/*
  Main callback function.
  */

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK DrawProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MagnifyProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgGotoPage(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgViewLog(HWND, UINT, WPARAM, LPARAM);

/*
   Message and command dispatch infrastructure.  The following type
   definitions and functions are used by the message and command dispatching
   mechanism and do not need to be changed.
   */

    /* Function pointer prototype for message handling functions. */
typedef LRESULT (*PFNMSG)(HWND,UINT,WPARAM,LPARAM);

    /* Function pointer prototype for command handling functions. */
typedef LRESULT (*PFNCMD)(HWND,WORD,WORD,HWND);

    /* Enumerated type used to determine which default window procedure */
    /* should be called by the message- and command-dispatching mechanism */
    /* if a message or command is not handled explicitly. */
typedef enum
{
   edwpNone,            /* Do not call any default procedure. */
   edwpWindow,          /* Call DefWindowProc. */
   edwpDialog,          /* Call DefDlgProc (This should be used only for */
                        /* custom dialogs - standard dialog use edwpNone). */
   edwpMDIChild,        /* Call DefMDIChildProc. */
   edwpMDIFrame         /* Call DefFrameProc. */
} EDWP;                /* Enumeration for Default Window Procedures */

    /* This structure maps messages to message handling functions. */
typedef struct _MSD
{
    UINT   uMessage;
    PFNMSG pfnmsg;
} MSD;                 /* MeSsage Dispatch structure */

    /* This structure contains all of the information that a window */
    /* procedure passes to DispMessage in order to define the message */
    /* dispatching behavior for the window. */
typedef struct _MSDI
{
    int  cmsd;          /* Number of message dispatch structs in rgmsd */
    MSD *rgmsd;         /* Table of message dispatch structures */
    EDWP edwp;          /* Type of default window handler needed. */
} MSDI, FAR *LPMSDI;   /* MeSsage Dipatch Information */

    /* This structure maps command IDs to command handling functions. */
typedef struct _CMD
{
    WORD   wCommand;
    PFNCMD pfncmd;
} CMD;                 /* CoMmand Dispatch structure */

    /* This structure contains all of the information that a command */
    /* message procedure passes to DispCommand in order to define the */
    /* command dispatching behavior for the window. */
typedef struct _CMDI
{
    int  ccmd;          /* Number of command dispatch structs in rgcmd */
    CMD *rgcmd;         /* Table of command dispatch structures */
    EDWP edwp;          /* Type of default window handler needed. */
} CMDI, FAR *LPCMDI;   /* CoMmand Dispatch Information */

    /* Message and command dispatching functions.  They look up messages */
    /* and commands in the dispatch tables and call the appropriate handler */
    /* function. */
LRESULT DispMessage(LPMSDI, HWND, UINT, WPARAM, LPARAM);
LRESULT DispCommand(LPCMDI, HWND, WPARAM, LPARAM);

    /* Message dispatch information for the main window */
MSDI msdiMain;
    /* Command dispatch information for the main window */
CMDI cmdiMain;

/* Function protoptype for Creating Toolbar */
BOOL CreateTBar(HWND);

/* Function prototypes for status bar creation */
BOOL CreateSBar(HWND);
void InitializeStatusBar(HWND);
void UpdateStatusBar(LPSTR, WORD, WORD);


/* Various */
void ChangePage(int);
void ReadHelp(HWND, LPTSTR); 
void SizeClientWindow(HWND);
int ChooseShrink();
void ChangeZoom(int);
DWORD WINAPI ViewLogSentinel(LPVOID);
void SetScrollBars(HWND);
void DispVersion();
