//========================================================================
//
// This file is under the GPLv2 or later license
//
// Copyright (C) 2005-2006 Kristian Høgsberg <krh@redhat.com>
// Copyright (C) 2005, 2018 Albert Astals Cid <aacid@kde.org>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "goo/gtypes.h"
#include "goo/GooList.h"
#include "goo/GooString.h"
#include "Object.h"

class PageLabelInfo {
public:
  PageLabelInfo(Object *tree, int numPages);
  ~PageLabelInfo();
  PageLabelInfo(const PageLabelInfo &) = delete;
  PageLabelInfo& operator=(const PageLabelInfo &) = delete;
  GBool labelToIndex(GooString *label, int *index);
  GBool indexToLabel(int index, GooString *label);

private:
  void parse(Object *tree);

private:
  struct Interval {
    Interval(Object *dict, int baseA);
    ~Interval();
    Interval(const Interval &) = delete;
    Interval& operator=(const Interval &) = delete;
    GooString *prefix;
    enum NumberStyle {
      None,
      Arabic,
      LowercaseRoman,
      UppercaseRoman,
      UppercaseLatin,
      LowercaseLatin
    } style;
    int first, base, length;
  };

  GooList intervals;
};
