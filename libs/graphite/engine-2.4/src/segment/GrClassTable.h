/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrClassTable.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    The GrClassTable and related classes that store the classes that are used in substitutions.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GR_CTABLE_INCLUDED
#define GR_CTABLE_INCLUDED

#include <cstring>

//:End Ignore

namespace gr
{

/*----------------------------------------------------------------------------------------------
	A glyph ID / index pair, a member of the GrInputClass where the items are sorted
	by glyph ID.

	Hungarian: gix
----------------------------------------------------------------------------------------------*/
class GrGlyphIndexPair
{
	friend class GrInputClass;
	friend class GrClassTable;

	gid16 GlyphID()	{ return lsbf(m_gidBIG); }
	data16 Index()	{ return lsbf(m_nBIGIndex); }

	gid16	m_gidBIG;
	data16	m_nBIGIndex;
};

/*----------------------------------------------------------------------------------------------
	A class consisting of a mapping from glyph ID to index, used for classes that
	function as input classes (eg, occurrng in the left-hand side of a rule).
	
	Hungarian: clsin
----------------------------------------------------------------------------------------------*/
class GrInputClass
{
	friend class GrClassTable;

protected:
	/*------------------------------------------------------------------------------------------
		Copy the raw memory into the instance.
	------------------------------------------------------------------------------------------*/
#ifdef NDEBUG
	void CopyFrom(data16 * pchwStart, int)
#else
	void CopyFrom(data16 * pchwStart, int cchw)
#endif
	{
		m_cgixBIG = pchwStart[0];
		m_digixBIGInit = pchwStart[1];
		m_cBIGLoop = pchwStart[2];
		m_igixBIGStart = pchwStart[3];

		int cgix = NumberOfGlyphs();
		m_pgixFirst = m_prggixBuffer;
		if (cgix > 64)
		{
			m_vgix.resize(cgix);
			m_pgixFirst = &m_vgix[0];
		}
		gAssert((4 + (cgix * 2)) == cchw);
		#ifdef _DEBUG
			std::memset(m_pgixFirst, 0, cgix * sizeof(GrGlyphIndexPair));
		#endif
		Assert(sizeof(GrGlyphIndexPair) == sizeof(gid16) + sizeof(data16));
		GrGlyphIndexPair * pgixStart = reinterpret_cast<GrGlyphIndexPair*>(pchwStart + 4);
		std::copy(pgixStart, pgixStart + cgix, m_pgixFirst);
	}

	int NumberOfGlyphs()		{ return lsbf(m_cgixBIG); }
	int LoopCount()				{ return lsbf(m_cBIGLoop); }
	int InitialSearchRange()	{ return lsbf(m_digixBIGInit); }
	int StartSearch()			{ return lsbf(m_igixBIGStart); }
		
	int FindIndex(gid16 gid);

protected:
	//	Instance variables:
	data16	m_cgixBIG;				// number of glyphs in the class

	//	constants for fast binary search
	data16	m_digixBIGInit;		// (max power of 2 <= m_cgix);
								//		size of initial range to consider
	data16	m_cBIGLoop;			// log2(max power of 2 <= m_cgix);
								//		indicates how many iterations are necessary
	data16	m_igixBIGStart;		// m_cgix - m_digixInit;
								//		where to start search

	GrGlyphIndexPair m_prggixBuffer[64];
	std::vector<GrGlyphIndexPair> m_vgix;
	GrGlyphIndexPair * m_pgixFirst;
};


/*----------------------------------------------------------------------------------------------
	Contains all the classes used for substitution rules.
	
	Hungarian: ctbl
----------------------------------------------------------------------------------------------*/

class GrClassTable
{
	friend class FontMemoryUsage;

public:
	//	Constructor & destructor:
	GrClassTable()
		:	m_prgichwOffsets(NULL),
			m_prgchwBIGGlyphList(NULL)
	{
	}

	~GrClassTable()
	{
		delete[] m_prgichwOffsets;
		delete[] m_prgchwBIGGlyphList;
	}

	int NumberOfClasses()			{ return m_ccls; }
	int NumberOfInputClasses()		{ return m_ccls - m_cclsLinear; }
	int NumberOfOutputClasses()		{ return m_cclsLinear; }

	bool ReadFromFont(GrIStream & grstrm, int fxdVersion);
	void CreateEmpty();

	int FindIndex(int icls, gid16 chwGlyphID);
	gid16 GetGlyphID(int icls, int ichw);
	int NumberOfGlyphsInClass(int icls);

	gid16 GlyphAt(int ichw)
	{
		return lsbf(m_prgchwBIGGlyphList[ichw]);
	}
	data16 * GlyphListLoc(int ichw)
	{
		return m_prgchwBIGGlyphList + ichw;
	}

protected:
	//	Instance variables:
	int		m_ccls;				// number of classes
	int		m_cclsLinear;		// number of classes in linear format

	data16 *	m_prgichwOffsets;

	//	Two formats are included in the following array: the first section consists of
	//	flat ordered lists of glyphs, used for the "output" classes that use linear format.
	//	This provides an index-to-glyph mapping.
	//	The second section contains data in the format of GrInputClasses, used for "input"
	//	classes that need a binary-search format. This provides a glyph-to-index mapping.
	//	We don't create instances of GrInputClasses, because that would make reading
	//	from the ECF file slow. Instead we just set up a single instance at the time
	//	we're interested in it.
	//	NOTE that all this data has been slurped directly from the ECF file and therefore
	//	uses BIG-ENDIAN format.
	gid16 *	m_prgchwBIGGlyphList;

};

} // namespace gr

#endif // !GR_CTABLE_INCLUDED

