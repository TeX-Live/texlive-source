#include "wingui.h"
#include "xdvi-config.h"

/*
  Displays a dialog box with version information
*/
void DispVersion()
{
  char  szFullPath[256];
  DWORD dwVerHnd;
  DWORD dwVerInfoSize;
  char szBuf[256];
  extern HWND hWndMain;
  extern HINSTANCE hInst;
  GetModuleFileName(hInst, szFullPath, sizeof(szFullPath));
  dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
  if (dwVerInfoSize) {
    /*  If we were able to get the information, process it: */
    HANDLE  hMem;
    LPVOID  lpvMem;
    char    szGetName[256];
    int     cchRoot;
    BOOL  fRet;
    UINT  cchVer = 0;
    LPSTR lszVer = NULL;
    
    hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
    lpvMem = GlobalLock(hMem);
    GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);
    lstrcpy(szGetName, "\\StringFileInfo\\040904E4\\ProductVersion");
    cchRoot = lstrlen(szGetName);
    fRet = VerQueryValue(lpvMem, szGetName, &lszVer, &cchVer);
    if (fRet && cchVer && lszVer) {
      wsprintf(szBuf, "(o)windvi %s", lszVer);
    }
    else {
      wsprintf(szBuf, "Failed to get product version number.");
    }
  }
  else {
    wsprintf(szBuf, "Failed to get version information.");
  }
  MessageBox(NULL, szBuf, 
	     "Version Information", MB_APPLMODAL | MB_ICONHAND | MB_OK);
  CleanExit(0);
}
