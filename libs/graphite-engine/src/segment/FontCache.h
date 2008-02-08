/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: FontCache.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    A cache of all the font-face objects known to mankind. There is exactly one instance
	of a FontCache.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef FONTCACHE_INCLUDED
#define FONTCACHE_INCLUDED

//:End Ignore

namespace gr
{

class FontFace;
class FontMemoryUsage;

/*----------------------------------------------------------------------------------------------
	TODO: change from a sorted list to a hash table, if performance so requires.
----------------------------------------------------------------------------------------------*/
class FontCache {
	friend class FontMemoryUsage;

public:
	FontCache()
	{
		m_cfci = 0;
		m_prgfci = NULL;
		m_cfciMax = 0;
		m_cfface = 0;
		m_flush = kflushAuto;
	}

	~FontCache()
	{
		delete[] m_prgfci;
		m_prgfci = NULL;
		m_cfci = 0;
		m_cfciMax = 0;
		m_cfface = 0;
	}

	void Initialize()
	{
		m_cfci = 0;
		m_prgfci = new CacheItem[12];
		m_cfciMax = 12;
		m_cfface = 0;
	}

	struct CacheItem
	{
		wchar_t szFaceName[32];	// type should match std::wstring
		FontFace * pffaceRegular;
		FontFace * pffaceBold;
		FontFace * pffaceItalic;
		FontFace * pffaceBI;
	};

	void GetFontFace(std::wstring strFaceName, bool fBold, bool fItalic, FontFace ** ppfface);
	void CacheFontFace(std::wstring strFaceName, bool fBold, bool fItalic, FontFace * pfface);
	bool RemoveFontFace(std::wstring strFaceName, bool fBold, bool fItalic, bool fZapCache = true);
	void DeleteIfEmpty();
	void AssertEmpty();

	int GetFlushMode()
	{
		return m_flush;
	}
	void SetFlushMode(int);

	// Debugging:
	//bool DbgCheckFontCache();

	void calculateMemoryUsage(FontMemoryUsage & fmu);

protected:
	int FindCacheItem(std::wstring strFaceName);
	void InsertCacheItem(int ifci);

protected:
	// member variables;
	int m_cfci;			// number of items (font-families)
	int m_cfciMax;		// amount of space available
	int m_cfface;		// number of font-faces
	CacheItem * m_prgfci;

	int m_flush;
};

} // namespace gr


#endif // !FONTCACHE_INCLUDED
