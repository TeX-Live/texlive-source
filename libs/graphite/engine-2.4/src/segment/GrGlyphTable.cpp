/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrGlyphTable.cpp
Responsibility: Sharon Correll
Last reviewed: 10Aug99, JT, quick look over

Description:
    Implements the GrGlyphTable class and related classes.
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
	Fill in the glyph table from the font stream.
----------------------------------------------------------------------------------------------*/	
bool GrGlyphTable::ReadFromFont(GrIStream & grstrmGloc, long lGlocStart, 
	GrIStream & grstrmGlat, long lGlatStart, 
	data16 chwBWAttr, data16 chwJStrAttr, int cJLevels, int nCompAttr1, int cnCompPerLig, 
	int fxdSilfVersion)
{
	//	Create the glyph sub-table for the single style.
	GrGlyphSubTable * pgstbl = new GrGlyphSubTable();

	//	Gloc table--offsets into Glat table
	grstrmGloc.SetPositionInFont(lGlocStart);

	//	version
    int fxdGlocVersion = GrEngine::ReadVersion(grstrmGloc);
	if (fxdGlocVersion > kGlocVersion)
		return false;

	//	flags
	short snFlags = grstrmGloc.ReadShortFromFont();

	//	number of attributes
	unsigned short cAttrs = grstrmGloc.ReadUShortFromFont();

	//	Create a single sub-table for the single style.
	pgstbl->Initialize(fxdSilfVersion, snFlags,
		chwBWAttr, chwJStrAttr, data16(chwJStrAttr + cJLevels),
		m_cglf, cAttrs, nCompAttr1, cnCompPerLig);

	SetSubTable(0, pgstbl);

	return pgstbl->ReadFromFont(grstrmGloc, m_cglf, grstrmGlat, lGlatStart, fxdSilfVersion);
}

bool GrGlyphSubTable::ReadFromFont(GrIStream & grstrmGloc, int cGlyphs,
	GrIStream & grstrmGlat, long lGlatStart, int fxdSilfVersion)
{
	//	attribute value offsets--slurp
	if (m_fGlocShort)
	{
		grstrmGloc.ReadBlockFromFont(m_prgibBIGAttrValues, ((cGlyphs + 1) * sizeof(data16)));
	}
	else
	{
		grstrmGloc.ReadBlockFromFont(m_prgibBIGAttrValues, ((cGlyphs + 1) * sizeof(data32)));
	}

	//	length of buffer needed for glyph attribute values:
	int cbBufLen = GlocLookup(data16(cGlyphs));
	
	//	TODO: slurp debugger offsets: 'm_prgibBIGGlyphAttrDebug'

	//	Glat table
	grstrmGlat.SetPositionInFont(lGlatStart);

	//	version
	int fxdGlatVersion = grstrmGlat.ReadIntFromFont();
	if (fxdGlatVersion > kGlatVersion)
		return false;

	m_pgatbl = new GrGlyphAttrTable();
	m_pgatbl->Initialize(fxdGlatVersion, fxdSilfVersion, cbBufLen);

	//	Back up over the version number and include it right in the attribute value entry
	//	buffer, since the offsets take it into account.
	grstrmGlat.SetPositionInFont(lGlatStart);

	//	Slurp the entire block of entries.
	grstrmGlat.ReadBlockFromFont(m_pgatbl->m_prgbBIGEntries, cbBufLen);

	return true;
}

/*----------------------------------------------------------------------------------------------
	Create an empty glyph table with no glyph attributes. 
----------------------------------------------------------------------------------------------*/	
void GrGlyphTable::CreateEmpty()
{
	//	Create the glyph sub-table for the single style.
	GrGlyphSubTable * pgstbl = new GrGlyphSubTable();
	
	//	Create a single sub-table for the single style.
	pgstbl->Initialize(0, 0, 0, 0, 0, m_cglf, 0, 0, 0);

	SetSubTable(0, pgstbl);

	pgstbl->CreateEmpty();
}

void GrGlyphSubTable::CreateEmpty()
{
	m_pgatbl = new GrGlyphAttrTable();
	m_pgatbl->Initialize(0, 0, 0);
}

/*----------------------------------------------------------------------------------------------
	Initialization.
----------------------------------------------------------------------------------------------*/
void GrGlyphSubTable::Initialize(int fxdSilfVersion, utf16 chwFlags,
	data16 chwBWAttr, data16 chwJStrAttr, data16 chwJStrHWAttr,
	int cGlyphs, int cAttrs, int nCompAttr1, int cnCompPerLig)
{
	m_fxdSilfVersion = fxdSilfVersion;
	m_fHasDebugStrings = HasAttrNames(chwFlags);
	m_nAttrIDLim = cAttrs;
	m_chwBWAttr = chwBWAttr;
	m_chwJStrAttr = chwJStrAttr;
	m_chwJStrHWAttr = chwJStrHWAttr;
	m_fGlocShort = !LongFormat(chwFlags);

	m_nCompAttr1 = nCompAttr1;

	if (m_fGlocShort)
	{ // item # cGlyphs holds length, which is convenient for dealing with last item
		m_prgibBIGAttrValues = new byte[(cGlyphs + 1) * sizeof(data16)];
	}
	else
	{
		m_prgibBIGAttrValues = new byte[(cGlyphs + 1) * sizeof(data32)];
	}
														
														
	if (m_fHasDebugStrings)
	{
		m_prgibBIGGlyphAttrDebug = new data16[cAttrs + 1];
	}

	//	Make a cache to hold a list of defined components for each glyph. This is
	//	calculated lazily, as needed, so we also include (as the first item) a flag to
	//	indicate whether or not it has been calculated; hence the "+1".
	m_cnCompPerLig = cnCompPerLig;
	m_prgnDefinedComponents = new int[(m_cnCompPerLig + 1) * cGlyphs];
	std::fill_n(m_prgnDefinedComponents, (m_cnCompPerLig + 1) * cGlyphs, 0);

	//	Now the instance is ready to have the locations and the debug strings
	//	read from the file.
}

/*----------------------------------------------------------------------------------------------
	Return the attribute value for the given glyph.
----------------------------------------------------------------------------------------------*/
int GrGlyphSubTable::GlyphAttrValue(gid16 chwGlyphID, int nAttrID)
{
	if (m_nAttrIDLim == 0)
	{
		//	No attributes defined (eg, invalid font)
		return 0;
	}
	if (nAttrID >= m_nAttrIDLim || nAttrID >= 0xFFFF)
	{
		gAssert(false);
		return 0;
	}
	int ibMin = GlocLookup(chwGlyphID);		// where this glyph's attrs start
	int	ibLim = GlocLookup(chwGlyphID + 1);	// where next glyph starts

	int nRet = m_pgatbl->GlyphAttr16BitValue(ibMin, ibLim, data16(nAttrID));
	if ((data16)nAttrID == m_chwJStrAttr)
	{
		//	For justify.stretch, which can be a 32-bit value, add on the high word.
		unsigned int nRetHW = m_pgatbl->GlyphAttr16BitValue(ibMin, ibLim, m_chwJStrHWAttr);
		nRet = (nRetHW << 16) | nRet;
	}
	return ConvertValueForVersion(nRet, nAttrID);
}

/*----------------------------------------------------------------------------------------------
	Convert the glyph attribute value from the version of the font table to what is
	expected by the engine.
----------------------------------------------------------------------------------------------*/
int GrGlyphSubTable::ConvertValueForVersion(int nValue, int nAttrID)
{
	return ConvertValueForVersion(nValue, nAttrID, (int)m_chwBWAttr, m_fxdSilfVersion);
}

int GrGlyphSubTable::ConvertValueForVersion(int nValue, int nAttrID, int nBWAttr, int fxdVersion)
{
	if ((nBWAttr >= 0 && nAttrID == (int)nBWAttr) ||
		(nBWAttr == -1 && nAttrID == (int)kslatBreak))
	{
		if (fxdVersion < 0x00020000)
		{
			//switch (nValue)
			//{
			//case klbv1WordBreak:	return klbWordBreak;
			//case klbv1HyphenBreak:	return klbHyphenBreak;
			//case klbv1LetterBreak:	return klbLetterBreak;
			//case klbv1ClipBreak:	return klbClipBreak;
			//default:
			//	break;
			//}
			//switch (nValue * -1)
			//{
			//case klbv1WordBreak:	return (klbWordBreak * -1);
			//case klbv1HyphenBreak:	return (klbHyphenBreak * -1);
			//case klbv1LetterBreak:	return (klbLetterBreak * -1);
			//case klbv1ClipBreak:	return (klbClipBreak * -1);
			//default:
			//	break;
			//}

			//	Breakweight values in version 2 are 10 times what they were in version 1.
			if (abs(nValue) <= 7)
				return nValue * 10;
		}
	}
	return nValue;
}

/*----------------------------------------------------------------------------------------------
	Return the attribute value for a glyph. Loop through the attribute runs, looking
	for the one containing the given attribute. Assume the value is 0 if not found.

	@param ibMin		- byte offset of first run for the glyph of interest
	@param ibLim		- byte offset of first run for the following glyph
	@param nAttrID		- attribute to find
----------------------------------------------------------------------------------------------*/
int GrGlyphAttrTable::GlyphAttr16BitValue(int ibMin, int ibLim, data16 nAttrID)
{
	GrGlyphAttrRun gatrun;
	byte * pbBIGEnt = m_prgbBIGEntries + ibMin;

	if (m_fxdGlatVersion < 0x00020000 && nAttrID >= 0xFF) {
		gAssert(false);
		return 0;
	}

	if (ibMin >= m_cbEntryBufLen || ibLim >= m_cbEntryBufLen)
	{
		// Invalid offset
		return 0;
	}

	while (pbBIGEnt < m_prgbBIGEntries + ibLim)
	{
		//	Suck the run of attribute values into the instance, so we can see
		//	what's there.
		gatrun.CopyFrom(pbBIGEnt, m_fxdGlatVersion);

		if (nAttrID < gatrun.m_suMinAttrID)
			//	Attribute not found--assume value is 0.
			return 0;
		else if (gatrun.m_suMinAttrID + gatrun.m_cAttrs > nAttrID)
		{
			//	Found the value in this run.
			data16 chw = lsbf(gatrun.m_rgchwBIGValues[nAttrID - gatrun.m_suMinAttrID]);
			if ((chw & 0x8000) == 0)
				return (int)chw;
			else
				return 0xFFFF0000 | chw;	// sign extension
		}
		else
			//	Go to next run.
			pbBIGEnt += gatrun.ByteCount(m_fxdGlatVersion);
	}

	//	Attribute not found--assume value is 0;
	return 0;
}

/*----------------------------------------------------------------------------------------------
	Given a point on the glyph, determine which component (if any) the point is in, and return
	its index relative to the components that are defined for this glyph; ie, the index within
	the compRef array.
	Return -1 if the click with not inside any component, or if there are no components
	defined for this glyph.
	CURRENTLY NOT USED - if ever used, need to incorporate floating point
----------------------------------------------------------------------------------------------*/
int GrGlyphSubTable::ComponentContainingPoint(gid16 chwGlyphID, int x, int y)
{
	int i = CalculateDefinedComponents(chwGlyphID);
	for (int inComp = 0; inComp < m_cnCompPerLig; inComp++)
	{
		//	The m_pnComponents list holds a list of the component attributes that are defined
		//	for this glyph, padded with -1's.
		int nCompID = m_prgnDefinedComponents[i + inComp];
		if (nCompID == -1)
			break; // hit the end of the list

		//	The value of the component attribute is actually the attribute ID for the
		//	first of the component box fields--the top. There should a contiguous group of
		//	four fields: top, bottom, left, right.
		int nFieldIDMin = GlyphAttrValue(chwGlyphID, nCompID);
		gAssert(nFieldIDMin != 0);
		int nTop = GlyphAttrValue(chwGlyphID, nFieldIDMin); 
		int nBottom = GlyphAttrValue(chwGlyphID, nFieldIDMin + 1); 
		int nLeft = GlyphAttrValue(chwGlyphID, nFieldIDMin + 2); 
		int nRight = GlyphAttrValue(chwGlyphID, nFieldIDMin + 3);

		if (nLeft <= x && x < nRight &&
			nBottom <= y && y < nTop)
		{
			return inComp;
		}
	}
	return -1;
}

/*----------------------------------------------------------------------------------------------
	Fill in the box with the source-device coordinates corresponding to the given component.
	The rectangle is relative to the top-left of the glyph.
	Return false if the component is not defined.

	@param xysEmSquare		- of the font, in logical units
	@param chwGlyphID		- the glyph of interest
	@param icomp			- the component of interest (eg, for the "oe" ligature,
								the index for component.o would be 0,
								and the index for component.e would be 1);
								these is as defined by the glyph, not as used in the rules
	@param mFontEmUnits		- the number of em-units in the font, for scaling
	@param dysFontAscent	- ascent of the font, for adjusting coordinate systems
	@param pxsLeft, pxsRight, pysTop, pysBottom
							- return values, in source device coordinates
	@param fTopOrigin		- convert values to a coordinate system where the origin is the
								top of the line, not the baseline
----------------------------------------------------------------------------------------------*/
bool GrGlyphSubTable::ComponentBoxLogUnits(float xysEmSquare,
	gid16 chwGlyphID, int icomp, int mFontEmUnits, float dysFontAscent,
	float * pxsLeft, float * pysTop, float * pxsRight, float * pysBottom,
	bool fTopOrigin)
{
	int i = CalculateDefinedComponents(chwGlyphID);

	//	The m_pnComponents list holds a list of the component attributes that are defined
	//	for this glyph, padded with -1's.
	int nCompID = m_prgnDefinedComponents[i + icomp];
	if (nCompID == -1)
	{
		// Ooops, the component is undefined for this glyph.
		*pxsLeft = 0;
		*pxsRight = 0;
		*pysTop = 0;
		*pysBottom = 0;
		return false;
	}

	//	The value of the component attribute is actually the attribute ID for the
	//	first of the component box fields--the top. There should a contiguous group of
	//	four fields: top, bottom, left, right.
	int nFieldIDMin = GlyphAttrValue(chwGlyphID, nCompID);
	if (nFieldIDMin == 0)
	{
		// Component inadequately defined. This can happen when we defined components
		// for a glyph and then substituted another glyph for it.
		*pxsLeft = 0;
		*pxsRight = 0;
		*pysTop = 0;
		*pysBottom = 0;
		return false;
	}
	int mTop = GlyphAttrValue(chwGlyphID, nFieldIDMin); 
	int mBottom = GlyphAttrValue(chwGlyphID, nFieldIDMin + 1); 
	int mLeft = GlyphAttrValue(chwGlyphID, nFieldIDMin + 2); 
	int mRight = GlyphAttrValue(chwGlyphID, nFieldIDMin + 3);

	////int xysFontEmUnits;
	////// ysHeight should correspond to the pixels in the font's em square
	////GrResult res = pgg->GetFontEmSquare(&xysFontEmUnits);
	////if (ResultFailed(res))
	////	return false;

	*pxsLeft = GrEngine::GrIFIMulDiv(mLeft, xysEmSquare, mFontEmUnits);
	*pxsRight = GrEngine::GrIFIMulDiv(mRight, xysEmSquare, mFontEmUnits);
	*pysTop = GrEngine::GrIFIMulDiv(mTop, xysEmSquare, mFontEmUnits);
	*pysBottom = GrEngine::GrIFIMulDiv(mBottom, xysEmSquare, mFontEmUnits);

	if (*pxsLeft > *pxsRight)
	{
		float xsTmp = *pxsLeft;
		*pxsLeft = *pxsRight;
		*pxsRight = xsTmp;
	}
	if (*pysTop < *pysBottom)
	{
		float ysTmp = *pysTop;
		*pysTop = *pysBottom;
		*pysBottom = ysTmp;
	}

	if (fTopOrigin)
	{
		//	Currently top and bottom coordinates are such that 0 is the baseline and positive is
		//	upwards, while for the box we want to return, 0 is the top of the glyph and positive
		//	is downwards. So switch systems.
		*pysTop = (dysFontAscent - *pysTop);
		*pysBottom = (dysFontAscent - *pysBottom);
	}

	return true;
}

/*----------------------------------------------------------------------------------------------
	Calculate the list of defined components for the given glyph, if any.
	Return the index to the first relevant item in the array.
	Private.
	TODO: With the current implementation that stores the glyph ids in each slot, this
	mechanism is redundant and obsolete, and should be removed.
----------------------------------------------------------------------------------------------*/
int GrGlyphSubTable::CalculateDefinedComponents(gid16 chwGlyphID)
{
	//	The first item is a flag indicating whether the list has been calculated.
	int iFlag = chwGlyphID * (m_cnCompPerLig + 1);	// index of has-been-calculated flag
	int iMin = iFlag + 1;	// first actual value; +1 in order to skip the flag
	if (m_prgnDefinedComponents[iFlag] == 0)
	{
		int iTmp = iMin;
		for (int nCompID = m_nCompAttr1; nCompID < m_nCompAttr1 + m_cComponents; nCompID++)
		{
			if (ComponentIsDefined(chwGlyphID, nCompID))
				m_prgnDefinedComponents[iTmp++] = nCompID;
			if (iTmp - iMin >= m_cnCompPerLig)
				break;	// ignore any components beyond the maximum allowed
		}
		//	Fill in the rest of the list with -1.
		for( ; iTmp < iMin + m_cnCompPerLig; iTmp++)
			m_prgnDefinedComponents[iTmp] = -1;

		m_prgnDefinedComponents[iFlag] = 1;	// has been calculated
	}
	return iMin;
}

/*----------------------------------------------------------------------------------------------
	Return true if the given attr ID, which is a component definition attribute, indicates
	that that component is defined for the given glyph.

	@param chwGlyphID		- the glyph of interest
	@param nAttrID			- the attribute of interest, assumed to be a component glyph
								attribute (eg. component.e.top)
----------------------------------------------------------------------------------------------*/
bool GrGlyphSubTable::ComponentIsDefined(gid16 chwGlyphID, int nAttrID)
{

	gAssert(nAttrID - m_nCompAttr1 < m_cComponents);
	if (nAttrID - m_nCompAttr1 >= m_cComponents)
		return false;

	return (GlyphAttrValue(chwGlyphID, nAttrID) != 0);
}

/*----------------------------------------------------------------------------------------------
	Return the index of the given component as defined for the given glyph ID. that is,
	convert the global ID for the component to the one within this glyph. Return -1
	if the component is not defined for the given glyph.
	For instance, given the glyph ID "oe" (the oe ligature) and the attr ID for component.o,
	the result would be 0; for component.e the result would be 1.
----------------------------------------------------------------------------------------------*/
int GrGlyphSubTable::ComponentIndexForGlyph(gid16 chwGlyphID, int nCompID)
{
	int i = CalculateDefinedComponents(chwGlyphID);

	for (int iLp = 0; iLp < m_cnCompPerLig; iLp++)
	{
		if (m_prgnDefinedComponents[i + iLp] == nCompID)
			return iLp;
	}
	return -1;
}

/*----------------------------------------------------------------------------------------------
	Return the ID of the nth defined component attribute for the given glyph.
----------------------------------------------------------------------------------------------*/
//int GrGlyphSubTable::NthComponentID(gid16 chwGlyphID, int n)
//{
//	int i = CalculateDefinedComponents(chwGlyphID);
//	return m_pnComponents[i + 1 + n];
//}

} // namespace gr

//:End Ignore
