/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrEngine.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description: Contains the implementation of the FontFace class.
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	   Include files
//:>********************************************************************************************
#include "Main.h"
#include <functional>
#ifdef _MSC_VER
#pragma hdrstop
#endif
// any other headers (not precompiled)

#undef THIS_FILE
DEFINE_THIS_FILE

//:End Ignore

namespace gr
{

//:>********************************************************************************************
//:>	   Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	   Local Constants and static variables
//:>********************************************************************************************

FontCache * FontFace::s_pFontCache = 0;

//:>********************************************************************************************
//:>	New interface
//:>********************************************************************************************
/*----------------------------------------------------------------------------------------------
	Return the appropriate FontFace, initialized with Graphite tables.
	Called from the font constructor.
----------------------------------------------------------------------------------------------*/
FontFace * FontFace::GetFontFace(Font * pfont,
	std::wstring strFaceName, bool fBold, bool fItalic,
	bool fDumbFallback)
{
	if (s_pFontCache == NULL)
		s_pFontCache = new FontCache;

	FontFace * pfface;
	s_pFontCache->GetFontFace(strFaceName, fBold, fItalic, &pfface);
	if (pfface)
		return pfface;

	// Create a new font face.
	pfface = new FontFace();
	pfface->InitFontFace(pfont, strFaceName, fBold, fItalic, fDumbFallback);
	return pfface;
}

/*----------------------------------------------------------------------------------------------
	Initialize the engine using the given Graphite font.
----------------------------------------------------------------------------------------------*/
GrResult FontFace::InitFontFace(Font * pfont,
	std::wstring stuFaceName, bool fBold, bool fItalic,
	bool fDumbFallback)
{
	AssertPtrN(pfont);

	m_pgreng = new GrEngine;
	m_pgreng->m_pfface = this;

	m_pgreng->m_nScriptTag = 0; // not currently used, but set it to something

	std::wstring stuFeatures;

	//stuFeatures = stuFeaturesArg;

	if (wcscmp(stuFaceName.c_str(), m_pgreng->m_stuFaceName.c_str()) != 0)
	{
		s_pFontCache->RemoveFontFace(m_pgreng->FaceName(), m_pgreng->Bold(), m_pgreng->Italic());

		m_pgreng->DestroyEverything(); // nothing we've cached is safe or useful.
		m_pgreng->m_stuFaceName = stuFaceName; // if this is buggy, using assign might fix it
	}

	//GrResult res = kresFail;
//#ifdef GR_FW
//	// Read from the control file indicated by the registry, if any.
//	std::wstring stuFaceNameTmp;
//	stuFaceNameTmp.assign(rgchFaceName, wcslen(rgchFaceName));
//	res = InitFromControlFile(pfont, stuFaceName.c_str(), fBold, fItalic);
//#endif // GR_FW

	// Read directly from the font.
	m_pgreng->DestroyContents();
	GrResult res = kresUnexpected;
	try {
		res = m_pgreng->ReadFontTables(pfont, fItalic);
	}
	catch (...)
	{
		if (fDumbFallback && m_pgreng->DumbFallback() && !m_pgreng->BadFont())
		{
			// If we have a basically good font but can't do Graphite,
			// just go with empty tables, which should be set up.
			res = m_pgreng->m_resFontRead;
		}
		else
		{
			if (m_cfonts == 0)
			{
				//s_pFontCache->DeleteIfEmpty();
				//Assert(s_pFontCache == 0);
				delete this;	// will also delete GrEngine
			}
			else
			{
				delete m_pgreng;
			}
			m_pgreng = NULL;
			throw; // throw original exception
		}
	}

	m_pgreng->m_resFontValid = res;

	m_pgreng->m_fBold = fBold;
	m_pgreng->m_fItalic = fItalic;

	s_pFontCache->CacheFontFace(m_pgreng->FaceName(), fBold, fItalic, this);

	return m_pgreng->m_resFontValid;
}

/*----------------------------------------------------------------------------------------------
	Read the cmap and Graphite tables from the font that is selected into the graphics
	device.

	@return A GrResult indicating whether we were successful in loading the Graphite font:
	- kresOk means success
	- kresFail means the Graphite font could not be found or the basic font tables
		(head, name, cmap) could not be loaded
	- kresUnexpected means the Graphite tables could not be loaded
	- kresFalse means it is	not a Graphite font at all (has no Silf table).
----------------------------------------------------------------------------------------------*/
GrResult GrEngine::ReadFontTables(Font * pfont, bool fItalic)
{
	GrResult res = kresOk;
	m_ferr = kferrOkay;
	GrBufferIStream grstrm;

	FontException fexptn;
	fexptn.version = -1;
	fexptn.subVersion = -1;

	bool fOk = false;

	const void * pHeadTbl; const void * pSileTbl; const void * pSilfTbl;
 	const void * pFeatTbl; const void * pGlatTbl; const void * pGlocTbl; const void * pSillTbl;
	//const void * pCmapTbl; const void * pNameTbl;
	size_t cbHeadSz, cbSilfSz, cbFeatSz, cbGlatSz, cbGlocSz, cbSillSz;
	//size_t cbCmapSz, cbSileSz, cbNameSz;

	m_fFakeItalic = false;

	bool fBasicTables = false;
	bool fSilf = false; // does the font have some sort of Silf table?
	int nCheckSum = 0;

	bool fBadBase = false, 
		fMismatchedBase = false,
		fFontIsItalic = false;

	// First read the head table. This gives us the checksum that we are using as a
	// unique identifer. If it is the same as the one stored, and things appear set up,
	// don't reload the tables.
	res = (pHeadTbl = pfont->getTable(TtfUtil::TableIdTag(ktiHead), &cbHeadSz)) ? kresOk : kresFail;
	fOk = pHeadTbl && (cbHeadSz == 0 || TtfUtil::CheckTable(ktiHead, pHeadTbl, cbHeadSz));
	if (res == kresFail)
	{
		m_stuInitError = L"could not locate head table for Graphite rendering";
		m_ferr = kferrFindHeadTable;
		goto LUnexpected;
	}
	if (!fOk)
	{
		m_stuInitError = L"could not read design units for Graphite rendering";
		m_ferr = kferrReadDesignUnits;
		goto LUnexpected;
	}
	m_mFontEmUnits = TtfUtil::DesignUnits(pHeadTbl);
	nCheckSum = TtfUtil::HeadTableCheckSum(pHeadTbl);
	fFontIsItalic = TtfUtil::IsItalic(pHeadTbl);

	if (m_nFontCheckSum == nCheckSum && m_ptman)
		return m_resFontRead;	// already initialized in some form

	DestroyContents();

	Assert(!m_ptman);
	Assert(!m_pctbl);
	Assert(!m_pgtbl);
	Assert(!m_prgpsd);
	m_prgpsd = NULL;
	m_cpsd = 0;

	m_ptman = new GrTableManager(this);

	m_fFakeItalic = (fItalic && !fFontIsItalic);

	// Look for an Sile table. If there is one, this is an extension font containing only
	// the Graphite tables. Read the base font name out of the Sile table, and use it to
	// read the cmap. Then replace the original font name to read the Graphite tables.

	m_fUseSepBase = false;
	m_stuBaseFaceName.erase();

	// TODO: rework the handling of the Sile table.
	pSileTbl = NULL;
	//pSileTbl = pfont->getTable(TtfUtil::TableIdTag(ktiSile));
	//if (pSileTbl)
	//{
	//	pgg->get_FontCharProperties(&chrpOriginal);
	//	grstrm.OpenBuffer(pSileTbl, sile_tbl.size());
	//	m_fUseSepBase = ReadSileTable(pgg, grstrm, 0, &m_mFontEmUnits, &fMismatchedBase);
	//	grstrm.Close();

	//	if (!m_fUseSepBase)
	//	{
	//		SwitchGraphicsFont(pgg, false); // back to Graphite table font
	//		m_stuBaseFaceName.erase();
	//		fBadBase = true;
	//		m_fUseSepBase = false;
	//	}
	//	// Otherwise leave the GrGraphics in a state to read from the base font.
	//}

	// We don't need the offset table, and there's no way to get it anyway
	// without a font file.

	// cmap and name
	Assert(!m_fCmapTblCopy);
	Assert(!m_fNameTblCopy);
	fOk = SetCmapAndNameTables(pfont);
	if (!fOk)
	{
		goto LUnexpected;
	}

	fBasicTables = true;

	// If we have a bad base file, don't do Graphite stuff.
	if (fBadBase || fMismatchedBase)
		goto LUnexpected;

	/****
	Obtain font name from InitNew() now instead of reading from font file. InitNew should
	should have a correct font name passed to it since it should come from a font registered
	by GrFontInst.exe. This commented code could be use to verify name in font file matches.
	NOTE: if we ever use this code again, make sure we're using the base font name table,
	not the Graphite wrapper font name table.
	// find the font family name
	if (!TtfUtil::Get31EngFamilyInfo(vbName.Begin(), lnNameOff, lnNameSz))
	{	// use Name table which is Symbol encode instead
		// this could cause problems if a real Symbol writing system is used in the name table
		// however normally real Unicode values are used instead a Symbol writing system
		if (!TtfUtil::Get30EngFamilyInfo(vbName.Begin(), lnNameOff, lnNameSz))
		{
			ReturnResult(kresFail);
		}
		// test for Symbol writing system. first byte of Unicode id should be 0xF0
		if (vbName[lnNameOff + 1] == (unsigned char)0xF0) // 1 - Unicode id is big endian
			ReturnResult(kresFail);
	}
	if (!TtfUtil::SwapWString(vbName.Begin() + lnNameOff, lnNameSz / isizeof(utf16)))
		ReturnResult(kresFail);

	m_stuFaceName = std::wstring((utf16 *)(vbName.begin() + lnNameOff), lnNameSz / isizeof(utf16));
	****/

	// Silf
	res = (pSilfTbl = pfont->getTable(TtfUtil::TableIdTag(ktiSilf), &cbSilfSz)) ? kresOk : kresFail;
	fOk = pSilfTbl && (cbSilfSz == 0 || TtfUtil::CheckTable(ktiSilf, pSilfTbl, cbSilfSz));
	if (!fOk)
	{
		m_stuInitError = L"could not load Silf table for Graphite rendering";
		m_ferr = kferrLoadSilfTable;
		goto LUnexpected;
	}

	// Feat
	res = (pFeatTbl = pfont->getTable(TtfUtil::TableIdTag(ktiFeat), &cbFeatSz)) ? kresOk : kresFail;
	fOk = pFeatTbl && (cbFeatSz == 0 || TtfUtil::CheckTable(ktiFeat, pFeatTbl, cbFeatSz));
	if (!fOk)
	{
		// TODO: just create an empty set of features, since this is not disastrous.
		m_stuInitError = L"could not load Feat table for Graphite rendering";
		m_ferr = kferrLoadFeatTable;
		goto LUnexpected;
	}

	// Glat
	res = (pGlatTbl = pfont->getTable(TtfUtil::TableIdTag(ktiGlat), &cbGlatSz)) ? kresOk : kresFail;
	fOk = pGlatTbl && (cbGlatSz == 0 || TtfUtil::CheckTable(ktiGlat, pGlatTbl, cbGlatSz));
	if (!fOk)
	{
		m_stuInitError = L"could not load Glat table for Graphite rendering";
		m_ferr = kferrLoadGlatTable;
		goto LUnexpected;
	}

	// Gloc
	res = (pGlocTbl = pfont->getTable(TtfUtil::TableIdTag(ktiGloc), &cbGlocSz)) ? kresOk : kresFail;
	fOk = pGlocTbl && (cbGlocSz == 0 || TtfUtil::CheckTable(ktiGloc, pGlocTbl, cbGlocSz));
	if (!fOk)
	{
		m_stuInitError = L"could not load Gloc table for Graphite rendering";
		m_ferr = kferrLoadGlocTable;
		goto LUnexpected;
	}

	// Sill
	try {
		res = (pSillTbl = pfont->getTable(TtfUtil::TableIdTag(ktiSill), &cbSillSz)) ? kresOk : kresFail;
		fOk = pSillTbl && (cbSillSz == 0 || TtfUtil::CheckTable(ktiSill, pSillTbl, cbSillSz));
	}
	catch (...)
	{
		fOk = true;
		pSillTbl = NULL;
	}
	// if table couldn't be loaded, this is not disastrous.

//	ibGlocStart = cbGlatTbl;
	fOk = CheckTableVersions(&grstrm,
    		(const byte *)pSilfTbl, 0,
    		(const byte *)pGlocTbl, 0,
    		(const byte *)pFeatTbl, 0,
    		&m_fxdBadVersion);
	if (!fOk)
	{
//		wchar_t rgch1[50];
//		wchar_t rgch2[50];
//#if defined(_WIN32)
//		// This version does not work in Windows, in spite of being documented:
//		// swprintf(rgch, 50, L"%d", (m_fxdBadVersion >> 16));
//		// Removing the size argument make it work.
//		swprintf(rgch1, L"%d", (m_fxdBadVersion >> 16));
//		swprintf(rgch2, L"%d", (m_fxdBadVersion & 0x0000FFFF));
//#else
//		swprintf(rgch1, 50, L"%d", (m_fxdBadVersion >> 16));
//		swprintf(rgch2, 50, L"%d", (m_fxdBadVersion & 0x0000FFFF));
//#endif
		char rgch[50]; // more than enough space to print two 16-bit ints
		char *pch = &rgch[0];
		sprintf(rgch, "%d.%d", (m_fxdBadVersion >> 16), (m_fxdBadVersion & 0x0000FFFF));
		std::wstring stu = L"unsupported version (";
		//stu.append(rgch1);
		//stu.append(L".");
		//stu.append(rgch2);
		while (*pch != 0)
			stu.push_back((wchar_t)*pch++);
		stu.append(L") of Graphite tables");
		m_stuInitError.assign(stu.c_str());
		m_ferr = kferrBadVersion;
		goto LUnexpected;
	}

	try 
	{
		// Parse the "Silf" table.
		grstrm.OpenBuffer((const byte*)pSilfTbl, cbSilfSz);
		int chwGlyphIDMax, fxdVersion;
		bool f = ReadSilfTable(grstrm, 0, 0, &chwGlyphIDMax, &fxdVersion);
		grstrm.Close();
		if (!f)
		{
			m_ferr = kferrReadSilfTable;
			fexptn.errorCode = m_ferr;
			throw fexptn;
		}
	
		//	Parse the "Gloc" and "Glat" tables.
		{
			GrBufferIStream grstrmGlat;
	
			grstrm.OpenBuffer((const byte *)pGlocTbl, cbGlocSz);
			grstrmGlat.OpenBuffer((const byte *)pGlatTbl, cbGlatSz);
			f = ReadGlocAndGlatTables(grstrm, 0, grstrmGlat, 0, chwGlyphIDMax, fxdVersion);
			grstrm.Close();
			grstrmGlat.Close();
			if (!f)
			{
				m_ferr = kferrReadGlocGlatTable;
				fexptn.errorCode = m_ferr;
				throw fexptn;
			}
		}
	
		//	Parse the "Feat" table.
		grstrm.OpenBuffer((const byte *)pFeatTbl, cbFeatSz);
		f = ReadFeatTable(grstrm, 0);
		grstrm.Close();
		if (!f)
		{
			m_ferr = kferrReadFeatTable;
			fexptn.errorCode = m_ferr;
			throw fexptn;
		}

		//	Parse the "Sill" table.
		if (pSillTbl)
		{
			grstrm.OpenBuffer((const byte *)pSillTbl, cbFeatSz);
			f = ReadSillTable(grstrm, 0);
			grstrm.Close();
			if (!f)
			{
				m_ferr = kferrReadSillTable;
				fexptn.errorCode = m_ferr;
				throw fexptn;
			}
		}
		else
			m_langtbl.CreateEmpty();
	}
	catch (...)
	{
		fSilf = false;
		m_resFontRead = kresUnexpected;
		try {
			DestroyContents(false);
		}
		catch (...)
		{}
		goto LUnexpected;
	}

	m_stuErrCtrlFile.erase();
	m_nFontCheckSum = nCheckSum;
	m_resFontRead = kresOk;
	m_ferr = kferrOkay;
	ReturnResult(kresOk);

LUnexpected:
	// Don't do this, because it is possible to use a base font with an empty Graphite
	// engine:
	//if (m_fUseSepBase || m_stuBaseFaceName.Length() > 0)
	//{
	//	SwitchGraphicsFont(pgg, false); // back to Graphite table font
	//	m_stuBaseFaceName.erase();
	//	m_fUseSepBase = false;
	//}

	CreateEmpty();
	m_nFontCheckSum = nCheckSum;
	if (!fBasicTables)
		m_resFontRead = kresFail;		// bad font
	else if (!fSilf)
		m_resFontRead = kresFalse;		// no Silf table--not a Graphite font
	else
		m_resFontRead = kresUnexpected;	// couldn't read the Graphite tables

	fexptn.errorCode = m_ferr;
	fexptn.version = m_fxdBadVersion >> 16;
	fexptn.subVersion = m_fxdBadVersion & 0x0000FFFF;
	throw fexptn;

	ReturnResult(m_resFontRead);
}

/*----------------------------------------------------------------------------------------------
	Read the cmap and name tables.
	
	This is called from two places. One is when we first initialize the engine. Also, if the
	font tables have not been actually copied from the font, they may have been deleted
	when the font was deleted. So when reusing the engine, set them to something valid based
	on the current font.
----------------------------------------------------------------------------------------------*/
bool GrEngine::SetCmapAndNameTables(Font * pfont)
{
	GrResult res = kresOk;
	bool fOk;
	const void * pCmapTbl;
 	const void * pNameTbl;
	size_t cbCmapSz, cbNameSz;

	// cmap
	if (!m_fCmapTblCopy)
	{
		res = (pCmapTbl = pfont->getTable(TtfUtil::TableIdTag(ktiCmap), &cbCmapSz)) ? kresOk : kresFail;
		fOk = pCmapTbl && (cbCmapSz == 0 || TtfUtil::CheckTable(ktiCmap, pCmapTbl, cbCmapSz));
		if (!fOk)
		{
			m_stuInitError = L"could not locate cmap table";
			m_ferr = kferrFindCmapTable;
			return false;
		}

		if (pCmapTbl && cbCmapSz > 0)
		{
			// Make a private copy of the cmap for the engine's use.
			m_pCmapTbl = new byte[cbCmapSz];
			std::copy(reinterpret_cast<const byte*>(pCmapTbl), 
					reinterpret_cast<const byte*>(pCmapTbl) + cbCmapSz, m_pCmapTbl);
			m_fCmapTblCopy = true;
			m_cbCmapTbl = cbCmapSz;
		}
		else
		{
			m_pCmapTbl = const_cast<byte*>(reinterpret_cast<const byte*>(pCmapTbl));
			m_fCmapTblCopy = false;
		}

		// MS Unicode cmap
		m_pCmap_3_1 = TtfUtil::FindCmapSubtable(m_pCmapTbl, 3, 1);
		m_pCmap_3_10 = TtfUtil::FindCmapSubtable(m_pCmapTbl, 3, 10);
		if (!m_pCmap_3_1)
			m_pCmap_3_1 = TtfUtil::FindCmapSubtable(m_pCmapTbl, 3, 0);
		if (!m_pCmap_3_1)
		{
			m_stuInitError = L"failure to load cmap subtable";
			m_ferr = kferrLoadCmapSubtable;
			return false;
		}
		if (!TtfUtil::CheckCmap31Subtable(m_pCmap_3_1))
		{
			m_stuInitError = L"checking cmap subtable failed";
			m_ferr = kferrCheckCmapSubtable;
			return false;
		}
	}
	else
	{
		Assert(m_pCmapTbl);
	}

	// name table - eventually need feature label strings

	// Currently the only stuff we're getting from the name table are our feature names,
	// so use the version from the Graphite font (not the base font if any).
	//////if (m_fUseSepBase)
	//////	pgg->SetupGraphics(&chrpOriginal);

	if (!m_fNameTblCopy)
	{
		res = (pNameTbl = (const byte *)pfont->getTable(TtfUtil::TableIdTag(ktiName), &cbNameSz)) ? kresOk : kresFail;
		fOk = pNameTbl && (cbNameSz == 0 || TtfUtil::CheckTable(ktiName, pNameTbl, cbNameSz));
		if (!fOk)
		{
			m_stuInitError = L"could not locate name table";
			m_ferr = kferrFindNameTable;
			return false;
		}

		if (pNameTbl && cbNameSz > 0)
		{
			// Make a private copy of the name table for the engine's use.
			m_pNameTbl = new byte[cbNameSz];
			std::copy(reinterpret_cast<const byte*>(pNameTbl), 
				reinterpret_cast<const byte*>(pNameTbl) + cbNameSz, m_pNameTbl);
			m_fNameTblCopy = true;
			m_cbNameTbl = cbNameSz;
		}
		else
		{
			m_pNameTbl = const_cast<byte*>(reinterpret_cast<const byte*>(pNameTbl));
			m_fNameTblCopy = false;
		}
	}
	else
	{
		Assert(m_pNameTbl);
	}

	return true;
}


//void FontFace::DbgCheckFontFace()
//{
//	Assert(m_cfonts < 5000);
//	Assert(m_cfonts >= 0);
//	wchar_t chw0 = m_pgreng->m_stuFaceName[0];
//	Assert(chw0 >= 0x0040); // A
//	Assert(chw0 <= 0x007A); // z
//}

} // namespace gr

//:End Ignore
