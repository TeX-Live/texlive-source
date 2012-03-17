/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: SimpleTextSrc.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	A simple text source that shows how to use this interface within Graphite.
-------------------------------------------------------------------------------*//*:End Ignore*/
#pragma once
#ifndef GRTXTSRC_INCLUDED
#define GRTXTSRC_INCLUDED

using namespace gr;

/*----------------------------------------------------------------------------------------------
	Class: SimpleTextSrc
	This class provides a simple implementation for a text source for the Graphite engine.
	There are no paragraph properties of interest and one set of character properties that
	apply to the entire string.

	This class is a subclass of IColorTextSource so that it can be used by the
	WinSegmentPainter class, which expects the getColors method to be defined.
----------------------------------------------------------------------------------------------*/
class SimpleTextSrc : public IColorTextSource
{
public:
	// Constructor:
	SimpleTextSrc(gr::utf16 * pszText);
	~SimpleTextSrc();

/*
	virtual long IncRefCount(void)
	{
		return InterlockedIncrement(&m_cref);
	}
	virtual long DecRefCount(void)
	{
		long cref = InterlockedDecrement(&m_cref);
		if (cref == 0) {
			m_cref = 1;
			delete this;
		}
		return cref;
	}
*/
	// -------------------------------------------------------------------------------
	// Interface methods:

	virtual UtfType utfEncodingForm()
	{
		return kutf16;
	}
	virtual size_t getLength()
	{
		return m_cchLength;
	}
	virtual size_t fetch(toffset /*ichMin*/, size_t /*cch*/, utf32 * /*prgchBuffer*/)
	{
		throw;
	}
	virtual size_t fetch(toffset ichMin, size_t cch, gr::utf16 * prgchwBuffer);
	virtual size_t fetch(toffset /*ichMin*/, size_t /*cch*/, utf8  * /*prgchsBuffer*/)
	{
		throw;
	};

	virtual bool getRightToLeft(toffset ich);
	virtual unsigned int getDirectionDepth(toffset ich);
	virtual float getVerticalOffset(toffset ich);

	virtual isocode getLanguage(toffset /*ich*/)
	{
		isocode ret;
		ret.rgch[0] = 'e'; ret.rgch[1] = 'n'; ret.rgch[2] = 0; ret.rgch[3] = 0;
		return ret;
	}

	virtual std::pair<toffset, toffset> propertyRange(toffset /*ich*/)
	{
		std::pair<toffset, toffset> pairRet;
		pairRet.first = 0;
		pairRet.second = m_cchLength;
		return pairRet;
	}

	virtual size_t getFontFeatures(toffset /*ich*/, FeatureSetting * /*prgfset*/)
	{
		return 0; // no features in this simple implementation
	}

	virtual bool sameSegment(toffset /*ich1*/, toffset /*ich2*/)
	{
		return true;
	}

	virtual bool featureVariations()
	{
		return false;
	}

	virtual void getColors(toffset /*ich*/, int * pclrFore, int * pclrBack)
	{
		*pclrFore = kclrBlack;
		*pclrBack = kclrTransparent;
	}

protected:
	long m_cref;
	gr::utf16 * m_prgchText;
	int m_cchLength;
};


#endif // !GRTXTSRC_INCLUDED
