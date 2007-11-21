/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2004 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: TestCase.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Definition of TestCase class for Graphite regression test program.
-------------------------------------------------------------------------------*//*:End Ignore*/

#ifdef _MSC_VER
#pragma once
#endif
#ifndef TESTCASE_H
#define TESTCASE_H 1

#define NO_EXCEPTIONS 1

class TestCase
{
public:
	//	Methods to set up test cases.
	void SetupSimpleTest();
	void SetupSimpleBacktrackTest();
	void SetupBurmese1();
	void SetupBurmese2();
	void SetupBurmese3();
	void SetupBurmese4();
	void SetupRoman();
	void SetupRomanFeatures();
	void SetupNoWhiteSpace();
	void SetupNoWhiteSpaceFailure();
	void SetupOnlyWhiteSpace();
	void SetupCrossLine1();
	void SetupCrossLine2();
	void SetupCrossLine3();
	void SetupCrossLine4();
	void SetupArabic1();
	void SetupArabic2();
	//	*** Add more methods here. ***

protected:
	//	Auxiliary functions to set up test cases.
	void SetupBurmeseAux(int charCount, int glyphCount, int clickTestCount);
	void SetupArabicAux(int charCount, int glyphCount);
	std::wstring BurmeseText();
	std::wstring RomanText();
	std::wstring CrossLineText();
	std::wstring ArabicText();

public:
	const static int kAbsent = -100;	// not present in data

public:
	//	constructor:
	TestCase();
	//	destructor:
	~TestCase();

	static int SetupTests(TestCase **);
	static void DeleteTests();

	//	Getters:
	std::string TestName()			{ return m_testName; }
	bool RunDebugger()				{ return m_debug; }
	bool TraceLog()					{ return m_traceLog; }

	std::wstring Text()				{ return m_text; }
	std::wstring FontName()			{ return m_fontName; }
	std::string FontFile()			{ return m_fontFile; }
	size_t FontSize()				{ return m_fontSize; }
	LineBrk PrefBreak()				{ return m_prefBreak; }
	LineBrk WorstBreak()			{ return m_worstBreak; }
	int FeatureID(int i)			{ return m_fset[i].id; }
	int FeatureValue(int i)			{ return m_fset[i].value; }
	FeatureSetting * Features()		{ return m_fset; }
	int AvailWidth()				{ return m_availWidth; }
	bool Backtrack()				{ return m_backtrack; }
	TrWsHandling Twsh()				{ return m_twsh; }
	bool Rtl()						{ return m_rtl; }
	bool ParaRtl()					{ return m_paraRtl; }
	size_t FirstChar()				{ return m_firstChar; }
	size_t InputContextBlock(gr::byte ** ppContextBlock)
	{
		*ppContextBlock = m_contextBlockIn;
		return m_contextBlockInSize;
	}

	bool InitWithPrevSeg()			{ return m_initWithPrev; }
	bool NoSegment()				{ return m_noSegment; }
	int SegWidth()					{ return m_segWidth; }
	int CharCount()					{ return m_charCount; }
	int GlyphCount()				{ return m_glyphCount; }
	int GlyphID(int i)				{ return m_glyphArray[i]; }
	int XPos(int i)					{ return m_xPositions[i]; }
	int YPos(int i)					{ return m_yPositions[i]; }
	int AdvWidth(int i)				{ return m_advWidths[i]; }
	int InsPtFlag(int i)			{ return m_insPointFlags[i]; }

	int NumberOfClickTests()		{ return m_clickTestCount; }
	int XClick(int i)				{ return m_clickTests[i].xClick; }
	int YClick(int i)				{ return m_clickTests[i].yClick; }
	int ClickCharIndex(int i)		{ return m_clickTests[i].charIndex; }
	bool ClickAssocPrev(int i)		{ return (bool)m_clickTests[i].assocPrev; }
	int Sel1Top(int i)				{ return m_clickTests[i].sel1Top; }
	int Sel1Bottom(int i)			{ return m_clickTests[i].sel1Bottom; }
	int Sel1Left(int i)				{ return m_clickTests[i].sel1Left; }
	int Sel2Top(int i)				{ return m_clickTests[i].sel2Top; }
	int Sel2Bottom(int i)			{ return m_clickTests[i].sel2Bottom; }
	int Sel2Left(int i)				{ return m_clickTests[i].sel2Left; }
	size_t OutputContextBlockSize()	{ return m_contextBlockOutSize; }
	bool CompareContextBlock(gr::byte * pResult)
	{
		for (size_t i = 0; i < m_contextBlockOutSize; i++)
		{
			if (m_contextBlockOut[i] != pResult[i])
				return false;
		}
		return true;
	}

	struct ClickTest
	{
		int xClick;
		int yClick;
		int charIndex;
		int assocPrev;	// boolean: 0 or 1
		int sel1Top;
		int sel1Bottom;
		int sel1Left;	// we only need left or right, not both
		int sel2Top;
		int sel2Bottom;
		int sel2Left;

		const static int fieldCnt = 10;
	};

protected:
	std::string m_testName;
	bool m_debug;		// break into the debugger when running this test
	bool m_traceLog;	// generate a logging file (tracelog.txt) for this test; if this is turned on for
						// more than one test, the tests will be appended

#define MAXFEAT 10

	//	Input parameters:
	std::wstring m_text;			// default: none
	std::wstring m_fontName;		// default: empty
	std::string m_fontFile;			// default: empty
	size_t m_fontSize;				// default: 12
	LineBrk m_prefBreak;			// default: word-break
	LineBrk m_worstBreak;			// default: clip-break
	FeatureSetting m_fset[MAXFEAT];	// default: none
	int m_availWidth;				// default: 100
	bool m_bold;					// default: false
	bool m_italic;					// default: false
	bool m_rtl;						// default: false
	bool m_backtrack;				// default: false
	TrWsHandling m_twsh;			// default: ktwshAll
	bool m_paraRtl;					// default: false
	size_t m_firstChar;				// default: 0
	size_t m_contextBlockInSize;	// default: 0
	gr::byte * m_contextBlockIn;	// default: NULL -- DELETE
	bool m_initWithPrev;			// default: false
	// start of line flag
	// resolution
	// justification

	//	Output:
	bool m_noSegment;	// no segment should be generated
	int m_segWidth;
	int m_charCount;
	int m_glyphCount;
	gid16 * m_glyphArray;
	int * m_xPositions;
	int * m_yPositions;
	int * m_advWidths;
	bool * m_insPointFlags;
	int m_clickTestCount;
	ClickTest * m_clickTests;
	size_t m_contextBlockOutSize;
	gr::byte * m_contextBlockOut;
	// glyphs-to-chars
	// chars-to-glyphs
	// pdichwContext
	// arrow key behavior
	// etc.

	//	Setters:
	void SetCharCount(int charCount);
	void SetGlyphCount(int glyphCount);
	void SetGlyphList(gid16 * glyphList);
	void SetXPositions(int * posList);
	void SetYPositions(int * posList);
	void SetAdvWidths(int * advWidths);
	void SetInsPtFlags(bool * flags);
	void SetClickTests(int clickTestCount, int * clickStuff);
	void SetInputContextBlock(int contextBlockInSize, gr::byte * contextBlockIn);
	void SetOutputContextBlock(int contextBlockOutSize, gr::byte * contextBlockIn);
};


#endif // !TESTCASE_H

