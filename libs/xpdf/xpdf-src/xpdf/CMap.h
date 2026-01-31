//========================================================================
//
// CMap.h
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef CMAP_H
#define CMAP_H

#include <aconf.h>

#include "gtypes.h"
#include "CharTypes.h"

#if MULTITHREADED
#include "GMutex.h"
#endif

class GString;
class GHash;
class Object;
class Stream;
struct CMapVectorEntry;
class CMapCache;

//------------------------------------------------------------------------

class CMap {
public:

  // Parse a CMap from <obj>, which can be a name or a stream.  Sets
  // the initial reference count to 1.  Returns NULL on failure.
  static CMap *parse(CMapCache *cache, GString *collectionA, Object *obj,
		     GHash *usedCMaps = NULL);

  // Create the CMap specified by <collection> and <cMapName>.  Sets
  // the initial reference count to 1.  Returns NULL on failure.
  static CMap *parse(CMapCache *cache, GString *collectionA,
		     GString *cMapNameA, GHash *usedCMaps = NULL);

  // Parse a CMap from <str>.  Sets the initial reference count to 1.
  // Returns NULL on failure.
  static CMap *parse(CMapCache *cache, GString *collectionA, Stream *str,
		     GHash *usedCMaps = NULL);

  ~CMap();

  void incRefCnt();
  void decRefCnt();

  // Return collection name (<registry>-<ordering>).
  GString *getCollection() { return collection; }

  // Return true if this CMap matches the specified <collectionA>, and
  // <cMapNameA>.
  GBool match(GString *collectionA, GString *cMapNameA);

  // Return the CID corresponding to the character code starting at
  // <s>, which contains <len> bytes.  Sets *<c> to the char code, and
  // *<nUsed> to the number of bytes used by the char code.
  CID getCID(char *s, int len, CharCode *c, int *nUsed);

  // Return the writing mode (0=horizontal, 1=vertical).
  int getWMode() { return wMode; }

private:

  void parse2(CMapCache *cache, int (*getCharFunc)(void *), void *data,
	      GHash *usedCMaps);
  CMap(GString *collectionA, GString *cMapNameA);
  CMap(GString *collectionA, GString *cMapNameA, int wModeA);
  void useCMap(CMapCache *cache, char *useName, GHash *usedCMaps);
  void useCMap(CMapCache *cache, Object *obj, GHash *usedCMaps);
  void copyVector(CMapVectorEntry *dest, CMapVectorEntry *src);
  void addCIDs(Guint start, Guint end, Guint nBytes, CID firstCID);
  void freeCMapVector(CMapVectorEntry *vec);

  GString *collection;
  GString *cMapName;
  GBool isIdent;		// true if this CMap is an identity mapping,
				//   or is based on one (via usecmap)
  int wMode;			// writing mode (0=horizontal, 1=vertical)
  CMapVectorEntry *vector;	// vector for first byte (NULL for
				//   identity CMap)
#if MULTITHREADED
  GAtomicCounter refCnt;
#else
  int refCnt;
#endif
};

//------------------------------------------------------------------------

#define cMapCacheSize 4

class CMapCache {
public:

  CMapCache();
  ~CMapCache();

  // Get the <cMapName> CMap for the specified character collection.
  // Increments its reference count; there will be one reference for
  // the cache plus one for the caller of this function.  Returns NULL
  // on failure.
  CMap *getCMap(GString *collection, GString *cMapName,
		GHash *usedCMaps = NULL);

private:

  CMap *cache[cMapCacheSize];
};

#endif
