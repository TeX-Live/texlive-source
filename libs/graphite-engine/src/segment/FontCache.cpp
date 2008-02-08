/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: FontCache.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	A cache of all the font-face objects.
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

//:>********************************************************************************************
//:>	   Methods
//:>********************************************************************************************

namespace gr
{

/*----------------------------------------------------------------------------------------------
	Store the given font-face in the cache. Overwrite whatever was there before.
	Return NULL if there is nothing appropriate there.
----------------------------------------------------------------------------------------------*/
void FontCache::GetFontFace(std::wstring strFaceName, bool fBold, bool fItalic,
	FontFace ** ppfface)
{
	int ifci = FindCacheItem(strFaceName);
	if (ifci < 0) // no fonts of that family present
	{
		*ppfface = NULL;
		return;
	}

	CacheItem * pfci = m_prgfci + ifci;
	if (fBold)
	{
		if (fItalic)
			*ppfface = pfci->pffaceBI;
		else
			*ppfface = pfci->pffaceBold;
	}
	else
	{
		if (fItalic)
			*ppfface = pfci->pffaceItalic;
		else
			*ppfface = pfci->pffaceRegular;
	}
}

/*----------------------------------------------------------------------------------------------
	Store the given font-face in the cache. Overwrite whatever was there before.
----------------------------------------------------------------------------------------------*/
void FontCache::CacheFontFace(std::wstring strFaceName, bool fBold, bool fItalic,
	FontFace * pfface)
{
	if (m_prgfci == NULL)
		Initialize();

	int ifciIns = FindCacheItem(strFaceName);
	int ifci = ifciIns;
	if (ifciIns < 0)
	{
		ifci = (ifciIns + 1) * -1;
		InsertCacheItem(ifci);
		std::copy(strFaceName.c_str(), strFaceName.c_str() +
				  (strFaceName.size() + 1), m_prgfci[ifci].szFaceName);
	}

	CacheItem * pfci = m_prgfci + ifci;

	bool fPrevNull;
	if (fBold)
	{
		if (fItalic)
		{
			fPrevNull = (pfci->pffaceBI == NULL);
			pfci->pffaceBI = pfface;
		}
		else
		{
			fPrevNull = (pfci->pffaceBold == NULL);
			pfci->pffaceBold = pfface;
		}
	}
	else
	{
		if (fItalic)
		{
			fPrevNull = (pfci->pffaceItalic == NULL);
			pfci->pffaceItalic = pfface;
		}
		else
		{
			fPrevNull = (pfci->pffaceRegular == NULL);
			pfci->pffaceRegular = pfface;
		}
	}

	if (fPrevNull && (pfface != NULL))
		m_cfface++;
}

/*----------------------------------------------------------------------------------------------
	Remove the given font-face from the cache. Return false if it couldn't be found.
----------------------------------------------------------------------------------------------*/
bool FontCache::RemoveFontFace(std::wstring strFaceName, bool fBold, bool fItalic,
	bool fZapCache)
{
	int ifci = FindCacheItem(strFaceName);
	if (ifci < 0)
	{
		return false;
	}

	bool fPrevVal;
	CacheItem * pfci = m_prgfci + ifci;
	if (fBold)
	{
		if (fItalic)
		{
			fPrevVal = (pfci->pffaceBI != NULL);
			pfci->pffaceBI = NULL;
		}
		else
		{
			fPrevVal = (pfci->pffaceBold != NULL);
			pfci->pffaceBold = NULL;
		}
	}
	else
	{
		if (fItalic)
		{
			fPrevVal = (pfci->pffaceItalic != NULL);
			pfci->pffaceItalic = NULL;
		}
		else
		{
			fPrevVal = (pfci->pffaceRegular != NULL);
			pfci->pffaceRegular = NULL;
		}
	}

	if (fPrevVal)
		m_cfface--;

	Assert(m_cfface >= 0);

	if (m_flush == kflushAuto && fZapCache)
	{
		DeleteIfEmpty();
	}

	return fPrevVal;
}

/*----------------------------------------------------------------------------------------------
	Search for the font-family in the cache and return its index.
	If not present, return a negative number indicating where it was expected.
----------------------------------------------------------------------------------------------*/
int FontCache::FindCacheItem(std::wstring strFaceName)
{
	if (m_cfci == 0)
		return -1;

	Assert(m_prgfci);

	// Use a binary-chop search.

	int ifciLow = 0;
	int ifciHigh = m_cfci;
	while (true)
	{
		int ifciMid = (ifciHigh + ifciLow) >> 1; // divide by 2
		CacheItem * pfci = m_prgfci + ifciMid;
		int tst = wcscmp(strFaceName.c_str(), (const wchar_t *)pfci->szFaceName);
		if (tst == 0)
			return ifciMid; // found it
		if (ifciLow + 1 == ifciHigh)
		{
			// not there; return where expected: 0 -> -1, 1 -> -2
			Assert(ifciMid == ifciLow);
			if (tst < 0)
				return (ifciLow * -1) - 1;
			else
				return (ifciHigh * -1) - 1;
		}

		// Keep looking.
		if (tst < 0)
			ifciHigh = ifciMid;
		else
			ifciLow = ifciMid;
	}
}

/*----------------------------------------------------------------------------------------------
	Insert space in the cache at location ifci.
----------------------------------------------------------------------------------------------*/
void FontCache::InsertCacheItem(int ifci)
{
	if (m_cfci == m_cfciMax)
	{
		// Cache is full; double the space.
		CacheItem * m_prgfciOld = m_prgfci;
		m_prgfci = new CacheItem[m_cfciMax * 2];
		std::copy(m_prgfciOld, m_prgfciOld + m_cfciMax, m_prgfci);
		delete[] m_prgfciOld;
		m_cfciMax *= 2;
	}

	// This copy involves overlapping ranges, so we need copy_backward not copy
	// to satisfy the preconditions
	std::copy_backward(m_prgfci + ifci, m_prgfci + m_cfci, m_prgfci + m_cfci + 1);
	m_cfci++;

	// Initialize inserted item.
	CacheItem * pfci = m_prgfci + ifci;
	pfci->pffaceRegular = NULL;
	pfci->pffaceBold = NULL;
	pfci->pffaceItalic = NULL;
	pfci->pffaceBI = NULL;
}

/*----------------------------------------------------------------------------------------------
	Delete the cache if it is empty.
----------------------------------------------------------------------------------------------*/
void FontCache::DeleteIfEmpty()
{
	if (m_cfface <= 0)
		// All items are NULL; delete this cache. Probably this is because
		// the program is exiting.
		FontFace::ZapFontCache();
}

/*----------------------------------------------------------------------------------------------
	Check that the cache is indeed empty.
----------------------------------------------------------------------------------------------*/
void FontCache::AssertEmpty()
{
#ifdef _DEBUG
	for (int ifci = 0; ifci < m_cfci; ifci++)
	{
		CacheItem * pfci = m_prgfci + ifci;
		Assert(pfci->pffaceRegular == NULL);
		Assert(pfci->pffaceBold == NULL);
		Assert(pfci->pffaceItalic == NULL);
		Assert(pfci->pffaceBI == NULL);
	}
#endif // _DEBUG
}

/*----------------------------------------------------------------------------------------------
	Set the flush mode on the cache to indicate whether or not it should be deleted when
	it becomes empty.
----------------------------------------------------------------------------------------------*/
void FontCache::SetFlushMode(int flush)
{
	m_flush = flush;

	if (m_flush == kflushAuto)
	{
		// Delete any font faces that have no remaining corresponding fonts.
		// Work backwards so as we remove items the loop still works.
		for (int ifci = m_cfci; --ifci >= 0; )
		{
			CacheItem * pfci = m_prgfci + ifci;
			if (pfci->pffaceRegular && pfci->pffaceRegular->NoFonts())
				RemoveFontFace(pfci->szFaceName, false, false, false);
			if (pfci->pffaceBold && pfci->pffaceBold->NoFonts())
				RemoveFontFace(pfci->szFaceName, true, false, false);
			if (pfci->pffaceItalic && pfci->pffaceItalic->NoFonts())
				RemoveFontFace(pfci->szFaceName, false, true, false);
			if (pfci->pffaceBI && pfci->pffaceBI->NoFonts())
				RemoveFontFace(pfci->szFaceName, true, true, false);
		}

		if (m_cfface <= 0)
			FontFace::ZapFontCache();
	}
}

} // namespace gr
