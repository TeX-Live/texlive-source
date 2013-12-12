// fake/StructRootTree.h: compatibility between poppler versions
//
// Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
// You may freely use, modify and/or distribute this file.

// poppler 0.25.0 and later have a header <StructRootTree.h> and declare
//   StructRootTree *Catalog::getStructRootTree()
// whereas poppler 0.24.4 and before have no such header and declare
//   Object *Catalog::getStructRootTree().

// We always #include <StructRootTree.h> from image/epdf.h and for
// poppler 0.24.4 this file defines GETSTRUCTTREEROOT_RETURNS_OBJECT
// used in lua/lepdflib.cc to distinguish the different versions.

#define GETSTRUCTTREEROOT_RETURNS_OBJECT 1

