//========================================================================
//
// config.h
//
// Copyright 1996-2007 Glyph & Cog, LLC
//
//========================================================================

/* ------------------------------------------------------------------------
* Changed by Martin Schröder <martin@pdftex.org>
* $Id: config.h 421 2008-04-26 21:59:55Z oneiros $
* Changelog:
* ------------------------------------------------------------------------
* r268 | oneiros | 2007-11-11 22:45:02 +0100 (So, 11 Nov 2007) | 3 lines
* 
* Merge xpdf 3.02pl2
* svn merge -r215:266 http://scm.foundry.supelec.fr/svn/pdftex/vendor/xpdf/current .
* 
* ------------------------------------------------------------------------
* r215 | oneiros | 2007-07-31 18:32:04 +0200 (Di, 31 Jul 2007) | 2 lines
* 
* Merge changes from trunk into stable: 203-207,213-214
* 
* ------------------------------------------------------------------------
* r151 | ms | 2007-06-25 18:53:17 +0200 (Mo, 25 Jun 2007) | 3 lines
* 
* Merging xpdf 3.02 from HEAD into stable
* svn merge -r149:150 --dry-run svn+ssh://svn/srv/svn/repos/pdftex/trunk/source/src/libs/xpdf .
* 
* ------------------------------------------------------------------------
* r86 | ms | 2007-01-31 13:01:00 +0100 (Mi, 31 Jan 2007) | 2 lines
* 
* 1.40.2
* 
* ------------------------------------------------------------------------
* r38 | ms | 2005-08-21 14:00:00 +0200 (So, 21 Aug 2005) | 2 lines
* 
* 1.30.1
* 
* ------------------------------------------------------------------------
* r11 | ms | 2004-09-06 14:01:00 +0200 (Mo, 06 Sep 2004) | 2 lines
* 
* 1.20a
* 
* ------------------------------------------------------------------------
* r6 | ms | 2003-10-06 14:01:00 +0200 (Mo, 06 Okt 2003) | 2 lines
* 
* released v1.11b
* 
* ------------------------------------------------------------------------
* r4 | ms | 2003-10-05 14:00:00 +0200 (So, 05 Okt 2003) | 2 lines
* 
* Moved sources to src
* 
* ------------------------------------------------------------------------
* r1 | ms | 2003-08-02 14:00:00 +0200 (Sa, 02 Aug 2003) | 1 line
* 
* 1.11a
* ------------------------------------------------------------------------ */

#ifndef CONFIG_H
#define CONFIG_H

//------------------------------------------------------------------------
// version
//------------------------------------------------------------------------

// xpdf version
#define xpdfVersion          "3.02pl2"
#define xpdfVersionNum       3.02
#define xpdfMajorVersion     3
#define xpdfMinorVersion     2
#define xpdfUpdateVersion    0
#define xpdfMajorVersionStr  "3"
#define xpdfMinorVersionStr  "2"
#define xpdfUpdateVersionStr "0"

// supported PDF version
#define supportedPDFVersionStr "1.7"
#define supportedPDFVersionNum 1.7

// copyright notice
#define xpdfCopyright "Copyright 1996-2007 Glyph & Cog, LLC"

// Windows resource file stuff
#define winxpdfVersion "WinXpdf 3.02"
#define xpdfCopyrightAmp "Copyright 1996-2007 Glyph && Cog, LLC"

//------------------------------------------------------------------------
// paper size
//------------------------------------------------------------------------

// default paper size (in points) for PostScript output
#ifdef A4_PAPER
#define defPaperWidth  595    // ISO A4 (210x297 mm)
#define defPaperHeight 842
#else
#define defPaperWidth  612    // American letter (8.5x11")
#define defPaperHeight 792
#endif

//------------------------------------------------------------------------
// config file (xpdfrc) path
//------------------------------------------------------------------------

// user config file name, relative to the user's home directory
#if defined(VMS) || (defined(WIN32) && !defined(__CYGWIN32__))
#define xpdfUserConfigFile "xpdfrc"
#else
#define xpdfUserConfigFile ".xpdfrc"
#endif

// system config file name (set via the configure script)
#ifdef SYSTEM_XPDFRC
#define xpdfSysConfigFile SYSTEM_XPDFRC
#else
// under Windows, we get the directory with the executable and then
// append this file name
#define xpdfSysConfigFile "xpdfrc"
#endif

//------------------------------------------------------------------------
// X-related constants
//------------------------------------------------------------------------

// default maximum size of color cube to allocate
#define defaultRGBCube 5

// number of fonts (combined t1lib, FreeType, X server) to cache
#define xOutFontCacheSize 64

// number of Type 3 fonts to cache
#define xOutT3FontCacheSize 8

//------------------------------------------------------------------------
// popen
//------------------------------------------------------------------------

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define popen _popen
#define pclose _pclose
#endif

#if defined(VMS) || defined(VMCMS) || defined(DOS) || defined(OS2) || defined(__EMX__) || defined(WIN32) || defined(__DJGPP__) || defined(MACOS)
#define POPEN_READ_MODE "rb"
#else
#define POPEN_READ_MODE "r"
#endif

//------------------------------------------------------------------------
// Win32 stuff
//------------------------------------------------------------------------

#ifdef CDECL
#undef CDECL
#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define CDECL __cdecl
#else
#define CDECL
#endif

#endif
