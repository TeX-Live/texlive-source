/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999 - 2008 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: FileFont.cpp
Responsibility: Keith Stribley
Last reviewed: Not yet.

Description:
		A Font is an object that represents a font-family + bold + italic setting, that contains
	Graphite tables.
----------------------------------------------------------------------------------------------*/

#include "Main.h"

#include "FontTableCache.h"
#include "FileFont.h"
#include <stdio.h>


// TBD do this properly
#define FUDGE_FACTOR 72
#define fix26_6(x) (x >> 6) + (x & 32 ? (x > 0 ? 1 : 0) : (x < 0 ? -1 : 0))

namespace gr
{


FileFont::FileFont()
	: Font(),
	m_pfile(NULL),
	m_pTableCache(NULL),
	m_mAscent(0),
	m_mDescent(0),
	m_mEmSquare(0),
	m_pointSize(0),
	m_dpiX(FUDGE_FACTOR),
	m_dpiY(FUDGE_FACTOR),
	m_fIsValid(false),
	m_pHeader(NULL),
	m_pTableDir(NULL),
	m_xScale(1.0f),
	m_yScale(1.0f)
{
	m_fItalic = false;
	m_fBold = false;
	// colors are not used
	m_clrFore = (unsigned long)kclrBlack;
	m_clrBack = (unsigned long)kclrTransparent;
}

FileFont::FileFont(FILE * pfile, float pointSize, 
				   unsigned int dpiX, unsigned int dpiY)
	: Font(),
	m_pfile(pfile),
	m_pTableCache(NULL),
	m_mAscent(0),
	m_mDescent(0),
	m_mEmSquare(0),
	m_pointSize(pointSize),
	m_dpiX(dpiX),
	m_dpiY(dpiY),
	m_fIsValid(false),
	m_pHeader(NULL),
	m_pTableDir(NULL),
	m_xScale(1.0f),
	m_yScale(1.0f)
{
	Assert(m_pfile); // shouldn't be null
	initializeFromFace();
	
}

FileFont::FileFont(char * fileName, float pointSize, 
				   unsigned int dpiX, unsigned int dpiY)
	: Font(),
	m_pfile(NULL),
	m_pTableCache(NULL),
	m_mAscent(0),
	m_mDescent(0),
	m_mEmSquare(0),
	m_pointSize(pointSize),
	m_dpiX(dpiX),
	m_dpiY(dpiY),
	m_fIsValid(false),
	m_pHeader(NULL),
	m_pTableDir(NULL),
	m_xScale(1.0f),
	m_yScale(1.0f)
{
	Assert(fileName); // shouldn't be null but we play safe anyway
	m_pfile = fopen(fileName, "rb");

	initializeFromFace();
}

FileFont::FileFont(std::string fileName, float pointSize, 
				   unsigned int dpiX, unsigned int dpiY)
	: Font(),
	m_pfile(NULL),
	m_pTableCache(NULL),
	m_mAscent(0),
	m_mDescent(0),
	m_mEmSquare(0),
	m_pointSize(pointSize),
	m_dpiX(dpiX),
	m_dpiY(dpiY),
	m_fIsValid(false),
	m_pHeader(NULL),
	m_pTableDir(NULL),
	m_xScale(1.0f),
	m_yScale(1.0f)
{
	Assert(fileName.length()); // shouldn't be null but we play safe anyway
	m_pfile = fopen(fileName.c_str(), "rb");

	initializeFromFace();
}


void
FileFont::initializeFromFace()
{
	if (m_dpiY == 0) m_dpiY = m_dpiX;
	m_fItalic = false;
	m_fBold = false;
	// colors are not used
	m_clrFore = (unsigned long)kclrBlack;
	m_clrBack = (unsigned long)kclrTransparent;

	if (m_pfile)
	{
		size_t lOffset, lSize;
		TtfUtil::GetHeaderInfo(lOffset, lSize);
		m_pHeader = new byte [lSize];
		m_fIsValid = true;
		if (!m_pHeader) 
		{
			m_fIsValid = false;
			return;
		}
		m_fIsValid = (fseek(m_pfile, lOffset, SEEK_SET) == 0);
		size_t bytesRead = fread(m_pHeader, 1, lSize, m_pfile);
		Assert(bytesRead == lSize);
		m_fIsValid = TtfUtil::CheckHeader(m_pHeader);

		if (!m_fIsValid) return;
		m_fIsValid = TtfUtil::GetTableDirInfo(m_pHeader, lOffset, lSize);
		m_pTableDir = new byte[lSize];
		if (!m_pTableDir) 
		{
			m_fIsValid = false;
			return;
		}
		// if lOffset hasn't changed this isn't needed
		fseek(m_pfile, lOffset, SEEK_SET);
		bytesRead = fread(m_pTableDir, 1, lSize, m_pfile);
		Assert(bytesRead == lSize);
		
		// now read head tables
		m_fIsValid = TtfUtil::GetTableInfo(ktiOs2, m_pHeader, m_pTableDir, 
										  lOffset, lSize);
		if (!m_fIsValid) 
		{
			return;
		}
		byte * pTable = readTable(ktiOs2, lSize);
		// get ascent, descent, style while we have the OS2 table loaded
		if (!m_fIsValid || !pTable) 
		{
			return;
		}
		m_fIsValid = TtfUtil::FontOs2Style(pTable, m_fBold, m_fItalic);
		m_mAscent = static_cast<float>(TtfUtil::FontAscent(pTable));
		m_mDescent = static_cast<float>(TtfUtil::FontDescent(pTable));
		
		pTable = readTable(ktiName, lSize);
		if (!m_fIsValid || !pTable) 
		{
			return;
		}
		if (!TtfUtil::Get31EngFamilyInfo(pTable, lOffset, lSize))
		{
			// not English name
			m_fIsValid = false;
			return;
		}
		Assert(lSize %2 == 0);// should be utf16
		utf16 rgchwFace[128];
		int cchw = (lSize / isizeof(utf16)) + 1;
		cchw = min(cchw, 128);
		utf16 * pTable16 = reinterpret_cast<utf16*>(pTable + lOffset);
		std::copy(pTable16, pTable16 + cchw - 1, rgchwFace);
		rgchwFace[cchw - 1] = 0;  // zero terminate
		TtfUtil::SwapWString(rgchwFace, cchw - 1);
// We could use something like "if (sizeof(std::wstring::value_type) == 4)" here,
// but a compile-time switch is preferable.
#if SIZEOF_WCHAR_T == 4
			for (int cch16 = 0; cch16 < cchw; )
			{
				int cch16Used = 0;
				utf32 ch32 = GrCharStream::Utf16ToUtf32(&(rgchwFace[cch16]), 
														cchw - cch16, &cch16Used);
				m_stuFaceName.push_back(ch32);
				cch16 += cch16Used;
			}
//		}
#else
		// m_stuFaceName.assign(rgchwFace);
		// VS 2005, MinGW32, and AIX need this:
		for (int cch16 = 0; cch16 < cchw; cch16++)
			m_stuFaceName.push_back(rgchwFace[cch16]);
#endif
		pTable = readTable(ktiHead, lSize);
		if (!m_fIsValid || !pTable) 
		{
			return;
		}
		m_mEmSquare = static_cast<float>(TtfUtil::DesignUnits(pTable));
		// can now set the scale
		m_xScale = scaleFromDpi(m_dpiX);
		m_yScale = scaleFromDpi(m_dpiY);
	}
	else
	{
		m_fIsValid = false;
	}
}

byte * 
FileFont::readTable(int /*TableId*/ tid, size_t & size)
{
	const TableId tableId = TableId(tid);
	bool isValid = true;
	size_t lOffset = 0, lSize = 0;
	if (!m_pTableCache)
	{
		m_pTableCache = new FontTableCache();		
	}
	if (!m_pTableCache) return NULL;
	
	byte * pTable = m_pTableCache->getTable(tableId);
	size = m_pTableCache->getTableSize(tableId);
	// check whether it is already in the cache
	if (pTable) return pTable;
	isValid &= TtfUtil::GetTableInfo(tableId, m_pHeader, m_pTableDir, 
			lOffset, lSize);
	if (!isValid) 
		return NULL;
	fseek(m_pfile, lOffset, SEEK_SET);
	// only allocate if needed
	pTable = m_pTableCache->allocateTable(tableId, lSize);
	
	if (!pTable) 
	{
		isValid = false;
		return NULL;
	}
	size_t bytesRead = fread(pTable, 1, lSize, m_pfile);
	isValid = bytesRead == lSize;
	if (isValid)
	{
		isValid &= TtfUtil::CheckTable(tableId, pTable, lSize);
	}
	if (!isValid) 
	{
		return 0;
	}
	size = lSize;
	return pTable;
}


FileFont::~FileFont()
{
	
	// if this is the last copy of the font sharing these tables
	// delete them
	if (m_pTableCache)
	{
		m_pTableCache->decrementFontCount();
		if (m_pTableCache->getFontCount() == 0)
		{
			delete [] m_pHeader;
			delete [] m_pTableDir;
			delete m_pTableCache;
			m_pTableCache = NULL;
			if (m_pfile)
				fclose(m_pfile);
		}
	}
	else
	{
		delete [] m_pHeader;
		delete [] m_pTableDir;
		if (m_pfile)
			fclose(m_pfile);
	}
	// note the DecFontCount(); is done in the Font base class
}
	
Font * FileFont::copyThis()
{
	FileFont * copy = new FileFont(*this);
	return copy;
}

/**
* A copy constructor that allows a change of point size or dpi
* The underlying table cache will be shared between the fonts, so
* this should have a low overhead.
*/
FileFont::FileFont(const FileFont & font, float pointSize, 
				   unsigned int dpiX, unsigned int dpiY)
: Font(font),
	m_pfile(font.m_pfile),
	m_mAscent(font.m_mAscent),
	m_mDescent(font.m_mDescent),
	m_mEmSquare(font.m_mEmSquare),
	m_pointSize(font.m_pointSize),
	m_dpiX(font.m_dpiX),
	m_dpiY(font.m_dpiY),
	m_fIsValid(font.m_fIsValid),
	m_pHeader(font.m_pHeader),
	m_pTableDir(font.m_pTableDir),
	m_xScale(font.m_xScale),
	m_yScale(font.m_yScale)
{
	if (pointSize > 0)
	{
		m_pointSize = pointSize;
		if (dpiX > 0)
		{
			m_dpiX = dpiX;
			if (dpiY > 0) m_dpiY = dpiY;
			else dpiY = dpiX;
		}
		m_xScale = scaleFromDpi(m_dpiX);
		m_yScale = scaleFromDpi(m_dpiY);
	}
	m_fItalic = font.m_fItalic;
	m_fBold = font.m_fBold;
	// colors are not used
	m_clrFore = font.m_clrFore;
	m_clrBack = font.m_clrBack;
	m_stuFaceName.assign(font.m_stuFaceName);
	
	m_pTableCache = font.m_pTableCache;
	// use the same table cache between instances
	if (m_pTableCache)
		m_pTableCache->incrementFontCount();
	
	
	// I dont' see why we need to reget the face, but WinFont does
	//m_pfface = FontFace::GetFontFace(this, m_fpropDef.szFaceName,
	//					m_fpropDef.fBold, m_fpropDef.fItalic);
}



	//virtual FontErrorCode isValidForGraphite(int * pnVersion = NULL, int * pnSubVersion = NULL);

/*----------------------------------------------------------------------------------------------
	Read a table from the font.
----------------------------------------------------------------------------------------------*/	
const void * 
FileFont::getTable(fontTableId32 tableID, size_t * pcbSize)
{
	*pcbSize = 0;
	// use a cache to reduce the number of times tables have to be reloaded
	if (m_pTableCache == NULL)
	{
		// constructor automatically sets the font count to 1
		m_pTableCache = new FontTableCache();
	} 
	TableId tid;
	for (int i = 0; i<ktiLast; i++)
	{
		tid = static_cast<TableId>(i);
		if (tableID == TtfUtil::TableIdTag(tid))
		{
			if (m_pTableCache->getTable(tid))
			{
				*pcbSize = m_pTableCache->getTableSize(tid);
				return m_pTableCache->getTable(tid);
			}
			break;	
		}
	}
	Assert(tid < ktiLast);
	size_t tableSize = 0;
	void * pTable = readTable(tid, tableSize);
	*pcbSize = static_cast<int>(tableSize);
	return pTable;
}


void FileFont::getFontMetrics(float * pAscent, float * pDescent,
		float * pEmSquare)
{
		if (pEmSquare) *pEmSquare = m_mEmSquare * m_xScale;
		if (pAscent) *pAscent = m_mAscent * m_yScale;
		if (pDescent) *pDescent = m_mDescent * m_yScale;
}


bool FileFont::FontHasGraphiteTables(FILE * file)
{
	FileFont testFont(file, 1.0f, FUDGE_FACTOR);
	return testFont.fontHasGraphiteTables();
}

bool FileFont::FontHasGraphiteTables(char * fileName)
{
	FileFont testFont(fileName, 1.0f, FUDGE_FACTOR);
	return testFont.fontHasGraphiteTables();
}

bool FileFont::fontHasGraphiteTables()
{
	size_t tableSize;
	bool isGraphiteFont = m_fIsValid;
	isGraphiteFont &= (readTable(ktiSilf, tableSize) != NULL);
	return isGraphiteFont;
}

} // namespace gr
