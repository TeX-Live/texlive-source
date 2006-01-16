#ifndef _GS32LIB_H_
#define _GS32LIB_H_

#pragma warning( disable : 4007 4096 4018 4244 )  

#ifdef  __cplusplus
extern "C" {
#endif

#include <gsdll.h>

#if defined(WIN32)
# if defined(GSW32_DLL)
#  if defined(MAKE_GSW32_DLL)
#   define GSW32DLL __declspec( dllexport)
#  else
#   define GSW32DLL __declspec( dllimport)
#  endif
# else
#  define GSW32DLL
# endif
#else /* ! WIN32 */
# define GSW32DLL
#endif

/* Functions from ghostscr.c */
extern GSW32DLL char *gs_locate();
extern GSW32DLL int gs_version_cmp(const char *, const char *);
extern GSW32DLL BOOL gs_dll_initialize();
extern GSW32DLL BOOL gs_dll_release();
extern GSW32DLL char *gs_dir;
extern GSW32DLL char *gs_dll;
extern GSW32DLL char *gs_lib;
extern GSW32DLL char *gs_revision;
extern GSW32DLL char *gs_product;

extern GSW32DLL PFN_gsdll_init pgsdll_init;
extern GSW32DLL PFN_gsdll_execute_begin pgsdll_execute_begin;
extern GSW32DLL PFN_gsdll_execute_cont pgsdll_execute_cont;
extern GSW32DLL PFN_gsdll_execute_end pgsdll_execute_end;
extern GSW32DLL PFN_gsdll_exit pgsdll_exit;
extern GSW32DLL PFN_gsdll_draw pgsdll_draw;
extern GSW32DLL PFN_gsdll_get_bitmap_row pgsdll_get_bitmap_row;
extern GSW32DLL PFN_gsdll_lock_device pgsdll_lock_device;

extern GSW32DLL HANDLE hGsDll;

#ifdef  __cplusplus
}
#endif

#endif /* _GS32LIB_H_ */
