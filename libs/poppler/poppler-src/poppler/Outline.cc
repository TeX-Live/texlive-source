//========================================================================
//
// Outline.cc
//
// Copyright 2002-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2005 Marco Pesenti Gritti <mpg@redhat.com>
// Copyright (C) 2008, 2016, 2017 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2009 Nick Jones <nick.jones@network-box.com>
// Copyright (C) 2016 Jason Crain <jason@aquaticape.us>
// Copyright (C) 2017 Adrian Johnson <ajohnson@redneon.com>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#include <config.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "goo/gmem.h"
#include "goo/GooString.h"
#include "goo/GooList.h"
#include "XRef.h"
#include "Link.h"
#include "PDFDocEncoding.h"
#include "Outline.h"
#include "UTF.h"

//------------------------------------------------------------------------

Outline::Outline(Object *outlineObj, XRef *xref) {
  items = nullptr;
  if (!outlineObj->isDict()) {
    return;
  }
  Object first = outlineObj->dictLookupNF("First");
  items = OutlineItem::readItemList(nullptr, &first, xref);
}

Outline::~Outline() {
  if (items) {
    deleteGooList(items, OutlineItem);
  }
}

//------------------------------------------------------------------------

OutlineItem::OutlineItem(Dict *dict, int refNumA, OutlineItem *parentA, XRef *xrefA) {
  Object obj1;

  refNum = refNumA;
  parent = parentA;
  xref = xrefA;
  title = nullptr;
  action = nullptr;
  kids = nullptr;


  obj1 = dict->lookup("Title");
  if (obj1.isString()) {
    GooString *s = obj1.getString();
    titleLen = TextStringToUCS4(s, &title);
  } else {
    titleLen = 0;
  }

  obj1 = dict->lookup("Dest");
  if (!obj1.isNull()) {
    action = LinkAction::parseDest(&obj1);
  } else {
    obj1 = dict->lookup("A");
    if (!obj1.isNull()) {
      action = LinkAction::parseAction(&obj1);
    }
  }

  firstRef = dict->lookupNF("First");
  lastRef = dict->lookupNF("Last");
  nextRef = dict->lookupNF("Next");

  startsOpen = gFalse;
  obj1 = dict->lookup("Count");
  if (obj1.isInt()) {
    if (obj1.getInt() > 0) {
      startsOpen = gTrue;
    }
  }
}

OutlineItem::~OutlineItem() {
  close();
  if (title) {
    gfree(title);
  }
  if (action) {
    delete action;
  }
}

GooList *OutlineItem::readItemList(OutlineItem *parent, Object *firstItemRef, XRef *xrefA) {
  GooList *items = new GooList();

  char* alreadyRead = (char *)gmalloc(xrefA->getNumObjects());
  memset(alreadyRead, 0, xrefA->getNumObjects());

  OutlineItem *parentO = parent;
  while (parentO) {
    alreadyRead[parentO->refNum] = 1;
    parentO = parentO->parent;
  }

  Object *p = firstItemRef;
  while (p->isRef() && 
	 (p->getRefNum() >= 0) && 
         (p->getRefNum() < xrefA->getNumObjects()) &&
         !alreadyRead[p->getRefNum()]) {
    Object obj = p->fetch(xrefA);
    if (!obj.isDict()) {
      break;
    }
    alreadyRead[p->getRefNum()] = 1;
    OutlineItem *item = new OutlineItem(obj.getDict(), p->getRefNum(), parent, xrefA);
    items->append(item);
    p = &item->nextRef;
  }

  gfree(alreadyRead);

  if (!items->getLength()) {
    delete items;
    items = nullptr;
  }

  return items;
}

void OutlineItem::open() {
  if (!kids) {
    kids = readItemList(this, &firstRef, xref);
  }
}

void OutlineItem::close() {
  if (kids) {
    deleteGooList(kids, OutlineItem);
    kids = nullptr;
  }
}
