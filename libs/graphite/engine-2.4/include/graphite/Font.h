/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: Font.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    A Font is an object that represents a font-family + bold + italic setting, that contains
	Graphite tables. This file also includes related iterators: FeatureIterator,
	FeatureSettingIterator, FeatLabelLangIterator, LanguageIterator.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef FONT_INCLUDED
#define FONT_INCLUDED

//:End Ignore

namespace gr
{

class Font;
class FontFace;
class FeatureSettingIterator;
class Segment;
class LayoutEnvironment;
class ITextSource;
class FontMemoryUsage;

typedef struct tagFontProps
{
	unsigned long clrFore;
	unsigned long clrBack;
	bool fBold;
	bool fItalic;
	float pixHeight;
	wchar_t szFaceName[ 32 ];
} FontProps;


/*----------------------------------------------------------------------------------------------
	A FontError object is an exception that is thrown when there is an error in initializing
	a Graphite font.
----------------------------------------------------------------------------------------------*/
struct FontException
{
	FontErrorCode errorCode;
	int version;
	int subVersion;
};

/*----------------------------------------------------------------------------------------------
	Iterator to provide access to a font's features.
----------------------------------------------------------------------------------------------*/
class FeatureIterator
{
	friend class Font;
	friend class FeatureSettingIterator;

public:
	FeatureIterator() // needed for creating std::pair of these, and for default FSI
	{
		m_pfont = NULL;
		m_ifeat = 0;
		m_cfeat = 0;
	}

protected:
	FeatureIterator(Font * pfont, int ifeat, size_t cfeat)
	{
		m_pfont = pfont;
		m_ifeat = ifeat;
		m_cfeat = cfeat;
	}
public:
	featid operator*();
	FeatureIterator operator++();
	FeatureIterator operator+=(int n);
	bool operator==(FeatureIterator &);
	bool operator!=(FeatureIterator &);
	int operator-(FeatureIterator &);

protected:
	Font * m_pfont;
	size_t m_ifeat;	// feature being pointed at
	size_t m_cfeat;	// number of features for this font

	FeatureSettingIterator BeginSetting();
	FeatureSettingIterator EndSetting();
};

/*----------------------------------------------------------------------------------------------
	Iterator to provide access to the defined values for a single font feature.
----------------------------------------------------------------------------------------------*/
class FeatureSettingIterator
{
	friend class Font;
	friend class FeatureIterator;

public:
	FeatureSettingIterator() // needed for creating std::pair of these, I think
	{
		m_ifset = 0;
		m_cfset = 0;
	}
protected:
	FeatureSettingIterator(FeatureIterator fit, int ifset, size_t cfset)
	{
		m_fit = fit;
		m_ifset = ifset;
		m_cfset = cfset;
	}
public:
	int operator*();
	FeatureSettingIterator operator++();
	FeatureSettingIterator operator +=(int n);
	bool operator==(FeatureSettingIterator &);
	bool operator!=(FeatureSettingIterator &);
	int operator-(FeatureSettingIterator);

protected:
	FeatureIterator m_fit;
	size_t m_ifset;	// which setting being pointed at
	size_t m_cfset;	// number of settings for this feature
};

/*----------------------------------------------------------------------------------------------
	Iterator to provide access to the languages available for the feature label strings.
----------------------------------------------------------------------------------------------*/
class FeatLabelLangIterator
{
	friend class Font;

public:
	FeatLabelLangIterator() // needed for creating std::pair of these, and for default FSI
	{
		m_pfont = NULL;
		m_ilang = 0;
		m_clang = 0;
	}

protected:
	FeatLabelLangIterator(Font * pfont, int ilang, size_t clang)
	{
		m_pfont = pfont;
		m_ilang = ilang;
		m_clang = clang;
	}
public:
	data16 operator*();
	FeatLabelLangIterator operator++();
	FeatLabelLangIterator operator+=(int n);
	bool operator==(FeatLabelLangIterator &);
	bool operator!=(FeatLabelLangIterator &);
	int operator-(FeatLabelLangIterator &);

protected:
	Font * m_pfont;
	size_t m_ilang;	// language being pointed at
	size_t m_clang;	// number of languages for this font
};

/*----------------------------------------------------------------------------------------------
	Iterator to provide access to a font's defined languages--ie, those that have feature
	settings associated with them.
----------------------------------------------------------------------------------------------*/
class LanguageIterator
{
	friend class Font;

public:
	LanguageIterator() // needed for creating std::pair of these, and for default FSI
	{
		m_pfont = NULL;
		m_ilang = 0;
		m_clang = 0;
	}

protected:
	LanguageIterator(Font * pfont, int ilang, size_t clang)
	{
		m_pfont = pfont;
		m_ilang = ilang;
		m_clang = clang;
	}
public:
	isocode operator*();	// returns a 4-char array
	LanguageIterator operator++();
	LanguageIterator operator+=(int n);
	bool operator==(LanguageIterator &);
	bool operator!=(LanguageIterator &);
	int operator-(LanguageIterator &);

protected:
	Font * m_pfont;
	size_t m_ilang;	// language being pointed at
	size_t m_clang;	// number of languages for this font
};

/*----------------------------------------------------------------------------------------------
	Abstract superclass for Graphite fonts. A font represents a face name, size, and bold and
	italic styles.
----------------------------------------------------------------------------------------------*/
class Font {

	friend class FeatureIterator;
	friend class FeatureSettingIterator;
	friend class FeatLabelLangIterator;
	friend class LanguageIterator;
	friend class FontMemoryUsage;

public:
	virtual ~Font();

	/**
	* Returns a copy of the recipient. Specifically needed to store the 
	* Font in a segment. 
	* @internal
	* @return pointer to copy
	*/
	virtual Font * copyThis() = 0;

	/**
	* Return wether the font is bold.
	* @return true when bold
	*/
	virtual bool bold() = 0;
	/**
	* Return wether the font is italic.
	* @return true when italic
	*/
	virtual bool italic() = 0;

	/** 
	* Returns the slope of the italic (if the font is italic)
	* @return ratio of slope from the vertical
	*/
	virtual float fakeItalicRatio()	{ return 0; } // no support for fake italic

	/**
	* Returns the font ascent.
	* Value is the same as that returned by getFontMetrics()
	* @return the font ascent in device co-ordinates
	*/
	virtual float ascent() = 0;

	/**
	* Returns the font descent.
	* Value is the same as that returned by getFontMetrics()
	* @return the font descent in device co-ordinates
	*/
	virtual float descent() = 0;

	/**
	* Returns the total height of the font. 
	* @return font height in device co-ordinates
	*/
	virtual float height() = 0;

	/** 
	* Returns the x and y resolution of the device co-ordinate space.
	*/
	virtual unsigned int getDPIx() = 0;
	virtual unsigned int getDPIy() = 0;

	/**
	* Returns a pointer to the start of a table in the font.
	* If the Font class cannot easily determine the length of the table, 
	* it may set 0 as the length (while returning a non-NULL pointer to 
	* the table). This means that certain kinds of error checking cannot 
	* be done by the Graphite engine.
	* Throws an exception if there is some other error in reading the 
	* table, or if the table asked for is not in the font.
	* @param tableID the TTF ID of the table as a 32-bit long in native machine byte order
	* @param pcbSize pointer to a size_t to hold the table size.
	* @return address of the buffer containing the table or 0
	*/
	virtual const void * getTable(fontTableId32 tableID, size_t * pcbSize) = 0;
	/**
	* Fetches the basic metrics of the font in device co-ordinates
	* (normaly pixels).
	* @param pAscent pointer to hold font ascent.
	* @param pDescent pointer to hold font descent.
	* @param pEmSquare pointer to hold font EM square.
	*/
	virtual void getFontMetrics(float * pAscent, float * pDescent = NULL,
		float * pEmSquare = NULL) = 0;
	/**
	* Converts the point number of a glyph’s on-curve point to a pair of 
	* x/y coordinates in pixels. The default implementation will read the 
	* curve information directly from the font and perform a simple 
	* transformation to pixels. Some subclasses (e.g., WinFont) will use 
	* a system-level API call to return hinted metrics.
	* Note that the coordinates returned are floating point values in the 
	* device co-ordinate space (normaly pixels).
	* @param gid glyph id
	* @param pointNum within glyph
	* @param xyReturn reference to a Point object to hold the x,y result
	*/
	virtual void getGlyphPoint(gid16 glyphID, unsigned int pointNum, gr::Point & pointReturn);
					
	/**
	* Returns the metrics of a glyph in the font. The default 
	* implementation will read the information directly from the font and 
	* perform a simple transformation to pixels. Some subclasses (e.g., 
	* WinFont) will use a system-level API call to return hinted metrics.
	* Note that the coordinates returned are floating point values in the 
	* device co-ordinate space (normaly pixels).
	* @param glyphID
	* @param boundingBox reference to gr::Rect to hold bounding box of glyph
	* @param advances refererence to gr::Point to hold the horizontal / vertical advances
	*/
	virtual void getGlyphMetrics(gid16 glyphID, gr::Rect & boundingBox, gr::Point & advances);

	static void SetFlushMode(int);
	static int GetFlushMode();

	// obsolete:
	//virtual FontErrorCode isValidForGraphite(int * pnVersion = NULL, int * pnSubVersion = NULL) = 0;

	// Features:
	std::pair<FeatureIterator, FeatureIterator> getFeatures();
	FeatureIterator featureWithID(featid id);
	bool getFeatureLabel(FeatureIterator, lgid language, utf16 * label);
	FeatureSettingIterator getDefaultFeatureValue(FeatureIterator);
	std::pair<FeatureSettingIterator, FeatureSettingIterator> getFeatureSettings(FeatureIterator);
	bool getFeatureSettingLabel(FeatureSettingIterator, lgid language, utf16 * label);

	std::pair<FeatLabelLangIterator, FeatLabelLangIterator> getFeatureLabelLanguages();

	// Languages:
	std::pair<LanguageIterator, LanguageIterator> getSupportedLanguages();

	// Script directions, Verticle, LTR, RTL etc see enum ScriptDirCode
	// the posibilities.  This returns a bit set where more than one scripts may 
	// be set, it's up to the app to decide which is the prefered direction to use.
	ScriptDirCode getSupportedScriptDirections() const throw();
	
	// Debugging:
	//static bool DbgCheckFontCache();

	static FontMemoryUsage calculateMemoryUsage();
	///FontMemoryUsage calculateMemoryUsage(bool fBold = false, bool fItalic = false);

public:
	// For use in segment creation:
	void RenderLineFillSegment(Segment * pseg, ITextSource * pts, LayoutEnvironment & layout,
		toffset ichStart, toffset ichStop, float xsMaxWidth, bool fBacktracking);
	void RenderRangeSegment(Segment * pseg, ITextSource * pts, LayoutEnvironment & layout,
		toffset ichStart, toffset ichEnd);
	void RenderJustifiedSegment(Segment * pseg, ITextSource * pts, LayoutEnvironment & layout,
		toffset ichStart, toffset ichEnd, float xsCurrentWidth, float xsDesiredWidth);

protected:
	Font();
	Font(const Font &);
	
	FontFace & fontFace(bool fDumbFallback = false);

	// Feature access:
	FeatureIterator BeginFeature();
	FeatureIterator EndFeature();

	size_t NumberOfFeatures();
	featid FeatureID(size_t ifeat);
	size_t FeatureWithID(featid id);
	bool GetFeatureLabel(size_t ifeat, lgid language, utf16 * label);
	int GetFeatureDefault(size_t ifeat);
	size_t NumberOfSettings(size_t ifeat);
	int GetFeatureSettingValue(size_t ifeat, size_t ifset);
	bool GetFeatureSettingLabel(size_t ifeat, size_t ifset, lgid language, utf16 * label);

	// Feature-label languages:
	FeatLabelLangIterator BeginFeatLang();
	FeatLabelLangIterator EndFeatLang();

	size_t NumberOfFeatLangs();
	short FeatLabelLang(size_t ilang);

	// Language access:
	LanguageIterator BeginLanguage();
	LanguageIterator EndLanguage();

	size_t NumberOfLanguages();
	isocode LanguageCode(size_t ilang);

	virtual void UniqueCacheInfo(std::wstring & stuFace, bool & fBold, bool & fItalic);

private:
	FontFace * m_pfface;	// set up with Graphite tables

	// Cache of common tables:
	const void * m_pHead;
	const void * m_pHhea;
	const void * m_pHmtx;
	const void * m_pLoca;
	const void * m_pGlyf;
	size_t m_cbHmtxSize;
	size_t m_cbLocaSize;
	bool m_fTablesCached;

	void initialiseFontFace(bool fDumbFallback);
	void EnsureTablesCached();
};

inline Font::Font() : m_pfface(0), m_fTablesCached(false)
{ }

} // namespace gr


#endif // !FONT_INCLUDED
