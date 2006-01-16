/**************************************************************************
 *
 *  ttconfig.h                                                1.2
 *
 *    Configuration settings header file.
 *
 *  Copyright 1996-2002 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 *  Notes:
 *
 *    All the configuration #define statements have been gathered in
 *    this file to allow easy check and modification.
 *
 *    The platform specificities are gathered in arch/<sys>/ft_conf.h
 *    which is included by this file.
 *
 *  Changes between 1.2 and 1.1 :
 *
 *  - the EXPORT_DEF/EXPORT_FUNC/LOCAL_DEF/LOCAL_FUNC mechanism have
 *    replaced with a more elaborate one (with FT_ before) that allows
 *    addition of qualifiers _after_ the type, which is needed for DLLs.
 *
 *************************************************************************/

#ifndef TTCONFIG_H
#define TTCONFIG_H



/* --------------- auto configuration ---------------------------------- */

/*************************************************************************/
/* Here we include the file ft_conf.h for system dependent stuff.        */
/* The specific makefile is responsible for providing the right path to  */
/* this file.                                                            */

#include "ft_conf.h"


/*************************************************************************/
/*                                                                       */
/* Some more settings used to be here, but they are now chosen           */
/* in the system-dependent file arch/<system>/ft_conf.h                  */



/* --------------- special debugging ----------------------------------- */

/*************************************************************************/
/* Define this if you want to generate a special debug version of the    */
/* rasterizer.  This will progressively draw the glyphs while the        */
/* computations are done directly on the graphics screen... (with        */
/* inverted glyphs).                                                     */
/*                                                                       */
/* Use it at your own risk!  It is not maintained currently.             */
/*                                                                       */
/* IMPORTANT: This is reserved to developers willing to debug the        */
/*            rasterizer, which seems working very well in its           */
/*            current state...                                           */

/* #define DEBUG_RASTER */


/*************************************************************************/
/* Define this to have a simple debugger version of RunIns().            */
/*                                                                       */
/* Use it at your own risk!  It is not maintained currently.             */

/* #define DEBUG_INTERPRETER */


/*************************************************************************/
/* Define this to have some housekeeping of allocation and deallocation. */
/*                                                                       */
/* Please note that probably not all OS-specific versions of ttmemory.c  */
/* provide this functionality.                                           */

/* #define DEBUG_MEMORY */


/*************************************************************************/
/* Define this to have bounds checking for file buffer frames.           */
/*                                                                       */
/* Please note that probably not all OS-specific versions of ttfile.c    */
/* provide this functionality.                                           */

/* #define DEBUG_FILE */



/* --------------- arithmetic and processor support -------------------- */

/*************************************************************************/
/* define ALIGNMENT to your processor/environment preferred alignment    */
/* size. A value of 8 should work on all current processors, even        */
/* 64-bits ones.                                                         */

#ifndef ALIGNMENT
#define ALIGNMENT 8
#endif



/* --------------- compilation model and naughty processors support ---- */

/*************************************************************************/
/* Under normal operation, the library is compiled into a single object  */
/* file. This gets rids of all the external symbols defined in each      */
/* component interface, and de-pollutes the name-space.                  */
/*                                                                       */
/* However, you can still compile components into separate object files, */
/* and gather all of them into the library. But you should then declare  */
/* some functions as externally visible.                                 */
/*                                                                       */
/* We use two macros, namely FT_INTERNAL_FUNC and FT_INTERNAL_DEF, which */
/* apply to functions that are internal to the engine, and should never  */
/* be seen or linked by a client application.                            */
/*                                                                       */
/* FT_INTERNAL_DEF   used in header (.h) files, to define a function     */
/*                   that will be seen by other components. This         */
/*                   translates to "extern" in normal mode, and to       */
/*                   "static" in single-object mode.                     */
/*                                                                       */
/* FT_INTERNAL_FUNC  used in implementation (.c) files, just before      */
/*                   the function body. This translates to nothing       */
/*                   in normal mode, and to "static" in single-object    */
/*                   mode.                                               */
/*                                                                       */
/* It is called a "MAKE_OPTION" because the macro must be defined in the */
/* Makefile, rather than this one. It allows any developer to quickly    */
/* switch from one mode to the other without messing with "ttconfig.h"   */
/* each time.                                                            */
/*                                                                       */
/* The following macros are needed to compile the library with some      */
/* 16-bit (the infamous `_cdecl', `__loadds', `_export', `FAR', etc.     */
/* stuff) or perhaps C++ compilers (extern "C"). The typename is passed  */
/* as an argument, which allows the macro to add stuff _after_ the       */
/* typename, which is required by 16-bit msdos compiler syntax.          */

#ifndef TT_MAKE_OPTION_SINGLE_OBJECT

#ifndef FT_INTERNAL_FUNC
#define FT_INTERNAL_FUNC( type )   type
#endif
#ifndef FT_INTERNAL_DEF
#define FT_INTERNAL_DEF(  type )   extern type
#endif

#else /* ! TT_MAKE_OPTION_SINGLE_OBJECT */

#ifndef FT_INTERNAL_FUNC
#define FT_INTERNAL_FUNC( type )   static type
#endif
#ifndef FT_INTERNAL_DEF
#define FT_INTERNAL_DEF(  type )   static type
#endif

#endif /* TT_MAKE_OPTION_SINGLE_OBJECT */

/* This is for variables; default is using the same as functions. */
#ifndef FT_INTERNAL_DECL
#define FT_INTERNAL_DECL( type )  FT_INTERNAL_DEF(  type )
#endif
#ifndef FT_INTERNAL_VAR
#define FT_INTERNAL_VAR(  type )  FT_INTERNAL_DECL( type )
#endif


/*************************************************************************/
/* Define FT_EXPORT_DEF and FT_EXPORT_FUNC as needed to build e.g. a DLL.*/
/* All variables and functions visible from outside have these prefixes. */

/* Backward compatibility only. Don't use for new code. */
#ifdef EXPORT_DEF
#define FT_EXPORT_DEF(  type )   EXPORT_DEF  type
#endif
#ifdef EXPORT_FUNC
#define FT_EXPORT_FUNC( type )   EXPORT_FUNC type
#endif
/* End of backward compatibility section. */

#ifndef FT_EXPORT_DEF
#define FT_EXPORT_DEF(  type )   extern type
#endif

#ifndef FT_EXPORT_FUNC
#define FT_EXPORT_FUNC( type )   type
#endif

/* This is for variables; default is using the same as functions. */
#ifndef FT_EXPORT_DECL
#define FT_EXPORT_DECL( type )  FT_EXPORT_DEF(  type )
#endif
#ifndef FT_EXPORT_VAR
#define FT_EXPORT_VAR(  type )  FT_EXPORT_DECL( type )
#endif



/* --------------- miscellaneous --------------------------------------- */

/*************************************************************************/
/* The number of extensions available.  Don't change this value          */
/* except if you add new extensions to the engine.                       */

#define TT_MAX_EXTENSIONS  8



/* --------------- automatic setup -- don't touch ---------------------- */

/*************************************************************************/
/* If HAVE_TT_TEXT is defined we don't provide a default typedef for     */
/* defining TT_Text.                                                     */

#ifndef HAVE_TT_TEXT
#define HAVE_TT_TEXT
  typedef char  TT_Text;
#endif


/*************************************************************************/
/* We define NULL in case it's not defined yet.  The default             */
/* location is stdlib.h.                                                 */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif


/*************************************************************************/
/* Some systems can't use vfprintf for error messages on stderr; if      */
/* HAVE_PRINT_FUNCTION is defined, the Print macro must be supplied      */
/* externally (having the same parameters).                              */
/*                                                                       */
/* This is only used by the "ttdebug" component, which should be linked  */
/* to the engine only in debug mode.                                     */

#if defined( DEBUG_LEVEL_TRACE ) || defined( DEBUG_LEVEL_ERROR )
#ifndef HAVE_PRINT_FUNCTION
#define Print( format, ap )  vfprintf( stderr, (format), (ap) )
#endif
#endif


/* --------------- internal (developer) configuration toggles ---------- */

/*************************************************************************/
/* Do not undefine this configuration macro. It is now a default that    */
/* must be kept in all release builds.                                   */
#undef TT_STATIC_INTERPRETER


/*************************************************************************/
/* Define this if you want to generate a static raster.  This makes a    */
/* non re-entrant version of the scan-line converter, which is about     */
/* 10% faster and 50% bigger than an indirect one!                       */
/*                                                                       */
/* NOTE : this is different from TT_CONFIG_OPTION_STATIC_RASTER.         */
/*        This one only affects the calling convention.                  */
#undef TT_STATIC_RASTER


#endif /* TTCONFIG_H */

/* END */
