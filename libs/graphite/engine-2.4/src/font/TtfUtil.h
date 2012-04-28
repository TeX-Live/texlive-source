/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2000, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: TtfUtil.h
Responsibility: Alan Ward
Last reviewed: Not yet.

Description:
    Utility class for handling TrueType font files.
----------------------------------------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma once
#endif
#ifndef TTFUTIL_H
#define TTFUTIL_H

// I don't know why I need to duplicate these there when they are in GrPlatform.h:
#ifdef _MSC_VER
#pragma warning(disable: 4127) // conditional expression is constant
#pragma warning(disable: 4290) // exception specification ignored.
#endif

#include <cstddef>
#include <stdexcept>
#include "GrPlatform.h"

// Enumeration used to specify a table in a TTF file
enum TableId
{
	ktiCmap, ktiCvt, ktiCryp, ktiHead, ktiFpgm, ktiGdir, ktiGlyf, 
	ktiHdmx, ktiHhea, ktiHmtx, ktiLoca, ktiKern, ktiLtsh, ktiMaxp, 
	ktiName, ktiOs2, ktiPost, ktiPrep, ktiFeat, ktiGlat, ktiGloc,
	ktiSilf, ktiSile, ktiSill,
	ktiLast /*This gives the enum length - it is not a real table*/
};

/*----------------------------------------------------------------------------------------------
	Class providing utility methods to parse a TrueType font file (TTF).
	Callling application handles all file input and memory allocation.
	Assumes minimal knowledge of TTF file format.
----------------------------------------------------------------------------------------------*/
namespace TtfUtil
{
	////////////////////////////////// tools to find & check TTF tables
	bool GetHeaderInfo(size_t & lOffset, size_t & lSize);
	bool CheckHeader(const void * pHdr);
	bool GetTableDirInfo(const void * pHdr, size_t & lOffset, size_t & lSize);
	bool GetTableInfo(TableId ktiTableId, const void * pHdr, const void * pTableDir, 
		size_t & lOffset, size_t & lSize);
	bool CheckTable(TableId ktiTableId, const void * pTable, size_t lTableSize);

	////////////////////////////////// simple font wide info 
	size_t  GlyphCount(const void * pMaxp); 
	size_t  MaxCompositeComponentCount(const void * pMaxp);
	size_t  MaxCompositeLevelCount(const void * pMaxp);
	size_t  LocaGlyphCount(size_t lLocaSize, const void * pHead) throw (std::domain_error); 
	int DesignUnits(const void * pHead);
	int HeadTableCheckSum(const void * pHead);
	void HeadTableCreateTime(const void * pHead, unsigned int * pnDateBC, unsigned int * pnDateAD);
	void HeadTableModifyTime(const void * pHead, unsigned int * pnDateBC, unsigned int * pnDateAD);
	bool IsItalic(const void * pHead);
	int FontAscent(const void * pOs2);
	int FontDescent(const void * pOs2);
	bool FontOs2Style(const void *pOs2, bool & fBold, bool & fItalic);
	bool Get31EngFamilyInfo(const void * pName, size_t & lOffset, size_t & lSize);
	bool Get31EngFullFontInfo(const void * pName, size_t & lOffset, size_t & lSize);
	bool Get30EngFamilyInfo(const void * pName, size_t & lOffset, size_t & lSize);
	bool Get30EngFullFontInfo(const void * pName, size_t & lOffset, size_t & lSize);
	int PostLookup(const void * pPost, size_t lPostSize, const void * pMaxp, 
		const char * pPostName);

	////////////////////////////////// utility methods helpful for name table
	bool GetNameInfo(const void * pName, int nPlatformId, int nEncodingId,
		int nLangId, int nNameId, size_t & lOffset, size_t & lSize);
	//size_t NameTableLength(const gr::byte * pTable);
	int GetLangsForNames(const void * pName, int nPlatformId, int nEncodingId,
		int *nameIdList, int cNameIds, short *langIdList);
	void SwapWString(void * pWStr, size_t nSize = 0) throw (std::invalid_argument);

	////////////////////////////////// cmap lookup tools 
	void * FindCmapSubtable(const void * pCmap, int nPlatformId = 3, 
		int nEncodingId = 1); 
	bool CheckCmap31Subtable(const void * pCmap31);
	gr::gid16 Cmap31Lookup(const void * pCmap31, int nUnicodeId); 
	unsigned int Cmap31NextCodepoint(const void *pCmap31, unsigned int nUnicodeId,
		int * pRangeKey = 0);
	bool CheckCmap310Subtable(const void *pCmap310);
	gr::gid16 Cmap310Lookup(const void * pCmap310, unsigned int uUnicodeId); 
	unsigned int Cmap310NextCodepoint(const void *pCmap310, unsigned int nUnicodeId,
		int * pRangeKey = 0);

	///////////////////////////////// horizontal metric data for a glyph
	bool HorMetrics(gr::gid16 nGlyphId, const void * pHmtx, size_t lHmtxSize, 
		const void * pHhea, int & nLsb, unsigned int & nAdvWid);

	///////////////////////////////// convert our TableId enum to standard TTF tags
	gr::fontTableId32 TableIdTag(const TableId);

	////////////////////////////////// primitives for loca and glyf lookup 
	size_t LocaLookup(gr::gid16 nGlyphId, const void * pLoca, size_t lLocaSize, 
		const void * pHead) throw (std::out_of_range); 
	void * GlyfLookup(const void * pGlyf, size_t lGlyfOffset);

	////////////////////////////////// primitves for simple glyph data
	bool GlyfBox(const void * pSimpleGlyf, int & xMin, int & yMin, 
		int & xMax, int & yMax);

	int GlyfContourCount(const void * pSimpleGlyf); 
	bool GlyfContourEndPoints(const void * pSimpleGlyf, int * prgnContourEndPoint, 
		int cnPointsTotal, size_t & cnPoints);
	bool GlyfPoints(const void * pSimpleGlyf, int * prgnX, int * prgnY, 
		char * prgbFlag, int cnPointsTotal, int & cnPoints);
	
	// primitive to find the glyph ids in a composite glyph
	bool GetComponentGlyphIds(const void * pSimpleGlyf, int * prgnCompId, 
		size_t cnCompIdTotal, size_t & cnCompId);
	// primitive to find the placement data for a component in a composite glyph
	bool GetComponentPlacement(const void * pSimpleGlyf, int nCompId,
		bool fOffset, int & a, int & b);						
	// primitive to find the transform data for a component in a composite glyph
	bool GetComponentTransform(const void * pSimpleGlyf, int nCompId,
		float & flt11, float & flt12, float & flt21, float & flt22, bool & fTransOffset);

	////////////////////////////////// operate on composite or simple glyph (auto glyf lookup)
	void * GlyfLookup(gr::gid16 nGlyphId, const void * pGlyf, const void * pLoca, 
		size_t lLocaSize, const void * pHead); // primitive used by below methods

	// below are primary user methods for handling glyf data
	bool IsSpace(gr::gid16 nGlyphId, const void * pLoca, size_t lLocaSize, const void * pHead);
	bool IsDeepComposite(gr::gid16 nGlyphId, const void * pGlyf, const void * pLoca, 
		size_t lLocaSize, const void * pHead);

	bool GlyfBox(gr::gid16 nGlyphId, const void * pGlyf, const void * pLoca, size_t lLocaSize, 
		const void * pHead, int & xMin, int & yMin, int & xMax, int & yMax);
	bool GlyfContourCount(gr::gid16 nGlyphId, const void * pGlyf, const void * pLoca, 
		size_t lLocaSize, const void *pHead, size_t & cnContours);
	bool GlyfContourEndPoints(gr::gid16 nGlyphId, const void * pGlyf, const void * pLoca, 
		size_t lLocaSize,	const void * pHead, int * prgnContourEndPoint, size_t & cnPoints); 
	bool GlyfPoints(gr::gid16 nGlyphId, const void * pGlyf, const void * pLoca, 
		size_t lLocaSize, const void * pHead, const int * prgnContourEndPoint, size_t cnEndPoints, 
		int * prgnX, int * prgnY, bool * prgfOnCurve, size_t & cnPoints);

	// utitily method used by high-level GlyfPoints 
	bool SimplifyFlags(char * prgbFlags, int cnPoints);
	bool CalcAbsolutePoints(int * prgnX, int * prgnY, int cnPoints);

} // end of namespace TtfUtil

#endif
