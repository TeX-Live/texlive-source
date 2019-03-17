/*

Simple tray menu

Originally written in 2011 by Tomasz M. Trzeciak, Public Domain

2019 updates, by Siep Kroonenberg:
- added option to open data files with default application
- updated menu content

compiling with gcc (size optimized):
echo 1 ICON "tl-tray-menu.ico">tl-tray-menu.rc
windres tl-tray-menu.rc tl-tray-menu-rc.o
gcc -Os -s -mwindows -o tl-tray-menu.exe tl-tray-menu-rc.o tl-tray-menu.c

*/

#define _WIN32_IE 0X0500 // minimum support for tray baloon (IE5)

#include <windows.h>
#include <shellapi.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_STR 32768

static char strBuf[MAX_STR];

#define DIE(...) { \
  _snprintf( strBuf, 4*MAX_PATH, __VA_ARGS__ ); \
  MessageBox( NULL, strBuf, "ERROR!", MB_ICONERROR | MB_SETFOREGROUND );\
  return 1; \
}

#define IDI_ICON       1    // resource number of icon 
#define IDI_TRAY       100
#define WM_TRAYMSG     101

#define MAX_MENU_ENTRIES 31
#define MENU_CONFIG      ( MAX_MENU_ENTRIES + 1 )
#define MENU_EXIT        ( MAX_MENU_ENTRIES + 2 )

// buffer holding menu labels and commands 
// (used only with config file)
static char menuStrings[MAX_STR]; 

// default menu labels
char *menuLabels[MAX_MENU_ENTRIES] = {
"&Package Manager",
"&Documentation",
"&Editor", 
"&Command Prompt",
NULL };

// default menu commands
char *menuCommands[MAX_MENU_ENTRIES] = {
"bin\\win32\\tlshell.exe",
"doc.html",
"bin\\win32\\texworks.exe",
"tlpkg\\installer\\tl-cmd.bat",
NULL };

char configInfo[4*MAX_PATH] = "\
The menu can be customized with a configuration file.\n\
For an example and instructions see:\n";

HMENU hPopMenu;
NOTIFYICONDATA nid;

// has s executable extension according to PATHEXT?
int is_exe ( char * s ) {
  int i, j, extl;
  char * pathext, * ext, * t, * t1;
  if ( !s || !(*s)) DIE( "is_exe invoked with NULL or empty string" );
  pathext = getenv( "PATHEXT" );
  if (!pathext) return 1;
  ext = strrchr( s, '.' );
  if (!ext) return 1;
  extl = strlen( ext );
  t = s;
  while ( 1 ) {
    t1 = strchr( t, ';' );
    if ( !t1 ) t1 = s + strlen( s );
    if ( t1 - t == extl && !_strnicmp( ext, t, extl )) {
      return 1;
    } else if ( *t1 ) {
      t = ++t1;
    } else {
      return 0;
    }
  }
  return 0;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch ( msg )             
  {
    case WM_CREATE:
    {
      hPopMenu = CreatePopupMenu();
      int i;
      for ( i = 0; menuLabels[i]; i++ )
      {
        if ( menuCommands[i] )
          AppendMenu( hPopMenu, MF_STRING, i, menuLabels[i] );
      }
      AppendMenu( hPopMenu, MF_SEPARATOR, 0, NULL );
      AppendMenu( hPopMenu, MF_STRING, MENU_CONFIG,  "More..." );
      AppendMenu( hPopMenu, MF_STRING, MENU_EXIT,    "E&xit" );
    }
    break;
    
    case WM_TRAYMSG:
    {
      switch ( lParam )
      {
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        {
          POINT pnt;
          GetCursorPos( &pnt );
          SetForegroundWindow( hWnd ); // needed to get keyboard focus
          TrackPopupMenu( hPopMenu, TPM_LEFTALIGN, pnt.x, pnt.y, 0, hWnd, NULL );
        }
        break;
      }
    }
    break;
    
    case WM_COMMAND:
    {
      switch ( LOWORD( wParam ) )
      {
        case MENU_CONFIG:
          MessageBox( NULL, configInfo, nid.szTip, MB_SETFOREGROUND );
        break;

        case MENU_EXIT:
          Shell_NotifyIcon( NIM_DELETE, &nid );
          ExitProcess(0);
        break;

        default:
        {
          if (is_exe( menuCommands[LOWORD(wParam )])) {
            STARTUPINFO si;
            PROCESS_INFORMATION pi;
            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);
            //si.dwFlags = STARTF_USESHOWWINDOW;
            //si.wShowWindow = SW_HIDE ;
            ZeroMemory( &pi, sizeof(pi) );
            if( !CreateProcess(
              NULL,     // module name (uses command line if NULL)
              menuCommands[LOWORD(wParam)],     // command line
              NULL,     // process security atrributes
              NULL,     // thread security atrributes
              TRUE,     // handle inheritance
              0,        // creation flags, e.g. CREATE_NO_WINDOW,
                        // DETACHED_PROCESS
              NULL,     // pointer to environment block (uses parent if NULL)
              NULL,     // starting directory (uses parent if NULL)
              &si,      // STARTUPINFO structure
              &pi )     // PROCESS_INFORMATION structure
            ) DIE( "Failed to spawn command (error code %d):\n%s", 
                   GetLastError(), menuCommands[LOWORD(wParam)] );
          } else {
            // data file; try to open with default application
            ShellExecute( NULL, NULL, menuCommands[LOWORD(wParam)],
                NULL, NULL, SW_SHOWNORMAL );
          }
        }
      }
    }
    break;

    default:
      return DefWindowProc( hWnd, msg, wParam, lParam );

  }

  return 0;
}

int APIENTRY WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR     lpCmdLine,
        int       nCmdShow )
{

  // scratch variables

  char *s;
  
  // get file name of this executable
  
  static char dirSelf[MAX_PATH];
  DWORD nchars = GetModuleFileName(NULL, dirSelf, MAX_PATH);
  if ( !nchars || ( nchars == MAX_PATH ) ) DIE( "Cannot get own path." );
  if ( s = strrchr( dirSelf, '\\' ) ) *s = '\0'; // remove file name part
  
  // set current directory 
  
  if ( !SetCurrentDirectory( dirSelf ) ) 
    DIE( "Failed to change current directory to:\n%s", dirSelf );
  
  // prepend bin/win32 to PATH
  
  strcpy( strBuf, dirSelf );
  if ( strlen( dirSelf ) + strlen( "\\bin\\win32;" )
      + strlen( getenv( "PATH" )) >= MAX_STR )
    DIE( "Path getting too long" );
  strcat( strBuf, "\\bin\\win32;" );
  strcat( strBuf, getenv( "PATH" ) );
  SetEnvironmentVariable( "PATH", strBuf );
 
  // configuration
  
  static char fileConfig[MAX_PATH];
  s = dirSelf + strlen(dirSelf) + 1;
  strcpy( fileConfig, s );
  if ( s = strrchr( fileConfig, '.' ) ) *s = '\0'; // remove file extension part
  strcat( fileConfig, ".ini" );
  
  strcat( configInfo, dirSelf );
  strcat( configInfo, "\\tlpkg\\installer\\" );
  strcat( configInfo, fileConfig );
  
  if ( GetFileAttributes( fileConfig ) == INVALID_FILE_ATTRIBUTES ) {
  
    // validate default configuration (menuCommands should be existing files)
    
    int i;
    for ( i = 0; menuLabels[i]; i++ ) 
      if ( GetFileAttributes( menuCommands[i] ) == INVALID_FILE_ATTRIBUTES )
        menuCommands[i] = NULL;

  } else {

    //  read menu configuration from ini file

    FILE *CONFIG = fopen( fileConfig, "rt" );
    if ( CONFIG == NULL )
      DIE( "Failed to open configuration file:\n%s\\%s", dirSelf, fileConfig );

    char strSect[] = "[menu]";
    int menuItem = -1;
    int i = 0;
    int lineNo;
    for ( lineNo = 1 ; fgets( strBuf, MAX_STR, CONFIG ) 
                       && menuItem < MAX_MENU_ENTRIES ; lineNo++ )
    {
      if ( menuItem < 0 )
      {
        if ( *strBuf == '[' && 
             strncmp( strBuf, strSect, strlen( strSect ) ) == 0 ) menuItem++;
        continue;
      }
      if ( *strBuf == ';' ) continue; // skip comments
      if ( *strBuf == '[' ) break;    // next section
      s = &menuStrings[i];
      i += ExpandEnvironmentStrings( strBuf, s, MAX_STR - i );
      if ( i >= MAX_STR ) 
        DIE( "Exceeded 32KB limit for configuration." );
      if ( i-2 && menuStrings[i-2] == '\n' ) { menuStrings[i-2] = '\0'; i--; }
      menuLabels[menuItem] = s;
      while ( *s && *s != '=' ) s++;
      if ( *s == '\0' ) 
        DIE( "Missing '=' on line %d in configuration file:\n%s\\%s", 
             lineNo, dirSelf, fileConfig );
      *s++ = '\0'; // assign first, then increment
      menuCommands[menuItem] = s;
      menuItem++;
    }
    if ( menuItem < 0 ) 
      DIE( "Missing [menu] section in configuration file:\n%s\\%s", 
           dirSelf, fileConfig );
    menuLabels[menuItem] = NULL;
    menuCommands[menuItem] = NULL;

    fclose( CONFIG );

  }
  
  // create a hidden window for messaging

  MSG msg;
  WNDCLASS wc;
  HWND hWnd;
  char szAppName[] = "TrayMenu";

  ZeroMemory( &wc, sizeof wc );
  wc.hInstance     = hInstance;
  wc.lpszClassName = szAppName;
  wc.lpfnWndProc   = (WNDPROC)WndProc;
  wc.style         = 0;
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.hIcon         = LoadIcon( NULL, IDI_APPLICATION );
  wc.hCursor       = LoadCursor( NULL, IDC_ARROW );

  if ( FALSE == RegisterClass( &wc ) ) DIE( "Failed to register window class" );

  hWnd = CreateWindow(
    szAppName,
    "",
    0,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    0,
    0,
    hInstance,
    0);

  if ( hWnd == NULL ) DIE( "Failed to create window" );

  // tray icon stuff
  
  nid.cbSize = sizeof nid;
  nid.hWnd = hWnd;
  nid.uID = IDI_TRAY;
  nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
  nid.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_ICON ) ); 
  nid.uCallbackMessage = WM_TRAYMSG;
  strcpy( nid.szTip, "TeX Live Menu" );
#if (_WIN32_IE >= 0X0500)
  nid.uFlags |= NIF_INFO;
  nid.dwInfoFlags = NIIF_INFO;
  strcpy( nid.szInfo, "Click on the tray icon\nto activate the menu." );
  strcpy( nid.szInfoTitle, nid.szTip );
#endif
  Shell_NotifyIcon( NIM_ADD, &nid );
  
  // main message loop:
  
  while ( GetMessage( &msg, NULL, 0, 0 ) > 0 )
  {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
  }

  return msg.wParam;
}

