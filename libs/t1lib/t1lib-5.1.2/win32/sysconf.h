/*--------------------------------------------------------------------------
  ----- File:        sysconf.h.in
  ----- Author:      Rainer Menzner (rmz@neuroinformatik.ruhr-uni-bochum.de)
  ----- Date:        02/24/1998
  ----- Description: This file is part of the t1-library. Some system
                     dependent definitions are stored here. sysconf.h
		     is generated from this file automatically by the
		     configure-script. 
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-1998. 
                     As of version 0.5, t1lib is distributed under the
		     GNU General Public Library Lincense. The
		     conditions can be found in the files LICENSE and
		     LGPL, which should reside in the toplevel
		     directory of the distribution.  Please note that 
		     there are parts of t1lib that are subject to
		     other licenses:
		     The parseAFM-package is copyrighted by Adobe Systems
		     Inc.
		     The type1 rasterizer is copyrighted by IBM and the
		     X11-consortium.
  ----- Warranties:  Of course, there's NO WARRANTY OF ANY KIND :-)
  ----- Credits:     I want to thank IBM and the X11-consortium for making
                     their rasterizer freely available.
		     Also thanks to Piet Tutelaers for his ps2pk, from
		     which I took the rasterizer sources in a format
		     independent from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/
  

#undef __CHAR_UNSIGNED__   
#define SIZEOF_CHAR         1
#define SIZEOF_SHORT        2
#define SIZEOF_INT          4
#define SIZEOF_LONG         4
#define SIZEOF_LONG_LONG    8
#define SIZEOF_FLOAT        4
#define SIZEOF_DOUBLE       8
#define SIZEOF_LONG_DOUBLE  12
#define SIZEOF_VOID_P       4

#if defined(_MSC_VER)
#  define T1LIB_IDENT		"5.1.2"
#  define GLOBAL_CONFIG_DIR	"c:/usr/local/share/t1lib"
#  define T1_AA_TYPE16		short
#  define T1_AA_TYPE32		int
#endif

#if defined(_MSC_VER) && !defined(_WIN32)
# define _WIN32 1
#endif
