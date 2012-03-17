/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is sila.mozdev.org code.
 *
 * The Initial Developer of the Original Code is 
 * Keith Stribley.
 * Portions created by the Initial Developer are Copyright (C) 2004
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GRUTFTXTSRC_INCLUDED
#define GRUTFTXTSRC_INCLUDED

#include <vector>
#include <assert.h>

#include <graphite/GrAppData.h>
#include <graphite/GrStructs.h>
#include <graphite/ITextSource.h>

class nsIGrSegWrapper;

/*-----------------------------------------------------------------------------
  Class: ITextSource
  This class provides an interface of a text source for the Graphite engine.
------------------------------------------------------------------------------*/
class GrUtfTextSrc : public gr::IColorTextSource
{
public:
  GrUtfTextSrc();
  ~GrUtfTextSrc();
  bool setText(const gr::utf8 * pszText, int len);
  bool setText(const char * pszText, int len);
  bool setText(const gr::utf16 * pszText, int len);
  bool setText(const gr::utf32 * pszText, int len);
  void setColors(int foreground, int background) 
  {
    mForeground = foreground; 
    mBackground = background;
  };
  //void setSelectionDetails(nsIGrSegWrapper * wrapper);
  void setFont(gr::Font * font) { mFont = font; };
  void setPointSize(float & pointSize) { mPointSize = pointSize; };
  // --------------------------------------------------------------------------
  // New V2 interface:
  
  virtual gr::UtfType utfEncodingForm() { return mType; };
  virtual size_t getLength() { return mLength; };
  virtual size_t fetch(gr::toffset ichMin, size_t cch, gr::utf32 * prgchBuffer);
  virtual size_t fetch(gr::toffset ichMin, size_t cch, gr::utf16 * prgchwBuffer);
  virtual size_t fetch(gr::toffset ichMin, size_t cch, gr::utf8  * prgchsBuffer);
  virtual gr::GrResult getFaceName(int /*ich*/, unsigned int /*cchMax*/,
    gr::utf16 * prgchFaceName, unsigned int * pcchLen) 
  {
    prgchFaceName[0] = 0;
    *pcchLen = 0;
    return gr::kresNotImpl; 
  };
  //virtual std::wstring getFaceName(int ich) { return mFont->getFaceName(); };
  virtual float getFontSize(gr::toffset ich);
  virtual bool getBold(gr::toffset ich);
  virtual bool getItalic(gr::toffset ich);
  virtual bool getRightToLeft(gr::toffset ich);
  virtual unsigned int getDirectionDepth(gr::toffset ich);
  virtual float getVerticalOffset(gr::toffset /*ich*/) { return 0;};
  virtual gr::isocode getLanguage(gr::toffset ich);

  virtual std::pair<gr::toffset, gr::toffset> propertyRange(gr::toffset ich);
  virtual size_t getFontFeatures(gr::toffset ich, gr::FeatureSetting * prgfset);
  virtual bool sameSegment(gr::toffset /*ich1*/, gr::toffset /*ich2*/) { return true; };
  virtual bool featureVariations() { return false; };

protected:
  bool checkBuffer8();
  bool checkBuffer16();
  bool checkBuffer32();

private:
  size_t mLength;
  size_t mBufferLength8;
  size_t mBufferLength16;
  size_t mBufferLength32;
  gr::utf8 * mData8;
  gr::utf16 * mData16;
  gr::utf32 * mData32;
  gr::UtfType mType;
  int mForeground;
  int mBackground;
  int mSelectForeground;
  int mSelectBackground;
  bool mIsSelected;
  std::wstring mFaceName;
  std::vector<bool> mSelectionVector;
  gr::Font * mFont;
  bool mRtl;
  float mPointSize;

public: // methods that will go
  // Temporary--eventually these will be of interest only to SegmentPainter.
  virtual void getColors(gr::toffset ich, int * pclrFore, int * pclrBack);

  // Shouldn't be here!  
  virtual gr::GrResult Fetch(int /*ichMin*/, int /*ichLim*/, gr::utf16 * /*prgchBuf*/) { return gr::kresNotImpl; };
  virtual gr::GrResult get_Length(int * /*pcch*/) { return gr::kresNotImpl; };
  virtual gr::GrResult GetFontVariations(int /*ich*/,
    wchar_t * /*prgchFontVar*/, int /*ichMax*/, int * /*pich*/,
    int * /*pichMin*/, int * /*pichLim*/) { return gr::kresNotImpl; };

};


#if !defined(GR_NAMESPACE)
using namespace gr;
#endif

#endif // !GRUTFTXTSRC_INCLUDED

