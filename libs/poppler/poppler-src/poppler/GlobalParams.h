//========================================================================
//
// GlobalParams.h
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2005, 2007-2010, 2012, 2015, 2017, 2018 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2005 Jonathan Blandford <jrb@redhat.com>
// Copyright (C) 2006 Takashi Iwai <tiwai@suse.de>
// Copyright (C) 2006 Kristian Høgsberg <krh@redhat.com>
// Copyright (C) 2007 Krzysztof Kowalczyk <kkowalczyk@gmail.com>
// Copyright (C) 2009 Jonathan Kew <jonathan_kew@sil.org>
// Copyright (C) 2009 Petr Gajdos <pgajdos@novell.com>
// Copyright (C) 2009, 2011, 2012, 2014, 2015 William Bader <williambader@hotmail.com>
// Copyright (C) 2010 Hib Eris <hib@hiberis.nl>
// Copyright (C) 2011 Pino Toscano <pino@kde.org>
// Copyright (C) 2012, 2017 Adrian Johnson <ajohnson@redneon.com>
// Copyright (C) 2012 Thomas Freitag <Thomas.Freitag@alfa.de>
// Copyright (C) 2013 Jason Crain <jason@aquaticape.us>
// Copyright (C) 2018 Adam Reichold <adam.reichold@t-online.de>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <assert.h>
#include "poppler-config.h"
#include <stdio.h>
#include "goo/gtypes.h"
#include "CharTypes.h"
#include "UnicodeMap.h"
#include <unordered_map>
#include <string>

#ifdef MULTITHREADED
#include "goo/GooMutex.h"
#endif

class GooString;
class GooList;
class NameToCharCode;
class CharCodeToUnicode;
class CharCodeToUnicodeCache;
class UnicodeMapCache;
class CMap;
class CMapCache;
struct XpdfSecurityHandler;
class GlobalParams;
class GfxFont;
class Stream;
class SysFontList;

//------------------------------------------------------------------------

// The global parameters object.
extern GlobalParams *globalParams;

//------------------------------------------------------------------------

enum SysFontType {
  sysFontPFA,
  sysFontPFB,
  sysFontTTF,
  sysFontTTC
};

//------------------------------------------------------------------------

enum PSLevel {
  psLevel1,
  psLevel1Sep,
  psLevel2,
  psLevel2Sep,
  psLevel3,
  psLevel3Sep
};

//------------------------------------------------------------------------

enum EndOfLineKind {
  eolUnix,			// LF
  eolDOS,			// CR+LF
  eolMac			// CR
};

//------------------------------------------------------------------------

class GlobalParams {
public:

  // Initialize the global parameters
  GlobalParams(const char *customPopplerDataDir = NULL);

  ~GlobalParams();

  GlobalParams(const GlobalParams &) = delete;
  GlobalParams& operator=(const GlobalParams &) = delete;

  void setupBaseFonts(char *dir);

  //----- accessors

  CharCode getMacRomanCharCode(char *charName);

  // Return Unicode values for character names.  Used for general text
  // extraction.
  Unicode mapNameToUnicodeText(const char *charName);

  // Return Unicode values for character names.  Used for glyph
  // lookups or text extraction with ZapfDingbats fonts.
  Unicode mapNameToUnicodeAll(const char *charName);

  UnicodeMap *getResidentUnicodeMap(GooString *encodingName);
  FILE *getUnicodeMapFile(GooString *encodingName);
  FILE *findCMapFile(GooString *collection, GooString *cMapName);
  FILE *findToUnicodeFile(GooString *name);
  GooString *findFontFile(GooString *fontName);
  GooString *findBase14FontFile(GooString *base14Name, GfxFont *font);
  GooString *findSystemFontFile(GfxFont *font, SysFontType *type,
			      int *fontNum, GooString *substituteFontName = NULL, 
		              GooString *base14Name = NULL);
  GBool getPSExpandSmaller();
  GBool getPSShrinkLarger();
  PSLevel getPSLevel();
  GooString *getTextEncodingName();
  EndOfLineKind getTextEOL();
  GBool getTextPageBreaks();
  GBool getEnableFreeType();
  GBool getOverprintPreview() { return overprintPreview; }
  GBool getPrintCommands();
  GBool getProfileCommands();
  GBool getErrQuiet();

  CharCodeToUnicode *getCIDToUnicode(GooString *collection);
  UnicodeMap *getUnicodeMap(GooString *encodingName);
  CMap *getCMap(GooString *collection, GooString *cMapName, Stream *stream = NULL);
  UnicodeMap *getTextEncoding();
#ifdef ENABLE_PLUGINS
  GBool loadPlugin(char *type, char *name);
#endif

  GooList *getEncodingNames();

  //----- functions to set parameters
  void addFontFile(GooString *fontName, GooString *path);
  void setPSExpandSmaller(GBool expand);
  void setPSShrinkLarger(GBool shrink);
  void setPSLevel(PSLevel level);
  void setTextEncoding(char *encodingName);
  GBool setTextEOL(char *s);
  void setTextPageBreaks(GBool pageBreaks);
  GBool setEnableFreeType(char *s);
  void setOverprintPreview(GBool overprintPreviewA);
  void setPrintCommands(GBool printCommandsA);
  void setProfileCommands(GBool profileCommandsA);
  void setErrQuiet(GBool errQuietA);

  static GBool parseYesNo2(const char *token, GBool *flag);

  //----- security handlers

  void addSecurityHandler(XpdfSecurityHandler *handler);
  XpdfSecurityHandler *getSecurityHandler(char *name);

private:

  void parseNameToUnicode(GooString *name);
  UnicodeMap *getUnicodeMap2(GooString *encodingName);

  void scanEncodingDirs();
  void addCIDToUnicode(GooString *collection, GooString *fileName);
  void addUnicodeMap(GooString *encodingName, GooString *fileName);
  void addCMapDir(GooString *collection, GooString *dir);

  //----- static tables

  NameToCharCode *		// mapping from char name to
    macRomanReverseMap;		//   MacRomanEncoding index

  //----- user-modifiable settings

  NameToCharCode *		// mapping from char name to Unicode for ZapfDingbats
    nameToUnicodeZapfDingbats;
  NameToCharCode *		// mapping from char name to Unicode for text
    nameToUnicodeText;		// extraction
  // files for mappings from char collections
  // to Unicode, indexed by collection name
  std::unordered_map<std::string, std::string> cidToUnicodes;
  // mappings from Unicode to char codes,
  // indexed by encoding name
  std::unordered_map<std::string, UnicodeMap> residentUnicodeMaps;
  // files for mappings from Unicode to char
  // codes, indexed by encoding name
  std::unordered_map<std::string, std::string> unicodeMaps;
  // list of CMap dirs, indexed by collection
  std::unordered_multimap<std::string, std::string> cMapDirs;
  GooList *toUnicodeDirs;		// list of ToUnicode CMap dirs [GooString]
  GBool baseFontsInitialized;
#ifdef _WIN32
  // windows font substitutes (for CID fonts)
  std::unordered_map<std::string, std::string> substFiles;
#endif
  // font files: font name mapped to path
  std::unordered_map<std::string, std::string> fontFiles;
  SysFontList *sysFonts;	// system fonts
  GBool psExpandSmaller;	// expand smaller pages to fill paper
  GBool psShrinkLarger;		// shrink larger pages to fit paper
  PSLevel psLevel;		// PostScript level to generate
  GooString *textEncoding;	// encoding (unicodeMap) to use for text
				//   output
  EndOfLineKind textEOL;	// type of EOL marker to use for text
				//   output
  GBool textPageBreaks;		// insert end-of-page markers?
  GBool enableFreeType;		// FreeType enable flag
  GBool overprintPreview;	// enable overprint preview
  GBool printCommands;		// print the drawing commands
  GBool profileCommands;	// profile the drawing commands
  GBool errQuiet;		// suppress error messages?

  CharCodeToUnicodeCache *cidToUnicodeCache;
  CharCodeToUnicodeCache *unicodeToUnicodeCache;
  UnicodeMapCache *unicodeMapCache;
  CMapCache *cMapCache;
  
#ifdef ENABLE_PLUGINS
  GooList *plugins;		// list of plugins [Plugin]
  GooList *securityHandlers;	// list of loaded security handlers
				//   [XpdfSecurityHandler]
#endif

#ifdef MULTITHREADED
  GooMutex mutex;
  GooMutex unicodeMapCacheMutex;
  GooMutex cMapCacheMutex;
#endif

  const char *popplerDataDir;
};

#endif
