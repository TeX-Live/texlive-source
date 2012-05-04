/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2000, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: TtfUtil.cpp
Responsibility: Alan Ward
Last reviewed: Not yet.

Description:
    Implements the methods for TtfUtil class. This file should remain portable to any C++ 
	environment by only using standard C++ and the TTF structurs defined in Tt.h.
	AW 2011-10-21: Add support for multi-level composite glyphs. Contribution by Alexey Kryukov.
-------------------------------------------------------------------------------*//*:End Ignore*/


/***********************************************************************************************
	Include files
***********************************************************************************************/
// Language headers
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <climits>
#include <stdexcept>
// Platform headers
// Module headers
#include "TtfUtil.h"
#include "TtfTypes.h"

/***********************************************************************************************
	Forward declarations
***********************************************************************************************/

/***********************************************************************************************
	Local Constants and static variables
***********************************************************************************************/
namespace 
{
	// max number of components allowed in composite glyphs
	const int kMaxGlyphComponents = 8;

	// These are basic byte order swapping functions
	template<typename T> inline T rev16(const T d) {
		T r =  (d & 0xff) << 8; r |= (d & 0xff00) >> 8;
		return r;
	}

	template<typename T> inline T rev32(const T d) {
		T r  = (d & 0xff) << 24; r |= (d & 0xff00) << 8;
		  r |= (d & 0xff0000) >> 8; r |= (d & 0xff000000) >> 24;
		return r;
	}

	// This is the generic read function which does the swapping
	template<typename T> inline T read(const T d) {
		return d;
	}
	
#if !defined WORDS_BIGENDIAN || defined PC_OS
	template<> inline TtfUtil::uint16 read(const TtfUtil::uint16 d) {
		return rev16(d);
	}
	
	template<> inline TtfUtil::int16 read(const TtfUtil::int16 d) {
		return rev16(d);
	}

	template<> inline TtfUtil::uint32 read(const TtfUtil::uint32 d) {
		return rev32(d);
	}
	
	template<> inline TtfUtil::int32 read(const TtfUtil::int32 d) {
		return rev32(d);
	}
#endif

	template <int R, typename T>
	inline float fixed_to_float(const T f) {
		return float(f)/float(2^R);
	}

#define MAKE_TAG(a,b,c,d) ((a << 24UL) + (b << 16UL) + (c << 8UL) + (d))
	const gr::fontTableId32 mapIdToTag[] = {
		MAKE_TAG('c','m','a','p'),
		MAKE_TAG('c','v','t',' '),
		MAKE_TAG('c','r','y','p'),
		MAKE_TAG('h','e','a','d'),
		MAKE_TAG('f','p','g','m'),
		MAKE_TAG('g','d','i','r'),
		MAKE_TAG('g','l','y','f'), 
		MAKE_TAG('h','d','m','x'),
		MAKE_TAG('h','h','e','a'),
		MAKE_TAG('h','m','t','x'),
		MAKE_TAG('l','o','c','a'),
		MAKE_TAG('k','e','r','n'),
		MAKE_TAG('L','T','S','H'),
		MAKE_TAG('m','a','x','p'), 
		MAKE_TAG('n','a','m','e'),
		MAKE_TAG('O','S','/','2'),
		MAKE_TAG('p','o','s','t'),
		MAKE_TAG('p','r','e','p'),
		MAKE_TAG('F','e','a','t'),
		MAKE_TAG('G','l','a','t'),
		MAKE_TAG('G','l','o','c'),
		MAKE_TAG('S','i','l','f'),
		MAKE_TAG('S','i','l','e'),
		MAKE_TAG('S','i','l','l')
 	};


/*----------------------------------------------------------------------------------------------
	Table of standard Postscript glyph names. From Martin Hosken. Disagress with ttfdump.exe
---------------------------------------------------------------------------------------------*/
	const int kcPostNames = 258;

	const char * rgPostName[kcPostNames] = {
		".notdef", ".null", "nonmarkingreturn", "space", "exclam", "quotedbl", "numbersign", 
		"dollar", "percent", "ampersand", "quotesingle", "parenleft", 
		"parenright", "asterisk", "plus", "comma", "hyphen", "period", "slash", 
		"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", 
		"nine", "colon", "semicolon", "less", "equal", "greater", "question", 
		"at", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", 
		"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 
		"bracketleft", "backslash", "bracketright", "asciicircum", 
		"underscore", "grave", "a", "b", "c", "d", "e", "f", "g", "h", "i", 
		"j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", 
		"x", "y", "z", "braceleft", "bar", "braceright", "asciitilde", 
		"Adieresis", "Aring", "Ccedilla", "Eacute", "Ntilde", "Odieresis", 
		"Udieresis", "aacute", "agrave", "acircumflex", "adieresis", "atilde", 
		"aring", "ccedilla", "eacute", "egrave", "ecircumflex", "edieresis", 
		"iacute", "igrave", "icircumflex", "idieresis", "ntilde", "oacute", 
		"ograve", "ocircumflex", "odieresis", "otilde", "uacute", "ugrave", 
		"ucircumflex", "udieresis", "dagger", "degree", "cent", "sterling", 
		"section", "bullet", "paragraph", "germandbls", "registered", 
		"copyright", "trademark", "acute", "dieresis", "notequal", "AE", 
		"Oslash", "infinity", "plusminus", "lessequal", "greaterequal", "yen", 
		"mu", "partialdiff", "summation", "product", "pi", "integral", 
		"ordfeminine", "ordmasculine", "Omega", "ae", "oslash", "questiondown", 
		"exclamdown", "logicalnot", "radical", "florin", "approxequal", 
		"Delta", "guillemotleft", "guillemotright", "ellipsis", "nonbreakingspace", 
		"Agrave", "Atilde", "Otilde", "OE", "oe", "endash", "emdash", 
		"quotedblleft", "quotedblright", "quoteleft", "quoteright", "divide", 
		"lozenge", "ydieresis", "Ydieresis", "fraction", "currency", 
		"guilsinglleft", "guilsinglright", "fi", "fl", "daggerdbl", "periodcentered", 
		"quotesinglbase", "quotedblbase", "perthousand", "Acircumflex", 
		"Ecircumflex", "Aacute", "Edieresis", "Egrave", "Iacute", 
		"Icircumflex", "Idieresis", "Igrave", "Oacute", "Ocircumflex", 
		"apple", "Ograve", "Uacute", "Ucircumflex", "Ugrave", "dotlessi", 
		"circumflex", "tilde", "macron", "breve", "dotaccent", "ring", 
		"cedilla", "hungarumlaut", "ogonek", "caron", "Lslash", "lslash", 
		"Scaron", "scaron", "Zcaron", "zcaron", "brokenbar", "Eth", "eth", 
		"Yacute", "yacute", "Thorn", "thorn", "minus", "multiply", 
		"onesuperior", "twosuperior", "threesuperior", "onehalf", "onequarter", 
		"threequarters", "franc", "Gbreve", "gbreve", "Idotaccent", "Scedilla", 
		"scedilla", "Cacute", "cacute", "Ccaron", "ccaron", 
		"dcroat" };

} // end of namespace

/***********************************************************************************************
	Methods
***********************************************************************************************/

/* Note on error processing: The code guards against bad glyph ids being used to look up data
in open ended tables (loca, hmtx). If the glyph id comes from a cmap this shouldn't happen 
but it seems prudent to check for user errors here. The code does assume that data obtained 
from the TTF file is valid otherwise (though the CheckTable method seeks to check for 
obvious problems that might accompany a change in table versions). For example an invalid 
offset in the loca table which could exceed the size of the glyf table is NOT trapped.
Likewise if numberOf_LongHorMetrics in the hhea table is wrong, this will NOT be trapped, 
which could cause a lookup in the hmtx table to exceed the table length. Of course, TTF tables
that are completely corrupt will cause unpredictable results. */

/* Note on composite glyphs: It is unclear how to build composite glyphs in some cases, 
so this code represents Alan's best guess until test cases can be found. See notes on
the high-level GlyfPoints method. */

namespace TtfUtil
{


/*----------------------------------------------------------------------------------------------
	Get offset and size of the offset table needed to find table directory.
	Return true if success, false otherwise.
	lSize excludes any table directory entries.
----------------------------------------------------------------------------------------------*/
bool GetHeaderInfo(size_t & lOffset, size_t & lSize)
{
	lOffset = 0;
	lSize   = offsetof(Sfnt::OffsetSubTable, table_directory);
	assert(sizeof(uint32) + 4*sizeof (uint16) == lSize);
	return true;
}

/*----------------------------------------------------------------------------------------------
	Check the offset table for expected data.
	Return true if success, false otherwise.
----------------------------------------------------------------------------------------------*/
bool CheckHeader(const void * pHdr)
{
	const Sfnt::OffsetSubTable * pOffsetTable  
		= reinterpret_cast<const Sfnt::OffsetSubTable *>(pHdr);

	return read(pOffsetTable->scaler_type) == Sfnt::OffsetSubTable::TrueTypeWin;
}

/*----------------------------------------------------------------------------------------------
	Get offset and size of the table directory.
	Return true if successful, false otherwise.
----------------------------------------------------------------------------------------------*/
bool GetTableDirInfo(const void * pHdr, size_t & lOffset, size_t & lSize)
{
	const Sfnt::OffsetSubTable * pOffsetTable
		= reinterpret_cast<const Sfnt::OffsetSubTable *>(pHdr);

	lOffset = offsetof(Sfnt::OffsetSubTable, table_directory);
	lSize   = read(pOffsetTable->num_tables) 
		* sizeof(Sfnt::OffsetSubTable::Entry);
	
	return true;
}


/*----------------------------------------------------------------------------------------------
	Get offset and size of the specified table.
	Return true if successful, false otherwise. On false, offset and size will be 0.
----------------------------------------------------------------------------------------------*/
bool GetTableInfo(TableId ktiTableId, const void * pHdr, const void * pTableDir, 
						   size_t & lOffset, size_t & lSize)
{
	gr::fontTableId32 lTableTag = TableIdTag(ktiTableId);
	if (!lTableTag)
	{
		lOffset = 0;
		lSize = 0;
		return false;
	}

	const Sfnt::OffsetSubTable * pOffsetTable 
		= reinterpret_cast<const Sfnt::OffsetSubTable *>(pHdr);
	const Sfnt::OffsetSubTable::Entry 
		* entry_itr = reinterpret_cast<const Sfnt::OffsetSubTable::Entry *>(
			pTableDir),
		* const  dir_end = entry_itr + read(pOffsetTable->num_tables);

	assert(read(pOffsetTable->num_tables) < 40);
	if (read(pOffsetTable->num_tables) > 40)
		return false;

	for (;entry_itr != dir_end; ++entry_itr) // 40 - safe guard
	{
		if (read(entry_itr->tag) == lTableTag)
		{
			lOffset = read(entry_itr->offset);
			lSize   = read(entry_itr->length);
			return true;
		}
	}

	return false;
}

/*----------------------------------------------------------------------------------------------
	Check the specified table. Tests depend on the table type.
	Return true if successful, false otherwise.
----------------------------------------------------------------------------------------------*/
bool CheckTable(TableId ktiTableId, const void * pTable, size_t lTableSize)
{
	using namespace Sfnt;
	
	switch(ktiTableId)
	{
	case ktiCmap: // cmap
	{
		const Sfnt::CharacterCodeMap * const pCmap 
			= reinterpret_cast<const Sfnt::CharacterCodeMap *>(pTable);
		assert(read(pCmap->version) == 0);
		return read(pCmap->version) == 0;
	}

	case ktiHead: // head
	{
		const Sfnt::FontHeader * const pHead 
			= reinterpret_cast<const Sfnt::FontHeader *>(pTable);
		bool r = read(pHead->version) == OneFix 
			&& read(pHead->magic_number) == FontHeader::MagicNumber 
			&& read(pHead->glyph_data_format) 
					== FontHeader::GlypDataFormat 
			&& (read(pHead->index_to_loc_format) 
					== FontHeader::ShortIndexLocFormat 
				|| read(pHead->index_to_loc_format) 
					== FontHeader::LongIndexLocFormat) 
			&& sizeof(FontHeader) <= lTableSize;
		assert(r); return r;
	}

	case ktiPost: // post
	{
		const Sfnt::PostScriptGlyphName * const pPost 
			= reinterpret_cast<const Sfnt::PostScriptGlyphName *>(pTable);
		const fixed format = read(pPost->format);
		bool r = format == PostScriptGlyphName::Format1 
			|| format == PostScriptGlyphName::Format2 
			|| format == PostScriptGlyphName::Format3 
			|| format == PostScriptGlyphName::Format25;
		assert(r); return r;
	}

	case ktiHhea: // hhea
	{
		const Sfnt::HorizontalHeader * pHhea = 
			reinterpret_cast<const Sfnt::HorizontalHeader *>(pTable);
		bool r = read(pHhea->version) == OneFix 
			&& read(pHhea->metric_data_format) == 0
			&& sizeof (Sfnt::HorizontalHeader) <= lTableSize;
		assert(r); return r;
	}

	case ktiMaxp: // maxp
	{
		const Sfnt::MaximumProfile * pMaxp = 
			reinterpret_cast<const Sfnt::MaximumProfile *>(pTable);
		bool r = read(pMaxp->version) == OneFix 
			&& sizeof(Sfnt::MaximumProfile) <= lTableSize;
		assert(r); return r;
	}

	case ktiOs2: // OS/2
	{
		const Sfnt::Compatibility * pOs2 
			= reinterpret_cast<const Sfnt::Compatibility *>(pTable);
		if (read(pOs2->version) == 0)
		{ // OS/2 table version 1 size
//			if (sizeof(Sfnt::Compatibility) 
//					- sizeof(uint32)*2 - sizeof(int16)*2 
//					- sizeof(uint16)*3 <= lTableSize)
			if (sizeof(Sfnt::Compatibility0) <= lTableSize)
				return true;
		}
		else if (read(pOs2->version) == 1)
		{ // OS/2 table version 2 size
//			if (sizeof(Sfnt::Compatibility) 
//					- sizeof(int16) *2 
//					- sizeof(uint16)*3 <= lTableSize)
			if (sizeof(Sfnt::Compatibility1) <= lTableSize)
				return true;
		}
		else if (read(pOs2->version) == 2)
		{ // OS/2 table version 3 size
			if (sizeof(Sfnt::Compatibility2) <= lTableSize)
				return true;
		}
		else if (read(pOs2->version) == 3)
		{ // OS/2 table version 4 size - version 4 changed the meaning of some fields which we don't use
			if (sizeof(Sfnt::Compatibility3) <= lTableSize)
				return true;
		}
		else
			return false;
	}

	case ktiName:
	{
		const Sfnt::FontNames * pName 
			= reinterpret_cast<const Sfnt::FontNames *>(pTable);
		assert(read(pName->format) == 0);
		return read(pName->format) == 0;
	}

	default:
		break;
	}

	return true;
}

/*----------------------------------------------------------------------------------------------
	Return the number of glyphs in the font. Should never be less than zero.

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
size_t GlyphCount(const void * pMaxp)
{
	const Sfnt::MaximumProfile * pTable = 
			reinterpret_cast<const Sfnt::MaximumProfile *>(pMaxp);
	return read(pTable->num_glyphs);
}

/*----------------------------------------------------------------------------------------------
	Return the maximum number of components for any composite glyph in the font.

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
size_t  MaxCompositeComponentCount(const void * pMaxp)
{
	const Sfnt::MaximumProfile * pTable = 
			reinterpret_cast<const Sfnt::MaximumProfile *>(pMaxp);
	return read(pTable->max_component_elements);
}

/*----------------------------------------------------------------------------------------------
	Composite glyphs can be composed of glyphs that are themselves composites.
	This method returns the maximum number of levels like this for any glyph in the font.
	A non-composite glyph has a level of 1.

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
size_t  MaxCompositeLevelCount(const void * pMaxp)
{
	const Sfnt::MaximumProfile * pTable = 
			reinterpret_cast<const Sfnt::MaximumProfile *>(pMaxp);
	return read(pTable->max_component_depth);
}

/*----------------------------------------------------------------------------------------------
	Return the number of glyphs in the font according to a differt source.
	Should never be less than zero. Return -1 on failure.

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
size_t LocaGlyphCount(size_t lLocaSize, const void * pHead) throw(std::domain_error)
{

	const Sfnt::FontHeader * pTable 
		= reinterpret_cast<const Sfnt::FontHeader *>(pHead);

	if (read(pTable->index_to_loc_format) 
		== Sfnt::FontHeader::ShortIndexLocFormat)
	// loca entries are two bytes and have been divided by two
		return (lLocaSize >> 1) - 1;
	
	if (read(pTable->index_to_loc_format) 
		== Sfnt::FontHeader::LongIndexLocFormat)
	 // loca entries are four bytes
		return (lLocaSize >> 2) - 1;

	//return -1;
	throw std::domain_error("head table in inconsistent state. The font may be corrupted");
}

/*----------------------------------------------------------------------------------------------
	Return the design units the font is designed with
----------------------------------------------------------------------------------------------*/
int DesignUnits(const void * pHead)
{
	const Sfnt::FontHeader * pTable = 
			reinterpret_cast<const Sfnt::FontHeader *>(pHead);
	
	return read(pTable->units_per_em);
}

/*----------------------------------------------------------------------------------------------
	Return the checksum from the head table, which serves as a unique identifer for the font.
----------------------------------------------------------------------------------------------*/
int HeadTableCheckSum(const void * pHead)
{
	const Sfnt::FontHeader * pTable = 
			reinterpret_cast<const Sfnt::FontHeader *>(pHead);
	
	return read(pTable->check_sum_adjustment);
}

/*----------------------------------------------------------------------------------------------
	Return the create time from the head table. This consists of a 64-bit integer, which
	we return here as two 32-bit integers.

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
void HeadTableCreateTime(const void * pHead,
	unsigned int * pnDateBC, unsigned int * pnDateAD)
{
	const Sfnt::FontHeader * pTable = 
			reinterpret_cast<const Sfnt::FontHeader *>(pHead);
	
	*pnDateBC = read(pTable->created[0]);
	*pnDateAD = read(pTable->created[1]);
}

/*----------------------------------------------------------------------------------------------
	Return the modify time from the head table.This consists of a 64-bit integer, which
	we return here as two 32-bit integers.

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
void HeadTableModifyTime(const void * pHead,
	unsigned int * pnDateBC, unsigned int *pnDateAD)
{
	const Sfnt::FontHeader * pTable = 
			reinterpret_cast<const Sfnt::FontHeader *>(pHead);
	
	*pnDateBC = read(pTable->modified[0]);
	*pnDateAD = read(pTable->modified[1]);
}

/*----------------------------------------------------------------------------------------------
	Return true if the font is italic.
----------------------------------------------------------------------------------------------*/
bool IsItalic(const void * pHead)
{
	const Sfnt::FontHeader * pTable = 
			reinterpret_cast<const Sfnt::FontHeader *>(pHead);

	return ((read(pTable->mac_style) & 0x00000002) != 0);
}

/*----------------------------------------------------------------------------------------------
	Return the ascent for the font
----------------------------------------------------------------------------------------------*/
int FontAscent(const void * pOs2)
{
	const Sfnt::Compatibility * pTable = reinterpret_cast<const Sfnt::Compatibility *>(pOs2);

	return read(pTable->win_ascent);
}

/*----------------------------------------------------------------------------------------------
	Return the descent for the font
----------------------------------------------------------------------------------------------*/
int FontDescent(const void * pOs2)
{
	const Sfnt::Compatibility * pTable = reinterpret_cast<const Sfnt::Compatibility *>(pOs2);

	return read(pTable->win_descent);
}

/*----------------------------------------------------------------------------------------------
	Get the bold and italic style bits.
	Return true if successful. false otherwise.
	In addition to checking the OS/2 table, one could also check
		the head table's macStyle field (overridden by the OS/2 table on Win)
		the sub-family name in the name table (though this can contain oblique, dark, etc too)
----------------------------------------------------------------------------------------------*/
bool FontOs2Style(const void *pOs2, bool & fBold, bool & fItalic)
{
	const Sfnt::Compatibility * pTable = reinterpret_cast<const Sfnt::Compatibility *>(pOs2);

	fBold = (read(pTable->fs_selection) & Sfnt::Compatibility::Bold) != 0;
	fItalic = (read(pTable->fs_selection) & Sfnt::Compatibility::Italic) != 0;
	
	return true;
}

/*----------------------------------------------------------------------------------------------
	Method for searching name table.
----------------------------------------------------------------------------------------------*/
bool GetNameInfo(const void * pName, int nPlatformId, int nEncodingId,
		int nLangId, int nNameId, size_t & lOffset, size_t & lSize)
{
	lOffset = 0;
	lSize = 0;

	const Sfnt::FontNames * pTable = reinterpret_cast<const Sfnt::FontNames *>(pName);
	uint16 cRecord = read(pTable->count);
	uint16 nRecordOffset = read(pTable->string_offset);
	const Sfnt::NameRecord * pRecord = reinterpret_cast<const Sfnt::NameRecord *>(pTable + 1);

	for (int i = 0; i < cRecord; ++i)
	{
		if (read(pRecord->platform_id) == nPlatformId && 
			read(pRecord->platform_specific_id) == nEncodingId &&
			read(pRecord->language_id) == nLangId && 
			read(pRecord->name_id) == nNameId)
		{
			lOffset = read(pRecord->offset) + nRecordOffset;
			lSize = read(pRecord->length);
			return true;
		}
		pRecord++;
	}

	return false;
}

/*----------------------------------------------------------------------------------------------
	Return all the lang-IDs that have data for the given name-IDs. Assume that there is room
	in the return array (langIdList) for 128 items. The purpose of this method is to return
	a list of all possible lang-IDs.
----------------------------------------------------------------------------------------------*/
int GetLangsForNames(const void * pName, int nPlatformId, int nEncodingId,
		int * nameIdList, int cNameIds, short * langIdList)
{
	const Sfnt::FontNames * pTable = reinterpret_cast<const Sfnt::FontNames *>(pName);
	uint16 cRecord = read(pTable->count);
	//uint16 nRecordOffset = swapw(pTable->stringOffset);
	const Sfnt::NameRecord * pRecord = reinterpret_cast<const Sfnt::NameRecord *>(pTable + 1);

	int cLangIds = 0;
	for (int i = 0; i < cRecord; ++i)
	{
		if (read(pRecord->platform_id) == nPlatformId && 
			read(pRecord->platform_specific_id) == nEncodingId)
		{
			bool fNameFound = false;
			int nLangId = read(pRecord->language_id);
			int nNameId = read(pRecord->name_id);
			for (int j = 0; j < cNameIds; j++)
			{
				if (nNameId == nameIdList[j])
				{
					fNameFound = true;
					break;
				}
			}
			if (fNameFound)
			{
				// Add it if it's not there.
				int ilang;
				for (ilang = 0; ilang < cLangIds; ilang++)
					if (langIdList[ilang] == nLangId)
						break;
				if (ilang >= cLangIds)
				{
					langIdList[cLangIds] = short(nLangId);
					cLangIds++;
				}
				if (cLangIds == 128)
					return cLangIds;
			}
		}
		pRecord++;
	}

	return cLangIds;
}

/*----------------------------------------------------------------------------------------------
	Get the offset and size of the font family name in English for the MS Platform with Unicode
	writing system. The offset is within the pName data. The string is double byte with MSB
	first.
----------------------------------------------------------------------------------------------*/
bool Get31EngFamilyInfo(const void * pName, size_t & lOffset, size_t & lSize)
{
	return GetNameInfo(pName, Sfnt::NameRecord::Microsoft, 1, 1033, 
		Sfnt::NameRecord::Family, lOffset, lSize);
}

/*----------------------------------------------------------------------------------------------
	Get the offset and size of the full font name in English for the MS Platform with Unicode
	writing system. The offset is within the pName data. The string is double byte with MSB
	first.

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
bool Get31EngFullFontInfo(const void * pName, size_t & lOffset, size_t & lSize)
{
	return GetNameInfo(pName, Sfnt::NameRecord::Microsoft, 1, 1033, 
		Sfnt::NameRecord::Fullname, lOffset, lSize);
}

/*----------------------------------------------------------------------------------------------
	Get the offset and size of the font family name in English for the MS Platform with Symbol
	writing system. The offset is within the pName data. The string is double byte with MSB
	first.
----------------------------------------------------------------------------------------------*/
bool Get30EngFamilyInfo(const void * pName, size_t & lOffset, size_t & lSize)
{
	return GetNameInfo(pName, Sfnt::NameRecord::Microsoft, 0, 1033, 
		Sfnt::NameRecord::Family, lOffset, lSize);
}

/*----------------------------------------------------------------------------------------------
	Get the offset and size of the full font name in English for the MS Platform with Symbol
	writing system. The offset is within the pName data. The string is double byte with MSB
	first.

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
bool Get30EngFullFontInfo(const void * pName, size_t & lOffset, size_t & lSize)
{
	return GetNameInfo(pName, Sfnt::NameRecord::Microsoft, 0, 1033, 
		Sfnt::NameRecord::Fullname, lOffset, lSize);
}

/*----------------------------------------------------------------------------------------------
	Return the Glyph ID for a given Postscript name. This method finds the first glyph which
	matches the requested Postscript name. Ideally every glyph should have a unique Postscript 
	name (except for special names such as .notdef), but this is not always true.
	On failure return value less than zero.
	   -1 - table search failed
	   -2 - format 3 table (no Postscript glyph info)
	   -3 - other failures

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
int PostLookup(const void * pPost, size_t lPostSize, const void * pMaxp, 
						const char * pPostName)
{
	using namespace Sfnt;
	
	const Sfnt::PostScriptGlyphName * pTable 
		= reinterpret_cast<const Sfnt::PostScriptGlyphName *>(pPost);
	fixed format = read(pTable->format);

	if (format == PostScriptGlyphName::Format3)
	{ // format 3 - no Postscript glyph info in font
		return -2;
	}

	// search for given Postscript name among the standard names
	int iPostName = -1; // index in standard names
	for (int i = 0; i < kcPostNames; i++)
	{
		if (!strcmp(pPostName, rgPostName[i]))
		{
			iPostName = i;
			break;
		}
	}

	if (format == PostScriptGlyphName::Format1)
	{ // format 1 - use standard Postscript names
		return iPostName;
	}
	
	if (format == PostScriptGlyphName::Format25)
	{ 
		if (iPostName == -1)
			return -1;
		
		const PostScriptGlyphName25 * pTable25 
			= static_cast<const PostScriptGlyphName25 *>(pTable);
		int cnGlyphs = GlyphCount(pMaxp);
		for (gr::gid16 nGlyphId = 0; nGlyphId < cnGlyphs && nGlyphId < kcPostNames; 
				nGlyphId++)
		{ // glyph_name_index25 contains bytes so no byte swapping needed
		  // search for first glyph id that uses the standard name 
			if (nGlyphId + pTable25->offset[nGlyphId] == iPostName)
				return nGlyphId;
		}
	}

	if (format == PostScriptGlyphName::Format2)
	{ // format 2
		const PostScriptGlyphName2 * pTable2 
			= static_cast<const PostScriptGlyphName2 *>(pTable);
		
		int cnGlyphs = read(pTable2->number_of_glyphs);

		if (iPostName != -1)
		{ // did match a standard name, look for first glyph id mapped to that name
			for (gr::gid16 nGlyphId = 0; nGlyphId < cnGlyphs; nGlyphId++)
			{
				if (read(pTable2->glyph_name_index[nGlyphId]) == iPostName)
					return nGlyphId;
			}
			return -1; // no glyph with this standard name
		}

		else
		{ // did not match a standard name, search font specific names
			size_t nStrSizeGoal = strlen(pPostName);
			const char * pFirstGlyphName = reinterpret_cast<const char *>(
				&pTable2->glyph_name_index[0] + cnGlyphs);
			const char * pGlyphName = pFirstGlyphName;
			int iInNames = 0; // index in font specific names
			bool fFound = false;
			const char * const endOfTable 
				= reinterpret_cast<const char *>(pTable2) + lPostSize;
			while (pGlyphName < endOfTable && !fFound) 
			{ // search Pascal strings for first matching name
				size_t nStringSize = size_t(*pGlyphName);
				if (nStrSizeGoal != nStringSize ||
					strncmp(pGlyphName + 1, pPostName, nStringSize))
				{ // did not match
					++iInNames;
					pGlyphName += nStringSize + 1;
				}
				else
				{ // did match
					fFound = true;
				}
			}
			if (!fFound)
				return -1; // no font specific name matches request

			iInNames += kcPostNames;
			for (gr::gid16 nGlyphId = 0; nGlyphId < cnGlyphs; nGlyphId++)
			{ // search for first glyph id that maps to the found string index
				if (read(pTable2->glyph_name_index[nGlyphId]) == iInNames)
					return nGlyphId;
			}
			return -1; // no glyph mapped to this index (very strange)
		}
	}

	return -3;
}

/*----------------------------------------------------------------------------------------------
	Convert a Unicode character string from big endian (MSB first, Motorola) format to little 
	endian (LSB first, Intel) format. 
	nSize is the number of Unicode characters in the string. It should not include any 
	terminating null. If nSize is 0, it is assumed the string is null terminated. nSize 
	defaults to 0.
	Return true if successful, false otherwise. 
----------------------------------------------------------------------------------------------*/
void SwapWString(void * pWStr, size_t nSize /* = 0 */) throw (std::invalid_argument)
{
	if (pWStr == 0)
		throw std::invalid_argument("null pointer given");

	uint16 * pStr = reinterpret_cast<uint16 *>(pWStr);
	uint16 * const pStrEnd = pStr + (nSize == 0 ? gr::utf16len(pStr) : nSize);

#ifndef USE_STDLIB_WORKAROUNDS
	std::transform(pStr, pStrEnd, pStr, read<uint16>);
#else
	for (int i = 0; i < nSize; i++)
	{	// swap the wide characters in the string
		pStr[i] = gr::utf16(read(uint16(pStr[i])));
	}
#endif
}

/*----------------------------------------------------------------------------------------------
	Get the left-side bearing and and advance width based on the given tables and Glyph ID
	Return true if successful, false otherwise. On false, one or both value could be INT_MIN
----------------------------------------------------------------------------------------------*/
bool HorMetrics(gr::gid16 nGlyphId, const void * pHmtx, size_t lHmtxSize, const void * pHhea, 
						 int & nLsb, unsigned int & nAdvWid)
{
	const Sfnt::HorizontalMetric * phmtx = 
		reinterpret_cast<const Sfnt::HorizontalMetric *>(pHmtx);

	const Sfnt::HorizontalHeader * phhea = 
		reinterpret_cast<const Sfnt::HorizontalHeader *>(pHhea);

	size_t cLongHorMetrics = read(phhea->num_long_hor_metrics);
	if (nGlyphId < cLongHorMetrics) 
	{	// glyph id is acceptable
		nAdvWid = read(phmtx[nGlyphId].advance_width);
		nLsb = read(phmtx[nGlyphId].left_side_bearing);
	}
	else
	{
		nAdvWid = read(phmtx[cLongHorMetrics - 1].advance_width);

		// guard against bad glyph id
		size_t lLsbOffset = sizeof(Sfnt::HorizontalMetric) * cLongHorMetrics +
			sizeof(int16) * (nGlyphId - cLongHorMetrics); // offset in bytes
		if (lLsbOffset + 1 >= lHmtxSize) // + 1 because entries are two bytes wide
		{
			nLsb = 0;
			return false;
		}
		const int16 * pLsb = reinterpret_cast<const int16 *>(phmtx) + 
			lLsbOffset / sizeof(int16);
		nLsb = read(*pLsb);
	}

	return true;
}

/*----------------------------------------------------------------------------------------------
	Return a pointer to the requested cmap subtable. By default find the Microsoft Unicode
	subtable. Pass nEncoding as -1 to find first table that matches only nPlatformId.
	Return NULL if the subtable cannot be found.
----------------------------------------------------------------------------------------------*/
void * FindCmapSubtable(const void * pCmap, 
								 int nPlatformId, /* =3 */
								 int nEncodingId) /* = 1 */
{
	const Sfnt::CharacterCodeMap * pTable = 
		reinterpret_cast<const Sfnt::CharacterCodeMap *>(pCmap);
	
	uint16 csuPlatforms = read(pTable->num_subtables);
	for (int i = 0; i < csuPlatforms; i++)
	{
		if (read(pTable->encoding[i].platform_id) == nPlatformId &&
			(nEncodingId == -1 || read(pTable->encoding[i].platform_specific_id) == nEncodingId))
		{
			const void * pRtn = reinterpret_cast<const uint8 *>(pCmap) + 
				read(pTable->encoding[i].offset);
			return const_cast<void *>(pRtn);
		}
	}

	return 0;
}

/*----------------------------------------------------------------------------------------------
	Check the Microsoft Unicode subtable for expected values
----------------------------------------------------------------------------------------------*/
bool CheckCmap31Subtable(const void * pCmap31)
{
	const Sfnt::CmapSubTable * pTable = reinterpret_cast<const Sfnt::CmapSubTable *>(pCmap31);
	// Bob H says ome freeware TT fonts have version 1 (eg, CALIGULA.TTF) 
	// so don't check subtable version. 21 Mar 2002 spec changes version to language.

	return read(pTable->format) == 4;
}

/*----------------------------------------------------------------------------------------------
	Return the Glyph ID for the given Unicode ID in the Microsoft Unicode subtable.
	(Actually this code only depends on subtable being format 4.)
	Return 0 if the Unicode ID is not in the subtable.
----------------------------------------------------------------------------------------------*/
gr::gid16 Cmap31Lookup(const void * pCmap31, int nUnicodeId)
{
	const Sfnt::CmapSubTableFormat4 * pTable = reinterpret_cast<const Sfnt::CmapSubTableFormat4 *>(pCmap31);

	uint16 nSeg = read(pTable->seg_count_x2) >> 1;
  
	uint16 n;
    	const uint16 * pLeft, * pMid;
	uint16 cMid, chStart, chEnd;

	// Binary search of the endCode[] array
	pLeft = &(pTable->end_code[0]);
	n = nSeg;
	while (n > 0)
	{
		cMid = n >> 1;           // Pick an element in the middle
		pMid = pLeft + cMid;
		chEnd = read(*pMid);
		if (nUnicodeId <= chEnd)
		{
			if (cMid == 0 || nUnicodeId > read(pMid[-1]))
					break;          // Must be this seg or none!
			n = cMid;            // Continue on left side, omitting mid point
		}
		else
		{
			pLeft = pMid + 1;    // Continue on right side, omitting mid point
			n -= (cMid + 1);
		}
	}

	if (!n)
	return 0;

	// Ok, we're down to one segment and pMid points to the endCode element
	// Either this is it or none is.

	chStart = read(*(pMid += nSeg + 1));
	if (chEnd >= nUnicodeId && nUnicodeId >= chStart)
	{
		// Found correct segment. Find Glyph Id
		int16 idDelta = read(*(pMid += nSeg));
		uint16 idRangeOffset = read(*(pMid += nSeg));

		if (idRangeOffset == 0)
			return (uint16)(idDelta + nUnicodeId); // must use modulus 2^16

		// Look up value in glyphIdArray
		gr::gid16 nGlyphId = read(*(pMid + (nUnicodeId - chStart) + (idRangeOffset >> 1)));
		// If this value is 0, return 0. Else add the idDelta
		return nGlyphId ? nGlyphId + idDelta : 0;
	}

	return 0;
}

/*----------------------------------------------------------------------------------------------
	Return the next Unicode value in the cmap. Pass 0 to obtain the first item.
	Returns 0xFFFF as the last item.
	pRangeKey is an optional key that is used to optimize the search; its value is the range
	in which the character is found.

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
unsigned int Cmap31NextCodepoint(const void *pCmap31, unsigned int nUnicodeId, int * pRangeKey)
{
	const Sfnt::CmapSubTableFormat4 * pTable = reinterpret_cast<const Sfnt::CmapSubTableFormat4 *>(pCmap31);

	uint16 nRange = read(pTable->seg_count_x2) >> 1;

	uint32 nUnicodePrev = (uint32)nUnicodeId;

	const uint16 * pStartCode = &(pTable->end_code[0])
		+ nRange // length of end code array
		+ 1;   // reserved word

	if (nUnicodePrev == 0)
	{
		// return the first codepoint.
		if (pRangeKey)
			*pRangeKey = 0;
		return read(pStartCode[0]);
	}
	else if (nUnicodePrev >= 0xFFFF)
	{
		if (pRangeKey)
			*pRangeKey = nRange - 1;
		return 0xFFFF;
	}

	int iRange = (pRangeKey) ? *pRangeKey : 0;
	// Just in case we have a bad key:
	while (iRange > 0 && read(pStartCode[iRange]) > nUnicodePrev)
		iRange--;
	while (read(pTable->end_code[iRange]) < nUnicodePrev)
		iRange++;

	// Now iRange is the range containing nUnicodePrev.
	unsigned int nStartCode = read(pStartCode[iRange]);
	unsigned int nEndCode = read(pTable->end_code[iRange]);

	if (nStartCode > nUnicodePrev)
		// Oops, nUnicodePrev is not in the cmap! Adjust so we get a reasonable
		// answer this time around.
		nUnicodePrev = nStartCode - 1;

	if (nEndCode > nUnicodePrev)
	{
		// Next is in the same range; it is the next successive codepoint.
		if (pRangeKey)
			*pRangeKey = iRange;
		return nUnicodePrev + 1;
	}

	// Otherwise the next codepoint is the first one in the next range.
	// There is guaranteed to be a next range because there must be one that
	// ends with 0xFFFF.
	if (pRangeKey)
		*pRangeKey = iRange + 1;
	return read(pStartCode[iRange + 1]);
}

/*----------------------------------------------------------------------------------------------
	Check the Microsoft UCS-4 subtable for expected values.

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
bool CheckCmap310Subtable(const void *pCmap310)
{
	const Sfnt::CmapSubTable * pTable = reinterpret_cast<const Sfnt::CmapSubTable *>(pCmap310);
	return read(pTable->format) == 12;
}

/*----------------------------------------------------------------------------------------------
	Return the Glyph ID for the given Unicode ID in the Microsoft UCS-4 subtable.
	(Actually this code only depends on subtable being format 12.)
	Return 0 if the Unicode ID is not in the subtable.
----------------------------------------------------------------------------------------------*/
gr::gid16 Cmap310Lookup(const void * pCmap310, unsigned int uUnicodeId)
{
	const Sfnt::CmapSubTableFormat12 * pTable = reinterpret_cast<const Sfnt::CmapSubTableFormat12 *>(pCmap310);

	//uint32 uLength = read(pTable->length); //could use to test for premature end of table
	uint32 ucGroups = read(pTable->num_groups);

	for (unsigned int i = 0; i < ucGroups; i++)
	{
		uint32 uStartCode = read(pTable->group[i].start_char_code);
		uint32 uEndCode = read(pTable->group[i].end_char_code);
		if (uUnicodeId >= uStartCode && uUnicodeId <= uEndCode)
		{
			uint32 uDiff = uUnicodeId - uStartCode;
			uint32 uStartGid = read(pTable->group[i].start_glyph_id);
			return static_cast<gr::gid16>(uStartGid + uDiff);
		}
	}

	return 0;
}

/*----------------------------------------------------------------------------------------------
	Return the next Unicode value in the cmap. Pass 0 to obtain the first item.
	Returns 0x10FFFF as the last item.
	pRangeKey is an optional key that is used to optimize the search; its value is the range
	in which the character is found.

	Note: this method is not currently used by the Graphite engine.
----------------------------------------------------------------------------------------------*/
unsigned int Cmap310NextCodepoint(const void *pCmap310, unsigned int nUnicodeId, int * pRangeKey)
{
	const Sfnt::CmapSubTableFormat12 * pTable = reinterpret_cast<const Sfnt::CmapSubTableFormat12 *>(pCmap310);

	int nRange = read(pTable->num_groups);

	uint32 nUnicodePrev = (uint32)nUnicodeId;

	if (nUnicodePrev == 0)
	{
		// return the first codepoint.
		if (pRangeKey)
			*pRangeKey = 0;
		return read(pTable->group[0].start_char_code);
	}
	else if (nUnicodePrev >= 0x10FFFF)
	{
		if (pRangeKey)
			*pRangeKey = nRange;
		return 0x10FFFF;
	}

	int iRange = (pRangeKey) ? *pRangeKey : 0;
	// Just in case we have a bad key:
	while (iRange > 0 && read(pTable->group[iRange].start_char_code) > nUnicodePrev)
		iRange--;
	while (read(pTable->group[iRange].end_char_code) < nUnicodePrev)
		iRange++;

	// Now iRange is the range containing nUnicodePrev.

	unsigned int nStartCode = read(pTable->group[iRange].start_char_code);
	unsigned int nEndCode = read(pTable->group[iRange].end_char_code);

	if (nStartCode > nUnicodePrev)
		// Oops, nUnicodePrev is not in the cmap! Adjust so we get a reasonable
		// answer this time around.
		nUnicodePrev = nStartCode - 1;

	if (nEndCode > nUnicodePrev)
	{
		// Next is in the same range; it is the next successive codepoint.
		if (pRangeKey)
			*pRangeKey = iRange;
		return nUnicodePrev + 1;
	}

	// Otherwise the next codepoint is the first one in the next range, or 10FFFF if we're done.
	if (pRangeKey)
		*pRangeKey = iRange + 1;
	return (iRange + 1 >= nRange) ? 0x10FFFF : read(pTable->group[iRange + 1].start_char_code);
}

/*----------------------------------------------------------------------------------------------
	Return the offset stored in the loca table for the given Glyph ID.
	(This offset is into the glyf table.)
	Return -1 if the lookup failed.
	Technically this method should return an unsigned long but it is unlikely the offset will
		exceed 2^31.
----------------------------------------------------------------------------------------------*/
size_t LocaLookup(gr::gid16 nGlyphId, 
		const void * pLoca, size_t lLocaSize, 
		const void * pHead) throw (std::out_of_range)
{
	const Sfnt::FontHeader * pTable = reinterpret_cast<const Sfnt::FontHeader *>(pHead);

	// CheckTable verifies the index_to_loc_format is valid
	if (read(pTable->index_to_loc_format) == Sfnt::FontHeader::ShortIndexLocFormat)
	{ // loca entries are two bytes and have been divided by two
		if (nGlyphId <= (lLocaSize >> 1) - 1) // allow sentinel value to be accessed
		{
			const uint16 * pShortTable = reinterpret_cast<const uint16 *>(pLoca);
			return (read(pShortTable[nGlyphId]) << 1);
		}
	}
	
	if (read(pTable->index_to_loc_format) == Sfnt::FontHeader::LongIndexLocFormat)
	{ // loca entries are four bytes
		if (nGlyphId <= (lLocaSize >> 2) - 1)
		{
			const uint32 * pLongTable = reinterpret_cast<const uint32 *>(pLoca);
			return read(pLongTable[nGlyphId]);
		}
	}

	// only get here if glyph id was bad
	//return -1;
	throw std::out_of_range("glyph id out of range for font");
}

/*----------------------------------------------------------------------------------------------
	Return a pointer into the glyf table based on the given offset (from LocaLookup).
	Return NULL on error.
----------------------------------------------------------------------------------------------*/
void * GlyfLookup(const void * pGlyf, size_t nGlyfOffset)
{
	const uint8 * pByte = reinterpret_cast<const uint8 *>(pGlyf);
	return const_cast<uint8 *>(pByte + nGlyfOffset);
}

/*----------------------------------------------------------------------------------------------
	Get the bounding box coordinates for a simple glyf entry (non-composite).
	Return true if successful, false otherwise.
----------------------------------------------------------------------------------------------*/
bool GlyfBox(const void * pSimpleGlyf, int & xMin, int & yMin, 
					  int & xMax, int & yMax)
{
	const Sfnt::Glyph * pGlyph = reinterpret_cast<const Sfnt::Glyph *>(pSimpleGlyf);

	xMin = read(pGlyph->x_min);
	yMin = read(pGlyph->y_min);
	xMax = read(pGlyph->x_max);
	yMax = read(pGlyph->y_max);

	return true;
}

/*----------------------------------------------------------------------------------------------
	Return the number of contours for a simple glyf entry (non-composite)
	Returning -1 means this is a composite glyph
----------------------------------------------------------------------------------------------*/
int GlyfContourCount(const void * pSimpleGlyf)
{
	const Sfnt::Glyph * pGlyph = reinterpret_cast<const Sfnt::Glyph *>(pSimpleGlyf);
	return read(pGlyph->number_of_contours); // -1 means composite glyph
}

/*----------------------------------------------------------------------------------------------
	Get the point numbers for the end points of the glyph contours for a simple
	glyf entry (non-composite). 
	cnPointsTotal - count of contours from GlyfContourCount(); (same as number of end points)
	prgnContourEndPoints - should point to a buffer large enough to hold cnPoints integers
	cnPoints - count of points placed in above range
	Return true if successful, false otherwise.
		False could indicate a multi-level composite glyphs.
----------------------------------------------------------------------------------------------*/
bool GlyfContourEndPoints(const void * pSimpleGlyf, int * prgnContourEndPoint, 
								   int cnPointsTotal, int & cnPoints)
{
	const Sfnt::SimpleGlyph * pGlyph = reinterpret_cast<const Sfnt::SimpleGlyph *>(pSimpleGlyf);

	int cContours = read(pGlyph->number_of_contours);
	if (cContours < 0)
		return false; // this method isn't supposed handle composite glyphs

	for (int i = 0; i < cContours && i < cnPointsTotal; i++)
	{
		prgnContourEndPoint[i] = read(pGlyph->end_pts_of_contours[i]);
	}

	cnPoints = cContours;
	return true;
}

/*----------------------------------------------------------------------------------------------
	Get the points for a simple glyf entry (non-composite)
	cnPointsTotal - count of points from largest end point obtained from GlyfContourEndPoints
	prgnX & prgnY - should point to buffers large enough to hold cnPointsTotal integers
		The ranges are parallel so that coordinates for point(n) are found at offset n in both 
		ranges. This is raw point data with relative coordinates.
	prgbFlag - should point to a buffer a large enough to hold cnPointsTotal bytes
		This range is parallel to the prgnX & prgnY
	cnPoints - count of points placed in above ranges
	Return true if successful, false otherwise. 
		False could indicate a composite glyph
----------------------------------------------------------------------------------------------*/
bool GlyfPoints(const void * pSimpleGlyf, int * prgnX, int * prgnY, 
		char * prgbFlag, int cnPointsTotal, int & cnPoints)
{
	using namespace Sfnt;
	
	const Sfnt::SimpleGlyph * pGlyph = reinterpret_cast<const Sfnt::SimpleGlyph *>(pSimpleGlyf);
	int cContours = read(pGlyph->number_of_contours);
	// return false for composite glyph
	if (cContours <= 0)
		return false;
	int cPts = read(pGlyph->end_pts_of_contours[cContours - 1]) + 1;
	if (cPts > cnPointsTotal)
		return false;

	// skip over bounding box data & point to byte count of instructions (hints)
	const uint8 * pbGlyph = reinterpret_cast<const uint8 *>
		(&pGlyph->end_pts_of_contours[cContours]);
	
	// skip over hints & point to first flag
	int cbHints = read(*(const uint16 *)pbGlyph);
	pbGlyph += sizeof(uint16);
	pbGlyph += cbHints;

	// load flags & point to first x coordinate
	int iFlag = 0;
	while (iFlag < cPts)
	{
		if (!(*pbGlyph & SimpleGlyph::Repeat))
		{ // flag isn't repeated
			prgbFlag[iFlag] = (char)*pbGlyph;
			pbGlyph++;
			iFlag++;
		}
		else
		{ // flag is repeated; count specified by next byte
			char chFlag = (char)*pbGlyph;
			pbGlyph++;
			int cFlags = (int)*pbGlyph;
			pbGlyph++;
			prgbFlag[iFlag] = chFlag;
			iFlag++;
			for (int i = 0; i < cFlags; i++)
			{
				prgbFlag[iFlag + i] = chFlag;
			}
			iFlag += cFlags;
		}
	}
	if (iFlag != cPts)
		return false;

	// load x coordinates
	iFlag = 0;
	while (iFlag < cPts)
	{
		if (prgbFlag[iFlag] & SimpleGlyph::XShort)
		{
			prgnX[iFlag] = *pbGlyph;
			if (!(prgbFlag[iFlag] & SimpleGlyph::XIsPos))
			{
				prgnX[iFlag] = -prgnX[iFlag];
			}
			pbGlyph++;
		}
		else
		{
			if (prgbFlag[iFlag] & SimpleGlyph::XIsSame)
			{
				prgnX[iFlag] = 0;
				// do NOT increment pbGlyph
			}
			else
			{
				prgnX[iFlag] = read(*(const int16 *)pbGlyph);
				pbGlyph += sizeof(int16);
			}
		}
		iFlag++;
	}
		
	// load y coordinates
	iFlag = 0;
	while (iFlag < cPts)
	{
		if (prgbFlag[iFlag] & SimpleGlyph::YShort)
		{
			prgnY[iFlag] = *pbGlyph;
			if (!(prgbFlag[iFlag] & SimpleGlyph::YIsPos))
			{
				prgnY[iFlag] = -prgnY[iFlag];
			}
			pbGlyph++;
		}
		else
		{
			if (prgbFlag[iFlag] & SimpleGlyph::YIsSame)
			{
				prgnY[iFlag] = 0;
				// do NOT increment pbGlyph
			}
			else
			{
				prgnY[iFlag] = read(*(const int16 *)pbGlyph);
				pbGlyph += sizeof(int16);
			}
		}
		iFlag++;
	}
		
	cnPoints = cPts;
	return true;
}

/*----------------------------------------------------------------------------------------------
	Fill prgnCompId with the component Glyph IDs from pSimpleGlyf.
	Client must allocate space before calling.
	pSimpleGlyf - assumed to point to a composite glyph
	cCompIdTotal - the number of elements in prgnCompId 
	cCompId  - the total number of Glyph IDs stored in prgnCompId
	Return true if successful, false otherwise
		False could indicate a non-composite glyph or the input array was not big enough
----------------------------------------------------------------------------------------------*/
bool GetComponentGlyphIds(const void * pSimpleGlyf, int * prgnCompId, 
		size_t cnCompIdTotal, size_t & cnCompId)
{
	using namespace Sfnt;
	
	if (GlyfContourCount(pSimpleGlyf) >= 0)
		return false;

	const Sfnt::SimpleGlyph * pGlyph = reinterpret_cast<const Sfnt::SimpleGlyph *>(pSimpleGlyf);
	// for a composite glyph, the special data begins here
	const uint8 * pbGlyph = reinterpret_cast<const uint8 *>(&pGlyph->end_pts_of_contours[0]);

	uint16 GlyphFlags;
	size_t iCurrentComp = 0;
	do 
	{
		GlyphFlags = read(*((const uint16 *)pbGlyph));
		pbGlyph += sizeof(uint16);
		prgnCompId[iCurrentComp++] = read(*((const uint16 *)pbGlyph));
		pbGlyph += sizeof(uint16);
		if (iCurrentComp >= cnCompIdTotal) 
			return false;
		int nOffset = 0;
		nOffset += GlyphFlags & CompoundGlyph::Arg1Arg2Words ? 4 : 2;
		nOffset += GlyphFlags & CompoundGlyph::HaveScale ? 2 : 0;
		nOffset += GlyphFlags & CompoundGlyph::HaveXAndYScale  ? 4 : 0;
		nOffset += GlyphFlags & CompoundGlyph::HaveTwoByTwo  ? 8 :  0;
		pbGlyph += nOffset;
	} while (GlyphFlags & CompoundGlyph::MoreComponents);

	cnCompId = iCurrentComp;

	return true;
}

/*----------------------------------------------------------------------------------------------
	Return info on how a component glyph is to be placed
	pSimpleGlyph - assumed to point to a composite glyph
	nCompId - glyph id for component of interest
	bOffset - if true, a & b are the x & y offsets for this component
			  if false, b is the point on this component that is attaching to point a on the
				preceding glyph
	Return true if successful, false otherwise
		False could indicate a non-composite glyph or that component wasn't found
----------------------------------------------------------------------------------------------*/
bool GetComponentPlacement(const void * pSimpleGlyf, int nCompId,
									bool fOffset, int & a, int & b)
{
	using namespace Sfnt;
	
	if (GlyfContourCount(pSimpleGlyf) >= 0)
		return false;

	const Sfnt::SimpleGlyph * pGlyph = reinterpret_cast<const Sfnt::SimpleGlyph *>(pSimpleGlyf);
	// for a composite glyph, the special data begins here
	const uint8 * pbGlyph = reinterpret_cast<const uint8 *>(&pGlyph->end_pts_of_contours[0]);

	uint16 GlyphFlags;
	do 
	{
		GlyphFlags = read(*((const uint16 *)pbGlyph));
		pbGlyph += sizeof(uint16);
		if (read(*((const uint16 *)pbGlyph)) == nCompId)
		{
			pbGlyph += sizeof(uint16); // skip over glyph id of component
			fOffset = (GlyphFlags & CompoundGlyph::ArgsAreXYValues) == CompoundGlyph::ArgsAreXYValues;

			if (GlyphFlags & CompoundGlyph::Arg1Arg2Words )
			{
				a = read(*(const int16 *)pbGlyph);
				pbGlyph += sizeof(int16);
				b = read(*(const int16 *)pbGlyph);
				pbGlyph += sizeof(int16);
			}
			else
			{ // args are signed bytes
				a = *pbGlyph++;
				b = *pbGlyph++;
			}
			return true;
		}
		pbGlyph += sizeof(uint16); // skip over glyph id of component
		int nOffset = 0;
		nOffset += GlyphFlags & CompoundGlyph::Arg1Arg2Words  ? 4 : 2;
		nOffset += GlyphFlags & CompoundGlyph::HaveScale ? 2 : 0;
		nOffset += GlyphFlags & CompoundGlyph::HaveXAndYScale  ? 4 : 0;
		nOffset += GlyphFlags & CompoundGlyph::HaveTwoByTwo  ? 8 :  0;
		pbGlyph += nOffset;
	} while (GlyphFlags & CompoundGlyph::MoreComponents);

	// didn't find requested component
	fOffset = true;
	a = 0;
	b = 0;
	return false;
}

/*----------------------------------------------------------------------------------------------
	Return info on how a component glyph is to be transformed
	pSimpleGlyph - assumed to point to a composite glyph
	nCompId - glyph id for component of interest
	flt11, flt11, flt11, flt11 - a 2x2 matrix giving the transform
	bTransOffset - whether to transform the offset from above method 
		The spec is unclear about the meaning of this flag
		Currently - initialize to true for MS rasterizer and false for Mac rasterizer, then
			on return it will indicate whether transform should apply to offset (MSDN CD 10/99)
	Return true if successful, false otherwise
		False could indicate a non-composite glyph or that component wasn't found
----------------------------------------------------------------------------------------------*/
bool GetComponentTransform(const void * pSimpleGlyf, int nCompId, 
									float & flt11, float & flt12, float & flt21, float & flt22, 
									bool & fTransOffset)
{
	using namespace Sfnt;
	
	if (GlyfContourCount(pSimpleGlyf) >= 0)
		return false;

	const Sfnt::SimpleGlyph * pGlyph = reinterpret_cast<const Sfnt::SimpleGlyph *>(pSimpleGlyf);
	// for a composite glyph, the special data begins here
	const uint8 * pbGlyph = reinterpret_cast<const uint8 *>(&pGlyph->end_pts_of_contours[0]);

	uint16 GlyphFlags;
	do 
	{
		GlyphFlags = read(*((const uint16 *)pbGlyph));
		pbGlyph += sizeof(uint16);
		if (read(*((const uint16 *)pbGlyph)) == nCompId)
		{
			pbGlyph += sizeof(uint16); // skip over glyph id of component
			pbGlyph += GlyphFlags & CompoundGlyph::Arg1Arg2Words  ? 4 : 2; // skip over placement data

			if (fTransOffset) // MS rasterizer
				fTransOffset = !(GlyphFlags & CompoundGlyph::UnscaledOffset); 
			else // Apple rasterizer
				fTransOffset = (GlyphFlags & CompoundGlyph::ScaledOffset) != 0;

			if (GlyphFlags & CompoundGlyph::HaveScale)
			{
				flt11 = fixed_to_float<14>(read(*(const uint16 *)pbGlyph));
				pbGlyph += sizeof(uint16);
				flt12 = 0;
				flt21 = 0;
				flt22 = flt11;
			}
			else if (GlyphFlags & CompoundGlyph::HaveXAndYScale)
			{
				flt11 = fixed_to_float<14>(read(*(const uint16 *)pbGlyph));
				pbGlyph += sizeof(uint16);
				flt12 = 0;
				flt21 = 0;
				flt22 = fixed_to_float<14>(read(*(const uint16 *)pbGlyph));
				pbGlyph += sizeof(uint16);
			}
			else if (GlyphFlags & CompoundGlyph::HaveTwoByTwo)
			{
				flt11 = fixed_to_float<14>(read(*(const uint16 *)pbGlyph));
				pbGlyph += sizeof(uint16);
				flt12 = fixed_to_float<14>(read(*(const uint16 *)pbGlyph));
				pbGlyph += sizeof(uint16);
				flt21 = fixed_to_float<14>(read(*(const uint16 *)pbGlyph));
				pbGlyph += sizeof(uint16);
				flt22 = fixed_to_float<14>(read(*(const uint16 *)pbGlyph));
				pbGlyph += sizeof(uint16);
			}
			else
			{ // identity transform
				flt11 = 1.0;
				flt12 = 0.0;
				flt21 = 0.0;
				flt22 = 1.0;
			}
			return true;
		}
		pbGlyph += sizeof(uint16); // skip over glyph id of component
		int nOffset = 0;
		nOffset += GlyphFlags & CompoundGlyph::Arg1Arg2Words  ? 4 : 2;
		nOffset += GlyphFlags & CompoundGlyph::HaveScale ? 2 : 0;
		nOffset += GlyphFlags & CompoundGlyph::HaveXAndYScale  ? 4 : 0;
		nOffset += GlyphFlags & CompoundGlyph::HaveTwoByTwo  ? 8 :  0;
		pbGlyph += nOffset;
	} while (GlyphFlags & CompoundGlyph::MoreComponents);

	// didn't find requested component
	fTransOffset = false;
	flt11 = 1;
	flt12 = 0;
	flt21 = 0;
	flt22 = 1;
	return false;
}

/*----------------------------------------------------------------------------------------------
	Return a pointer into the glyf table based on the given tables and Glyph ID
	Since this method doesn't check for spaces, it is good to call IsSpace before using it.
	Return NULL on error.
----------------------------------------------------------------------------------------------*/
void * GlyfLookup(gr::gid16 nGlyphId, const void * pGlyf, const void * pLoca, 
						   size_t lLocaSize, const void * pHead)
{
	// test for valid glyph id
	// CheckTable verifies the index_to_loc_format is valid
	
	const Sfnt::FontHeader * pTable 
		= reinterpret_cast<const Sfnt::FontHeader *>(pHead);

	if (read(pTable->index_to_loc_format) == Sfnt::FontHeader::ShortIndexLocFormat)
	{ // loca entries are two bytes (and have been divided by two)
		if (nGlyphId >= (lLocaSize >> 1) - 1) // don't allow nGlyphId to access sentinel
			throw std::out_of_range("glyph id out of range for font");
	}
	if (read(pTable->index_to_loc_format) == Sfnt::FontHeader::LongIndexLocFormat)
	{ // loca entries are four bytes
		if (nGlyphId >= (lLocaSize >> 2) - 1)
			throw std::out_of_range("glyph id out of range for font");
	}

	long lGlyfOffset = LocaLookup(nGlyphId, pLoca, lLocaSize, pHead);
	void * pSimpleGlyf = GlyfLookup(pGlyf, lGlyfOffset); // invalid loca offset returns null
	return pSimpleGlyf;
}

/*----------------------------------------------------------------------------------------------
	Determine if a particular Glyph ID has any data in the glyf table. If it is white space,
	there will be no glyf data, though there will be metric data in hmtx, etc.
----------------------------------------------------------------------------------------------*/
bool IsSpace(gr::gid16 nGlyphId, const void * pLoca, size_t lLocaSize, const void * pHead)
{
	size_t lGlyfOffset = LocaLookup(nGlyphId, pLoca, lLocaSize, pHead);
	
	// the +1 should always work because there is a sentinel value at the end of the loca table
	size_t lNextGlyfOffset = LocaLookup(nGlyphId + 1, pLoca, lLocaSize, pHead);

	return (lNextGlyfOffset - lGlyfOffset) == 0;
}

/*----------------------------------------------------------------------------------------------
	Determine if a particular Glyph ID is a multi-level composite.
	This is probably not needed since support for multi-level composites has been added.
----------------------------------------------------------------------------------------------*/
bool IsDeepComposite(gr::gid16 nGlyphId, const void * pGlyf, const void * pLoca, 
							 long lLocaSize, const void * pHead)
{
	if (IsSpace(nGlyphId, pLoca, lLocaSize, pHead)) {return false;}

	void * pSimpleGlyf = GlyfLookup(nGlyphId, pGlyf, pLoca, lLocaSize, pHead);
	if (pSimpleGlyf == NULL)
		return false; // no way to really indicate an error occured here

	if (GlyfContourCount(pSimpleGlyf) >= 0)
		return false;

	int rgnCompId[kMaxGlyphComponents]; // assumes only a limited number of glyph components
	size_t cCompIdTotal = kMaxGlyphComponents;
	size_t cCompId = 0;

	if (!GetComponentGlyphIds(pSimpleGlyf, rgnCompId, cCompIdTotal, cCompId))
		return false;

	for (size_t i = 0; i < cCompId; i++)
	{
		pSimpleGlyf = GlyfLookup(static_cast<gr::gid16>(rgnCompId[i]), 
										pGlyf, pLoca, lLocaSize, pHead);
		if (pSimpleGlyf == NULL) {return false;}

		if (GlyfContourCount(pSimpleGlyf) < 0)
			return true;
	}

	return false;
}

/*----------------------------------------------------------------------------------------------
	Get the bounding box coordinates based on the given tables and Glyph ID
	Handles both simple and composite glyphs.
	Return true if successful, false otherwise. On false, all point values will be INT_MIN
		False may indicate a white space glyph
----------------------------------------------------------------------------------------------*/
bool GlyfBox(gr::gid16  nGlyphId, const void * pGlyf, const void * pLoca, 
		size_t lLocaSize, const void * pHead, int & xMin, int & yMin, int & xMax, int & yMax)
{
	xMin = yMin = xMax = yMax = INT_MIN;

	if (IsSpace(nGlyphId, pLoca, lLocaSize, pHead)) {return false;}

	void * pSimpleGlyf = GlyfLookup(nGlyphId, pGlyf, pLoca, lLocaSize, pHead);
	if (pSimpleGlyf == NULL) {return false;}

	return GlyfBox(pSimpleGlyf, xMin, yMin, xMax, yMax);
}

/*----------------------------------------------------------------------------------------------
	Get the number of contours based on the given tables and Glyph ID.
	Handles both simple and composite glyphs.
	Return true if successful, false otherwise. On false, cnContours will be INT_MIN
		False may indicate a white space glyph (or component).
----------------------------------------------------------------------------------------------*/
bool GlyfContourCount(gr::gid16 nGlyphId, const void * pGlyf,
	const void * pLoca, size_t lLocaSize, const void * pHead, size_t & cnContours)
{
	cnContours = static_cast<size_t>(INT_MIN);

	if (IsSpace(nGlyphId, pLoca, lLocaSize, pHead)) {return false;}

	void * pSimpleGlyf = GlyfLookup(nGlyphId, pGlyf, pLoca, lLocaSize, pHead);
	if (pSimpleGlyf == NULL) {return false;}

	int cRtnContours = GlyfContourCount(pSimpleGlyf);
	if (cRtnContours >= 0)
	{
		cnContours = size_t(cRtnContours);
		return true;
	}
		
	//handle composite glyphs

	int rgnCompId[kMaxGlyphComponents]; // assumes only a limted number of glyph components
	size_t cCompIdTotal = kMaxGlyphComponents;
	size_t cCompId = 0;

	if (!GetComponentGlyphIds(pSimpleGlyf, rgnCompId, cCompIdTotal, cCompId))
		return false;

	cRtnContours = 0;
	int cTmp = 0;
	for (size_t i = 0; i < cCompId; i++)
	{
		if (IsSpace(static_cast<gr::gid16>(rgnCompId[i]), pLoca, lLocaSize, pHead)) {return false;}
		pSimpleGlyf = GlyfLookup(static_cast<gr::gid16>(rgnCompId[i]), pGlyf,
			pLoca, lLocaSize, pHead);
		if (pSimpleGlyf == 0) {return false;}
		if ((cTmp = GlyfContourCount(pSimpleGlyf)) < 0) 
		{
			size_t cNest = 0;
			if (!GlyfContourCount(static_cast<gr::gid16>(rgnCompId[i]), pGlyf, pLoca, lLocaSize, 
				pHead, 	cNest))
				return false;
			cTmp = (int) cNest;
		}
		cRtnContours += cTmp;
	}

	cnContours = size_t(cRtnContours);
	return true;
}

/*----------------------------------------------------------------------------------------------
	Get the point numbers for the end points of the glyph contours based on the given tables 
	and Glyph ID.
	Handles both simple and composite glyphs.
	cnPoints - count of contours from GlyfContourCount (same as number of end points)
	prgnContourEndPoints - should point to a buffer large enough to hold cnPoints integers
	Return true if successful, false otherwise. On false, all end points are INT_MIN
		False may indicate a white space glyph (or component).
----------------------------------------------------------------------------------------------*/
bool GlyfContourEndPoints(gr::gid16 nGlyphId, const void * pGlyf, const void * pLoca, 
	size_t lLocaSize, const void * pHead, 
	int * prgnContourEndPoint, size_t & cnPoints)
{
	std::fill_n(prgnContourEndPoint, cnPoints, INT_MIN);

	if (IsSpace(nGlyphId, pLoca, lLocaSize, pHead)) {return false;}

	void * pSimpleGlyf = GlyfLookup(nGlyphId, pGlyf, pLoca, lLocaSize, pHead);
	if (pSimpleGlyf == NULL) {return false;}

	int cContours = GlyfContourCount(pSimpleGlyf);
	int cActualPts = 0;
	if (cContours > 0)
		return GlyfContourEndPoints(pSimpleGlyf, prgnContourEndPoint, cnPoints, cActualPts);
	
	// handle composite glyphs
	
	int rgnCompId[kMaxGlyphComponents]; // assumes no glyph will be made of more than 8 components
	size_t cCompIdTotal = kMaxGlyphComponents;
	size_t cCompId = 0;

	if (!GetComponentGlyphIds(pSimpleGlyf, rgnCompId, cCompIdTotal, cCompId))
		return false;

	int * prgnCurrentEndPoint = prgnContourEndPoint;
	int cCurrentPoints = cnPoints;
	int nPrevPt = 0;
	for (size_t i = 0; i < cCompId; i++)
	{
		if (IsSpace(static_cast<gr::gid16>(rgnCompId[i]), pLoca, lLocaSize, pHead)) {return false;}
		pSimpleGlyf = GlyfLookup(static_cast<gr::gid16>(rgnCompId[i]), pGlyf, pLoca, lLocaSize, pHead);
		if (pSimpleGlyf == NULL) {return false;}

		if (!GlyfContourEndPoints(pSimpleGlyf, prgnCurrentEndPoint, cCurrentPoints, cActualPts))
		{
			size_t cNestedPts = ( size_t ) cCurrentPoints;
			if (!GlyfContourEndPoints(static_cast<gr::gid16>(rgnCompId[i]), pGlyf, pLoca, lLocaSize, 
										pHead, prgnCurrentEndPoint, cNestedPts))
				return false;
			cActualPts = cCurrentPoints - cNestedPts;
		} 
		// points in composite are numbered sequentially as components are added
		//  must adjust end point numbers for new point numbers
		for (int j = 0; j < cActualPts; j++)
			prgnCurrentEndPoint[j] += nPrevPt;
		nPrevPt = prgnCurrentEndPoint[cActualPts - 1] + 1;

		prgnCurrentEndPoint += cActualPts;
		cCurrentPoints -= cActualPts;
	}

	cnPoints = cCurrentPoints;
	return true;
}

/*----------------------------------------------------------------------------------------------
	Get the points for a glyph based on the given tables and Glyph ID
	Handles both simple and composite glyphs.
	cnPoints - count of points from largest end point obtained from GlyfContourEndPoints
	prgnX & prgnY - should point to buffers large enough to hold cnPoints integers
		The ranges are parallel so that coordinates for point(n) are found at offset n in 
		both ranges. These points are in absolute coordinates.
	prgfOnCurve - should point to a buffer a large enough to hold cnPoints bytes (bool)
		This range is parallel to the prgnX & prgnY
	Return true if successful, false otherwise. On false, all points may be INT_MIN
		False may indicate a white space glyph (or component), or a corrupt font
	// TODO: doesn't support composite glyphs whose components are themselves components
		It's not clear from the TTF spec when the transforms should be applied. Should the 
		transform be done before or after attachment point calcs? (current code - before) 
		Should the transform be applied to other offsets? (currently - no; however commented 
		out code is	in place so that if CompoundGlyph::UnscaledOffset on the MS rasterizer is 
		clear (typical) then yes, and if CompoundGlyph::ScaledOffset on the Apple rasterizer is 
		clear (typical?) then no). See GetComponentTransform.
		It's also unclear where point numbering with attachment poinst starts 
		(currently - first point number is relative to whole glyph, second point number is 
		relative to current glyph). 
----------------------------------------------------------------------------------------------*/
bool GlyfPoints(gr::gid16 nGlyphId, const void * pGlyf,
		const void * pLoca, size_t lLocaSize, const void * pHead,
		const int * prgnContourEndPoint, size_t cnEndPoints,
		int * prgnX, int * prgnY, bool * prgfOnCurve, size_t & cnPoints)
{
	std::fill_n(prgnX, cnPoints, INT_MAX);
	std::fill_n(prgnY, cnPoints, INT_MAX);

	if (IsSpace(nGlyphId, pLoca, lLocaSize, pHead)) 
		return false;

	void * pSimpleGlyf = GlyfLookup(nGlyphId, pGlyf, pLoca, lLocaSize, pHead);
	if (pSimpleGlyf == NULL)
		return false;

	int cContours = GlyfContourCount(pSimpleGlyf);
	int cActualPts;
	if (cContours > 0)
	{
		if (!GlyfPoints(pSimpleGlyf, prgnX, prgnY, (char *)prgfOnCurve, cnPoints, cActualPts))
			return false;
		CalcAbsolutePoints(prgnX, prgnY, cnPoints);
		SimplifyFlags((char *)prgfOnCurve, cnPoints);
		return true;
	}

	// handle composite glyphs	
	int rgnCompId[kMaxGlyphComponents]; // assumes no glyph will be made of more than 8 components
	size_t cCompIdTotal = kMaxGlyphComponents;
	size_t cCompId = 0;

	// this will fail if there are more components than there is room for
	if (!GetComponentGlyphIds(pSimpleGlyf, rgnCompId, cCompIdTotal, cCompId))
		return false;

	int * prgnCurrentX = prgnX;
	int * prgnCurrentY = prgnY;
	char * prgbCurrentFlag = (char *)prgfOnCurve; // converting bool to char should be safe
	int cCurrentPoints = cnPoints;
	bool fOffset = true, fTransOff = true;
	int a, b;
	float flt11, flt12, flt21, flt22;
	// int * prgnPrevX = prgnX; // in case first att pt number relative to preceding glyph
	// int * prgnPrevY = prgnY;
	for (size_t i = 0; i < cCompId; i++)
	{
		if (IsSpace(static_cast<gr::gid16>(rgnCompId[i]), pLoca, lLocaSize, pHead)) {return false;}
		void * pCompGlyf = GlyfLookup(static_cast<gr::gid16>(rgnCompId[i]), pGlyf,
			pLoca, lLocaSize, pHead);
		if (pCompGlyf == NULL) {return false;}

		if (!GlyfPoints(pCompGlyf, prgnCurrentX, prgnCurrentY, prgbCurrentFlag, 
			cCurrentPoints, cActualPts))
		{
			size_t cNestedPts = ( size_t ) cCurrentPoints;
			if (!GlyfPoints(static_cast<gr::gid16>(rgnCompId[i]), pGlyf, pLoca, lLocaSize, pHead, 
				prgnContourEndPoint, cnEndPoints, prgnCurrentX, prgnCurrentY, (bool *)prgbCurrentFlag, 
				cNestedPts))
			{
				return false;
			}
			cActualPts = cCurrentPoints - cNestedPts;
		} 
		else
		{
			// convert points to absolute coordinates
			// do before transform and attachment point placement are applied
			CalcAbsolutePoints(prgnCurrentX, prgnCurrentY, cActualPts);
		}

		if (!GetComponentPlacement(pSimpleGlyf, rgnCompId[i], fOffset, a, b))
			return false;
		if (!GetComponentTransform(pSimpleGlyf, rgnCompId[i], 
			flt11, flt12, flt21, flt22, fTransOff))
			return false;
		bool fIdTrans = flt11 == 1.0 && flt12 == 0.0 && flt21 == 0.0 && flt22 == 1.0;

		// apply transform - see main method note above
		// do before attachment point calcs
		if (!fIdTrans)
			for (int j = 0; j < cActualPts; j++)
			{
				int x = prgnCurrentX[j]; // store before transform applied
				int y = prgnCurrentY[j];
				prgnCurrentX[j] = (int)(x * flt11 + y * flt12);
				prgnCurrentY[j] = (int)(x * flt21 + y * flt22);
			}
			
		// apply placement - see main method note above
		int nXOff, nYOff;
		if (fOffset) // explicit x & y offsets
		{ 
			/* ignore fTransOff for now
			if (fTransOff && !fIdTrans) 
			{ 	// transform x & y offsets
				nXOff = (int)(a * flt11 + b * flt12);
				nYOff = (int)(a * flt21 + b * flt22);
			}
			else */ 
			{ // don't transform offset
				nXOff = a;
				nYOff = b;
			}
		}
		else  // attachment points
		{	// in case first point is relative to preceding glyph and second relative to current
			// nXOff = prgnPrevX[a] - prgnCurrentX[b];
			// nYOff = prgnPrevY[a] - prgnCurrentY[b];
			// first point number relative to whole composite, second relative to current glyph
			nXOff = prgnX[a] - prgnCurrentX[b];
			nYOff = prgnY[a] - prgnCurrentY[b];
		}
		for (int j = 0; j < cActualPts; j++)
		{
			prgnCurrentX[j] += nXOff;
			prgnCurrentY[j] += nYOff;
		}

		// prgnPrevX = prgnCurrentX;
		// prgnPrevY = prgnCurrentY;
		prgnCurrentX += cActualPts;
		prgnCurrentY += cActualPts;
		prgbCurrentFlag += cActualPts;
		cCurrentPoints -= cActualPts;
	}

	SimplifyFlags((char *)prgfOnCurve, cnPoints);

	cnPoints = cCurrentPoints;
	return true;
}

/*----------------------------------------------------------------------------------------------
	Simplify the meaning of flags to just indicate whether point is on-curve or off-curve.
---------------------------------------------------------------------------------------------*/
bool SimplifyFlags(char * prgbFlags, int cnPoints)
{
	for (int i = 0; i < cnPoints; i++)
		prgbFlags[i] = static_cast<char>(prgbFlags[i] & Sfnt::SimpleGlyph::OnCurve);
	return true;
}

/*----------------------------------------------------------------------------------------------
	Convert relative point coordinates to absolute coordinates
	Points are stored in the font such that they are offsets from one another except for the 
		first point of a glyph.
---------------------------------------------------------------------------------------------*/
bool CalcAbsolutePoints(int * prgnX, int * prgnY, int cnPoints)
{
	int nX = prgnX[0];
	int nY = prgnY[0];
	for (int i = 1; i < cnPoints; i++)
	{
		prgnX[i] += nX;
		nX = prgnX[i];
		prgnY[i] += nY;
		nY = prgnY[i];
	}

	return true;
}

/*----------------------------------------------------------------------------------------------
	Convert a numerical table ID to a string containing the actual name of the table.
	Returns native order unsigned long.
---------------------------------------------------------------------------------------------*/
gr::fontTableId32 TableIdTag(const TableId tid)
{
	assert(sizeof(mapIdToTag) == sizeof(gr::fontTableId32) * ktiLast);
	assert(tid < ktiLast);

	return mapIdToTag[tid];
}

/*----------------------------------------------------------------------------------------------
	Return the length of the 'name' table in bytes.
	Currently used.
---------------------------------------------------------------------------------------------*/
#if 0
size_t NameTableLength(const gr::byte * pTable)
{
	gr::byte * pb = (const_cast<gr::byte *>(pTable)) + 2; // skip format
	size_t cRecords = *pb++ << 8; cRecords += *pb++;
	int dbStringOffset0 = (*pb++) << 8; dbStringOffset0 += *pb++;
	int dbMaxStringOffset = 0;
	for (size_t irec = 0; irec < cRecords; irec++)
	{
		int nPlatform = (*pb++) << 8; nPlatform += *pb++;
		int nEncoding = (*pb++) << 8; nEncoding += *pb++;
		int nLanguage = (*pb++) << 8; nLanguage += *pb++;
		int nName = (*pb++) << 8; nName += *pb++;
		int cbStringLen = (*pb++) << 8; cbStringLen += *pb++;
		int dbStringOffset = (*pb++) << 8; dbStringOffset += *pb++;
		if (dbMaxStringOffset < dbStringOffset + cbStringLen)
			dbMaxStringOffset = dbStringOffset + cbStringLen;
	}
	return dbStringOffset0 + dbMaxStringOffset;
}
#endif

} // end of namespace TtfUtil
