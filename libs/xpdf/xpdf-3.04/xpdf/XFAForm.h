//========================================================================
//
// XFAForm.h
//
// Copyright 2012 Glyph & Cog, LLC
//
//========================================================================

#ifndef XFAFORM_H
#define XFAFORM_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "Form.h"

class ZxDoc;
class ZxElement;
class ZxAttr;

//------------------------------------------------------------------------

enum XFAHorizAlign {
  xfaHAlignLeft,
  xfaHAlignCenter,
  xfaHAlignRight
};

enum XFAVertAlign {
  xfaVAlignTop,
  xfaVAlignBottom,
  xfaVAlignMiddle
};

//------------------------------------------------------------------------

class XFAForm: public Form {
public:

  static XFAForm *load(PDFDoc *docA, Object *acroFormObj, Object *xfaObj);

  virtual ~XFAForm();

  virtual const char *getType() { return "XFA"; }

  virtual void draw(int pageNum, Gfx *gfx, GBool printing);

  virtual int getNumFields();
  virtual FormField *getField(int idx);

private:

  XFAForm(PDFDoc *docA, ZxDoc *xmlA, Object *resourceDictA, GBool fullXFAA);
  void scanFields(ZxElement *elem, GString *name, GString *dataName);

  ZxDoc *xml;
  GList *fields;		// [XFAFormField]
  Object resourceDict;
  GBool fullXFA;		// true for "Full XFA", false for
				//   "XFA Foreground"
  int curPageNum;		// current page number - used by scanFields()
  double curXOffset,		// current x,y offset - used by scanFields()
         curYOffset;

  friend class XFAFormField;
};

//------------------------------------------------------------------------

class XFAFormField: public FormField {
public:

  XFAFormField(XFAForm *xfaFormA, ZxElement *xmlA, GString *nameA,
	       GString *dataNameA, int pageNumA,
	       double xOffsetA, double yOffsetA);

  virtual ~XFAFormField();

  virtual const char *getType();
  virtual Unicode *getName(int *length);
  virtual Unicode *getValue(int *length);

  virtual Object *getResources(Object *res);

private:

  Unicode *utf8ToUnicode(GString *s, int *length);
  void draw(int pageNumA, Gfx *gfx, GBool printing, GfxFontDict *fontDict);
  void drawTextEdit(GfxFontDict *fontDict,
		    double w, double h, int rot,
		    GString *appearBuf);
  void drawBarCode(GfxFontDict *fontDict,
		   double w, double h, int rot,
		   GString *appearBuf);
  static double getMeasurement(ZxAttr *attr, double defaultVal);
  GString *getFieldValue(const char *valueChildType);
  ZxElement *findFieldData(ZxElement *elem, char *partName);
  void transform(int rot, double w, double h,
		 double *wNew, double *hNew, GString *appearBuf);
  void drawText(GString *text, GBool multiLine, int combCells,
		GString *fontName, GBool bold,
		GBool italic, double fontSize,
		XFAHorizAlign hAlign, XFAVertAlign vAlign,
		double x, double y, double w, double h,
		GBool whiteBackground,
		GfxFontDict *fontDict, GString *appearBuf);
  GfxFont *findFont(GfxFontDict *fontDict, GString *fontName,
		    GBool bold, GBool italic);
  void getNextLine(GString *text, int start,
		   GfxFont *font, double fontSize, double wMax,
		   int *end, double *width, int *next);

  XFAForm *xfaForm;
  ZxElement *xml;
  GString *name;
  GString *dataName;
  int pageNum;
  double xOffset, yOffset;

  friend class XFAForm;
};

#endif
