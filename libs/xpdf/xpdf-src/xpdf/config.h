//========================================================================
//
// config.h
//
// Copyright 1996-2025 Glyph & Cog, LLC
//
//========================================================================

#ifndef CONFIG_H
#define CONFIG_H

//------------------------------------------------------------------------
// version
//------------------------------------------------------------------------

// xpdf version
#define xpdfVersion          "4.06"
#define xpdfVersionNum       4.06
#define xpdfMajorVersion     4
#define xpdfMinorVersion     6
#define xpdfUpdateVersion    0
#define xpdfMajorVersionStr  "4"
#define xpdfMinorVersionStr  "6"
#define xpdfUpdateVersionStr "0"

// supported PDF version
#define supportedPDFVersionStr "2.0"
#define supportedPDFVersionNum 2.0

// copyright notice
#define xpdfCopyright "Copyright 1996-2025 Glyph & Cog, LLC"

// Windows resource file stuff
#define winxpdfVersion "WinXpdf 4.06"
#define xpdfCopyrightAmp "Copyright 1996-2025 Glyph && Cog, LLC"

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

// system config file name (set via the configure script)
#ifdef SYSTEM_XPDFRC
#define xpdfSysConfigFile SYSTEM_XPDFRC
#else
#define xpdfSysConfigFile "/etc/xpdfrc"
#endif

//------------------------------------------------------------------------
// X-related constants
//------------------------------------------------------------------------

// default maximum size of color cube to allocate
#define defaultRGBCube 5

//------------------------------------------------------------------------
// popen
//------------------------------------------------------------------------

/* not necessary: defined in kpathsea/win32lib.h.
#if defined(_MSC_VER) || defined(__BORLANDC__)
#define popen _popen
#define pclose _pclose
#endif
*/

#if defined(VMS) || defined(VMCMS) || defined(DOS) || defined(OS2) || defined(__EMX__) || defined(_WIN32) || defined(__DJGPP__)
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
