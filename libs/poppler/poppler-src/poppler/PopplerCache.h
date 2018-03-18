//========================================================================
//
// PopplerCache.h
//
// This file is licensed under the GPLv2 or later
//
// Copyright (C) 2009 Koji Otani <sho@bbr.jp>
// Copyright (C) 2009, 2010, 2017, 2018 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2010 Carlos Garcia Campos <carlosgc@gnome.org>
//
//========================================================================

#ifndef POPPLER_CACHE_H
#define POPPLER_CACHE_H

#include "Object.h"

class PopplerCacheItem
{
  public:
   PopplerCacheItem() = default;
   virtual ~PopplerCacheItem();

   PopplerCacheItem(const PopplerCacheItem &) = delete;
   PopplerCacheItem& operator=(const PopplerCacheItem &other) = delete;
};

class PopplerCacheKey
{
  public:
    PopplerCacheKey() = default;
    virtual ~PopplerCacheKey();
    virtual bool operator==(const PopplerCacheKey &key) const = 0;

    PopplerCacheKey(const PopplerCacheKey &) = delete;
    PopplerCacheKey& operator=(const PopplerCacheKey &other) = delete;
};

class PopplerCache
{
  public:
    PopplerCache(int cacheSizeA);
    ~PopplerCache();
    
    PopplerCache(const PopplerCache &) = delete;
    PopplerCache& operator=(const PopplerCache &other) = delete;

    /* The item returned is owned by the cache */
    PopplerCacheItem *lookup(const PopplerCacheKey &key);
    
    /* The key and item pointers ownership is taken by the cache */
    void put(PopplerCacheKey *key, PopplerCacheItem *item);
    
    /* The max size of the cache */
    int size();
    
    /* The number of items in the cache */
    int numberOfItems();
    
    /* The n-th item in the cache */
    PopplerCacheItem *item(int index);
    
    /* The n-th key in the cache */
    PopplerCacheKey *key(int index);
  
  private:
    PopplerCacheKey **keys;
    PopplerCacheItem **items;
    int lastValidCacheIndex;
    int cacheSize;
};

class PopplerObjectCache
{
  public:
    PopplerObjectCache (int cacheSizeA, XRef *xrefA);
    ~PopplerObjectCache();

    PopplerObjectCache(const PopplerObjectCache &) = delete;
    PopplerObjectCache& operator=(const PopplerObjectCache &other) = delete;

    Object *put(const Ref &ref);
    Object lookup(const Ref &ref);

  private:
    XRef *xref;
    PopplerCache *cache;
};

#endif
