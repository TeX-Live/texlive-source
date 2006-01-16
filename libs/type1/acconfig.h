/* Define to determine the integer type to be used in bitmaps.  The type used
   will be "unsigned BMTYPE".  */
#define BMTYPE int

/* Define to the length (in bytes) of the above type.  */
#define BMBYTES 4

/* Define if your C compiler does not do string concatenation.  */
#undef CC_K_AND_R

/* Define if your system has STREAMS (and if X uses it).  */
#undef HAVE_STREAMS

/* Define if you are using SunOS 4.x.  */
#undef SUNOS4

/* Define if you are using Linux 2.1.xxx -- 2.2.8,
   or if you find it necessary.  */
#undef FLAKY_SIGPOLL

/* Define if your system has <poll.h> and poll().  */
#undef HAVE_POLL

/* Define if your system has waitpid().  */
#undef HAVE_WAITPID

/* Define if your system has putenv().  */
#undef HAVE_PUTENV

/* Define to use the Xaw toolkit.  */
#undef XAW

/* Define this and XAW to use the Xaw3d toolkit.  */
#undef XAW3D

/* Define to use the Motif toolkit.  */
#undef MOTIF

/* Define to enable greyscale anti-aliasing for shrunken bitmaps.  */
#undef GREY

/* Define to enable radio buttons on the side of the window for common
   commands.  */
#undef BUTTONS

/* Define to enable calling of mktexpk to automatically create missing
   pixel files.  */
#undef MKTEXPK

/* Define to enable old-style MakeTeXPK scripts to automatically create missing
   pixel files.  */
#undef MAKETEXPK

/* Define to point to the mktexpk or MakeTeXPK script (optional).  */
#undef MKTEXPK_PATH

/* Define to enable gf format files (in addition to pk format).  */
#undef USE_GF

/* Define to enable PostScript support via ghostscript.  */
#undef PS_GS

/* Define to point to the ghostscript interpreter (optional).  */
#undef GS_PATH

/* Define to enable PostScript support via Display PostScript.  */
#undef PS_DPS

/* Define to enable PostScript support via NeWS (Sun OpenWindows).  */
#undef PS_NEWS

/* Define to enable support for a configuration file.  Its value must be a
   colon-separated list of paths to search.  */
#undef DEFAULT_CONFIG_PATH

/* Define to enable support for a configuration file (as above), including
   support for the variables $SELFAUTODIR and $SELFAUTOPARENT.  */
#undef SELFAUTO

/* Define to enable support for an additional app-defaults file (located with
   the texmf hierarchy).  */
#undef EXTRA_APP_DEFAULTS

/* Define to use DOS-style names for font files (e.g., dpi300/cmr10.pk instead
   of cmr10.300pk).  */
#undef DOSNAMES

/* Define to use A4 as the default paper size.  */
#undef A4

/* Define to enable right-to-left typesetting.  */
#undef TEXXET

/* Define to set a default mfmode and resolution.  */
#undef MFMODE

/* Define to use hypertex */
#undef HTEX

/* ??? */ 
#undef LDT1LIB

#undef MAKE_OMEGA_OCP_BY_DEFAULT
#undef MAKE_OMEGA_OFM_BY_DEFAULT
#undef MAKE_TEX_MF_BY_DEFAULT
#undef MAKE_TEX_PK_BY_DEFAULT
#undef MAKE_TEX_TFM_BY_DEFAULT

/* Define to compile in statusline */
#undef STATUSLINE

/* Define to compile in t1lib */
#undef T1LIB

#undef HAVE_CC_CONCAT

#undef HAVE_VSNPRINTF

#undef NOTOOL
