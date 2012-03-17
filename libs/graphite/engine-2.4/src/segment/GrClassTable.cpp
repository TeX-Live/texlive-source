/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrClassTable.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Implements the GrClassTable class and related classes.
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "Main.h"

#ifdef _MSC_VER
#pragma hdrstop
#endif

//:End Ignore

//:>********************************************************************************************
//:>	Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	Local Constants and static variables
//:>********************************************************************************************

namespace gr
{

//:>********************************************************************************************
//:>	Methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Fill in by reading from the file stream.
----------------------------------------------------------------------------------------------*/	
bool GrClassTable::ReadFromFont(GrIStream & grstrm, int fxdVersion)
{
	long lClassMapStart;
	grstrm.GetPositionInFont(&lClassMapStart);

	//	number of classes
	m_ccls = grstrm.ReadUShortFromFont();
	if (m_ccls >= kMaxReplcmtClasses)
		return false; // bad table
	if (fxdVersion < 0x00030000 && m_ccls > kMaxReplcmtClassesV1_2)
		return false; // bad table

	//	number of linear classes
	m_cclsLinear = grstrm.ReadUShortFromFont();
	if (m_cclsLinear > m_ccls)
		return false; // bad table

	//	class offsets
	m_prgichwOffsets = new data16[m_ccls + 1];
	gAssert(m_prgichwOffsets);
	if (!m_prgichwOffsets)
		return false; // bad table

	data16 * pchw = m_prgichwOffsets;
	int icls;
	for (icls = 0; icls <= m_ccls; icls++, pchw++)
	{
		*pchw = grstrm.ReadUShortFromFont();
	}

	//	Offsets are relative to the start class map; make them relative to the class list
	//	itself, and in terms of utf16s, not bytes.
	long lClassesPos;
	grstrm.GetPositionInFont(&lClassesPos);
	int cbDiff = lClassesPos - lClassMapStart;
	for (icls = 0; icls <= m_ccls; icls++)
	{
		m_prgichwOffsets[icls] = data16(int(m_prgichwOffsets[icls]) - cbDiff);
		gAssert((m_prgichwOffsets[icls] & 0x00000001) == 0);
		if ((m_prgichwOffsets[icls] & 0x00000001) != 0)
			return false; // bad table
		m_prgichwOffsets[icls] >>= 1;	// divide by 2
	}

	//	classes - slurp entire block (data remains in big-endian format)
	m_prgchwBIGGlyphList = new data16[m_prgichwOffsets[m_ccls]];
	gAssert(m_prgchwBIGGlyphList);
	if (!m_prgchwBIGGlyphList)
		return false; // bad table
	grstrm.ReadBlockFromFont(m_prgchwBIGGlyphList,
		m_prgichwOffsets[m_ccls] * sizeof(data16));

	return true;
}

/*----------------------------------------------------------------------------------------------
	Set up an empty class table.
----------------------------------------------------------------------------------------------*/	
void GrClassTable::CreateEmpty()
{
	//	number of classes
	m_ccls = 0;
	//	number of linear classes
	m_cclsLinear = 0;
}

/*----------------------------------------------------------------------------------------------
	Search for the glyph ID using a fast binary search, and return the matching index.
----------------------------------------------------------------------------------------------*/
int GrInputClass::FindIndex(gid16 gid)
{
	int digixInit = InitialSearchRange();
	int igixStart = StartSearch();
#ifdef _DEBUG
	int cgix = NumberOfGlyphs();
	int nPowerOf2 = 1;
	while (nPowerOf2 <= cgix)
		nPowerOf2 <<= 1;
	nPowerOf2 >>= 1;
	//	Now nPowerOf2 is the max power of 2 <= cgix
	gAssert((1 << LoopCount()) == nPowerOf2);		// LoopCount() == log2(nPowerOf2)
	gAssert(digixInit == nPowerOf2);
	gAssert(igixStart == cgix - digixInit);
#endif // _DEBUG

	int digixCurr = digixInit;

	GrGlyphIndexPair * pgixCurr = m_pgixFirst + igixStart;
	while (digixCurr > 0) 
	{
		int nTest;
		if (pgixCurr < m_pgixFirst)
			nTest = -1;
		else
			nTest = pgixCurr->GlyphID() - gid;

		if (nTest == 0)
			return pgixCurr->Index();

		digixCurr >>= 1;	// divide by 2
		if (nTest < 0)
			pgixCurr += digixCurr;
		else // (nTest > 0)
			pgixCurr -= digixCurr;
	}

	return -1;
}

/*----------------------------------------------------------------------------------------------
	Return the selector index for the given glyph ID.
----------------------------------------------------------------------------------------------*/
int GrClassTable::FindIndex(int icls, gid16 chwGlyphID)
{
	if (icls >= m_ccls)
	{
		gAssert(false); // bad compiler problem
		return 0;
	}

	if (icls < m_cclsLinear)
	{
		//	The class is an output class--uses linear format--and is being used as an
		//	input class. Shouldn't happen if the compiler is working right.
		gAssert(false);	// comment out for test procedures

		//	Do a slow linear search to find the glyph ID.
		int ichwMin = m_prgichwOffsets[icls];
		int ichwLim = m_prgichwOffsets[icls+1];
		int cchw = ichwLim - ichwMin;

		for (int ichw = 0; ichw < cchw; ichw++)
		{
			if (GlyphAt(ichwMin + ichw) == chwGlyphID)
				return ichw;
		}
		return -1;
	}

	//	Slurp the class into an instance so we can see what's there.
	GrInputClass clsin;
	int ichwMin = m_prgichwOffsets[icls];
	int ichwLim = m_prgichwOffsets[icls+1];
	clsin.CopyFrom(GlyphListLoc(ichwMin), (ichwLim - ichwMin));

	//	Do a fast binary search to find our glyph.
	int iRet = clsin.FindIndex(chwGlyphID);
	return iRet;
}

/*----------------------------------------------------------------------------------------------
	Return the glyph ID at the selector index.
----------------------------------------------------------------------------------------------*/
gid16 GrClassTable::GetGlyphID(int icls, int ichw)
{
	if (ichw < 0)
	{
		gAssert(false);
		return 0;
	}

	if (icls >= m_cclsLinear)
	{
		//	The class is an input class--sorted by glyph ID--and is being used as an
		//	output class. Shouldn't happen if the compiler is working right.
		gAssert(false);	// comment out for test procedures

		if (icls >= m_ccls) // bad compiler problem
            return 0;
		
		//	Do a slow linear search to find the index and answer the matching glyph.
		//	Slurp the class into an instance so we can see what's there.
		GrInputClass clsin;
		int ichwMin = m_prgichwOffsets[icls];
		int ichwLim = m_prgichwOffsets[icls+1];
		clsin.CopyFrom(GlyphListLoc(ichwMin), (ichwLim - ichwMin));

		int cgix = clsin.NumberOfGlyphs();
		for (int igix = 0; igix < cgix; igix++)
		{
			GrGlyphIndexPair * m_pgix = clsin.m_pgixFirst + igix;
			if (m_pgix->Index() == ichw)
				return m_pgix->GlyphID();
		}
		return 0;
	}

	int ichwMin = m_prgichwOffsets[icls];
	int ichwLim = m_prgichwOffsets[icls+1];

	if (ichw >= ichwLim - ichwMin)
		return 0;
	else
		return GlyphAt(ichwMin + ichw);
}

/*----------------------------------------------------------------------------------------------
	Return the number of glyphs in the class.
----------------------------------------------------------------------------------------------*/
int GrClassTable::NumberOfGlyphsInClass(int icls)
{
	int ichwMin = m_prgichwOffsets[icls];
	int ichwLim = m_prgichwOffsets[icls+1];

	if (icls >= m_cclsLinear)
	{
		if (icls >= m_ccls) // bad compiler problem
            return 0;
		
		GrInputClass clsin;
		clsin.CopyFrom(GlyphListLoc(ichwMin), (ichwLim - ichwMin));

		int cgix = clsin.NumberOfGlyphs();
		return cgix;
	}
	else
	{
		gAssert(false); // this method should not be used for output classes.
		return ichwLim - ichwMin;
	}
}

} // namespace gr

//:End Ignore

