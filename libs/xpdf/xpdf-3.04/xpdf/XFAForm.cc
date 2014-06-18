//========================================================================
//
// XFAForm.cc
//
// Copyright 2012 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include "GString.h"
#include "GList.h"
#include "GHash.h"
#include "Error.h"
#include "Object.h"
#include "PDFDoc.h"
#include "Gfx.h"
#include "GfxFont.h"
#include "Zoox.h"
#include "XFAForm.h"

#ifdef _WIN32
#  define strcasecmp stricmp
#  define strncasecmp strnicmp
#endif

//------------------------------------------------------------------------

// 5 bars + 5 spaces -- each can be wide (1) or narrow (0)
// (there are always exactly 3 wide elements;
// the last space is always narrow)
static Guchar code3Of9Data[128][10] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0x00
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0x10
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 1, 0, 0, 0, 1, 0, 0, 0 }, // ' ' = 0x20
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 1, 0, 1, 0, 0, 0, 0 }, // '$' = 0x24
  { 0, 0, 0, 1, 0, 1, 0, 1, 0, 0 }, // '%' = 0x25
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 1, 0, 1, 0, 0, 0 }, // '*' = 0x2a
  { 0, 1, 0, 0, 0, 1, 0, 1, 0, 0 }, // '+' = 0x2b
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 0 }, // '-' = 0x2d
  { 1, 1, 0, 0, 0, 0, 1, 0, 0, 0 }, // '.' = 0x2e
  { 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 }, // '/' = 0x2f
  { 0, 0, 0, 1, 1, 0, 1, 0, 0, 0 }, // '0' = 0x30
  { 1, 0, 0, 1, 0, 0, 0, 0, 1, 0 }, // '1'
  { 0, 0, 1, 1, 0, 0, 0, 0, 1, 0 }, // '2'
  { 1, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // '3'
  { 0, 0, 0, 1, 1, 0, 0, 0, 1, 0 }, // '4'
  { 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 }, // '5'
  { 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 }, // '6'
  { 0, 0, 0, 1, 0, 0, 1, 0, 1, 0 }, // '7'
  { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0 }, // '8'
  { 0, 0, 1, 1, 0, 0, 1, 0, 0, 0 }, // '9'
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0x40
  { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0 }, // 'A' = 0x41
  { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 }, // 'B'
  { 1, 0, 1, 0, 0, 1, 0, 0, 0, 0 }, // 'C'
  { 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 }, // 'D'
  { 1, 0, 0, 0, 1, 1, 0, 0, 0, 0 }, // 'E'
  { 0, 0, 1, 0, 1, 1, 0, 0, 0, 0 }, // 'F'
  { 0, 0, 0, 0, 0, 1, 1, 0, 1, 0 }, // 'G'
  { 1, 0, 0, 0, 0, 1, 1, 0, 0, 0 }, // 'H'
  { 0, 0, 1, 0, 0, 1, 1, 0, 0, 0 }, // 'I'
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0 }, // 'J'
  { 1, 0, 0, 0, 0, 0, 0, 1, 1, 0 }, // 'K'
  { 0, 0, 1, 0, 0, 0, 0, 1, 1, 0 }, // 'L'
  { 1, 0, 1, 0, 0, 0, 0, 1, 0, 0 }, // 'M'
  { 0, 0, 0, 0, 1, 0, 0, 1, 1, 0 }, // 'N'
  { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0 }, // 'O'
  { 0, 0, 1, 0, 1, 0, 0, 1, 0, 0 }, // 'P' = 0x50
  { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0 }, // 'Q'
  { 1, 0, 0, 0, 0, 0, 1, 1, 0, 0 }, // 'R'
  { 0, 0, 1, 0, 0, 0, 1, 1, 0, 0 }, // 'S'
  { 0, 0, 0, 0, 1, 0, 1, 1, 0, 0 }, // 'T'
  { 1, 1, 0, 0, 0, 0, 0, 0, 1, 0 }, // 'U'
  { 0, 1, 1, 0, 0, 0, 0, 0, 1, 0 }, // 'V'
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 }, // 'W'
  { 0, 1, 0, 0, 1, 0, 0, 0, 1, 0 }, // 'X'
  { 1, 1, 0, 0, 1, 0, 0, 0, 0, 0 }, // 'Y'
  { 0, 1, 1, 0, 1, 0, 0, 0, 0, 0 }, // 'Z'
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0x60
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0x70
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

//------------------------------------------------------------------------
// XFAForm
//------------------------------------------------------------------------

XFAForm *XFAForm::load(PDFDoc *docA, Object *acroFormObj, Object *xfaObj) {
  XFAForm *xfaForm;
  ZxDoc *xmlA;
  ZxElement *tmpl;
  Object catDict, resourceDictA, obj1;
  GString *data;
  GBool fullXFAA;
  GString *name;
  char buf[4096];
  int n, i;

  docA->getXRef()->getCatalog(&catDict);
  catDict.dictLookup("NeedsRendering", &obj1);
  fullXFAA = obj1.isBool() && obj1.getBool();
  obj1.free();
  catDict.free();

  if (xfaObj->isStream()) {
    data = new GString();
    xfaObj->streamReset();
    while ((n = xfaObj->getStream()->getBlock(buf, sizeof(buf))) > 0) {
      data->append(buf, n);
    }
  } else if (xfaObj->isArray()) {
    data = new GString();
    for (i = 1; i < xfaObj->arrayGetLength(); i += 2) {
      if (!xfaObj->arrayGet(i, &obj1)->isStream()) {
	error(errSyntaxError, -1, "XFA array element is wrong type");
	obj1.free();
	delete data;
	return NULL;
      }
      obj1.streamReset();
      while ((n = obj1.getStream()->getBlock(buf, sizeof(buf))) > 0) {
	data->append(buf, n);
      }
      obj1.free();
    }
  } else {
    error(errSyntaxError, -1, "XFA object is wrong type");
    return NULL;
  }

  xmlA = ZxDoc::loadMem(data->getCString(), data->getLength());
  delete data;
  if (!xmlA) {
    error(errSyntaxError, -1, "Invalid XML in XFA form");
    return NULL;
  }

  if (acroFormObj->isDict()) {
    acroFormObj->dictLookup("DR", &resourceDictA);
  }

  xfaForm = new XFAForm(docA, xmlA, &resourceDictA, fullXFAA);

  resourceDictA.free();

  if (xfaForm->xml->getRoot()) {
    if ((tmpl = xfaForm->xml->getRoot()->findFirstChildElement("template"))) {
      name = new GString("form");
      xfaForm->curPageNum = 1;
      xfaForm->curXOffset = xfaForm->curYOffset = 0;
      xfaForm->scanFields(tmpl, name, name);
      delete name;
    }
  }

  return xfaForm;
}

XFAForm::XFAForm(PDFDoc *docA, ZxDoc *xmlA, Object *resourceDictA,
		 GBool fullXFAA): Form(docA) {
  xml = xmlA;
  fields = new GList();
  resourceDictA->copy(&resourceDict);
  fullXFA = fullXFAA;
}

XFAForm::~XFAForm() {
  delete xml;
  deleteGList(fields, XFAFormField);
  resourceDict.free();
}

void XFAForm::scanFields(ZxElement *elem, GString *name, GString *dataName) {
  ZxAttr *attr;
  ZxNode *child;
  ZxElement *bindElem;
  GHash *names1, *names2;
  GString *childName, *fullName, *fullDataName;
  int i;

  //~ need to handle subform

  //~ need to handle exclGroup
  //~ - fields in an exclGroup may/must(?) not have names
  //~ - each field has an items element with the the value when that
  //~   field is selected

  if (elem->isElement("field")) {
    fields->append(new XFAFormField(this, elem, name->copy(),
				    dataName->copy(), curPageNum,
				    curXOffset, curYOffset));
  } else if (elem->isElement("breakBefore")) {
    if ((attr = elem->findAttr("targetType")) &&
	!attr->getValue()->cmp("pageArea") &&
	(attr = elem->findAttr("startNew")) &&
	!attr->getValue()->cmp("1")) {
      ++curPageNum;
    }
  } else if (elem->isElement("break")) {
    if ((attr = elem->findAttr("before")) &&
	!attr->getValue()->cmp("pageArea") &&
	(attr = elem->findAttr("startNew")) &&
	!attr->getValue()->cmp("1")) {
      ++curPageNum;
    }
  } else if (elem->isElement("contentArea")) {
    curXOffset = XFAFormField::getMeasurement(elem->findAttr("x"), 0);
    curYOffset = XFAFormField::getMeasurement(elem->findAttr("y"), 0);
  } else {
    names1 = new GHash();
    for (child = elem->getFirstChild(); child; child = child->getNextChild()) {
      if (child->isElement() &&
	  (attr = ((ZxElement *)child)->findAttr("name"))) {
	childName = attr->getValue();
	names1->replace(childName, names1->lookupInt(childName) + 1);
      }
    }
    names2 = new GHash();
    for (child = elem->getFirstChild(); child; child = child->getNextChild()) {
      if (child->isElement()) {
	if (!((bindElem = child->findFirstChildElement("bind")) &&
	      (attr = bindElem->findAttr("match")) &&
	      !attr->getValue()->cmp("none"))  &&
	    (attr = ((ZxElement *)child)->findAttr("name"))) {
	  childName = attr->getValue();
	  if (names1->lookupInt(childName) > 1) {
	    i = names2->lookupInt(childName);
	    fullName = GString::format("{0:t}.{1:t}[{2:d}]",
				       name, childName, i);
	    fullDataName = GString::format("{0:t}.{1:t}[{2:d}]",
					   dataName, childName, i);
	    names2->replace(childName, i + 1);
	  } else {
	    fullName = GString::format("{0:t}.{1:t}", name, childName);
	    fullDataName = GString::format("{0:t}.{1:t}", dataName, childName);
	  }
	} else {
	  fullName = name->copy();
	  fullDataName = dataName->copy();
	}
	scanFields((ZxElement *)child, fullName, fullDataName);
	delete fullName;
	delete fullDataName;
      }
    }
    delete names1;
    delete names2;
  }
}

void XFAForm::draw(int pageNum, Gfx *gfx, GBool printing) {
  GfxFontDict *fontDict;
  Object obj1;
  int i;

  // build the font dictionary
  if (resourceDict.isDict() &&
      resourceDict.dictLookup("Font", &obj1)->isDict()) {
    fontDict = new GfxFontDict(doc->getXRef(), NULL, obj1.getDict());
  } else {
    fontDict = NULL;
  }
  obj1.free();

  for (i = 0; i < fields->getLength(); ++i) {
    ((XFAFormField *)fields->get(i))->draw(pageNum, gfx, printing, fontDict);
  }

  delete fontDict;
}

int XFAForm::getNumFields() {
  return fields->getLength();
}

FormField *XFAForm::getField(int idx) {
  return (XFAFormField *)fields->get(idx);
}

//------------------------------------------------------------------------
// XFAFormField
//------------------------------------------------------------------------

XFAFormField::XFAFormField(XFAForm *xfaFormA, ZxElement *xmlA, GString *nameA,
			   GString *dataNameA, int pageNumA,
			   double xOffsetA, double yOffsetA) {
  xfaForm = xfaFormA;
  xml = xmlA;
  name = nameA;
  dataName = dataNameA;
  pageNum = pageNumA;
  xOffset = xOffsetA;
  yOffset = yOffsetA;
}

XFAFormField::~XFAFormField() {
  delete name;
  delete dataName;
}

const char *XFAFormField::getType() {
  ZxElement *uiElem;
  ZxNode *node;

  if ((uiElem = xml->findFirstChildElement("ui"))) {
    for (node = uiElem->getFirstChild(); node; node = node->getNextChild()) {
      if (node->isElement("textEdit")) {
	return "Text";
      } else if (node->isElement("barcode")) {
	return "BarCode";
      }
      //~ other field types go here
    }
  }
  return NULL;
}

Unicode *XFAFormField::getName(int *length) {
  //~ assumes name is UTF-8
  return utf8ToUnicode(name, length);
}

Unicode *XFAFormField::getValue(int *length) {
  ZxElement *uiElem;
  ZxNode *node;
  GString *s;

  //~ assumes value is UTF-8
  s = NULL;
  if ((uiElem = xml->findFirstChildElement("ui"))) {
    for (node = uiElem->getFirstChild(); node; node = node->getNextChild()) {
      if (node->isElement("textEdit")) {
	s = getFieldValue("text");
      } else if (node->isElement("barcode")) {
	s = getFieldValue("text");
      }
      //~ other field types go here
    }
  }
  if (!s) {
    return NULL;
  }
  return utf8ToUnicode(s, length);
}

Unicode *XFAFormField::utf8ToUnicode(GString *s, int *length) {
  Unicode *u;
  int n, size, c0, c1, c2, c3, c4, c5, i;

  n = size = 0;
  u = NULL;
  i = 0;
  while (i < s->getLength()) {
    if (n == size) {
      size = size ? size * 2 : 16;
      u = (Unicode *)greallocn(u, size, sizeof(Unicode));
    }
    c0 = s->getChar(i++) & 0xff;
    if (c0 <= 0x7f) {
      u[n++] = c0;
    } else if (c0 <= 0xdf && i < n) {
      c1 = s->getChar(i++) & 0xff;
      u[n++] = ((c0 & 0x1f) << 6) | (c1 & 0x3f);
    } else if (c0 <= 0xef && i+1 < n) {
      c1 = s->getChar(i++) & 0xff;
      c2 = s->getChar(i++) & 0xff;
      u[n++] = ((c0 & 0x0f) << 12) | ((c1 & 0x3f) << 6) | (c2 & 0x3f);
    } else if (c0 <= 0xf7 && i+2 < n) {
      c1 = s->getChar(i++) & 0xff;
      c2 = s->getChar(i++) & 0xff;
      c3 = s->getChar(i++) & 0xff;
      u[n++] = ((c0 & 0x07) << 18) | ((c1 & 0x3f) << 12) | ((c2 & 0x3f) << 6)
	       | (c3 & 0x3f);
    } else if (c0 <= 0xfb && i+3 < n) {
      c1 = s->getChar(i++) & 0xff;
      c2 = s->getChar(i++) & 0xff;
      c3 = s->getChar(i++) & 0xff;
      c4 = s->getChar(i++) & 0xff;
      u[n++] = ((c0 & 0x03) << 24) | ((c1 & 0x3f) << 18) | ((c2 & 0x3f) << 12)
	       | ((c3 & 0x3f) << 6) | (c4 & 0x3f);
    } else if (c0 <= 0xfd && i+4 < n) {
      c1 = s->getChar(i++) & 0xff;
      c2 = s->getChar(i++) & 0xff;
      c3 = s->getChar(i++) & 0xff;
      c4 = s->getChar(i++) & 0xff;
      c5 = s->getChar(i++) & 0xff;
      u[n++] = ((c0 & 0x01) << 30) | ((c1 & 0x3f) << 24) | ((c2 & 0x3f) << 18)
	       | ((c3 & 0x3f) << 12) | ((c4 & 0x3f) << 6) | (c5 & 0x3f);
    } else {
      u[n++] = '?';
    }
  }
  *length = n;
  return u;
}

void XFAFormField::draw(int pageNumA, Gfx *gfx, GBool printing,
			GfxFontDict *fontDict) {
  Page *page;
  PDFRectangle *pageRect;
  ZxElement *uiElem;
  ZxNode *node;
  ZxAttr *attr;
  GString *appearBuf;
  MemStream *appearStream;
  Object appearDict, appearance, obj1, obj2;
  double mat[6];
  double x, y, w, h, x2, y2, w2, h2, x3, y3, w3, h3;
  double anchorX, anchorY;
  int pageRot, rot, rot3;

  if (pageNumA != pageNum) {
    return;
  }

  page = xfaForm->doc->getCatalog()->getPage(pageNum);
  pageRect = page->getMediaBox();
  pageRot = page->getRotate();

  anchorX = 0;
  anchorY = 0;
  if ((attr = xml->findAttr("anchorType"))) {
    if (!attr->getValue()->cmp("topLeft")) {
      anchorX = 0;
      anchorY = 0;
    } else if (!attr->getValue()->cmp("topCenter")) {
      anchorX = 0.5;
      anchorY = 0;
    } else if (!attr->getValue()->cmp("topRight")) {
      anchorX = 1;
      anchorY = 0;
    } else if (!attr->getValue()->cmp("middleLeft")) {
      anchorX = 0;
      anchorY = 0.5;
    } else if (!attr->getValue()->cmp("middleCenter")) {
      anchorX = 0.5;
      anchorY = 0.5;
    } else if (!attr->getValue()->cmp("middleRight")) {
      anchorX = 1;
      anchorY = 0.5;
    } else if (!attr->getValue()->cmp("bottomLeft")) {
      anchorX = 0;
      anchorY = 1;
    } else if (!attr->getValue()->cmp("bottomCenter")) {
      anchorX = 0.5;
      anchorY = 1;
    } else if (!attr->getValue()->cmp("bottomRight")) {
      anchorX = 1;
      anchorY = 1;
    }
  }
  x = getMeasurement(xml->findAttr("x"), 0) + xOffset;
  y = getMeasurement(xml->findAttr("y"), 0) + yOffset;
  w = getMeasurement(xml->findAttr("w"), 0);
  h = getMeasurement(xml->findAttr("h"), 0);
  if ((attr = xml->findAttr("rotate"))) {
    rot = atoi(attr->getValue()->getCString());
    if ((rot %= 360) < 0) {
      rot += 360;
    }
  } else {
    rot = 0;
  }

  // get annot rect (UL corner, width, height) in XFA coords
  // notes:
  // - XFA coordinates are top-left origin, after page rotation
  // - XFA coordinates are dependent on choice of anchor point
  //   and field rotation
  switch (rot) {
  case 0:
  default:
    x2 = x - anchorX * w;
    y2 = y - anchorY * h;
    w2 = w;
    h2 = h;
    break;
  case 90:
    x2 = x - anchorY * h;
    y2 = y - (1 - anchorX) * w;
    w2 = h;
    h2 = w;
    break;
  case 180:
    x2 = x - (1 - anchorX) * w;
    y2 = y - (1 - anchorY) * h;
    w2 = w;
    h2 = h;
    break;
  case 270:
    x2 = x - (1 - anchorY) * h;
    y2 = y - anchorX * w;
    w2 = h;
    h2 = w;
    break;
  }

  // convert annot rect to PDF coords (LL corner, width, height),
  // taking page rotation into account
  switch (pageRot) {
  case 0:
  default:
    x3 = pageRect->x1 + x2;
    y3 = pageRect->y2 - (y2 + h2);
    w3 = w2;
    h3 = h2;
    break;
  case 90:
    x3 = pageRect->x1 + y2;
    y3 = pageRect->y1 + x2;
    w3 = h2;
    h3 = w2;
    break;
  case 180:
    x3 = pageRect->x2 - (x2 + w2);
    y3 = pageRect->y1 + y2;
    w3 = w2;
    h3 = h2;
    break;
  case 270:
    x3 = pageRect->x2 - (y2 + h2);
    y3 = pageRect->y1 + (x2 + w2);
    w3 = h2;
    h3 = w2;
    break;
  }
  rot3 = (rot + pageRot) % 360;

  // generate transform matrix
  switch (rot3) {
  case 0:
  default:
    mat[0] = 1;  mat[1] = 0;
    mat[2] = 0;  mat[3] = 1;
    mat[4] = 0;  mat[5] = 0;
    break;
  case 90:
    mat[0] =  0;  mat[1] = 1;
    mat[2] = -1;  mat[3] = 0;
    mat[4] =  h;  mat[5] = 0;
    break;
  case 180:
    mat[0] = -1;  mat[1] =  0;
    mat[2] =  0;  mat[3] = -1;
    mat[4] =  w;  mat[5] =  h;
    break;
  case 270:
    mat[0] = 0;  mat[1] = -1;
    mat[2] = 1;  mat[3] =  0;
    mat[4] = 0;  mat[5] =  w;
    break;
  }

  // get the appearance stream data
  appearBuf = new GString();
#if 0 //~ for debugging
  appearBuf->appendf("q 1 1 0 rg 0 0 {0:.4f} {1:.4f} re f Q\n", w, h);
#endif
  if ((uiElem = xml->findFirstChildElement("ui"))) {
    for (node = uiElem->getFirstChild(); node; node = node->getNextChild()) {
      if (node->isElement("textEdit")) {
	drawTextEdit(fontDict, w, h, rot3, appearBuf);
	break;
      } else if (node->isElement("barcode")) {
	drawBarCode(fontDict, w, h, rot3, appearBuf);
	break;
      }
      //~ other field types go here
    }
  }

  // create the appearance stream
  appearDict.initDict(xfaForm->doc->getXRef());
  appearDict.dictAdd(copyString("Length"),
		     obj1.initInt(appearBuf->getLength()));
  appearDict.dictAdd(copyString("Subtype"), obj1.initName("Form"));
  obj1.initArray(xfaForm->doc->getXRef());
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(w));
  obj1.arrayAdd(obj2.initReal(h));
  appearDict.dictAdd(copyString("BBox"), &obj1);
  obj1.initArray(xfaForm->doc->getXRef());
  obj1.arrayAdd(obj2.initReal(mat[0]));
  obj1.arrayAdd(obj2.initReal(mat[1]));
  obj1.arrayAdd(obj2.initReal(mat[2]));
  obj1.arrayAdd(obj2.initReal(mat[3]));
  obj1.arrayAdd(obj2.initReal(mat[4]));
  obj1.arrayAdd(obj2.initReal(mat[5]));
  appearDict.dictAdd(copyString("Matrix"), &obj1);
  if (xfaForm->resourceDict.isDict()) {
    appearDict.dictAdd(copyString("Resources"),
		       xfaForm->resourceDict.copy(&obj1));
  }
  appearStream = new MemStream(appearBuf->getCString(), 0,
			       appearBuf->getLength(), &appearDict);
  appearance.initStream(appearStream);
  gfx->drawAnnot(&appearance, NULL, x3, y3, x3 + w3, y3 + h3);
  appearance.free();
  delete appearBuf;
}

void XFAFormField::drawTextEdit(GfxFontDict *fontDict,
				double w, double h, int rot,
				GString *appearBuf) {
  ZxElement *valueElem, *textElem, *uiElem, *textEditElem, *combElem;
  ZxElement *fontElem, *paraElem;
  ZxAttr *attr;
  GString *value, *fontName;
  double fontSize;
  int maxChars, combCells;
  GBool multiLine, bold, italic;
  XFAHorizAlign hAlign;
  XFAVertAlign vAlign;

  if (!(value = getFieldValue("text"))) {
    return;
  }

  maxChars = 0;
  if ((valueElem = xml->findFirstChildElement("value")) &&
      (textElem = valueElem->findFirstChildElement("text")) &&
      (attr = textElem->findAttr("maxChars"))) {
    maxChars = atoi(attr->getValue()->getCString());
  }

  multiLine = gFalse;
  combCells = 0;
  if ((uiElem = xml->findFirstChildElement("ui")) &&
      (textEditElem = uiElem->findFirstChildElement("textEdit"))) {
    if ((attr = textEditElem->findAttr("multiLine")) &&
	!attr->getValue()->cmp("1")) {
      multiLine = gTrue;
    }
    if ((combElem = textEditElem->findFirstChildElement("comb"))) {
      if ((attr = combElem->findAttr("numberOfCells"))) {
	combCells = atoi(attr->getValue()->getCString());
      } else {
	combCells = maxChars;
      }
    }
  }
	
  fontName = NULL;
  fontSize = 10;
  bold = gFalse;
  italic = gFalse;
  if ((fontElem = xml->findFirstChildElement("font"))) {
    if ((attr = fontElem->findAttr("typeface"))) {
      fontName = attr->getValue()->copy();
    }
    if ((attr = fontElem->findAttr("weight"))) {
      if (!attr->getValue()->cmp("bold")) {
	bold = gTrue;
      }
    }
    if ((attr = fontElem->findAttr("posture"))) {
      if (!attr->getValue()->cmp("italic")) {
	italic = gTrue;
      }
    }
    if ((attr = fontElem->findAttr("size"))) {
      fontSize = getMeasurement(attr, fontSize);
    }
  }
  if (!fontName) {
    fontName = new GString("Courier");
  }

  hAlign = xfaHAlignLeft;
  vAlign = xfaVAlignTop;
  if ((paraElem = xml->findFirstChildElement("para"))) {
    if ((attr = paraElem->findAttr("hAlign"))) {
      if (!attr->getValue()->cmp("left")) {
	hAlign = xfaHAlignLeft;
      } else if (!attr->getValue()->cmp("center")) {
	hAlign = xfaHAlignCenter;
      } else if (!attr->getValue()->cmp("right")) {
	hAlign = xfaHAlignRight;
      }
      //~ other hAlign values (justify, justifyAll, radix) are
      //~   currently unsupported
    }
    if ((attr = paraElem->findAttr("vAlign"))) {
      if (!attr->getValue()->cmp("top")) {
	vAlign = xfaVAlignTop;
      } else if (!attr->getValue()->cmp("bottom")) {
	vAlign = xfaVAlignBottom;
      } else if (!attr->getValue()->cmp("middle")) {
	vAlign = xfaVAlignMiddle;
      }
    }
  }

  drawText(value, multiLine, combCells,
	   fontName, bold, italic, fontSize,
	   hAlign, vAlign, 0, 0, w, h, gFalse, fontDict, appearBuf);
  delete fontName;
}

void XFAFormField::drawBarCode(GfxFontDict *fontDict,
			       double w, double h, int rot,
			       GString *appearBuf) {
  ZxElement *uiElem, *barcodeElem, *fontElem;
  ZxAttr *attr;
  GString *value, *value2, *barcodeType, *textLocation, *fontName, *s1, *s2;
  XFAVertAlign textAlign;
  double wideNarrowRatio, fontSize;
  double yText, wText, yBarcode, hBarcode, wNarrow, xx;
  GBool doText;
  int dataLength;
  GBool bold, italic;
  char *p;
  int i, j, c;

  //--- get field value
  if (!(value = getFieldValue("text"))) {
    return;
  }

  //--- get field attributes
  barcodeType = NULL;
  wideNarrowRatio = 3;
  dataLength = 0;
  textLocation = NULL;
  if ((uiElem = xml->findFirstChildElement("ui")) &&
      (barcodeElem = uiElem->findFirstChildElement("barcode"))) {
    if ((attr = barcodeElem->findAttr("type"))) {
      barcodeType = attr->getValue();
    }
    if ((attr = barcodeElem->findAttr("wideNarrowRatio"))) {
      s1 = attr->getValue();
      if ((p = strchr(s1->getCString(), ':'))) {
	s2 = new GString(s1, 0, p - s1->getCString());
	wideNarrowRatio = atof(p + 1);
	if (wideNarrowRatio == 0) {
	  wideNarrowRatio = 1;
	}
	wideNarrowRatio = atof(s2->getCString()) / wideNarrowRatio;
	delete s2;
      } else {
	wideNarrowRatio = atof(s1->getCString());
      }
    }
    if ((attr = barcodeElem->findAttr("dataLength"))) {
      dataLength = atoi(attr->getValue()->getCString());
    }
    if ((attr = barcodeElem->findAttr("textLocation"))) {
      textLocation = attr->getValue();
    }
  }
  if (!barcodeType) {
    error(errSyntaxError, -1, "Missing 'type' attribute in XFA barcode field");
    return;
  }
  if (!dataLength) {
    error(errSyntaxError, -1,
	  "Missing 'dataLength' attribute in XFA barcode field");
    return;
  }

  //--- get font
  fontName = NULL;
  fontSize = 0.2 * h;
  bold = gFalse;
  italic = gFalse;
  if ((fontElem = xml->findFirstChildElement("font"))) {
    if ((attr = fontElem->findAttr("typeface"))) {
      fontName = attr->getValue()->copy();
    }
    if ((attr = fontElem->findAttr("weight"))) {
      if (!attr->getValue()->cmp("bold")) {
	bold = gTrue;
      }
    }
    if ((attr = fontElem->findAttr("posture"))) {
      if (!attr->getValue()->cmp("italic")) {
	italic = gTrue;
      }
    }
    if ((attr = fontElem->findAttr("size"))) {
      fontSize = getMeasurement(attr, fontSize);
    }
  }
  if (!fontName) {
    fontName = new GString("Courier");
  }

  //--- compute the embedded text type position
  doText = gTrue;
  yText = yBarcode = hBarcode = 0;
  if (textLocation && !textLocation->cmp("above")) {
    textAlign = xfaVAlignTop;
    yText = h;
    yBarcode = 0;
    hBarcode = h - fontSize;
  } else if (textLocation && !textLocation->cmp("belowEmbedded")) {
    textAlign = xfaVAlignBottom;
    yText = 0;
    yBarcode = 0;
    hBarcode = h;
  } else if (textLocation && !textLocation->cmp("aboveEmbedded")) {
    textAlign = xfaVAlignTop;
    yText = h;
    yBarcode = 0;
    hBarcode = h;
  } else if (textLocation && !textLocation->cmp("none")) {
    textAlign = xfaVAlignBottom; // make gcc happy
    doText = gFalse;
  } else { // default is "below"
    textAlign = xfaVAlignBottom;
    yText = 0;
    yBarcode = fontSize;
    hBarcode = h - fontSize;
  }
  wText = w;

  //--- remove extraneous start/stop chars
  //~ this may depend on barcode type
  value2 = value->copy();
  if (value2->getLength() >= 1 && value2->getChar(0) == '*') {
    value2->del(0);
  }
  if (value2->getLength() >= 1 &&
      value2->getChar(value2->getLength() - 1) == '*') {
    value2->del(value2->getLength() - 1);
  }

  //--- draw the bar code
  if (!barcodeType->cmp("code3Of9")) {
    appearBuf->append("0 g\n");
    wNarrow = w / ((7 + 3 * wideNarrowRatio) * (dataLength + 2));
    xx = 0;
    for (i = -1; i <= value2->getLength(); ++i) {
      if (i < 0 || i >= value2->getLength()) {
	c = '*';
      } else {
	c = value2->getChar(i) & 0x7f;
      }
      for (j = 0; j < 10; j += 2) {
	appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} re f\n",
			   xx, yBarcode,
			   (code3Of9Data[c][j] ? wideNarrowRatio : 1) * wNarrow,
			   hBarcode);
	xx += ((code3Of9Data[c][j] ? wideNarrowRatio : 1) +
	       (code3Of9Data[c][j+1] ? wideNarrowRatio : 1)) * wNarrow;
      }
    }
    // center the text on the drawn barcode (not the max length barcode)
    wText = (value2->getLength() + 2) * (7 + 3 * wideNarrowRatio) * wNarrow;
  } else {
    error(errSyntaxError, -1,
	  "Unimplemented barcode type in XFA barcode field");
  }
  //~ add other barcode types here

  //--- draw the embedded text
  if (doText) {
    appearBuf->append("0 g\n");
    drawText(value2, gFalse, 0,
	     fontName, bold, italic, fontSize,
	     xfaHAlignCenter, textAlign, 0, yText, wText, h, gTrue,
	     fontDict, appearBuf);
  }
  delete fontName;
  delete value2;
}

Object *XFAFormField::getResources(Object *res) {
  return xfaForm->resourceDict.copy(res);
}

double XFAFormField::getMeasurement(ZxAttr *attr, double defaultVal) {
  GString *s;
  double val, mul;
  GBool neg;
  int i;

  if (!attr) {
    return defaultVal;
  }
  s = attr->getValue();
  i = 0;
  neg = gFalse;
  if (i < s->getLength() && s->getChar(i) == '+') {
    ++i;
  } else if (i < s->getLength() && s->getChar(i) == '-') {
    neg = gTrue;
    ++i;
  }
  val = 0;
  while (i < s->getLength() && s->getChar(i) >= '0' && s->getChar(i) <= '9') {
    val = val * 10 + s->getChar(i) - '0';
    ++i;
  }
  if (i < s->getLength() && s->getChar(i) == '.') {
    ++i;
    mul = 0.1;
    while (i < s->getLength() && s->getChar(i) >= '0' && s->getChar(i) <= '9') {
      val += mul * (s->getChar(i) - '0');
      mul *= 0.1;
      ++i;
    }
  }
  if (neg) {
    val = -val;
  }
  if (i+1 < s->getLength()) {
    if (s->getChar(i) == 'i' && s->getChar(i+1) == 'n') {
      val *= 72;
    } else if (s->getChar(i) == 'p' && s->getChar(i+1) == 't') {
      // no change
    } else if (s->getChar(i) == 'c' && s->getChar(i+1) == 'm') {
      val *= 72 / 2.54;
    } else if (s->getChar(i) == 'm' && s->getChar(i+1) == 'm') {
      val *= 72 / 25.4;
    } else {
      // default to inches
      val *= 72;
    }
  } else {
    // default to inches
    val *= 72;
  }
  return val;
}

GString *XFAFormField::getFieldValue(const char *valueChildType) {
  ZxElement *valueElem, *datasets, *data, *elem;
  char *p;

  // check the <value> element within the field
  if ((valueElem = xml->findFirstChildElement("value")) &&
      (elem = valueElem->findFirstChildElement(valueChildType))) {
    if (elem->getFirstChild() &&
	elem->getFirstChild()->isCharData() &&
	((ZxCharData *)elem->getFirstChild())->getData()->getLength() > 0) {
      return ((ZxCharData *)elem->getFirstChild())->getData();
    }
  }

  // check the <datasets> packet
  if (!xfaForm->xml->getRoot() ||
      !(datasets =
	  xfaForm->xml->getRoot()->findFirstChildElement("xfa:datasets")) ||
      !(data = datasets->findFirstChildElement("xfa:data"))) {
    return NULL;
  }
  p = name->getCString();
  if (!strncmp(p, "form.",  5)) {
    p += 5;
  } else {
    return NULL;
  }
  elem = findFieldData(data, p);
  if (elem &&
      elem->getFirstChild() &&
      elem->getFirstChild()->isCharData() &&
      ((ZxCharData *)elem->getFirstChild())->getData()->getLength() > 0) {
    return ((ZxCharData *)elem->getFirstChild())->getData();
  }

  return NULL;
}

ZxElement *XFAFormField::findFieldData(ZxElement *elem, char *partName) {
  ZxNode *node;
  GString *nodeName;
  int curIdx, idx, n;

  curIdx = 0;
  for (node = elem->getFirstChild(); node; node = node->getNextChild()) {
    if (node->isElement()) {
      nodeName = ((ZxElement *)node)->getType();
      n = nodeName->getLength();
      if (!strncmp(partName, nodeName->getCString(), n)) { 
	if (partName[n] == '[') {
	  idx = atoi(partName + n + 1);
	  if (idx == curIdx) {
	    for (++n; partName[n] && partName[n-1] != ']'; ++n) ;
	  } else {
	    ++curIdx;
	    continue;
	  }
	}
	if (!partName[n]) {
	  return (ZxElement *)node;
	} else if (partName[n] == '.') {
	  return findFieldData((ZxElement *)node, partName + n + 1);
	}
      }
    }
  }
  return NULL;
}

void XFAFormField::transform(int rot, double w, double h,
			     double *wNew, double *hNew, GString *appearBuf) {
  switch (rot) {
  case 0:
  default:
    appearBuf->appendf("1 0 0 1 0 {0:.4f} cm\n", -h);
    break;
  case 90:
    appearBuf->appendf("0 1 -1 0 {0:.4f} 0 cm\n", w);
    *wNew = h;
    *hNew = w;
    break;
  case 180:
    appearBuf->appendf("-1 0 0 -1 {0:.4f} {1:.4f} cm\n", w, h);
    *wNew = w;
    *hNew = h;
    break;
  case 270:
    appearBuf->appendf("0 -1 1 0 0 {0:.4f} cm\n", h);
    *wNew = h;
    *hNew = w;
    break;
  }
}

void XFAFormField::drawText(GString *text, GBool multiLine, int combCells,
			    GString *fontName, GBool bold,
			    GBool italic, double fontSize,
			    XFAHorizAlign hAlign, XFAVertAlign vAlign,
			    double x, double y, double w, double h,
			    GBool whiteBackground,
			    GfxFontDict *fontDict, GString *appearBuf) {
  GfxFont *font;
  GString *s;
  double xx, yy, tw, charWidth, lineHeight;
  double rectX, rectY, rectW, rectH;
  int line, i, j, k, c, rectI;

  //~ deal with Unicode text (is it UTF-8?)

  // find the font
  if (!(font = findFont(fontDict, fontName, bold, italic))) {
    error(errSyntaxError, -1, "Couldn't find a font for '{0:t}', {1:s}, {2:s} used in XFA field",
	  fontName, bold ? "bold" : "non-bold",
	  italic ? "italic" : "non-italic");
    return;
  }

  // setup
  rectW = rectH = 0;
  rectI = appearBuf->getLength();
  appearBuf->append("BT\n");
  appearBuf->appendf("/{0:t} {1:.2f} Tf\n", font->getTag(), fontSize);

  // multi-line text
  if (multiLine) {

    // figure out how many lines will fit
    lineHeight = 1.2 * fontSize;

    // write a series of lines of text
    line = 0;
    i = 0;
    while (i < text->getLength()) {

      getNextLine(text, i, font, fontSize, w, &j, &tw, &k);
      if (tw > rectW) {
	rectW = tw;
      }

      // compute text start position
      switch (hAlign) {
      case xfaHAlignLeft:
      default:
	xx = x;
	break;
      case xfaHAlignCenter:
	xx = x + 0.5 * (w - tw);
	break;
      case xfaHAlignRight:
	xx = x + w - tw;
	break;
      }
      yy = y + h - fontSize * font->getAscent() - line * lineHeight;

      // draw the line
      appearBuf->appendf("1 0 0 1 {0:.4f} {1:.4f} Tm\n", xx, yy);
      appearBuf->append('(');
      for (; i < j; ++i) {
	c = text->getChar(i) & 0xff;
	if (c == '(' || c == ')' || c == '\\') {
	  appearBuf->append('\\');
	  appearBuf->append(c);
	} else if (c < 0x20 || c >= 0x80) {
	  appearBuf->appendf("\\{0:03o}", c);
	} else {
	  appearBuf->append(c);
	}
      }
      appearBuf->append(") Tj\n");

      // next line
      i = k;
      ++line;
    }
    rectH = line * lineHeight;
    rectY = y + h - rectH;

  // comb formatting
  } else if (combCells > 0) {

    // compute comb spacing
    tw = w / combCells;

    // compute text start position
    switch (hAlign) {
    case xfaHAlignLeft:
    default:
      xx = x;
      break;
    case xfaHAlignCenter:
      xx = x + (int)(0.5 * (combCells - text->getLength())) * tw;
      break;
    case xfaHAlignRight:
      xx = x + w - text->getLength() * tw;
      break;
    }
    rectW = text->getLength() * tw;
    switch (vAlign) {
    case xfaVAlignTop:
    default:
      yy = y + h - fontSize * font->getAscent();
      break;
    case xfaVAlignMiddle:
      yy = y + 0.5 * (h - fontSize * (font->getAscent() +
				      font->getDescent()));
      break;
    case xfaVAlignBottom:
      yy = y - fontSize * font->getDescent();
      break;
    }
    rectY = yy + fontSize * font->getDescent();
    rectH = fontSize * (font->getAscent() - font->getDescent());

    // write the text string
    for (i = 0; i < text->getLength(); ++i) {
      c = text->getChar(i) & 0xff;
      if (!font->isCIDFont()) {
	charWidth = fontSize * ((Gfx8BitFont *)font)->getWidth(c);
	appearBuf->appendf("1 0 0 1 {0:.4f} {1:.4f} Tm\n",
			   xx + i * tw + 0.5 * (tw - charWidth), yy);
      } else {
	appearBuf->appendf("1 0 0 1 {0:.4f} {1:.4f} Tm\n",
			   xx + i * tw, yy);
      }
      appearBuf->append('(');
      if (c == '(' || c == ')' || c == '\\') {
	appearBuf->append('\\');
	appearBuf->append(c);
      } else if (c < 0x20 || c >= 0x80) {
	appearBuf->appendf("{0:.4f} 0 Td\n", w);
      } else {
	appearBuf->append(c);
      }
      appearBuf->append(") Tj\n");
    }

  // regular (non-comb) formatting
  } else {

    // compute string width
    if (!font->isCIDFont()) {
      tw = 0;
      for (i = 0; i < text->getLength(); ++i) {
	tw += ((Gfx8BitFont *)font)->getWidth(text->getChar(i));
      }
    } else {
      // otherwise, make a crude estimate
      tw = text->getLength() * 0.5;
    }
    tw *= fontSize;
    rectW = tw;

    // compute text start position
    switch (hAlign) {
    case xfaHAlignLeft:
    default:
      xx = x;
      break;
    case xfaHAlignCenter:
      xx = x + 0.5 * (w - tw);
      break;
    case xfaHAlignRight:
      xx = x + w - tw;
      break;
    }
    switch (vAlign) {
    case xfaVAlignTop:
    default:
      yy = y + h - fontSize * font->getAscent();
      break;
    case xfaVAlignMiddle:
      yy = y + 0.5 * (h - fontSize * (font->getAscent() +
				      font->getDescent()));
      break;
    case xfaVAlignBottom:
      yy = y - fontSize * font->getDescent();
      break;
    }
    rectY = yy + fontSize * font->getDescent();
    rectH = fontSize * (font->getAscent() - font->getDescent());
    appearBuf->appendf("{0:.4f} {1:.4f} Td\n", xx, yy);

    // write the text string
    appearBuf->append('(');
    for (i = 0; i < text->getLength(); ++i) {
      c = text->getChar(i) & 0xff;
      if (c == '(' || c == ')' || c == '\\') {
	appearBuf->append('\\');
	appearBuf->append(c);
      } else if (c < 0x20 || c >= 0x80) {
	appearBuf->appendf("\\{0:03o}", c);
      } else {
	appearBuf->append(c);
      }
    }
    appearBuf->append(") Tj\n");
  }

  // cleanup
  appearBuf->append("ET\n");

  // draw a white rectangle behind the text
  if (whiteBackground) {
    switch (hAlign) {
    case xfaHAlignLeft:
    default:
      rectX = x;
      break;
    case xfaHAlignCenter:
      rectX = x + 0.5 * (w - rectW);
      break;
    case xfaHAlignRight:
      rectX = x + w - rectW;
      break;
    }
    rectX -= 0.25 * fontSize;
    rectW += 0.5 * fontSize;
    s = GString::format("q 1 g {0:.4f} {1:.4f} {2:.4f} {3:.4f} re f Q\n",
			rectX, rectY, rectW, rectH);
    appearBuf->insert(rectI, s);
    delete s;
  }
}

// Searches <fontDict> for a font matching(<fontName>, <bold>,
// <italic>).
GfxFont *XFAFormField::findFont(GfxFontDict *fontDict, GString *fontName,
				GBool bold, GBool italic) {
  GString *reqName, *testName;
  GfxFont *font;
  GBool foundName, foundBold, foundItalic;
  char *p;
  char c;
  int i, j;

  if (!fontDict) {
    return NULL;
  }

  reqName = new GString();
  for (i = 0; i < fontName->getLength(); ++i) {
    c = fontName->getChar(i);
    if (c != ' ') {
      reqName->append(c);
    }
  }

  for (i = 0; i < fontDict->getNumFonts(); ++i) {
    font = fontDict->getFont(i);
    if (!font || !font->getName()) {
      continue;
    }
    testName = new GString();
    for (j = 0; j < font->getName()->getLength(); ++j) {
      c = font->getName()->getChar(j);
      if (c != ' ') {
	testName->append(c);
      }
    }
    foundName = foundBold = foundItalic = gFalse;
    for (p = testName->getCString(); *p; ++p) {
      if (!strncasecmp(p, reqName->getCString(), reqName->getLength())) {
	foundName = gTrue;
      }
      if (!strncasecmp(p, "bold", 4)) {
	foundBold = gTrue;
      }
      if (!strncasecmp(p, "italic", 6) || !strncasecmp(p, "oblique", 7)) {
	foundItalic = gTrue;
      }
    }
    delete testName;
    if (foundName && foundBold == bold && foundItalic == italic) {
      delete reqName;
      return font;
    }
  }

  delete reqName;
  return NULL;
}

// Figure out how much text will fit on the next line.  Returns:
// *end = one past the last character to be included
// *width = width of the characters start .. end-1
// *next = index of first character on the following line
void XFAFormField::getNextLine(GString *text, int start,
			       GfxFont *font, double fontSize, double wMax,
			       int *end, double *width, int *next) {
  double w, dw;
  int j, k, c;

  // figure out how much text will fit on the line
  //~ what does Adobe do with tabs?
  w = 0;
  for (j = start; j < text->getLength() && w <= wMax; ++j) {
    c = text->getChar(j) & 0xff;
    if (c == 0x0a || c == 0x0d) {
      break;
    }
    if (font && !font->isCIDFont()) {
      dw = ((Gfx8BitFont *)font)->getWidth(c) * fontSize;
    } else {
      // otherwise, make a crude estimate
      dw = 0.5 * fontSize;
    }
    w += dw;
  }
  if (w > wMax) {
    for (k = j; k > start && text->getChar(k-1) != ' '; --k) ;
    for (; k > start && text->getChar(k-1) == ' '; --k) ;
    if (k > start) {
      j = k;
    }
    if (j == start) {
      // handle the pathological case where the first character is
      // too wide to fit on the line all by itself
      j = start + 1;
    }
  }
  *end = j;

  // compute the width
  w = 0;
  for (k = start; k < j; ++k) {
    if (font && !font->isCIDFont()) {
      dw = ((Gfx8BitFont *)font)->getWidth(text->getChar(k)) * fontSize;
    } else {
      // otherwise, make a crude estimate
      dw = 0.5 * fontSize;
    }
    w += dw;
  }
  *width = w;

  // next line
  while (j < text->getLength() && text->getChar(j) == ' ') {
    ++j;
  }
  if (j < text->getLength() && text->getChar(j) == 0x0d) {
    ++j;
  }
  if (j < text->getLength() && text->getChar(j) == 0x0a) {
    ++j;
  }
  *next = j;
}
