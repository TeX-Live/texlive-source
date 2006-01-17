/* 
   winhtex.c : implementation file
   Time-stamp: "99/07/23 17:33:42 popineau"
   
   Copyright (C) 1999
      Fabrice Popineau <Fabrice.Popineau@supelec.fr>

   This file is part of Windvi.

   Windvi is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   
   Windvi is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.
   
   You should have received a copy of the GNU General Public License
   along with Windvi; if not, write to the Free Software Foundation, 59
   Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#include "wingui.h"
#include "xdvi-config.h"
#include <wininet.h>

#include <kpathsea/c-proto.h>

static HINTERNET hInternetSession = NULL; 

static BOOL init_htex_features()
{
    /*    __asm int 3; */
  if (!hInternetSession) {
    hInternetSession = InternetOpen(_T("Windvi"), 
				    INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0); 
    if (!hInternetSession) { 
      fprintf(stderr, "Unable to open an Internet Session !n");
    }
  }
  return hInternetSession != NULL;
}

/*
  Windows can get rid of all the libwww stuff, because
  there is no need much than opening remote files, which
  can be done through the ShellExecute() call.
  Anyway, to be still compatible with the code written here, 
  some of the libwww functions have been rewritten.
*/

#include "winhtex.h"
#include <kpathsea/c-pathch.h>

/*
**  Search the URL and determine whether it is a relative or absolute URL.
**  We check to see if there is a ":" before any "/", "?", and "#". If this
**  is the case then we say it is absolute. Otherwise it is relative.
*/
BOOL HTURL_isAbsolute (const char * url)
{    
  /* Given the context, we can't let windvi believe that
     c:/foo.dvi is an absolute url ... */
  if (NAME_BEGINS_WITH_DEVICE(url))
    return NO;

  if (url) {	
    const char * ptr = url;
    while (*ptr) {
      if (*ptr == ':') return YES;
      if (*ptr == '/' || *ptr == '?' || *ptr == '#') break;
      ptr ++;
    }
  }	
  return NO;
}

int
invokeviewer(char *filename)
{
  int ret;
  ret = ShellExecute(NULL, "open", dvi_name, NULL, NULL, SW_SHOWNORMAL);
  if (ret <= 0)
    return 0;
  else 
    return 1;
}

char * HTParse (const char *aName, const char *relatedName, int wanted)
{
  int ret;
  char *result, *abs, *rel;
  DWORD len;
  BOOL bAgain = FALSE; 
  
#if 0
  if (aName && *aName) {
      abs = xmalloc(strlen(aName) + 2);
      strcpy(abs, aName);
      strcat(abs, "/");
  }
  else {
    /* FIXME: what does mean a null base url ? */
    abs = xstrdup("");
  }
#else
  abs = xstrdup(aName ? aName : "");
#endif
  rel = (relatedName ? relatedName : "");

  len = strlen(abs) + strlen(rel) + 45;
  result = xmalloc(len*sizeof(char));

#if 0
  fprintf(stderr, "HTParse called with aName = %s and relatedName = %s\n",
	  aName, relatedName);
#endif
  do {
    if (*rel) {
      ret = InternetCombineUrl(abs, rel, result, &len, ICU_BROWSER_MODE);
    }
    else {
      ret = InternetCanonicalizeUrl(abs, result, &len, ICU_BROWSER_MODE);
    }
    bAgain = FALSE;
    if (!ret) {
      switch (GetLastError()) {
      case ERROR_INSUFFICIENT_BUFFER:
#if 0
	fprintf(stderr, "InternetCombineUrl: buffer was not long enough (needed %d)\n", len+1);
#endif
	result = xrealloc(result, len+1);
	bAgain = TRUE;
	break;
      case ERROR_BAD_PATHNAME:
	fprintf(stderr, "InternetCombineUrl failed, one of the path names is bad:\n\t%s\n\t%s\n",
		aName, relatedName);
	break;
      case ERROR_INTERNET_INVALID_URL:
	fprintf(stderr, "InternetCombineUrl failed, url %s is malformed.\n", aName);
	break;
      case ERROR_INVALID_PARAMETER:
	fprintf(stderr, "InternetCombineUrl failed with `invalid parameter'.\n");
	break;	
      default:
	break;
      }
    }
  } while (bAgain);
  
  /* abs has always been malloc'ed */
  if (abs)
    free(abs);

  if (!ret) {
    /* FIXME: Try this poor thing ! */
    sprintf(result, "%s/%s", aName, relatedName);
  }
  
#if 0
  fprintf(stderr, "HTParse: result is %s\n", result);
#endif
  return result;    
}

int
www_fetch(char *url, char *savefile)
{    
  HINTERNET hUrlDump;
  extern HINTERNET hInternetSession; 
  DWORD dwSize=TRUE;
  LPSTR lpszData;
  LPSTR lpszOutPut;
  LPSTR lpszHolding;
  int nCounter=1;
  int nBufferSize;
  DWORD BigSize=8000;
  FILE *fSave;

  if ((fSave = fopen(savefile, "wb")) == NULL) {
    perror("www_fetch");
    return 0;
  }

  hUrlDump = InternetOpenUrl(hInternetSession, url, NULL, 0, 
			     INTERNET_FLAG_RAW_DATA, 0);
  if (!hUrlDump) {
    Win32Error("www_fetch/InternetOpenUrl");
    fclose(fSave);
    return 0;
  }

  do {

    /* Read the data */
    if(InternetReadFile(hUrlDump,(LPVOID)lpszData,BigSize,&dwSize) == FALSE) {
      Win32Error("www_fetch/InternetReadFile");
      break;
    }
    else {
      fwrite(lpszData, sizeof(char), dwSize , fSave);
      if (dwSize == 0) {
	break;
      }
    }
  }
  while (TRUE);
  
  /* Close the HINTERNET handle */
  InternetCloseHandle(hUrlDump);
  
  /* Set the cursor back to an arrow */
  SetCursor(LoadCursor(NULL,IDC_ARROW));
  
  /* Return */
  return TRUE;
}

void
htex_cleanup(int arg)
{
	/* Delete all the temp files we created */
	for (;nURLs>0; nURLs--) {
	  /* fprintf(stderr,"htex: Unlinking %s\n",filelist[nURLs-1].file); */
	  unlink(filelist[nURLs-1].file);
	}
	if (hInternetSession) 
	  InternetCloseHandle(hInternetSession);
}
