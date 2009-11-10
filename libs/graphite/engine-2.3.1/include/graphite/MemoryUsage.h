/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2007 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: MemoryUsage.h
Responsibility: Sharon Correll
Last reviewed: not yet

Description:
    Data structures to hold the calculations for memory usage.
----------------------------------------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma once
#endif
#ifndef MEMORYUSAGE_INCLUDED
#define MEMORYUSAGE_INCLUDED

//:End Ignore


namespace gr
{

/*----------------------------------------------------------------------------------------------
	A data structure that holds information about the memory usage for font/engine objects
----------------------------------------------------------------------------------------------*/
class FontMemoryUsage
{
	friend class FontCache;
	friend class FontFace;

public:
	FontMemoryUsage()
	{
		initialize();
	}
	void addEngine(GrEngine * pgreng);
	void prettyPrint(std::ostream & strm);

protected:
	// member variables:
	std::vector<size_t> vFontTotalsReg;
	std::vector<size_t> vFontTotalsBold;
	std::vector<size_t> vFontTotalsItalic;
	std::vector<size_t> vFontTotalsBI;
	std::vector<std::string> vstrFontNames;
	std::vector<size_t> vFaceCount;

	size_t	font;
	size_t	fontCache;
	size_t	fontFace;

	size_t	eng_count;
	size_t	eng_overhead;
	size_t	eng_scalars;
	size_t	eng_strings;
	size_t	eng_pointers;
	size_t	eng_cmap;
	size_t	eng_nameTable;

	size_t	pseudoMap;

	size_t	clstbl_counters;
	size_t	clstbl_offsets;
	size_t	clstbl_glyphList;

	size_t	glftbl_general;
	size_t	glftbl_compDefns;
	size_t	glftbl_attrTable;
	size_t	glftbl_attrOffsets;

	size_t	lngtbl_general;
	size_t	lngtbl_entries;
	size_t	lngtbl_featureSets;

	size_t	tman_general;

	size_t	pass_count;
	size_t	pass_general;
	size_t	pass_fsm;
	size_t	pass_ruleExtras;
	size_t	pass_constraintOffsets;
	size_t	pass_constraintCode;
	size_t	pass_actionOffsets;
	size_t	pass_actionCode;

	size_t	engst_general;
	size_t	engst_passState;

	size_t	sstrm_count;
	size_t	sstrm_general;
	size_t	sstrm_chunkMapsUsed;
	size_t	sstrm_chunkMapsAlloc;
	size_t	sstrm_reprocBuf;

	size_t	slot_count;
	size_t	slot_general;
	size_t	slot_abstract;
	size_t	slot_varLenBuf;
	size_t	slot_assocsUsed;
	size_t	slot_assocsAlloc;
	size_t	slot_attachUsed;
	size_t	slot_attachAlloc;

	// methods:
	void initialize();
	int total();
	void add(FontMemoryUsage & fmu);
};

/*----------------------------------------------------------------------------------------------
	A data structure that holds information about the memory usage for segment objects.
----------------------------------------------------------------------------------------------*/
class SegmentMemoryUsage
{
public:
	SegmentMemoryUsage()
	{
		initialize();
	}
	void addSegment(Segment & seg);
	void prettyPrint(std::ostream & strm);

protected:
	// member variables:
	size_t	seg_count;
	size_t	overhead;
	size_t	pointers;
	size_t	scalars;
	size_t	strings;
	size_t	metrics;
	size_t	associations;
	size_t	init;
	size_t	obsolete;

	size_t	slot_count;
	size_t	slot_abstract;
	size_t	slot_varLenBuf;
	size_t	slot_scalars;
	size_t	slot_clusterMembers;

	size_t	glyphInfo_count;
	size_t	glyphInfo;

	size_t	wastedVector;	// allocated space that is not used in vectors

	// methods:
	void initialize();
};

} // namespace gr

#endif // !MEMORYUSAGE_INCLUDED
