/* 

Ghostscript initialization for fpTeX.

Copyright (C) 1998, 99 Free Software Foundation, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/*
  This function returns the directory name where GS is installed
  and tries to make sure that either gswin32c.exe or gsdll32.dll
  will run as expected, by checking GS_LIB.

  Location of .exe or .dll :
  - look into the PATH for the dll, if found this will be the directory
  - look into the registry, warn user if a different version is found
  - check for GS_LIB in the environment
  - check for GS_LIB in the registry
  - else build a GS_LIB from rule of thumb.
*/

#include <string.h>
#include <win32lib.h>
#include <gs32lib.h>

#include "gvwgsver.h"

extern char *xstrdup(const char *);

#define GSDLL "gsdll32.dll"

/* GSW32DLL char *gs_dir = NULL; */
GSW32DLL char *gs_dll = NULL;
GSW32DLL char *gs_lib = NULL;
GSW32DLL char *gs_revision = NULL;
/*  GSW32DLL char *gs_product_values[] = { "AFPL Ghostscript", "Aladdin Ghostscript", "GNU Ghostscript" }; */
/*  GSW32DLL char *gs_product = NULL; */
/*  int gs_product_index; */

#define NB_ITEMS(x) (sizeof(x)/sizeof(x[0]))

/*
 *	Load the ghostscript dll gswin32.dll.                       
 *	See the dll.txt file in the ghostscript source distribution.
 *      We will try to make gs draw directly onto the screen/printer
 *      device, as it is using SetDIBitsToDevice(), and it may use
 *      more colors than the mem device has.
 *      This may not be the more efficient, and we will have to see
 *      how it adapts to printing. But there may be some overhead
 *      at getting the whole dib at scale 1.
 */
GSW32DLL PFN_gsdll_init pgsdll_init;
GSW32DLL PFN_gsdll_execute_begin pgsdll_execute_begin;
GSW32DLL PFN_gsdll_execute_cont pgsdll_execute_cont;
GSW32DLL PFN_gsdll_execute_end pgsdll_execute_end;
GSW32DLL PFN_gsdll_exit pgsdll_exit;
GSW32DLL PFN_gsdll_draw pgsdll_draw;
GSW32DLL PFN_gsdll_get_bitmap_row pgsdll_get_bitmap_row;
GSW32DLL PFN_gsdll_lock_device pgsdll_lock_device;

GSW32DLL HANDLE hGsDll = 0;

const char *gslocpath[] = { "fonts", "..\\fonts", "..\\..\\fonts",
			    "lib", "..\\lib" };


int 
gs_version_cmp(const char *sv1, const char *sv2)
{
  /*
    Assuming version numbers of the form : xx.yy
  */

  int nv1x, nv1y, nv2x, nv2y;
  if (sscanf(sv1, "%d.%d", &nv1x, &nv1y) == 2
      && sscanf(sv2, "%d.%d", &nv2x, &nv2y) == 2) {
    if (nv1x == nv2x) {
      return nv1y - nv2y;
    }
    else {
      return nv1x - nv2x;
    }
  }
  else {
    return -1;
  }
}

// Set the latest Ghostscript EXE or DLL from the registry
BOOL
gs_registry_locate()
{
    int count;
    int *ver;
    int gsver;
    char buf[256];
    char *p;
    int i;

    DWORD version = GetVersion();
    if ( ((HIWORD(version) & 0x8000)!=0) && ((HIWORD(version) & 0x4000)==0) )
	return FALSE;  // win32s

    count = 1;
    get_gs_versions(&count);
    if (count < 1)
	return FALSE;
    ver = (int *)malloc((count+1)*sizeof(int));
    if (ver == (int *)NULL)
	return FALSE;
    ver[0] = count+1;
    if (!get_gs_versions(ver)) {
	free(ver);
	return FALSE;
    }
    gsver = 0;
    for (i=1; i<=ver[0]; i++) {
	if (ver[i] > gsver)
	    gsver = ver[i];
    }
    free(ver);
    
    gs_revision = xmalloc(sizeof("99.99"));
    gs_revision[0] = (int)(gsver / 100) + 48;
    gs_revision[1] = '.';
    gs_revision[2] = (int)((gsver / 10) % 10) + 48;
    gs_revision[3] = (int)(gsver % 10) + 48;
    gs_revision[4] = '\0';

    if (!get_gs_string(gsver, "GS_DLL", buf, sizeof(buf)))
	return FALSE;
    // just to make sure
    for(p = buf; p && *p; p++)
      *p = (*p == '/' ? '\\' : *p);
    
    gs_dll = xstrdup(buf);

    if (!get_gs_string(gsver, "GS_LIB", buf, sizeof(buf)))
      return FALSE;

    gs_lib = xstrdup(buf);

    return TRUE;
}

char * gs_locate()
{
  char data[MAXPATHLEN];
  DWORD data_len;
  char *fp;

  /* FIXME : Should also try to open and read
     gsview32.ini !
     FIXME : What policy should we use ?
     What if gs_dll is found in the registry and
     in the PATH ?
     Currently, it the PATH points to some gs version, then this
     is the one that will be used. But in this case, GS_LIB must
     be set too, else the registry will be looked for.
  */

  if (gs_registry_locate() == FALSE) {
    gs_revision = xstrdup("(unknown)");
    if (SearchPath(NULL, GSDLL, NULL, sizeof(data), data, &fp) != 0) {
      gs_dll = xstrdup(data);
    }
    if (gs_lib = getenv("GS_LIB"))
      gs_lib = xstrdup(gs_lib);
  }

  return gs_dll;
}

BOOL gs_dll_initialize()
{

  if (!gs_dll || !*gs_dll) {
    fprintf(stderr, "gs not initialized, dll not found.\n");
    return FALSE;
  }
  
  if ((hGsDll = LoadLibrary(gs_dll)) == NULL) {
    fprintf(stderr, "LoadLibrary: Can't load gs dll.");
    hGsDll = 0;
    return FALSE;
  }
  
  pgsdll_init = (PFN_gsdll_init)GetProcAddress(hGsDll, "gsdll_init");
  pgsdll_execute_begin = (PFN_gsdll_execute_begin)GetProcAddress(hGsDll, "gsdll_execute_begin");
  pgsdll_execute_cont = (PFN_gsdll_execute_cont)GetProcAddress(hGsDll, "gsdll_execute_cont");
  pgsdll_execute_end = (PFN_gsdll_execute_end)GetProcAddress(hGsDll, "gsdll_execute_end");
  pgsdll_exit = (PFN_gsdll_exit)GetProcAddress(hGsDll, "gsdll_exit");
  pgsdll_draw = (PFN_gsdll_draw)GetProcAddress(hGsDll, "gsdll_draw");
  pgsdll_get_bitmap_row = (PFN_gsdll_get_bitmap_row)GetProcAddress(hGsDll, "gsdll_get_bitmap_row");
  pgsdll_lock_device = (PFN_gsdll_lock_device)GetProcAddress(hGsDll, "gsdll_lock_device");
  if (!pgsdll_init || !pgsdll_execute_begin || !pgsdll_execute_cont
      || !pgsdll_execute_end || !pgsdll_exit || !pgsdll_draw
      || !pgsdll_get_bitmap_row || !pgsdll_lock_device) {
    fprintf(stderr, "Not all functions found in gs dll.\n");
    return FALSE;
  }
  
  return TRUE;
}

BOOL gs_dll_release()
{
  if (hGsDll) {
    FreeLibrary(hGsDll);
    hGsDll = 0;
    return TRUE;
  }

  return FALSE;
}
