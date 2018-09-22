//========================================================================
//
// Dict.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2005 Kristian Høgsberg <krh@redhat.com>
// Copyright (C) 2006 Krzysztof Kowalczyk <kkowalczyk@gmail.com>
// Copyright (C) 2007-2008 Julien Rebetez <julienr@svn.gnome.org>
// Copyright (C) 2010, 2017, 2018 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2010 Paweł Wiejacha <pawel.wiejacha@gmail.com>
// Copyright (C) 2013 Thomas Freitag <Thomas.Freitag@alfa.de>
// Copyright (C) 2017 Adrian Johnson <ajohnson@redneon.com>
// Copyright (C) 2018 Adam Reichold <adam.reichold@t-online.de>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef DICT_H
#define DICT_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <atomic>
#include <string>
#include <vector>
#include <utility>

#include "poppler-config.h"
#include "Object.h"
#include "goo/GooMutex.h"

//------------------------------------------------------------------------
// Dict
//------------------------------------------------------------------------

class Dict {
public:

  // Constructor.
  Dict(XRef *xrefA);
  Dict(const Dict *dictA);
  Dict *copy(XRef *xrefA) const;

  // Destructor.
  ~Dict();

  Dict(const Dict &) = delete;
  Dict& operator=(const Dict &) = delete;

  // Get number of entries.
  int getLength() const { return static_cast<int>(entries.size()); }

  // Add an entry. (Copies key into Dict.)
  // val becomes a dead object after the call
  void add(const char *key, Object &&val);

  // Add an entry. (Takes ownership of key.)
  void add(char *key, Object &&val) = delete;

  // Update the value of an existing entry, otherwise create it
  // val becomes a dead object after the call
  void set(const char *key, Object &&val);
  // Remove an entry. This invalidate indexes
  void remove(const char *key);

  // Check if dictionary is of specified type.
  GBool is(const char *type) const;

  // Look up an entry and return the value.  Returns a null object
  // if <key> is not in the dictionary.
  Object lookup(const char *key, int recursion = 0) const;
  Object lookupNF(const char *key) const;
  GBool lookupInt(const char *key, const char *alt_key, int *value) const;

  // Iterative accessors.
  const char *getKey(int i) const { return entries[i].first.c_str(); }
  Object getVal(int i) const { return entries[i].second.fetch(xref); }
  Object getValNF(int i) const { return entries[i].second.copy(); }

  // Set the xref pointer.  This is only used in one special case: the
  // trailer dictionary, which is read before the xref table is
  // parsed.
  void setXRef(XRef *xrefA) { xref = xrefA; }
  
  XRef *getXRef() const { return xref; }
  
  GBool hasKey(const char *key) const;

private:
  friend class Object; // for incRef/decRef

  // Reference counting.
  int incRef() { return ++ref; }
  int decRef() { return --ref; }

  using DictEntry = std::pair<std::string, Object>;
  struct CmpDictEntry;

  std::atomic_bool sorted;
  XRef *xref;			// the xref table for this PDF file
  std::vector<DictEntry> entries;
  std::atomic_int ref;			// reference count
#ifdef MULTITHREADED
  mutable GooMutex mutex;
#endif

  const DictEntry *find(const char *key) const;
  DictEntry *find(const char *key);
};

#endif
