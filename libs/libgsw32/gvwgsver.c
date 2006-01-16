/* Copyright (C) 2000, Ghostgum Software Pty Ltd.  All rights reserved.
  
  This file is part of GSview.
  
  This program is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  Refer to the GSview Free Public Licence 
  (the "Licence") for full details.
  
  Every copy of GSview must include a copy of the Licence, normally in a 
  plain ASCII text file named LICENCE.  The Licence grants you the right 
  to copy, modify and redistribute GSview, but only under certain conditions 
  described in the Licence.  Among other things, the Licence requires that 
  the copyright notice and this notice be preserved on all copies.
*/

/* gvwgsver.c */

#ifdef DUMP_GSVER
#include <windows.h>
#include <stdio.h>
#else
#include <windows.h>
#endif
#include <stdlib.h>
#include "gvwgsver.h"

#define GS_PRODUCT_AFPL "AFPL Ghostscript"
#define GS_PRODUCT_ALADDIN "Aladdin Ghostscript"
#define GS_PRODUCT_GNU "GNU Ghostscript"

// Get Ghostscript versions for given product.
// Store results starting at pver + 1 + offset.
// Returns total number of versions in pver.
static int get_gs_versions_product(int *pver, int offset, 
    const char *gs_productfamily)
{
    // First find out how many versions of Ghostscript are available.
    HKEY hkey;
    DWORD cbData;
    HKEY hkeyroot;
    TCHAR key[256];
    int ver;
    TCHAR *p;
    int n = 0;

    wsprintf(key, TEXT("Software\\%s"), gs_productfamily);
    hkeyroot = HKEY_LOCAL_MACHINE;
    if (RegOpenKeyEx(hkeyroot, key, 0, KEY_READ, &hkey) == ERROR_SUCCESS) {
	// Now enumerate the keys
	cbData = sizeof(key) / sizeof(TCHAR);
	while (RegEnumKey(hkey, n, key, cbData) == ERROR_SUCCESS) {
	    n++;
	    ver = 0;
	    p = key;
	    while (*p && (*p!='.')) {
		ver = (ver * 10) + (*p - '0')*100;
		p++;
	    }
	    if (*p == '.')
		p++;
	    if (*p) {
		ver += (*p - '0') * 10;
		p++;
	    }
	    if (*p)
		ver += (*p - '0');
	    if (n + offset < pver[0])
		pver[n+offset] = ver;
	}
    }
    return n+offset;
}

// Query registry to find which versions of Ghostscript are installed.
// Return version numbers in an integer array.   
// On entry, the first element in the array must be the array size 
// in elements.
// If all is well, TRUE is returned.
// On exit, the first element is set to the number of Ghostscript
// versions installed, and subsequent elements to the version
// numbers of Ghostscript.
// e.g. on entry {5, 0, 0, 0, 0}, on exit {3, 550, 600, 596, 0}
// Returned version numbers may not be sorted.
//
// If Ghostscript is not installed at all, return FALSE
// and set pver[0] to 0.
// If the array is not large enough, return FALSE 
// and set pver[0] to the number of Ghostscript versions installed.
BOOL get_gs_versions(int *pver)
{
    // First find out how many versions of Ghostscript are available.
    int n;
    if (pver == (int *)NULL)
	    return FALSE;

    n = get_gs_versions_product(pver, 0, GS_PRODUCT_AFPL);
    n = get_gs_versions_product(pver, n, GS_PRODUCT_ALADDIN);
    n = get_gs_versions_product(pver, n, GS_PRODUCT_GNU);

    if (n >= pver[0]) {
	pver[0] = n;
	return FALSE;	// too small
    }

    if (n == 0) {
	pver[0] = 0;
	return FALSE;	// not installed
    }
    pver[0] = n;
    return TRUE;
}

 
/*
 * Get a named registry value.
 * Key = hkeyroot\\key, named value = name.
 * name, ptr, plen and return values are the same as in gp_getenv();
 */

int 
gp_getenv_registry(HKEY hkeyroot, const char *key, const char *name, 
    char *ptr, int *plen)
{
    HKEY hkey;
    DWORD cbData, keytype;
    BYTE b;
    LONG rc;
    BYTE *bptr = (BYTE *)ptr;

    if (RegOpenKeyEx(hkeyroot, key, 0, KEY_READ, &hkey)
	== ERROR_SUCCESS) {
	keytype = REG_SZ;
	cbData = *plen;
	if (bptr == (BYTE *)NULL)
	    bptr = &b;	/* Registry API won't return ERROR_MORE_DATA */
			/* if ptr is NULL */
	rc = RegQueryValueEx(hkey, (char *)name, 0, &keytype, bptr, &cbData);
	RegCloseKey(hkey);
	if (rc == ERROR_SUCCESS) {
	    *plen = cbData;
	    return 0;	/* found environment variable and copied it */
	} else if (rc == ERROR_MORE_DATA) {
	    /* buffer wasn't large enough */
	    *plen = cbData;
	    return -1;
	}
    }
    return 1;	/* not found */
}


static BOOL get_gs_string_product(int gs_revision, const char *name, 
    char *ptr, int len, const char *gs_productfamily)
{
    /* If using Win32, look in the registry for a value with
     * the given name.  The registry value will be under the key
     * HKEY_CURRENT_USER\Software\AFPL Ghostscript\N.NN
     * or if that fails under the key
     * HKEY_LOCAL_MACHINE\Software\AFPL Ghostscript\N.NN
     * where "AFPL Ghostscript" is actually gs_productfamily
     * and N.NN is obtained from gs_revision.
     */

    int code;
    char key[256];
    char dotversion[16];
    int length;
    DWORD version = GetVersion();

    if (((HIWORD(version) & 0x8000) != 0)
	  && ((HIWORD(version) & 0x4000) == 0)) {
	/* Win32s */
	return FALSE;
    }


    if (gs_revision % 100 == 0)
	wsprintf(dotversion, "%d.0", (int)(gs_revision/100));
    else
	wsprintf(dotversion, "%d.%02d", 
	    (int)(gs_revision / 100), (int)(gs_revision % 100));
    wsprintf(key, "Software\\%s\\%s", gs_productfamily, dotversion);

    length = len;
    code = gp_getenv_registry(HKEY_CURRENT_USER, key, name, ptr, &length);
    if ( code == 0 )
	return TRUE;	/* found it */

    length = len;
    code = gp_getenv_registry(HKEY_LOCAL_MACHINE, key, name, ptr, &length);

    if ( code == 0 )
	return TRUE;	/* found it */

    return FALSE;
}

BOOL get_gs_string(int gs_revision, const char *name, char *ptr, int len)
{
    if (get_gs_string_product(gs_revision, name, ptr, len, GS_PRODUCT_AFPL))
	return TRUE;
    if (get_gs_string_product(gs_revision, name, ptr, len, GS_PRODUCT_ALADDIN))
	return TRUE;
    if (get_gs_string_product(gs_revision, name, ptr, len, GS_PRODUCT_GNU))
	return TRUE;
    return FALSE;
}



// Set the latest Ghostscript EXE or DLL from the registry
BOOL
find_gs(char *gspath, int len, int minver, BOOL bDLL)
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
    if (gsver < minver)	// minimum version (e.g. for gsprint)
	return FALSE;
    
    if (!get_gs_string(gsver, "GS_DLL", buf, sizeof(buf)))
	return FALSE;

    if (bDLL) {
	strncpy(gspath, buf, len-1);
	return TRUE;
    }

    p = strrchr(buf, '\\');
    if (p) {
	p++;
	*p = 0;
	strncpy(p, "gswin32c.exe", sizeof(buf)-1-strlen(buf));
	strncpy(gspath, buf, len-1);
	return TRUE;
    }

    return FALSE;
}


#ifdef DUMP_GSVER
int main(int argc, char *argv[])
{
    BOOL flag;
    int n;
    int ver[6];
    int i;
    char buf[256];

    ver[0] = sizeof(ver) / sizeof(int);
    flag = get_gs_versions(ver);
    printf("Versions: %d\n", ver[0]);

    if (flag == FALSE) {
	printf("get_gs_versions failed, need %d\n", ver[0]);
	return 1;
    }

    for (i=1; i <= ver[0]; i++) {
	printf(" %d\n", ver[i]);
	if (get_gs_string(ver[i], "GS_DLL", buf, sizeof(buf)))
	    printf("   GS_DLL=%s\n", buf);
	if (get_gs_string(ver[i], "GS_LIB", buf, sizeof(buf)))
	    printf("   GS_LIB=%s\n", buf);
    }
    return 0;
}
#endif
