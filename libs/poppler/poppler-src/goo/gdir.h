//========================================================================
//
// gfile.h
//
// Miscellaneous file and directory name manipulation.
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
// Copyright (C) 2006 Kristian Høgsberg <krh@redhat.com>
// Copyright (C) 2009, 2011, 2012, 2017, 2018 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2009 Kovid Goyal <kovid@kovidgoyal.net>
// Copyright (C) 2013 Adam Reichold <adamreichold@myopera.com>
// Copyright (C) 2013, 2017 Adrian Johnson <ajohnson@redneon.com>
// Copyright (C) 2014 Bogdan Cristea <cristeab@gmail.com>
// Copyright (C) 2014 Peter Breitenlohner <peb@mppmu.mpg.de>
// Copyright (C) 2017 Christoph Cullmann <cullmann@kde.org>
// Copyright (C) 2017 Thomas Freitag <Thomas.Freitag@alfa.de>
// Copyright (C) 2018 Mojca Miklavec <mojca@macports.org>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef GDIR_H
#define GDIR_H

#include "poppler-config.h"
#include "gtypes.h"

class GooString;

//------------------------------------------------------------------------
// GDir and GDirEntry
//------------------------------------------------------------------------

class GDirEntry {
public:

  GDirEntry(char *dirPath, char *nameA, GBool doStat);
  ~GDirEntry();
  GooString *getName() { return name; }
  GooString *getFullPath() { return fullPath; }
  GBool isDir() { return dir; }

private:
  GDirEntry(const GDirEntry &other);
  GDirEntry& operator=(const GDirEntry &other);

  GooString *name;		// dir/file name
  GooString *fullPath;
  GBool dir;			// is it a directory?
};

class GDir {
public:

  GDir(char *name, GBool doStatA = gTrue);
  ~GDir();
  GDirEntry *getNextEntry();
  void rewind();

private:
  GDir(const GDir &other);
  GDir& operator=(const GDir &other);

  GooString *path;		// directory path
  GBool doStat;			// call stat() for each entry?
#if defined(_WIN32)
  WIN32_FIND_DATAA ffd;
  HANDLE hnd;
#elif defined(ACORN)
#elif defined(MACOS)
#else
  DIR *dir;			// the DIR structure from opendir()
#ifdef VMS
  GBool needParent;		// need to return an entry for [-]
#endif
#endif
};

#endif
