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

#include <graphite/GrClient.h>
#include <graphite/ITextSource.h>
#include <graphite/IGrJustifier.h>
#include <graphite/IGrEngine.h>
#include <graphite/SegmentAux.h>
#include <graphite/Font.h>
#include <graphite/Segment.h>
#include <graphite/GraphiteProcess.h>
#include <graphite/FileFont.h>

#include "GrUtfTextSrc.h"



GrUtfTextSrc::GrUtfTextSrc()
: mLength(0), 
	mBufferLength8(64), mBufferLength16(64), mBufferLength32(64),
	mData8(NULL), mData16(NULL), mData32(NULL), 
	mType(gr::kutf16), 
	mForeground(gr::kclrBlack), mBackground(gr::kclrTransparent), 
	mSelectForeground(0), mSelectBackground(0),
	mIsSelected(false),
	mFont(NULL),
	mRtl(false)
{

}

GrUtfTextSrc::~GrUtfTextSrc()
{
	if (mData8) delete [] mData8;
	if (mData16) delete [] mData16;
	if (mData32) delete [] mData32;
	mSelectionVector.clear();
}

bool GrUtfTextSrc::setText(const char * pszText, int len)
{
	return setText((gr::utf8*)pszText, len);
}

bool GrUtfTextSrc::setText(const gr::utf8 * pszText, int len)
{
	mType = gr::kutf8;
	mLength = len;
	if (!checkBuffer8()) return false;
	
	for(size_t i = 0; i < mLength; i++)
	{
		mData8[i] = pszText[i];
		// stray line feeds and other control characters from the html file 
		// cause artificial truncation of segments
		if (mData8[i] < 0x0020)//(pszText[i] == '\n' || pszText[i] == '\r') 
			mData8[i] = 0x20; // ZWSP
	}
	mData8[mLength] = '\0'; // zero-terminate
	mSelectionVector.clear();
	mIsSelected = false;
	return true;
}


bool GrUtfTextSrc::setText(const gr::utf16 * pszText, int len)
{
	mType = gr::kutf16;
	mLength = len;
	if (!checkBuffer16()) return false;
	
	for(size_t i = 0; i < mLength; i++)
	{
		mData16[i] = pszText[i];
		// stray line feeds and other control characters from the html file 
		// cause artificial truncation of segments
		if (mData16[i] < 0x0020)//(pszText[i] == '\n' || pszText[i] == '\r') 
			mData16[i] = 0x200B; // ZWSP
	}
	mData16[mLength] = '\0'; // zero-terminate
	mSelectionVector.clear();
	mIsSelected = false;
	return true;
}

bool GrUtfTextSrc::setText(const gr::utf32 * pszText, int len)
{
	mType = gr::kutf32;
	mLength = len;
	if (!checkBuffer32()) return false;
	
	for(size_t i = 0; i < mLength; i++)
	{
		mData32[i] = pszText[i];
	}
	mData32[mLength] = '\0'; // zero-terminate
	mSelectionVector.clear();
	mIsSelected = false;
	return true;
}

bool GrUtfTextSrc::checkBuffer8(void)
{
	if (!mData8 || mBufferLength8 < mLength + 1) 
	{
		do 
		{
			mBufferLength8 *= 2;
		} while (mBufferLength8 < mLength + 1);
		if (mData8) delete [] mData8;
		mData8 = new gr::utf8[mBufferLength8];
	}
	return (mData8) ? true : false;
}

bool GrUtfTextSrc::checkBuffer16(void)
{
	if (!mData16 || mBufferLength16 < mLength + 1) 
	{
		do 
		{
			mBufferLength16 *= 2;
		} while (mBufferLength16 < mLength + 1);
		if (mData16) delete [] mData16;
		mData16 = new gr::utf16[mBufferLength16];
	}
	return (mData16) ? true : false;
}

bool GrUtfTextSrc::checkBuffer32(void)
{
	if (!mData32 || mBufferLength32 < mLength + 1) 
	{
		do 
		{
			mBufferLength32 *= 2;
		} while (mBufferLength32 < mLength + 1);
		if (mData32) delete [] mData32;
		mData32 = new gr::utf32[mBufferLength32];
	}
	return (mData32) ? true : false;
}

size_t GrUtfTextSrc::fetch(gr::toffset ichMin, size_t cch, gr::utf8 * prgchwBuffer) 
{
	assert(cch <= mLength);
	if (cch > mLength)
	{
		return 0;
	}
	std::copy(mData8 + ichMin, mData8 + ichMin + cch, prgchwBuffer);
	return (cch - ichMin);
}

size_t GrUtfTextSrc::fetch(gr::toffset ichMin, size_t cch, gr::utf16 * prgchwBuffer) 
{
	assert(cch <= mLength);
	if (cch > mLength)
	{
		return 0;
	}
	std::copy(mData16 + ichMin, mData16 + ichMin + cch, prgchwBuffer);
	return (cch - ichMin);
}

size_t GrUtfTextSrc::fetch(gr::toffset ichMin, size_t cch, gr::utf32 * prgchwBuffer) 
{
	assert(cch <= mLength);
	if (cch > mLength)
	{
		return 0;
	}
	std::copy(mData32 + ichMin,	mData32 + ichMin + cch, prgchwBuffer);
	return (cch - ichMin);
}


bool GrUtfTextSrc::getRightToLeft(gr::toffset /*ich*/)
{
	return mRtl; // assumes src only contains one direction
}

unsigned int GrUtfTextSrc::getDirectionDepth(gr::toffset /*ich*/)
{
	return (mRtl) ? 1 : 0; // TBD
}

std::pair<gr::toffset, gr::toffset> GrUtfTextSrc::propertyRange(gr::toffset ich)
{
	std::pair<gr::toffset, gr::toffset> range(0, mLength);
	if (mIsSelected)
	{
		bool selectState = mSelectionVector[ich];
		// find start
		int s = ich - 1;
		int e = ich + 1;
		if (s > -1)
		for ( ; s >= 0; s--)
		{
			if (mSelectionVector[s] != selectState)
			{
				s++; // backup
				break;
			}
		}
		if (s < 0) s = 0;
		// find end
		for ( ; e < static_cast<int>(mLength); e++)
		{
			if (mSelectionVector[e] != selectState)
			{
				// don't need to backup for end
				break;
			}
		}
		range.first = s;
		range.second = e;
	}
	return range;
}
	
size_t GrUtfTextSrc::getFontFeatures(gr::toffset /*ich*/, gr::FeatureSetting * /*prgfset*/)
{
	return 0;
}

void GrUtfTextSrc::getColors(gr::toffset ich, int * pclrFore, int * pclrBack)
{
	// selections are handled here
	if (mIsSelected && mSelectionVector[ich])
	{
		*pclrFore = mSelectForeground;
		*pclrBack = mSelectBackground;
	}
	else
	{
		*pclrFore = mForeground;
		*pclrBack = mBackground;
	}
}


// these should be called I hope
float
GrUtfTextSrc::getFontSize(gr::toffset /*ich*/)
{
	assert(mFont);
	return mPointSize;
}

bool
GrUtfTextSrc::getBold(gr::toffset /*ich*/)
{
	assert(mFont);
//	NS_ASSERTION(false, "unexpected call to getBold");
//	return false;
	return mFont->bold();
}

bool
GrUtfTextSrc::getItalic(gr::toffset /*ich*/)
{
	assert(mFont);
	//NS_ASSERTION(false, "unexpected call to getItalic");
	//return false;
	return mFont->italic();
}

gr::isocode GrUtfTextSrc::getLanguage(gr::toffset /*ich*/)
{
  gr::isocode unknown;
  std::fill_n(unknown.rgch, 4, '\0');
  return unknown;
}

