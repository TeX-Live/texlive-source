/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrCharStream.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	

----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GR_CHARSTREAM_INCLUDED
#define GR_CHARSTREAM_INCLUDED

//:End Ignore

namespace gr
{

/*----------------------------------------------------------------------------------------------
	Makes a stream out of a text string that is serving as input to the process.
	
	Hungarian: chstrm
----------------------------------------------------------------------------------------------*/
class GrCharStream
{
public:
	//	Constructor and destructor:
	GrCharStream(ITextSource * pgts, int ichrMin, int ichrLim,
		bool fStartLine, bool fEndLine);

	~GrCharStream()
	{
		if (m_cchlRunMax >= 0)
			delete[] m_prgchlRunText;
	}

	//	Getters:
	ITextSource * TextSrc()		{ return m_pgts; }
	int Min()					{ return m_ichrMin; }
	int Pos()					{ return m_ichrPos; }
	int Lim()					{ return m_ichrLim; }
	bool StartLine()			{ return m_fStartLine; }
	bool EndLine()				{ return m_fEndLine; }

	//	offset from the beginning of the segment
	int SegOffset()				{ return m_ichrPos - m_ichrMin; }
	int SegOffset(int ichr)		{ return ichr - m_ichrMin; }

	bool AtEnd()
	{
		return (m_ichrPos >= m_ichrLim);
	}

	//	The input string is empty.
	bool IsEmpty()
	{
		return (m_ichrLim - m_ichrMin == 0);
	}

	//	When we are restarting in order to create a segment other than the first,
	//	back up the position in the stream slightly in order to reprocess a few characters
	//	before the line-break.
	void Backup(int cchrToReprocess)
	{
		Assert(m_ichrPos >= cchrToReprocess);
		m_ichrPos -= cchrToReprocess;
		m_ichrRunOffset = kPosInfinity;
		m_cchrBackedUp = cchrToReprocess;
	}

	//	When we found a hard-break in the input stream, we need to put it back, and since
	//	we're done, we set the end of the stream to that position.
	void HitHardBreak()
	{
		m_ichrPos -= 1;
		m_ichrRunOffset -= 1;
		m_ichrLim = m_ichrPos;
	}

	void Restart();

	int NextGet(GrTableManager *, GrFeatureValues * pfval, int * ichrSegOffset, int * pcchr);

	void CurrentFeatures(GrTableManager * ptman, GrFeatureValues * pfval);

	//	For transduction logging:
	int GetLogData(GrTableManager * ptman, int * rgnChars, bool * rgfNewRun,
		GrFeatureValues * rgfval, int cchwBackup,
		int * pcchwMaxRaw);
	void GetLogDataRaw(GrTableManager * ptman, int cchw, int cchwBackup,
		int cchwMax16bit, int * prgnChars,
		utf16 * prgchw2, utf16 * prgchw3, utf16 * prgchw4, utf16 * prgchw5, utf16 * prgchw6, 
		int * prgichwRaw);

protected:
	void SetUpFeatureValues(GrTableManager * ptman, int ichr);
public:
	static utf32 Utf8ToUtf32(utf8 * prgchs8bit, int cchs, int * pcchsUsed);
	static utf32 Utf16ToUtf32(utf16 * prgchw16bit, int cchw, int * pcchwUsed);
	bool AtUnicodeCharBoundary(int cchr)
	{
		return AtUnicodeCharBoundary(m_pgts, cchr);
	}
	static bool AtUnicodeCharBoundary(ITextSource * pgts, int ichr);
	static bool AtUnicodeCharBoundary(utf8 * prgchs, int cchs, int ichs, UtfType utf);
	static bool AtUnicodeCharBoundary(utf16 * prgchw, int cchw, int ichr, UtfType utf);
	static bool FromSurrogatePair(utf16 chwIn1, utf16 chwIn2, unsigned int * pch32Out);
	static long DecodeUtf8(const utf8 * rgchUtf8, int cchUtf8, int * pcbOut);

protected:
	//	Instance variables:
	//	Hungarian note: chr = raw characters, chl = long characters (UTF-32)
	//		chs = short characters (UTF-8), chw = wide characters (UTF-16)
	ITextSource * m_pgts;	// string to render
	UtfType m_utf;	// what encoding form the text-source uses
	int m_ichrMin;	// official start of segment relative to beginning of string
	int m_ichrLim;	// end of stream (potential end of seg) relative to beginning of string
	int m_ichrPos;	// stream position (0 = start of string)
	bool m_fStartLine;	// true if a line-break character should be prepended
	bool m_fEndLine;	// true if a line-break character should be appended
	int m_cchrBackedUp;	// number of characters backed up before beginning of segment

	// We read a run's worth of data at a time and cache it in the following variables:
	int	m_cchlRunMax;	// size of buffer allocated
	int * m_prgchlRunText;	// buffer containing current run
	int m_ichrRunMin;		// start of run relative to beginning of string
	int m_ichrRunLim;		// end of run relative to beginning of string
	int m_ichlRunOffset;	// index into m_prgchlRunText; kPosInfinity if nothing set up
	int m_ichrRunOffset;	// corresponding index into text source
	GrFeatureValues m_fvalRunFeats;

	std::vector<int> m_vislotNextChunkMap; // maps from 16-bit chars to 32-bit chars & glyphs

	enum
	{
		kzUtf8Mask1 = 0x7F,
		kzUtf8Mask2 = 0x1F,
		kzUtf8Mask3 = 0x0F,
		kzUtf8Mask4 = 0x07,
		kzUtf8Mask5 = 0x03,
		kzUtf8Mask6 = 0x01
	};

	enum
	{
		kzUtf8Flag1 = 0x00,
		kzUtf8Flag2 = 0xC0,
		kzUtf8Flag3 = 0xE0,
		kzUtf8Flag4 = 0xF0,
		kzUtf8Flag5 = 0xF8,
		kzUtf8Flag6 = 0xFC
	};

	enum
	{
		kzByteMask = 0x3F,
		kzByteMark = 0x80,
		kzUtf8ByteShift = 6,
		kzUnicodeMax = 0x7FFFFFFF
	};

	enum
	{
		kzUtf16Shift	 = 10,
		kzUtf16Inc       = 0x2400,
		kzUtf16HighFirst = 0xD800,
		kzUtf16HighLast  = 0xDBFF,
		kzUtf16LowFirst  = 0xDC00,
		kzUtf16LowLast   = 0xDFFF
	};
};

} // namespace gr

#endif // !GR_CHARSTREAM_INCLUDED

