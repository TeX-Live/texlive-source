/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001, 2005 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: Font.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	A font is an object that knows how to read tables from a (platform-specific) font resource.
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	   Include files
//:>********************************************************************************************
#include "Main.h"
#ifdef _MSC_VER
#pragma hdrstop
#endif
// any other headers (not precompiled)

#undef THIS_FILE
DEFINE_THIS_FILE

//:End Ignore

//:>********************************************************************************************
//:>	   Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	   Local Constants and static variables
//:>********************************************************************************************


namespace gr
{

//:>********************************************************************************************
//:>	Font methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Constructor.
----------------------------------------------------------------------------------------------*/
Font::Font(const Font & fontSrc) : m_pfface(fontSrc.m_pfface)
{
	if (m_pfface)
		m_pfface->IncFontCount();
}

/*----------------------------------------------------------------------------------------------
	Destructor.
----------------------------------------------------------------------------------------------*/
Font::~Font()
{
	if (m_pfface)
		m_pfface->DecFontCount();
}

/*----------------------------------------------------------------------------------------------
	Mark the font-cache as to whether it should be deleted when all the font instances go away.
----------------------------------------------------------------------------------------------*/
void Font::SetFlushMode(int flush)
{
	FontFace::SetFlushMode(flush);
}

/*----------------------------------------------------------------------------------------------
	Return the flush mode of the font-cache.
----------------------------------------------------------------------------------------------*/
int Font::GetFlushMode()
{
	return FontFace::GetFlushMode();
}

/*----------------------------------------------------------------------------------------------
	Return the internal object representing the font-face. This is the object that is stored
	in Graphite's internal cache.
----------------------------------------------------------------------------------------------*/
inline FontFace & Font::fontFace()
{
	if (m_pfface == 0)
		initialiseFontFace();
	return *m_pfface;
}

void Font::initialiseFontFace()
{
	std::wstring stuFaceName;
	bool         fBold, fItalic;

	UniqueCacheInfo(stuFaceName, fBold, fItalic);
	
	m_pfface = FontFace::GetFontFace(this, stuFaceName, fBold, fItalic);

	Assert(m_pfface != 0);
	
	m_pfface->IncFontCount();
}

/*----------------------------------------------------------------------------------------------
	Return uniquely identifying information that will be used a the key for this font
	in the font cache. This includes the font face name and the bold and italic flags.
----------------------------------------------------------------------------------------------*/
void Font::UniqueCacheInfo(std::wstring & stuFace, bool & fBold, bool & fItalic)
{
	size_t cbSize;
	const byte * pNameTbl = static_cast<const byte *>(getTable(TtfUtil::TableIdTag(ktiName), &cbSize));
	size_t lOffset, lSize;
	if (!TtfUtil::Get31EngFamilyInfo(pNameTbl, lOffset, lSize))
	{
		// TODO: try to find any name in any arbitrary language.
		Assert(false);
		return;
	}
	// byte * pvName = (byte *)pNameTbl + lOffset;
	utf16 rgchwFace[128];
	const size_t cchw = min(long(lSize / sizeof(utf16)), long(sizeof rgchwFace - 1));
	const utf16 *src_start = reinterpret_cast<const utf16 *>(pNameTbl+ lOffset);
	std::copy(src_start, src_start + cchw, rgchwFace);
	rgchwFace[cchw] = 0;  // zero terminate
	TtfUtil::SwapWString(rgchwFace, cchw);
	stuFace.assign(rgchwFace, rgchwFace + cchw);

	const void * pOs2Tbl = getTable(TtfUtil::TableIdTag(ktiOs2), &cbSize);
	TtfUtil::FontOs2Style(pOs2Tbl, fBold, fItalic);
	// Do we need to compare the results from the OS2 table with the italic flag in the
	// head table? (There is no requirement that they be consistent!)
}


/*----------------------------------------------------------------------------------------------
	A default unhinted implmentation of getGlyphPoint(..)
----------------------------------------------------------------------------------------------*/
void Font::getGlyphPoint(gid16 glyphID, unsigned int pointNum, Point & pointReturn)
{
	// Default values 
	pointReturn.x = 0;
	pointReturn.y = 0;

	// this isn't used very often, so don't bother caching
	size_t cbLocaSize = 0;
	const void * pGlyf = getTable(TtfUtil::TableIdTag(ktiGlyf), &cbLocaSize);
	if (pGlyf == 0)	return;

	const void * pHead = getTable(TtfUtil::TableIdTag(ktiHead), &cbLocaSize);
	if (pHead == 0) return;

	const void * pLoca = getTable(TtfUtil::TableIdTag(ktiLoca), &cbLocaSize);
	if (pLoca == 0)	return;

	const size_t MAX_CONTOURS = 32;
	int cnPoints = MAX_CONTOURS;
	bool rgfOnCurve[MAX_CONTOURS];
	int prgnX[MAX_CONTOURS];
	int prgnY[MAX_CONTOURS];
	
	if (TtfUtil::GlyfPoints(glyphID, pGlyf, pLoca, 
		cbLocaSize, pHead, 0, 0, 
		prgnX, prgnY, rgfOnCurve, cnPoints))
	{
		float nPixEmSquare;
		getFontMetrics(0, 0, &nPixEmSquare);

		const float nDesignUnitsPerPixel =  float(TtfUtil::DesignUnits(pHead)) / nPixEmSquare;
		pointReturn.x = prgnX[pointNum] / nDesignUnitsPerPixel;
		pointReturn.y = prgnY[pointNum] / nDesignUnitsPerPixel;
	}
}


/*----------------------------------------------------------------------------------------------
	A default unhinted implmentation of getGlyphMetrics(..)
----------------------------------------------------------------------------------------------*/
void Font::getGlyphMetrics(gid16 glyphID, gr::Rect & boundingBox, gr::Point & advances)
{
	// Setup default return values in case of failiure.
	boundingBox.left = 0;
	boundingBox.right = 0;
	boundingBox.bottom = 0;
	boundingBox.top = 0;
	advances.x = 0;
	advances.y = 0;

	// get the necessary tables.
	size_t locaSize, hmtxSize;
	const void * pHead = getTable(TtfUtil::TableIdTag(ktiHead), &locaSize);
	if (pHead == 0) return;

	const void * pHmtx = getTable(TtfUtil::TableIdTag(ktiHmtx), &hmtxSize);
	if (pHmtx == 0) return;

	// Calculate the number of design units per pixel.
	float pixelEmSquare;
	getFontMetrics(0, 0, &pixelEmSquare);
	const float designUnitsPerPixel = 
		float(TtfUtil::DesignUnits(pHead)) / pixelEmSquare;

	// Use the Hmtx and Head tables to find the glyph advances.
	int lsb;
	unsigned int advance = 0;
	if (TtfUtil::HorMetrics(glyphID, pHmtx, hmtxSize, pHead, 
			lsb, advance))
	{
		advances.x = (advance / designUnitsPerPixel);
		advances.y = 0.0f;		
	}

	const void * pGlyf = getTable(TtfUtil::TableIdTag(ktiGlyf), &locaSize);
	if (pGlyf == 0)	return;

//	const void * pHhea = getTable(TtfUtil::TableIdTag(ktiHhea), &locaSize);
//	if (pHhea == 0)	return;

	const void * pLoca = getTable(TtfUtil::TableIdTag(ktiLoca), &locaSize);
	if (pLoca == 0)	return;

	// Fetch the glyph bounding box, GlyphBox may return false for a 
	// whitespace glyph.
	// Note that using GlyfBox here allows attachment points (ie, points lying outside
	// the glyph's outline) to affect the bounding box, which might not be what we want.
	int xMin, xMax, yMin, yMax;
	if (TtfUtil::GlyfBox(glyphID, pGlyf, pLoca, locaSize, pHead,
			xMin, yMin, xMax, yMax))
	{
		boundingBox.left = (xMin / designUnitsPerPixel);
		boundingBox.bottom = (yMin / designUnitsPerPixel);
		boundingBox.right = (xMax / designUnitsPerPixel);
		boundingBox.top = (yMax / designUnitsPerPixel);
	}
}


/*----------------------------------------------------------------------------------------------
	Return a pair of iterators over a sequence of features defined for the font.
----------------------------------------------------------------------------------------------*/
std::pair<FeatureIterator, FeatureIterator> Font::getFeatures()
{
	std::pair<FeatureIterator, FeatureIterator> pairRet;
	pairRet.first = BeginFeature();
	pairRet.second = EndFeature();
	return pairRet;
}

/*----------------------------------------------------------------------------------------------
	Returns an iterator indicating the feature with the given ID. If no such feature,
	returns the end iterator.
----------------------------------------------------------------------------------------------*/
FeatureIterator Font::featureWithID(featid id)
{
	FeatureIterator fit = BeginFeature();
	FeatureIterator fitEnd = EndFeature();
	for ( ; fit != fitEnd ; ++fit)
	{
		if (*fit == id)
			return fit;
	}
	return fitEnd; // invalid
}

/*----------------------------------------------------------------------------------------------
	Returns the UI label for the indicated feature. Return false if there is no label for the
	language, or it is empty.
----------------------------------------------------------------------------------------------*/
bool Font::getFeatureLabel(FeatureIterator fit, lgid nLanguage, utf16 * label)
{
	return GetFeatureLabel(fit.m_ifeat, nLanguage, label);
}

/*----------------------------------------------------------------------------------------------
	Returns an iterator pointing at the default value for the feature.
----------------------------------------------------------------------------------------------*/
FeatureSettingIterator Font::getDefaultFeatureValue(FeatureIterator fit)
{
	size_t ifset = GetFeatureDefault(fit.m_ifeat);
	FeatureSettingIterator fsit = fit.BeginSetting();
	fsit += ifset;
	return fsit;
}

/*----------------------------------------------------------------------------------------------
	Returns a pair of iterators spanning the defined settings for the feature.
----------------------------------------------------------------------------------------------*/
std::pair<FeatureSettingIterator, FeatureSettingIterator>
	Font::getFeatureSettings(FeatureIterator fit)
{
	std::pair<FeatureSettingIterator, FeatureSettingIterator> pairRet;
	pairRet.first = fit.BeginSetting();
	pairRet.second = fit.EndSetting();
	return pairRet;	
}

/*----------------------------------------------------------------------------------------------
	Returns the UI label for the indicated feature. Return false if there is no label for the
	language, or it is empty.
----------------------------------------------------------------------------------------------*/
bool Font::getFeatureSettingLabel(FeatureSettingIterator fsit, lgid nLanguage, utf16 * label)
{
	//FeatureIterator * pfit = static_cast<FeatureIterator *>(fsit.m_pfit);
	return GetFeatureSettingLabel(fsit.m_fit.m_ifeat, fsit.m_ifset, nLanguage, label);
}

/*----------------------------------------------------------------------------------------------
	Private methods for feature and language access.
----------------------------------------------------------------------------------------------*/
size_t Font::NumberOfFeatures()
{
	return fontFace().NumberOfFeatures();
}
featid Font::FeatureID(size_t ifeat)
{
	return fontFace().FeatureID(ifeat);
}
size_t Font::FeatureWithID(featid id)
{
	return fontFace().FeatureWithID(id);
}
bool Font::GetFeatureLabel(size_t ifeat, lgid language, utf16 * label)
{
	return fontFace().GetFeatureLabel(ifeat, language, label);
}
int Font::GetFeatureDefault(size_t ifeat) // index of default setting
{
	return fontFace().GetFeatureDefault(ifeat);
}
size_t Font::NumberOfSettings(size_t ifeat)
{
	return fontFace().NumberOfSettings(ifeat);
}
int Font::GetFeatureSettingValue(size_t ifeat, size_t ifset)
{
	return fontFace().GetFeatureSettingValue(ifeat, ifset);
}
bool Font::GetFeatureSettingLabel(size_t ifeat, size_t ifset, lgid language, utf16 * label)
{
	return fontFace().GetFeatureSettingLabel(ifeat, ifset, language, label);
}
size_t Font::NumberOfFeatLangs()
{
	return fontFace().NumberOfFeatLangs();
}
short Font::FeatLabelLang(size_t ilang)
{
	return fontFace().FeatLabelLang(ilang);
}
size_t Font::NumberOfLanguages()
{
	return fontFace().NumberOfLanguages();
}
isocode Font::LanguageCode(size_t ilang)
{
	return fontFace().LanguageCode(ilang);
}

/*----------------------------------------------------------------------------------------------
	Return a pair of iterators over a sequence of feature-label languages for this font.
----------------------------------------------------------------------------------------------*/
std::pair<FeatLabelLangIterator, FeatLabelLangIterator> Font::getFeatureLabelLanguages()
{
	std::pair<FeatLabelLangIterator, FeatLabelLangIterator> pairRet;
	pairRet.first = BeginFeatLang();
	pairRet.second = EndFeatLang();
	return pairRet;
}

/*----------------------------------------------------------------------------------------------
	Return a pair of iterators over a sequence of languages defined for the font, that is,
	those that have features specified for them.
----------------------------------------------------------------------------------------------*/
std::pair<LanguageIterator, LanguageIterator> Font::getSupportedLanguages()
{
	std::pair<LanguageIterator, LanguageIterator> pairRet;
	pairRet.first = BeginLanguage();
	pairRet.second = EndLanguage();
	return pairRet;
}

/*----------------------------------------------------------------------------------------------
	Return a bitmap indicating which directions are supported by the font.
----------------------------------------------------------------------------------------------*/
ScriptDirCode Font::getSupportedScriptDirections() const throw()
{
	return m_pfface->ScriptDirection();
}

/*----------------------------------------------------------------------------------------------
	For segment creation.
----------------------------------------------------------------------------------------------*/
void Font::RenderLineFillSegment(Segment * pseg, ITextSource * pts, LayoutEnvironment & layout,
	toffset ichStart, toffset ichStop, float xsMaxWidth, bool fBacktracking)
{
	fontFace().RenderLineFillSegment(pseg, this, pts, layout, ichStart, ichStop, xsMaxWidth,
		fBacktracking);
}

void Font::RenderRangeSegment(Segment * pseg, ITextSource * pts, LayoutEnvironment & layout,
	toffset ichStart, toffset ichStop)
{
	fontFace().RenderRangeSegment(pseg, this, pts, layout, ichStart, ichStop);
}

void Font::RenderJustifiedSegment(Segment * pseg, ITextSource * pts,
	LayoutEnvironment & layout, toffset ichStart, toffset ichStop,
	float xsCurrentWidth, float xsDesiredWidth)
{
	fontFace().RenderJustifiedSegment(pseg, this, pts, layout, ichStart, ichStop,
		xsCurrentWidth, xsDesiredWidth);
}

/*----------------------------------------------------------------------------------------------
	Feature iterators.
----------------------------------------------------------------------------------------------*/
FeatureIterator Font::BeginFeature()
{
	FeatureIterator fit(this, 0, NumberOfFeatures());
	return fit;
}

FeatureIterator Font::EndFeature()
{
	int cfeat = NumberOfFeatures();
	FeatureIterator fit (this, cfeat, cfeat);
	return fit;
}

/*----------------------------------------------------------------------------------------------
	Feature-label language iterators.
----------------------------------------------------------------------------------------------*/
FeatLabelLangIterator Font::BeginFeatLang()
{
	FeatLabelLangIterator lgit(this, 0, NumberOfFeatLangs());
	return lgit;
}

FeatLabelLangIterator Font::EndFeatLang()
{
	int clang = NumberOfFeatLangs();
	FeatLabelLangIterator lgit(this, clang, clang);
	return lgit;
}


/*----------------------------------------------------------------------------------------------
	Language iterators.
----------------------------------------------------------------------------------------------*/
LanguageIterator Font::BeginLanguage()
{
	LanguageIterator lgit(this, 0, NumberOfLanguages());
	return lgit;
}

LanguageIterator Font::EndLanguage()
{
	int clang = NumberOfLanguages();
	LanguageIterator lgit(this, clang, clang);
	return lgit;
}


/*----------------------------------------------------------------------------------------------
	Debugging.
----------------------------------------------------------------------------------------------*/
//bool Font::DbgCheckFontCache()
//{
//	return FontFace::DbgCheckFontCache();
//}


//:>********************************************************************************************
//:>	FeatureIterator methods
//:>********************************************************************************************

//FeatureIterator::FeatureIterator(FeatureIterator & fitToCopy)
//{
//	m_pfont = fitToCopy.m_pfont;
//	m_ifeat = fitToCopy.m_ifeat;
//	m_cfeat = fitToCopy.m_cfeat;
//}

/*----------------------------------------------------------------------------------------------
	Dereference the iterator, returning a feature ID.
----------------------------------------------------------------------------------------------*/
featid FeatureIterator::operator*()
{
	if (m_ifeat >= m_cfeat)
	{
		Assert(false);
		return (unsigned int)kInvalid;
	}

	return m_pfont->FeatureID(m_ifeat);
}

/*----------------------------------------------------------------------------------------------
	Increment the iterator.
----------------------------------------------------------------------------------------------*/
FeatureIterator FeatureIterator::operator++()
{
	if (m_ifeat >= m_cfeat)
	{
		// Can't increment.
		Assert(false);
	}
	else
		m_ifeat++;

	return *this;
}

/*----------------------------------------------------------------------------------------------
	Increment the iterator by the given value.
----------------------------------------------------------------------------------------------*/
FeatureIterator FeatureIterator::operator+=(int n)
{
	if (m_ifeat + n >= m_cfeat)
	{
		// Can't increment.
		Assert(false);
		m_ifeat = m_cfeat;
	}
	else if (m_ifeat + n < 0)
	{
		// Can't decrement.
		Assert(false);
		m_ifeat = 0;
	}
	else
		m_ifeat += m_cfeat;

	return *this;
}

/*----------------------------------------------------------------------------------------------
	Test whether the two iterators are equal.
----------------------------------------------------------------------------------------------*/
bool FeatureIterator::operator==(FeatureIterator & fit)
{
	return (fit.m_ifeat == m_ifeat && fit.m_pfont == m_pfont);
}

bool FeatureIterator::operator!=(FeatureIterator & fit)
{
	return (fit.m_ifeat != m_ifeat || fit.m_pfont != m_pfont);
}

/*----------------------------------------------------------------------------------------------
	Return the number of items represented by the range of the two iterators.
----------------------------------------------------------------------------------------------*/
int FeatureIterator::operator-(FeatureIterator & fit)
{
	if (m_pfont != fit.m_pfont)
	{
		throw;
	}
	return (m_ifeat - fit.m_ifeat);
}


/*----------------------------------------------------------------------------------------------
	Iterators.
----------------------------------------------------------------------------------------------*/
FeatureSettingIterator FeatureIterator::BeginSetting()
{
	int cfset = m_pfont->NumberOfSettings(m_ifeat);
	FeatureSettingIterator fsit(*this, 0, cfset);
	return fsit;
}

FeatureSettingIterator FeatureIterator::EndSetting()
{
	int cfset = m_pfont->NumberOfSettings(m_ifeat);
	FeatureSettingIterator fsit(*this, cfset, cfset);
	return fsit;
}


//:>********************************************************************************************
//:>	   FeatureSettingIterator methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Dereference the iterator, returning a feature value.
----------------------------------------------------------------------------------------------*/
int FeatureSettingIterator::operator*()
{
	if (m_ifset >= m_cfset)
	{
		Assert(false);
		return kInvalid;
	}

	return m_fit.m_pfont->GetFeatureSettingValue(m_fit.m_ifeat, m_ifset);
}

/*----------------------------------------------------------------------------------------------
	Increment the iterator.
----------------------------------------------------------------------------------------------*/
FeatureSettingIterator FeatureSettingIterator::operator++()
{
	if (m_ifset >= m_cfset)
	{
		// Can't increment.
		Assert(false);
	}
	else
		m_ifset++;

	return *this;
}

/*----------------------------------------------------------------------------------------------
	Increment the iterator by the given value.
----------------------------------------------------------------------------------------------*/
FeatureSettingIterator FeatureSettingIterator::operator+=(int n)
{
	if (m_ifset + n >= m_cfset)
	{
		// Can't increment.
		Assert(false);
		m_ifset = m_cfset;
	}
	if (m_ifset + n < 0)
	{
		// Can't decrement.
		Assert(false);
		m_ifset = 0;
	}
	else
		m_ifset += n;

	return *this;
}

/*----------------------------------------------------------------------------------------------
	Test whether the two iterators are equal.
----------------------------------------------------------------------------------------------*/
bool FeatureSettingIterator::operator==(FeatureSettingIterator & fsit)
{
	//FeatureIterator * pfit = static_cast<FeatureIterator *>(m_pfit);
	//FeatureIterator * pfitOther = static_cast<FeatureIterator *>(fsit.m_pfit);
	//return (m_ifset == fsit.m_ifset && pfit == pfitOther);
	return (m_ifset == fsit.m_ifset && m_fit == fsit.m_fit);
}

bool FeatureSettingIterator::operator!=(FeatureSettingIterator & fsit)
{
	//FeatureIterator * pfit = static_cast<FeatureIterator *>(m_pfit);
	//FeatureIterator * pfitOther = static_cast<FeatureIterator *>(fsit.m_pfit);
	//return (m_ifset != fsit.m_ifset || pfit != pfitOther);

	return (m_ifset != fsit.m_ifset || m_fit != fsit.m_fit);
}

/*----------------------------------------------------------------------------------------------
	Return the number of items represented by the range of the two iterators.
----------------------------------------------------------------------------------------------*/
int FeatureSettingIterator::operator-(FeatureSettingIterator fsit)
{
	//FeatureIterator * pfit = static_cast<FeatureIterator *>(m_pfit);
	//FeatureIterator * pfitOther = static_cast<FeatureIterator *>(fsit.m_pfit);
	//if (pfit != pfitOther)

	if (m_fit != fsit.m_fit)
	{
		throw;
	}
	return (m_ifset - fsit.m_ifset);
}

//:>********************************************************************************************
//:>	FeatLabelLangIterator methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Dereference the iterator, returning a language LCID.
----------------------------------------------------------------------------------------------*/
data16 FeatLabelLangIterator::operator*()
{
	if (m_ilang >= m_clang)
	{
		Assert(false);
		return 0;
	}

	return m_pfont->FeatLabelLang(m_ilang);
}

/*----------------------------------------------------------------------------------------------
	Increment the iterator.
----------------------------------------------------------------------------------------------*/
FeatLabelLangIterator FeatLabelLangIterator::operator++()
{
	if (m_ilang >= m_clang)
	{
		// Can't increment.
		Assert(false);
	}
	else
		m_ilang++;

	return *this;
}

/*----------------------------------------------------------------------------------------------
	Increment the iterator by the given value.
----------------------------------------------------------------------------------------------*/
FeatLabelLangIterator FeatLabelLangIterator::operator+=(int n)
{
	if (m_ilang + n >= m_clang)
	{
		// Can't increment.
		Assert(false);
		m_ilang = m_clang;
	}
	else if (m_ilang + n < 0)
	{
		// Can't decrement.
		Assert(false);
		m_ilang = 0;
	}
	else
		m_ilang += m_clang;

	return *this;
}

/*----------------------------------------------------------------------------------------------
	Test whether the two iterators are equal.
----------------------------------------------------------------------------------------------*/
bool FeatLabelLangIterator::operator==(FeatLabelLangIterator & fllit)
{
	return (fllit.m_ilang == m_ilang && fllit.m_pfont == m_pfont);
}

bool FeatLabelLangIterator::operator!=(FeatLabelLangIterator & fllit)
{
	return (fllit.m_ilang != m_ilang || fllit.m_pfont != m_pfont);
}

/*----------------------------------------------------------------------------------------------
	Return the number of items represented by the range of the two iterators.
----------------------------------------------------------------------------------------------*/
int FeatLabelLangIterator::operator-(FeatLabelLangIterator & fllit)
{
	if (m_pfont != fllit.m_pfont)
	{
		throw;
	}
	return (m_ilang - fllit.m_ilang);
}


//:>********************************************************************************************
//:>	LanguageIterator methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Dereference the iterator, returning a language code.
----------------------------------------------------------------------------------------------*/
isocode LanguageIterator::operator*()
{
	if (m_ilang >= m_clang)
	{
		Assert(false);
		isocode codeRet;
		codeRet.rgch[1] = '?'; codeRet.rgch[2] = '?';
		codeRet.rgch[3] = '?'; codeRet.rgch[4] = 0;
		return codeRet;
	}

	return m_pfont->LanguageCode(m_ilang);
}

/*----------------------------------------------------------------------------------------------
	Increment the iterator.
----------------------------------------------------------------------------------------------*/
LanguageIterator LanguageIterator::operator++()
{
	if (m_ilang >= m_clang)
	{
		// Can't increment.
		Assert(false);
	}
	else
		m_ilang++;

	return *this;
}

/*----------------------------------------------------------------------------------------------
	Increment the iterator by the given value.
----------------------------------------------------------------------------------------------*/
LanguageIterator LanguageIterator::operator+=(int n)
{
	if (m_ilang + n >= m_clang)
	{
		// Can't increment.
		Assert(false);
		m_ilang = m_clang;
	}
	else if (m_ilang + n < 0)
	{
		// Can't decrement.
		Assert(false);
		m_ilang = 0;
	}
	else
		m_ilang += m_clang;

	return *this;
}

/*----------------------------------------------------------------------------------------------
	Test whether the two iterators are equal.
----------------------------------------------------------------------------------------------*/
bool LanguageIterator::operator==(LanguageIterator & lgit)
{
	return (lgit.m_ilang == m_ilang && lgit.m_pfont == m_pfont);
}

bool LanguageIterator::operator!=(LanguageIterator & lgit)
{
	return (lgit.m_ilang != m_ilang || lgit.m_pfont != m_pfont);
}

/*----------------------------------------------------------------------------------------------
	Return the number of items represented by the range of the two iterators.
----------------------------------------------------------------------------------------------*/
int LanguageIterator::operator-(LanguageIterator & lgit)
{
	if (m_pfont != lgit.m_pfont)
	{
		throw;
	}
	return (m_ilang - lgit.m_ilang);
}


} // namespace gr


