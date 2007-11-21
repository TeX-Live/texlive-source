/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2004 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: TestCase.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    File to set up test cases for the Graphite regression test program.

How to add a test:
	1.	Add the name of your test method to the class declaration in TestCase.h.
	2.	Increment the g_numberOfTests constant in this file.
	3.	Add a call to your test method in the SetupTests method in this file.
	4.	Copy one of the existing test methods such as SetupSimpleTest and change the name
		and data.

Things that still need testing:
	Ligatures
	Justification
	Fake italic
-------------------------------------------------------------------------------*//*:End Ignore*/

#include "main.h"

//:>********************************************************************************************
//:>	Test constants and methods
//:>********************************************************************************************

const int g_numberOfTests = 17;	// *** increment as tests are added ***

TestCase * g_ptcaseList;		// list of test cases

namespace gr {                  // and it was SC who got rid of the byte defn in GrPlatform.h!
typedef unsigned char byte;
}


/*----------------------------------------------------------------------------------------------
	Create the list of tests.
----------------------------------------------------------------------------------------------*/
int TestCase::SetupTests(TestCase ** pptcaseList)
{
	g_ptcaseList = new TestCase[g_numberOfTests];

	//	The number of methods called here should equal g_numberOfTests above.
	g_ptcaseList[0].SetupSimpleTest();
	g_ptcaseList[1].SetupSimpleBacktrackTest();
	g_ptcaseList[2].SetupBurmese1();
	g_ptcaseList[3].SetupBurmese2();
	g_ptcaseList[4].SetupBurmese3();
	g_ptcaseList[5].SetupBurmese4();
	g_ptcaseList[6].SetupRoman();
	g_ptcaseList[7].SetupRomanFeatures();
	g_ptcaseList[8].SetupNoWhiteSpace();
	g_ptcaseList[9].SetupNoWhiteSpaceFailure();
	g_ptcaseList[10].SetupOnlyWhiteSpace();
	g_ptcaseList[11].SetupCrossLine1();
	g_ptcaseList[12].SetupCrossLine2();
	g_ptcaseList[13].SetupCrossLine3();
	g_ptcaseList[14].SetupCrossLine4();
	g_ptcaseList[15].SetupArabic1();
	g_ptcaseList[16].SetupArabic2();
	// *** Add more method calls here. ***

	*pptcaseList = g_ptcaseList;

	return g_numberOfTests;
}

/*----------------------------------------------------------------------------------------------
	Set up a simple test.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupSimpleTest()
{
	m_testName = "Simple";
	m_debug = false;
	m_traceLog = false;

	//	Input:
	m_fontName = L"Graphite Test Roman";
	m_fontFile = "grtest_roman.ttf";
	m_text = L"This is a test.";	// text to render
	m_fontSize = 12;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 500;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;

	//	Output:
	m_segWidth = 86;			// physical width of segment

	const int charCnt = 15;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true, true, true, true,
		true, true,	true, true, true
	};

	const int glyphCnt = 15;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{55, 77, 78, 90,  3, 78, 90,  3, 68,  3, 91, 73, 90, 91, 17};
	int xPositions[] =  { 0,  9, 17, 22, 28, 33, 37, 43, 48, 55, 60, 64, 71, 77, 82};
	int yPositions[] =  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
	int advWidths[] =   { 9,  8,  4,  6,  4,  4,  6,  4,  7,  4,  4,  7,  6,  4,  4};

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left,
	const int clickTestCnt = 3;
	int clickStuff[] = {
		11, 25,    1, false,   0, 24,  8,   kAbsent, kAbsent, kAbsent,
		42,  5,    7, true,    0, 24, 42,   kAbsent, kAbsent, kAbsent,
		90, 40,   15, true,    0, 24, 85,   kAbsent, kAbsent, kAbsent
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
}

/*----------------------------------------------------------------------------------------------
	Set up a simple test with backtracking.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupSimpleBacktrackTest()
{
	m_testName = "Simple Backtrack";

	//	Input:
	m_fontName = L"Graphite Test Roman";
	m_fontFile = "grtest_roman.ttf";
	m_text = L"This is a test.";	// text to render
	m_fontSize = 12;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 500;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = true;

	//	Output:
	m_segWidth = 55;			// physical width of segment

	const int charCnt = 10;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true, true, true, true
	};

	const int glyphCnt = 10;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{55, 77, 78, 90,  3, 78, 90,  3, 68,  3};
	int xPositions[] =  { 0,  9, 17, 22, 28, 33, 37, 43, 48, 55};
	int yPositions[] =  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
	int advWidths[] =   { 9,  8,  4,  6,  4,  4,  6,  4,  7,  4};

	const int contextBlockOutSize = 10;
	gr::byte contextBlockOut[] = { 15, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

/*----------------------------------------------------------------------------------------------
	A set of tests using Burmese, to test complex positioning and non-white-space
	linebreaking.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupBurmese1()
{
	m_testName = "Burmese 1";
	m_debug = false;
	m_traceLog = false;

	//	Input:
	m_fontName = L"Graphite Test Burmese";
	m_fontFile = "grtest_burmese.ttf";
	m_text = BurmeseText();
	m_fontSize = 20;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 300;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_backtrack = false;

	//	Output:
	m_segWidth = 281;			// physical width of segment

	SetupBurmeseAux(
		64,		// character count
		45,		// glyph count
		5);		// number of click-tests

	const int contextBlockOutSize = 11;
	gr::byte contextBlockOut[] = { 20, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupBurmese2()
{
	m_testName = "Burmese 2";

	//	Input:
	m_fontName = L"Graphite Test Burmese";
	m_fontFile = "grtest_burmese.ttf";

	m_text = BurmeseText();
	m_fontSize = 20;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 275;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_backtrack = false;

	//	Output:
	m_segWidth = 215;			// physical width of segment

	SetupBurmeseAux(
		47,		// character count
		33,		// glyph count
		3);		// number of click-tests

	const int contextBlockOutSize = 11;
	gr::byte contextBlockOut[] = { 20, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupBurmese3()
{
	m_testName = "Burmese 3";
	//m_debug = true;

	//	Input:
	m_fontName = L"Graphite Test Burmese";
	m_fontFile = "grtest_burmese.ttf";

	m_text = BurmeseText();
	m_fontSize = 20;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 75;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_backtrack = false;

	//	Output:
	m_segWidth = 73;			// physical width of segment

	SetupBurmeseAux(
		19,		// character count
		13,		// glyph count
		2);		// number of click-tests

	const int contextBlockOutSize = 11;
	gr::byte contextBlockOut[] = { 20, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupBurmese4()
{
	m_testName = "Burmese 4";
	//m_traceLog = true;
	//m_debug = true;

	//	Input:
	m_fontName = L"Graphite Test Burmese";
	m_fontFile = "grtest_burmese.ttf";

	m_text = BurmeseText();
	m_fontSize = 20;				// font size in points
	m_prefBreak = klbWsBreak;		// preferred break-weight
	m_worstBreak = klbHyphenBreak;	// worst-case break-weight
	m_availWidth = 30;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_backtrack = false;

	//	Output:
	m_noSegment = true;
	m_segWidth = 0;			// physical width of segment

	SetupBurmeseAux(
		0,		// character count
		0,		// glyph count
		0);		// number of click-tests
}

void TestCase::SetupBurmeseAux(int charCnt, int glyphCnt, int clickTestCnt)
{
	m_rtl = false;

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true,  true,  false, false, false, true,  false, false, false, false,	// 0 - 9
		true,  false, false, true,  false, false, false, true,  true,  true,	// 10 - 19
		true,  false, true,  true,  false, false, true,  false, false, false,	// 20 - 29
		false, true,  false, false, true,  false, false, false, true,  true,	// 30 - 39
		true,  true,  true,  true,  true,  true,  true,   true, true,  false,	// 40 - 49
		true,  true,  true,  false, false, false, true,  false, false, true,	// 50 - 59
		false, false, false, true
	};

	// need glyphCnt elements in these arrays:
	//                     0                                      10                                      20                                      30                                      40
	gid16 glyphList[] =	{105,174,158,202,231,162,231,148,223,219,229,248,  3,226,162,173,216,231,177,195,115,231,170,204,243,  3,197,216,172,216,  3,233,  3,226,179,162,216,170,204,158,202,115,231,229,  3};
	int xPositions[] =  {  0, 17, 20, 22, 28, 29, 38, 40, 50, 55, 56, 57, 73, 79, 89, 99,102,109,110,111,120,129,130,131,140,143,149,167,174,191,198,205,215,222,232,234,245,252,253,262,264,271,280,278,281};
	int yPositions[] =  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
	int advWidths[] =   { 17,  2,  9,  4,  0, 10,  0, 17,  2,  0,  0, 15,  6, 10, 10,  2,  7,  0, 10,  9,  9,  0, 10,  7,  2,  6, 17,  7, 17,  7,  6, 10,  6, 10,  2, 10,  7, 10,  7,  9,  4,  9,  0,  0,  6};

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left,
	int clickStuff[] = {
		 10, 25,    1, true,    0, 25, 16,   kAbsent, kAbsent, kAbsent,
		 40,  5,   13, false,   0, 25, 39,   kAbsent, kAbsent, kAbsent,
		 93, 40,   19, false,   0, 25, 88,   0, 25, 77,
		251,  5,   52, true,    0, 25,251,   kAbsent, kAbsent, kAbsent,
		235, 30,   48, false,  -1, 27,230,   7, 19,243,
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
}

std::wstring TestCase::BurmeseText()
{
	std::wstring strRet;
	wchar_t charData[] = {
		0x1000, 0x1039, 0x101a, 0x1039, 0x101d, 0x1014, 0x1039, 0x101f, 0x1039, 0x200c,
		0x1015, 0x1039, 0x200c, 0x1010, 0x102f, 0x102d, 0x1037, 0x104f, 0x0020, 0x1015,
		0x1039, 0x101a, 0x1031, 0x102c, 0x1039, 0x200c, 0x101b, 0x1039, 0x101d, 0x1039,
		0x101f, 0x1004, 0x1039, 0x200c, 0x1019, 0x1039, 0x101f, 0x102f, 0x104a, 0x0020,
		0x101e, 0x102c, 0x101a, 0x102c, 0x0020, 0x1040, 0x0020, 0x1015, 0x1039, 0x101b,
		0x1031, 0x102c, 0x1019, 0x1039, 0x101f, 0x102f, 0x1014, 0x1039, 0x101f, 0x1004,
		0x1039, 0x200c, 0x1037, 0x0020, 0x1021, 0x1031, 0x102c, 0x1004, 0x1039, 0x200c,
		0x1019, 0x1039, 0x101b, 0x1004, 0x1039, 0x200c, 0x1019, 0x1039, 0x101f, 0x102f,
		0x1010, 0x102f, 0x102d, 0x1037, 0x101e, 0x100a, 0x1039, 0x200c, 0x0020, 0x1000,
		0x1039, 0x101a, 0x1039, 0x101d, 0x1014, 0x102f, 0x1039, 0x200c, 0x1015, 0x1039,
		0x200c, 0x1010, 0x102f, 0x102d, 0x1037, 0x104f, 0x0000
	};
	strRet.assign(charData);
	return strRet;
}

/*----------------------------------------------------------------------------------------------
	A set of tests using Roman script, which tests stacking diacritics, many-to-one glyphs,
	and features.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupRoman()
{
	m_testName = "Roman";
	//m_traceLog = true;
	//m_debug = true;

	//	Input:
	m_fontName = L"Graphite Test Roman";
	m_fontFile = "grtest_roman.ttf";
	m_text = RomanText();			// text to render
	m_fontSize = 36;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 500;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;

	//	Output:
	m_segWidth = 236;			// physical width of segment

	const int charCnt = 23;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true,  false, false, true,  true, true,  true, false,  false, true,
		true,  true,  false, false, true, false, true, false,  true,  false,
		false, false, false
	};

	const int glyphCnt = 21;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	//                    0                                       10                                      20
	gid16 glyphList[] =	{73,752,749, 87,833, 90, 75,2181,296, 68,751,750, 79,752, 44,752, 84,806,749,759,753};
	int xPositions[] =  { 0, 23, 23, 21, 41, 45, 64,  88,116,148,170,170,169,188,183,203,199,229,229,229,229};
	int yPositions[] =  { 0,  0, 10,  0, -6,  0,  0,   0,  0,  0,  0, 10,  0,  0,  0, 10,  0,  0,  0, 10, 20};
	int advWidths[] =   {21,  0,  0, 24,  0, 18, 24,  28, 31, 21,  0,  0, 13,  0, 15,  0, 37,  0,  0,  0,  0};

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left,
	const int clickTestCnt = 3;
	int clickStuff[] = {
		10, 15,    3, true,    0, 72, 20,   kAbsent, kAbsent, kAbsent,
		61, 50,    6, true,    0, 72, 63,   kAbsent, kAbsent, kAbsent,
		90, 40,    9, false,   0, 72, 87,   kAbsent, kAbsent, kAbsent
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
}

void TestCase::SetupRomanFeatures()
{
	m_testName = "Roman Features";
	//m_traceLog = true;
	//m_debug = true;

	//	Input:
	m_fontName = L"Graphite Test Roman";
	m_fontFile = "grtest_roman.ttf";
	m_text = RomanText();			// text to render
	m_fontSize = 36;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 500;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;

	m_fset[0].id = 1024;	m_fset[0].value = 2;	// capital eng with tail
	m_fset[1].id = 1026;	m_fset[1].value = 1;	// tone numbers
	m_fset[2].id = 1029;	m_fset[2].value = 1;	// vietnamese diacritics
	m_fset[3].id = 1032;	m_fset[3].value = 1;	// literacy alternates
	m_fset[4].id = 1034;	m_fset[4].value = 1;	// y-hook  alternate (default)
	m_fset[5].id = 0;

	//	Output:
	m_segWidth = 265;			// physical width of segment

	const int charCnt = 23;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true,  false, false, true,  true, true,  true, true,   true,  true,
		true,  true,  false, false, true, false, true, false,  true,  false,
		false, false, false
	};

	const int glyphCnt = 22;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	//                    0                                       10                                       20
	gid16 glyphList[] =	{73,752,749, 87,833, 90, 76,1581,128,122,298, 69,1964, 79,752, 44,752, 84,806,749,759,753};
	int xPositions[] =  { 0, 23, 23, 21, 41, 45, 64,  87,104,121,138,173, 174,198,217,211,231,227,258,258,258,258};
	int yPositions[] =  { 0,  0, 10,  0, -6,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,  0,  0,  0, 10, 20};
	int advWidths[] =   {21,  0,  0, 24,  0, 18, 23,  17, 17, 17, 34, 24,  24, 13,  0, 15,  0, 37,  0,  0,  0,  0};

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left,
	const int clickTestCnt = 3;
	int clickStuff[] = {
		 10, 15,    3, true,    0, 72,  20,   kAbsent, kAbsent, kAbsent,
		116,  5,    9, true,    0, 72, 120,   kAbsent, kAbsent, kAbsent,
		 90, 40,    7, false,   0, 72,  86,   kAbsent, kAbsent, kAbsent
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
}

std::wstring TestCase::RomanText()
{
	std::wstring strRet;
	wchar_t charData[] = {
		0x0065, 0x0303, 0x0300, 0x0070, 0x0361, 0x0073, 0x0067, 0x02e8, 0x02e5, 0x02e7,
		0x014a, 0x0061, 0x0302, 0x0301, 0x0069, 0x0303, 0x0049, 0x0303, 0x006d, 0x033c,
		0x0300, 0x0308, 0x0304, 0x0000
	};
	strRet.assign(charData);
	return strRet;
}

/*----------------------------------------------------------------------------------------------
	A set of tests for handling trailing whitespace.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupNoWhiteSpace()
{
	m_testName = "No white space";

	//	Input:
	m_fontName = L"Graphite Test Roman";
	m_fontFile = "grtest_roman.ttf";
	m_text = L"The  quick brown   fox.";	// text to render
	m_fontSize = 12;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 150;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_twsh = ktwshNoWs;
	m_paraRtl = true;

	//	Output:
	m_segWidth = 115;			// physical width of segment

	const int charCnt = 16;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true, true, true, true,
		true, true,	true, true, true, true
	};

	const int glyphCnt = 16;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{55, 77, 73,  3,  3, 88, 92, 78, 71, 82,  3, 70, 89, 86, 94, 85};
	int xPositions[] =  { 0,  9, 17, 24, 29, 34, 42, 50, 54, 61, 69, 74, 82, 87, 95,107};
	int yPositions[] =  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
	int advWidths[] =   { 9,  8,  7,  4,  4,  8,  8,  4,  7,  8,  4,  8,  5,  8, 11,  8};

	const int contextBlockOutSize = 10;
	gr::byte contextBlockOut[] = { 15, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
	SetClickTests(0, NULL);
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupNoWhiteSpaceFailure()
{
	m_testName = "No white space - failure";

	//	Input:
	m_fontName = L"Graphite Test Roman";
	m_fontFile = "grtest_roman.ttf";
	m_text = L"The quick brown   fox.";	// text to render
	m_firstChar = 15;				// spaces after brown
	m_fontSize = 12;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 2;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_twsh = ktwshNoWs;
	m_paraRtl = true;

	//	Output:
	m_noSegment = true;
	m_segWidth = 0;			// physical width of segment

	const int charCnt = 0;	// number of characters in the segment

	const int glyphCnt = 0;	// number of glyphs in the segment

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
}

void TestCase::SetupOnlyWhiteSpace()
{
	m_testName = "Only white space";

	//	Input:
	m_fontName = L"Graphite Test Roman";
	m_fontFile = "grtest_roman.ttf";
	m_text = L"   fox.";	// text to render
	m_fontSize = 12;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 2;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_twsh = ktwshOnlyWs;
	m_paraRtl = true;

	//	Output:
	m_segWidth = 0;			// physical width of segment - visible

	const int charCnt = 3;	// number of characters in the segment
	// need charCnt elements in this array:
	bool insPtFlags[] = { true, true, true };

	const int glyphCnt = 3;	// number of glyphs in the segment
	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{  3,   3,  3 };
	int xPositions[] =  { -4,  -9,-13 };
	int yPositions[] =  {  0,   0,  0 };
	int advWidths[] =   {  4,   4,  4 };

	//	TODO: add click tests when the bug fix with upstream tr white space is integrated.
	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left,
	const int clickTestCnt = 6;
	int clickStuff[] = {
		 2,  25,    0, false,   0, 24,  -1,   kAbsent, kAbsent, kAbsent,
		-1,   5,    0, false,   0, 24,  -1,   kAbsent, kAbsent, kAbsent,
		-6,  -3,    1, false,   0, 24,  -5,   kAbsent, kAbsent, kAbsent,
		-8,  -3,    2, true,    0, 24, -10,   kAbsent, kAbsent, kAbsent,
	   -13,  40,    3, true,    0, 24, -14,   kAbsent, kAbsent, kAbsent,
	   -18,   0,    3, true,    0, 24, -14,   kAbsent, kAbsent, kAbsent
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
}

/*----------------------------------------------------------------------------------------------
	A set of tests of cross-line contextualization.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupCrossLine1()
{
	m_testName = "Cross-line 1";

	//	Input:
	m_fontName = L"Graphite Test CrossLine";
	m_fontFile = "grtest_xline.ttf";
	m_text = CrossLineText();	// "abcddddefx@ghijkmmmmn$yopppppqrsss$z@tttwwww";
	m_fontSize = 30;				// font size in points
	m_prefBreak = klbHyphenBreak;	// preferred break-weight
	m_availWidth = 350;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;

	//	Output:
	m_segWidth = 312;			// physical width of segment

	const int charCnt = 10;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true, true, true, true
	};

	const int glyphCnt = 13;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{30, 67, 68, 69, 70, 70, 70, 70, 71, 72, 34, 90, 32 };
	int xPositions[] =  { 0, 40, 58, 79, 97,118,139,160,180,199,213,251,272 };
	int yPositions[] =  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };
	int advWidths[] =   {40, 18, 20, 18, 20, 20, 20, 20, 18, 13, 38, 20, 40 };

	//	TODO: add click tests
	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left,
	const int clickTestCnt = 0;
	int * clickStuff = NULL;
	//int clickStuff[] = ;
	//{
	//	10, 25,    1, false,   0, 24,  9,   kAbsent, kAbsent, kAbsent,
	//};

	const int contextBlockOutSize = 9;
	gr::byte contextBlockOut[] = { 20, 1, 0, 2, 0, 0, 4, 0, 0, 0 };

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupCrossLine2()
{
	m_testName = "Cross-line 2";
	//m_traceLog = true;
	//m_debug = true;

	//	Input:
	m_fontName = L"Graphite Test CrossLine";
	m_fontFile = "grtest_xline.ttf";
	m_text = CrossLineText();	// "abcddddefx@ghijkmmmmn$yopppppqrsss$z@tttwwww";
	m_firstChar = 10;
	m_fontSize = 30;				// font size in points
	m_prefBreak = klbHyphenBreak;	// preferred break-weight
	m_availWidth = 400;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_initWithPrev = true;

	const int contextBlockInSize = 9;	// output from Cross-line 1
	gr::byte contextBlockIn[] = { 20, 1, 0, 2, 0, 0, 4, 0, 0, 0 };

	//	Output:
	m_segWidth = 395;			// physical width of segment

	const int charCnt = 13;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		false, true, true, true, true, true, true, true, true, true, true, true, true
	};

	const int glyphCnt = 15;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{30, 34, 73, 74, 75, 76, 77, 79, 79, 79, 79, 80,  6, 91, 32 };
	int xPositions[] =  { 0, 40, 78, 99,119,131,143,163,196,228,260,293,314,334,355 };
	int yPositions[] =  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };
	int advWidths[] =   {40, 38, 20, 20, 11, 11, 20, 32, 32, 32, 32, 20, 20, 20, 40 };

	//	TODO: add click tests
	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left,
	const int clickTestCnt = 0;
	int * clickStuff = NULL;
	//int clickStuff[] = ;
	//{
	//	10, 25,    1, false,   0, 24,  9,   kAbsent, kAbsent, kAbsent,
	//};

	const int contextBlockOutSize = 9;
	gr::byte contextBlockOut[] = { 20, 1, 0, 3, 0, 0, 4, 0, 0, 0 };

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
	SetInputContextBlock(contextBlockInSize, contextBlockIn);
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupCrossLine3()
{
	m_testName = "Cross-line 3";

	//	Input:
	m_fontName = L"Graphite Test CrossLine";
	m_fontFile = "grtest_xline.ttf";
	m_text = CrossLineText();	// "abcddddefx@ghijkmmmmn$yopppppqrsss$z@tttwwww";
	m_firstChar = 23;
	m_fontSize = 30;				// font size in points
	m_prefBreak = klbHyphenBreak;	// preferred break-weight
	m_availWidth = 400;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_initWithPrev = true;

	const int contextBlockInSize = 9;	// output from Cross-line 2
	gr::byte contextBlockIn[] = { 20, 1, 0, 3, 0, 0, 4, 0, 0, 0 };

	//	Output:
	m_segWidth = 358;			// physical width of segment

	const int charCnt = 13;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true, true, true, true, true, false, true
	};

	const int glyphCnt = 16;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{30, 81,  6, 82, 82, 82, 82, 82, 83, 84, 85, 85, 85, 34,  8, 92 };
	int xPositions[] =  { 0, 40, 60, 81,102,123,144,164,185,206,220,236,253,269,307,339 };
	int yPositions[] =  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };
	int advWidths[] =   {40, 20, 20, 20, 20, 20, 20, 20, 20, 14, 16, 16, 16, 38, 32, 18 };

	//	TODO: add click tests
	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left,
	const int clickTestCnt = 0;
	int * clickStuff = NULL;
	//int clickStuff[] = ;
	//{
	//	10, 25,    1, false,   0, 24,  9,   kAbsent, kAbsent, kAbsent,
	//};

	const int contextBlockOutSize = 9;
	gr::byte contextBlockOut[] = { 20, 1, 0, 3, 0, 0, 4, 0, 0, 0 };

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
	SetInputContextBlock(contextBlockInSize, contextBlockIn);
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupCrossLine4()
{
	m_testName = "Cross-line 4";

	//	Input:
	m_fontName = L"Graphite Test CrossLine";
	m_fontFile = "grtest_xline.ttf";
	m_text = CrossLineText();	// "abcddddefx@ghijkmmmmn$yopppppqrsss$z@tttwwww";
	m_firstChar = 36;
	m_fontSize = 30;				// font size in points
	m_prefBreak = klbHyphenBreak;	// preferred break-weight
	m_availWidth = 400;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_initWithPrev = true;

	const int contextBlockInSize = 9;	// output from Cross-line 3
	gr::byte contextBlockIn[] = { 20, 1, 0, 3, 0, 0, 4, 0, 0, 0 };

	//	Output:
	m_segWidth = 248;			// physical width of segment

	const int charCnt = 8;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		false, true, true, true, true, true, true, true
	};

	const int glyphCnt = 10;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{ 8,  6, 86, 86, 86, 89, 89, 89, 89, 32 };
	int xPositions[] =  { 0, 32, 53, 64, 76, 87,117,147,177,208 };
	int yPositions[] =  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };
	int advWidths[] =   {32, 20, 11, 11, 11, 30, 30, 30, 30, 40 };

	//	TODO: add click tests
	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left,
	const int clickTestCnt = 0;
	int * clickStuff = NULL;
	//int clickStuff[] = ;
	//{
	//	10, 25,    1, false,   0, 24,  9,   kAbsent, kAbsent, kAbsent,
	//};

	const int contextBlockOutSize = 0;
	gr::byte * contextBlockOut = NULL;

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
	SetInputContextBlock(contextBlockInSize, contextBlockIn);
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

std::wstring TestCase::CrossLineText()
{
	// The equivalent data is in the "xlineTest.wpx" file.
	std::wstring strRet;
	strRet.assign(L"abcddddefx@ghijkmmmmn$yopppppqrsss$z@tttwwww");
	return strRet;
}

/*----------------------------------------------------------------------------------------------
	A set of tests using Arabic: RTL, bidi, and embedded direction codes.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupArabic1()
{
	m_testName = "Arabic 1";
	m_debug = true;
	m_traceLog = false;

	//	Input:
	m_fontName = L"Graphite Test Arabic";
	m_fontFile = "grtest_arabic.ttf";
	m_text = ArabicText();
	m_fontSize = 20;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 1000;			// width available for segment
	m_bold = false;
	m_italic = false;
	m_backtrack = false;

	//	Output:
	m_segWidth = 409;			// physical width of segment

	SetupArabicAux(
		61,		// character count
		61);	// glyph count

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left,
	int clickStuff[] = {
		199, 13,    37, true,    0, 35,196,   kAbsent, kAbsent, kAbsent,
		396, 13,     3, false,   8, 26,399,   10, 17, 397,
		396,  8,     6, true,    3, 11,391,    9, 22, 394,
		222,  5,    32, true,    0, 35,225,   kAbsent, kAbsent, kAbsent,
		217,  5,    31, false,   0, 35,215,    0, 35, 246,
	};
	SetClickTests(4, clickStuff);
}

void TestCase::SetupArabic2()
{
	m_testName = "Arabic 2";
	m_debug = false;
	m_traceLog = false;

	//	Input:
	m_fontName = L"Graphite Test Arabic";
	m_fontFile = "grtest_arabic.ttf";
	m_text = ArabicText();
	m_fontSize = 20;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 230;				// width available for segment-break after number
	m_bold = false;
	m_italic = false;
	m_backtrack = false;

	//	Output:
	m_segWidth = 193;			// physical width of segment

	SetupArabicAux(
		35,		// character count
		35);	// glyph count

	// The x-positions are different for a shorter segment:
	//                     0                                      10                                      20                                      30
	int xPositions[] =  {188,183,184,177,178,178,170,169,162,150,154,154,153,155,139,132,127,127,115,118,108,102, 84, 77, 77, 72, 65, 46, 46, 37, 31, 20, 10, 0, -6};
	SetXPositions(xPositions);
}

void TestCase::SetupArabicAux(int charCnt, int glyphCnt)
{
	m_rtl = true;

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true, true, true, true,	// 0 - 9
		true, true, true, true, true, true, true, true, true, true,	// 10 - 19
		true, true, true, true, true, true, true, true, true, true,	// 20 - 29
		true, true, true, true, true, true, true, true, true, true,	// 30 - 39
		true, true, true, true, true, true, true, true, true, true,	// 40 - 49
		true, true, true, true, true, true, true, true, true, true,	// 50 - 59
		true
	};

	// need glyphCnt elements in these arrays:
	//                     0                                        10                                      20                                               30                                                40                                                50                                                60
	gid16 glyphList[] =	{785,658,907,1182,913,907,1192,907,  3,811,914,909,911,934,592,  3,785,909,621,911, 12,  3,321,  3,236,659,731,555,925,961,  3,992,991,990,  3,821,924,712,474,882,527,  3,411,924,950,  3,236,990,991,992,993,995,236,  3,821,769,455,290,839,620,961};
	int xPositions[] =  {404,400,401, 393,394,394, 386,385,379,366,370,370,370,371,355,348,343,343,331,334,324,318,300,293,293,289,281,263,262,254,247,237,226,216,209,197,197,189,179,172,154,147,135,139,127,120,120,110, 99, 89, 79, 68, 68, 62, 50, 41, 36, 29, 19,  8,  0};
	int yPositions[] =  {  0,  0, -5,   0,  0,  4,   0,  0,  0,  0, -5, -3, -2,  4,  0,  0,  0,  1,  0, -3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
	int advWidths[] =   {  4,  4,  0,   6,  0,  0,   6,  0,  6, 12,  0,  0,  0,  0, 11,  6,  4,  0, 11,  0,  7,  6, 17,  6,  0,  4,  7, 18,  0,  8,  6, 10, 10, 10,  6, 11,  0,  7,  9,  7, 17,  6, 11,  0,  8,  6,  0, 10, 10, 10, 10, 10,  0,  6, 11,  9,  4,  6,  9, 11,  8};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetInsPtFlags(insPtFlags);
}

std::wstring TestCase::ArabicText()
{
	std::wstring strRet;
	wchar_t charData[] = {
		0x0628, 0x0628, 0x064e, 0x0644, 0x064e, 0x0654, 0x0627, 0x064e, 0x0020, 0x0686,
		0x0650, 0x0652, 0x0655, 0x06e0, 0x06a8, 0x0020, 0x0628, 0x0650, 0x06b9, 0x0652,
		0x0029, 0x0020, 0x0628, 0x0020, 0x200d, 0x062a, 0x06a8, 0x0633, 0x0670, 0x061b,
		0x0020, 0x06f1, 0x06f2, 0x06f3, 0x0020, 0x0633, 0x0670, 0x0639, 0x062f, 0x0645,
		0x067e, 0x0020, 0x0644, 0x0670, 0x060c, 0x0020, 0x202e, 0x06f1, 0x06f2, 0x06f3,
		0x06f4, 0x06f5, 0x202c, 0x0020, 0x0633, 0x0647, 0x0627, 0x0631, 0x0639, 0x0646,
		0x061b, 0x0000
	};
	strRet.assign(charData);
	return strRet;
}


// *** Add more methods here. ***


//:>********************************************************************************************
//:>	Utility methods.
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Delete the list of tests.
----------------------------------------------------------------------------------------------*/
void TestCase::DeleteTests()
{
	delete[] g_ptcaseList;
}

/*----------------------------------------------------------------------------------------------
	Constructor: initialize test case with default values.
----------------------------------------------------------------------------------------------*/
TestCase::TestCase()
{
	m_testName = "Unknown";
	m_debug = false;
	m_traceLog = false;

	m_fontName.erase();
	m_fontFile.erase();
	m_text.erase();					// text to render
	m_fontSize = 12;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_worstBreak = klbClipBreak;	// worst-case break-weight
	m_availWidth = 500;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_backtrack = false;
	m_twsh = ktwshAll;
	m_paraRtl = false;
	m_firstChar = 0;
	m_contextBlockInSize = 0;
	m_contextBlockIn = NULL;
	m_initWithPrev = false;
	memset(m_fset, 0, MAXFEAT * sizeof(FeatureSetting));

	m_noSegment = false;			// yes, a segment should be generated
	m_charCount = 0;
	m_glyphCount = 0;
	m_glyphArray = NULL;
	m_xPositions = NULL;
	m_yPositions = NULL;
	m_advWidths = NULL;
	m_insPointFlags = NULL;
	m_contextBlockOutSize = 0;
	m_contextBlockOut = NULL;

	m_clickTestCount = 0;
	m_clickTests = NULL;
}

/*----------------------------------------------------------------------------------------------
	Destructor.
----------------------------------------------------------------------------------------------*/
TestCase::~TestCase()
{
	delete[] m_glyphArray;
	delete[] m_xPositions;
	delete[] m_yPositions;
	delete[] m_advWidths;
	delete[] m_insPointFlags;
	delete[] m_clickTests;
	delete[] m_contextBlockIn;
	delete[] m_contextBlockOut;
}

/*----------------------------------------------------------------------------------------------
	Setters.
----------------------------------------------------------------------------------------------*/
void TestCase::SetCharCount(int charCount)
{
	m_charCount = charCount;
	m_insPointFlags = new bool[charCount];
}

void TestCase::SetGlyphCount(int glyphCount)
{
	m_glyphCount = glyphCount;
	m_glyphArray = new gid16[glyphCount];
	m_xPositions = new int[glyphCount];
	m_yPositions = new int[glyphCount];
	m_advWidths = new int[glyphCount];
}

void TestCase::SetGlyphList(gid16 * glyphList)
{
	for (int i = 0; i < m_glyphCount; i++)
		m_glyphArray[i] = glyphList[i];
}

void TestCase::SetXPositions(int * xPosList)
{
	for (int i = 0; i < m_glyphCount; i++)
		m_xPositions[i] = xPosList[i];
}

void TestCase::SetYPositions(int * yPosList)
{
	for (int i = 0; i < m_glyphCount; i++)
		m_yPositions[i] = yPosList[i];
}

void TestCase::SetAdvWidths(int * advWidths)
{
	for (int i = 0; i < m_glyphCount; i++)
		m_advWidths[i] = advWidths[i];
}

void TestCase::SetInsPtFlags(bool * flags)
{
	for (int i = 0; i < m_charCount; i++)
		m_insPointFlags[i] = flags[i];
}

void TestCase::SetClickTests(int clickTestCount, int * clickStuff)
{
	const int fc = ClickTest::fieldCnt;

	m_clickTestCount = clickTestCount;
	m_clickTests = new ClickTest[clickTestCount];
	for (int i = 0; i < clickTestCount; i++)
	{
		m_clickTests[i].xClick		= clickStuff[(i * fc) + 0];
		m_clickTests[i].yClick		= clickStuff[(i * fc) + 1];
		m_clickTests[i].charIndex	= clickStuff[(i * fc) + 2];
		m_clickTests[i].assocPrev	= clickStuff[(i * fc) + 3];
		m_clickTests[i].sel1Top		= clickStuff[(i * fc) + 4];
		m_clickTests[i].sel1Bottom	= clickStuff[(i * fc) + 5];
		m_clickTests[i].sel1Left	= clickStuff[(i * fc) + 6];
		m_clickTests[i].sel2Top		= clickStuff[(i * fc) + 7];
		m_clickTests[i].sel2Bottom	= clickStuff[(i * fc) + 8];
		m_clickTests[i].sel2Left	= clickStuff[(i * fc) + 9];
	}
}

void TestCase::SetInputContextBlock(int contextBlockInSize, gr::byte * pContextBlockIn)
{
	m_contextBlockInSize = contextBlockInSize;
	if (contextBlockInSize == 0)
	{
		m_contextBlockIn = NULL;
	}
	else
	{
		m_contextBlockIn = new gr::byte[contextBlockInSize];
		std::copy(pContextBlockIn, pContextBlockIn + contextBlockInSize, m_contextBlockIn);
	}
}

void TestCase::SetOutputContextBlock(int contextBlockOutSize, gr::byte * pContextBlockOut)
{
	m_contextBlockOutSize = contextBlockOutSize;
	m_contextBlockOut = new gr::byte[contextBlockOutSize];
	std::copy(pContextBlockOut, pContextBlockOut + contextBlockOutSize, m_contextBlockOut);
}
