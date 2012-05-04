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
	Ligatures (there is one small test of this)
	Justification
	Fake italic
-------------------------------------------------------------------------------*//*:End Ignore*/

#include "main.h"
#include <cstring>

//:>********************************************************************************************
//:>	Test constants and methods
//:>********************************************************************************************

const int g_numberOfTests = 31;	// *** increment as tests are added ***

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
	int cptcase = 0;

	//	The number of methods called here should equal g_numberOfTests above.
	g_ptcaseList[cptcase].SetupSimpleTest();			cptcase++;
	g_ptcaseList[cptcase].SetupSimpleBacktrackTest();	cptcase++;
	g_ptcaseList[cptcase].SetupSurrogateTest();			cptcase++;
	g_ptcaseList[cptcase].SetupBurmese1();				cptcase++;
	g_ptcaseList[cptcase].SetupBurmese2();				cptcase++;
	g_ptcaseList[cptcase].SetupBurmese3();				cptcase++;
	g_ptcaseList[cptcase].SetupBurmese4();				cptcase++;
	g_ptcaseList[cptcase].SetupRoman();					cptcase++;
	g_ptcaseList[cptcase].SetupRomanFeatures();			cptcase++;
	g_ptcaseList[cptcase].SetupStackingAndBridging();	cptcase++;
	g_ptcaseList[cptcase].SetupNoWhiteSpace();			cptcase++;
	g_ptcaseList[cptcase].SetupNoWhiteSpaceNoSeg();		cptcase++;
	g_ptcaseList[cptcase].SetupOnlyWhiteSpace();		cptcase++;
	g_ptcaseList[cptcase].SetupCrossLine1();			cptcase++;
	g_ptcaseList[cptcase].SetupCrossLine2();			cptcase++;
	g_ptcaseList[cptcase].SetupCrossLine3();			cptcase++;
	g_ptcaseList[cptcase].SetupCrossLine4();			cptcase++;
	g_ptcaseList[cptcase].SetupArabic1();				cptcase++;
	g_ptcaseList[cptcase].SetupArabic2();				cptcase++;
	g_ptcaseList[cptcase].SetupTaiViet1();				cptcase++;
	g_ptcaseList[cptcase].SetupTaiViet2();				cptcase++;
	g_ptcaseList[cptcase].SetupBurmesePositioning1();	cptcase++;
	g_ptcaseList[cptcase].SetupBurmesePositioning2();	cptcase++;
	g_ptcaseList[cptcase].SetupBurmeseFeature1();		cptcase++;
	g_ptcaseList[cptcase].SetupBurmeseFeature2();		cptcase++;
	g_ptcaseList[cptcase].SetupCharisPConstraint();		cptcase++;
	g_ptcaseList[cptcase].SetupDevanagari();			cptcase++;
	g_ptcaseList[cptcase].SetupKernGlat2();				cptcase++;
	g_ptcaseList[cptcase].SetupDumbFallback1();			cptcase++;
	g_ptcaseList[cptcase].SetupDumbFallback2();			cptcase++;
	g_ptcaseList[cptcase].SetupBadFont();				cptcase++;

//	g_ptcaseList[cptcase].SetupBugTest();				cptcase++;
//	g_ptcaseList[cptcase].SetupBugTest_GentiumNfc();	cptcase++;
	// *** Add more method calls here. ***

	assert(cptcase == g_numberOfTests);

	*pptcaseList = g_ptcaseList;

	return g_numberOfTests;
}

/*----------------------------------------------------------------------------------------------
	Set up a simple test.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupSimpleTest()
{
	m_testName = "Simple";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	gid16 glyphList[] =	{55, 75, 76, 86,  3, 76, 86,  3, 68,  3, 87, 72, 86, 87, 17};
	int xPositions[] =  { 0,  9, 17, 22, 28, 33, 37, 43, 48, 55, 60, 64, 71, 77, 82};
	int yPositions[] =  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
	int advWidths[] =   { 9,  8,  4,  6,  4,  4,  6,  4,  7,  4,  4,  7,  6,  4,  4};

	int bbLefts[] =     { 0,  9, 18, 22, 28, 33, 38, 43, 48, 55, 60, 65, 72, 78, 83};
	int bbRights[] =    { 9, 17, 21, 27, 33, 37, 43, 48, 55, 60, 64, 71, 77, 82, 85};
	int bbTops[] =      {10, 11, 10,  7,  0, 10,  7,  0,  7,  0,  9,  7,  7,  9,  1};
	int bbBottoms[] =   { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};

	int charsToGlyphs[] = {
		1, 1, 1,	2, 1, 2,	3, 1, 3,	4, 1, 4,	5, 1, 5,	6, 1, 6,	7, 1, 7
	};
	int c2gCount = 21;

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
	const int clickTestCnt = 3;
	int clickStuff[] = {
		11, 25,    1, false,   0, 24,  8,   kAbsent, kAbsent, kAbsent,	// below baseline
		42,  5,    7, true,    0, 24, 42,   kAbsent, kAbsent, kAbsent,	// near top of text
		90, 16,   15, true,    0, 24, 85,   kAbsent, kAbsent, kAbsent	// near baseline
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
	SetCharsToGlyphs(charsToGlyphs, c2gCount);
	SetClickTests(clickTestCnt, clickStuff);
}

/*----------------------------------------------------------------------------------------------
	Set up a simple test with backtracking.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupSimpleBacktrackTest()
{
	m_testName = "Simple Backtrack";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	gid16 glyphList[] =	{55, 75, 76, 86,  3, 76, 86,  3, 68,  3};
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
	SetBBs(NULL, NULL, NULL, NULL);
	SetInsPtFlags(insPtFlags);
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

/*----------------------------------------------------------------------------------------------
	Set up a test that includes surrogates.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupSurrogateTest()
{
	m_testName = "Surrogates";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Input:
	m_fontName = L"Graphite Test Roman";
	m_fontFile = "grtest_roman.ttf";
	m_text = L"abXXcdYYe";			// text to render
	m_text[2] = 0xD835;
	m_text[3] = 0xDD13;
	m_text[6] = 0xD835;
	m_text[7] = 0xDD10;
	m_fontSize = 12;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 500;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;

	//	Output:
	m_segWidth = 65;			// physical width of segment

	const int charCnt = 9;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, false, true, true, true, false, true
	};

	const int glyphCnt = 7;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{68, 69,1227, 70, 71,1015, 72};
	int xPositions[] =  { 0,  7,  15, 27, 34,  42, 58};
	int yPositions[] =  { 0,  0,   0,  0,  0,   0,  0};
	int advWidths[] =   { 7,  8,  12,  7,  8,  15,  7};

	int bbLefts[] =     { 0,  7,  15, 28, 35,  43, 59};
	int bbRights[] =    { 7, 14,  26, 34, 42,  57, 65};
	int bbTops[] =      { 7, 11,  10,  7, 11,  10,  7};
	int bbBottoms[] =   { 0,  0,  -3,  0,  0,   0,  0};

	int charsToGlyphs[] = {
		0, 1, 0,	1, 1, 1,	2, 1, 2,	3, 0,	4, 1, 3,	5, 1, 4,	6, 1, 5,	7, 0,
		8, 1, 6,
	};
	int c2gCount = 25;

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
	const int clickTestCnt = 3;
	int clickStuff[] = {
		17, 25,   2, false,   0, 24, 14,   kAbsent, kAbsent, kAbsent,	// below baseline
		25,  5,   4, true,    0, 24, 26,   kAbsent, kAbsent, kAbsent,	// near top of text
		55, 16,   8, true,    0, 24, 57,   kAbsent, kAbsent, kAbsent	// near baseline
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
	SetCharsToGlyphs(charsToGlyphs, c2gCount);
	SetClickTests(clickTestCnt, clickStuff);
}

/*----------------------------------------------------------------------------------------------
	A set of tests using Burmese, to test complex positioning and non-white-space
	linebreaking.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupBurmese1()
{
	m_testName = "Burmese 1";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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

	// Each group = glyph-index, base, number of attached, glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,0,		1,1,0,	2,2,2,3,4,	3,2,0,		4,2,0,		5,5,1,6,	6,5,0,		7,7,3,8,9,10,
		8,7,0,		9,7,0,	10,7,0,		11,11,0,	12,12,0,	13,13,0,	14,14,0,	15,15,0,
		16,16,1,17,	17,16,0,	18,18,1,19,			19,18,0,	20,20,1,21,	21,20,0,	22,22,1,23,
		23,22,0,	24,24,0,	25,25,0
	};
	int attCount = sizeof(attachments) / sizeof(int);
	SetAttachedClusters(attachments, attCount);

	const int contextBlockOutSize = 11;
	gr::byte contextBlockOut[] = { 20, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupBurmese2()
{
	m_testName = "Burmese 2";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	//m_traceLog = true;
	//m_skip = true;

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
	//m_skip = true;

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
	int advWidths[] =   { 17,  2,  9,  4,  0, 10,  0, 17,  2,  0,  0, 15,  6, 10, 10,  2,  7,  0, 10,  9,  9,  0, 10,  7,  2,  6, 17,  7, 17,  7,  6, 10,  6, 10, 13, 10,  7, 10,  7,  9,  4,  9,  0,  0,  6};

	int bbLefts[] =     {  0, 10, 21, 23, 23, 30, 33, 41, 51, 49, 53, 58, 73, 80, 90, 96,100,103,111,109,121,123,131,132,141,143,150,165,175,189,198,206,215,223,233,235,243,253,254,263,265,272,275,275,281};
	int bbRights[] =    { 16, 19, 28, 25, 28, 39, 38, 56, 54, 55, 56, 72, 79, 88, 98,101,109,109,119,119,129,129,139,138,142,149,166,173,190,197,205,214,222,231,244,244,251,261,260,271,267,280,280,278,288};
	int bbTops[] =      {  7,  7,  7, -1, 15,  7, 15,  7, -1, 15, -2, 15,  0,  7,  7,  7,  7, 15,  7, -1,  7, 15,  7, -1,  7,  0,  7,  7,  7,  7,  0,  7,  0,  7, 16,  7,  7,  7, -1,  7, -1,  7, 15, -2,  0};
	int bbBottoms[] =   {  0, -7, -1, -7,  9,  0,  9,  0, -7,  9, -5,  0,  0,  0,  0, -7,  0,  9, -4, -7,  0,  9,  0, -7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -7,  0,  0,  0, -7, -1, -7,  0,  9, -5,  0};

	// Each group = char-index, number of glyphs, glyph-indices.
	int charsToGlyphs[] = {
		0, 1, 0,	1, 1, 1,	2, 1, 1,	3, 1, 1,	4, 1, 1,	5, 1, 2,	6, 1, 3,	7, 1, 3,
		8, 1, 4,	9, 1, 4,	10, 1, 5,	11, 1, 6,	12, 1, 6,	13, 1, 7
	};
	int c2gCount = sizeof(charsToGlyphs) / sizeof(int);

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
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
	SetCharsToGlyphs(charsToGlyphs, c2gCount);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
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
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	m_segWidth = 278;			// physical width of segment

	const int charCnt = 26;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true, true, true, true,
		true, true, true, true, true, true, true, true, true, true,
		true, true, true, true, true, true
	};

	const int glyphCnt = 24;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	//                    0                                             10                                             20
	gid16 glyphList[] =	{72,1815,1768, 83,1789, 86, 74,1943,1956,1926,1061, 68,1777,1755,805,1815, 44,1815,  80,1833,1768,1855,1838,637};
	int xPositions[] =  { 0,  23,  23, 21,  41, 45, 64,  88,  98, 112, 116,148, 171, 171,170, 188,183, 203, 199, 230, 230, 230, 230,236};
	int yPositions[] =  { 0,   0,  10,  0,  -6,  0,  0,   0,   0,   0,   0,  0,   0,  10,  0,   0,  0,  10,   0,   0,   0,  10,  20,  0};
	int advWidths[] =   {21,   0,   0, 24,   0, 18, 24,  13,  13,   4,  31, 21,   0,   0, 13,   0, 15,   0,  37,   0,   0,   0,   0, 41};

	// Each group = glyph-index, base, number of attached, glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,2,1,2,	1,0,0,		2,0,0,		3,3,0,			4,4,0,		5,5,0,		6,6,0,		7,7,2,8,9,
		8,7,0,		9,7,0,		10,10,0,	11,11,2,12,13,	12,11,0,	13,11,0,	14,14,1,15,	15,14,0,
		16,16,1,17,	17,16,0,	18,18,4,19,20,21,22,		19,18,0,	20,18,0,	21,18,0,	22,18,0,
		23,23,0
	};
	int attCount = sizeof(attachments) / sizeof(int);

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
	const int clickTestCnt = 5;
	int clickStuff[] = {
		10, 15,    2, false,   0, 21,  1,   20, 39, 18,
		61, 50,    6, true,    0, 72, 63,   kAbsent, kAbsent, kAbsent,
		90, 40,    7, false,   0, 72, 87,   kAbsent, kAbsent, kAbsent,
       260, 40,   25, true,    0, 73,263,   kAbsent, kAbsent, kAbsent,	// ligature
       267, 40,   25, false,   0, 73,263,   kAbsent, kAbsent, kAbsent,	// ligature
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(NULL, NULL, NULL, NULL);
	SetAttachedClusters(attachments, attCount);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
}

void TestCase::SetupRomanFeatures()
{
	m_testName = "Roman Features";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	m_fset[5].id = 1051;	m_fset[5].value = 0;	// diacritic selection
	m_fset[6].id = 0;

	//	Output:
	m_segWidth = 307;			// physical width of segment

	const int charCnt = 26;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true,  false, false, true,  true, true,  true, true,   true,  true,
		true,  true,  false, false, true, false, true, false,  true,  false,
		false, false, false, true,  true, true
	};

	const int glyphCnt = 23;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	//                    0                                             10                                              20
	gid16 glyphList[] =	{72,1815,1768, 83,1789, 86,681,1659,1667,1662,1056,274,1778, 805,1815,  44,1815,  80,1833,1768,1855,1838,637};
	int xPositions[] =  { 0,  23,  23, 21,  41, 45, 64,  87, 104, 121, 138,173, 174, 198, 217, 211, 231, 227, 258, 258, 258, 258,265};
	int yPositions[] =  { 0,   0,  10,  0,  -6,  0,  0,   0,   0,   0,   0,  0,   0,   0,   0,   0,  10,   0,   0,   0,  10,  20,  0};
	int advWidths[] =   {21,   0,   0, 24,   0, 18, 23,  17,  17,  17,  34, 24,  24,  13,   0,  15,   0,  37,   0,   0,   0,   0, 41};

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
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
	SetBBs(NULL, NULL, NULL, NULL);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
}

std::wstring TestCase::RomanText()
{
	std::wstring strRet;
	wchar_t charData[] = {
		0x0065, 0x0303, 0x0300, 0x0070, 0x0361, 0x0073, 0x0067, 0x02e8, 0x02e5, 0x02e7,
		0x014a, 0x0061, 0x0302, 0x0301, 0x0069, 0x0303, 0x0049, 0x0303, 0x006d, 0x033c,
		0x0300, 0x0308, 0x0304, 0x0066, 0x0066, 0x0069, 0x0000
	};
	strRet.assign(charData);
	return strRet;
}

/*----------------------------------------------------------------------------------------------
	A set of tests for handling complex diacritic stacking and bridging.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupStackingAndBridging()
{
	m_testName = "Roman Stacking and Bridging";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Input:
	m_fontName = L"Graphite Test Roman";
	m_fontFile = "grtest_roman.ttf";
	m_fontSize = 36;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 500;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;

	// text to render
	wchar_t charData[] = {
		0x0061,0x035d,0x0061,0x0020,0x0074,0x035d,0x0061,0x0020,0x0061,0x0300,
		0x0300,0x035d,0x0061,0x0020,0x0283,0x0300,0x0300,0x035d,0x0061,0x0020,
		0x0061,0x0316,0x0316,0xf176,0x0061,0x0020,0x0283,0x0300,0x0300,0xf176,
		0x0061,0x0020,0x0061,0x0316,0x0316,0xf176,0x0061,0x0020,0x0283,0x035d,
		0xf176,0x0061,0x0000
	};
	m_text.assign(charData);

	//	Output:
	m_segWidth = 414;			// physical width of segment

	const int charCnt = 42;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true, true, true, true,
		true, true, true, true, true, true, true, true, true, true,
		true, true, true, true, true, true, true, true, true, true,
		true, true, true, true, true, true, true, true, true, true,
		true, true,
	};

	const int glyphCnt = 42;	// number of glyphs in the segment

	// 68 = a, 97 = t, 1305 = esh, 1768 = upper grave, 1765 = lower grave, 1802 = upper bridge,
	// 1801 = lower bridge

	// need glyphCnt elements in these arrays:
	//                    0                                           10                                            20                                             30                                            40     
	gid16 glyphList[] =	{68,1802, 68, 3,  87,1802, 68,  3,  68,1768,1768,1802,  68,  3,1305,1768,1768,1802, 68,  3, 68,1765,1765,1801, 68,  3,1305,1768,1768,1801, 68,  3, 68,1765,1765,1801, 68,  3,1305,1802,1801, 68};
	int xPositions[] =  { 0,  21, 21, 42, 56,  73, 69, 91, 105, 127, 127, 126, 126,147, 161, 181, 181, 178,177,199,212, 235, 235, 234,234,255, 269, 289, 289, 286,285,307,320, 343, 343, 342,342,363, 377, 393, 393,393};
	int yPositions[] =  { 0,  -7,  0,  0,  0,  -1,  0,  0,   0,   0,  10,  12,   0,  0,   0,  10,  20,  23,  0,  0,  0,   0, -9,  -17,  0,  0,   0,  10,  20,  -6,  0,  0,  0,   0,  -9, -17,  0,  0,   0,   3,  -6,  0};
	int advWidths[] =   {21,   0, 21, 13, 13,   0, 21, 13,  21,   0,   0,   0,  21, 13,  16,   0,   0,   0, 21, 13, 21,   0,  0,    0, 21, 13,  16,   0,   0,   0, 21, 13, 21,   0,   0,   0, 21, 13,  16,   0,   0, 21};

	int bbLefts[] =     { 1,   0, 23, 42, 56,  52, 71, 91, 106, 108, 108, 105, 128,147, 159, 162, 162, 157,179,199,214, 217, 217, 213,235,255, 267, 270, 270, 265,287,307,322, 325, 325, 321,343,363, 375, 373, 373,395};
	int bbRights[] =    {21,  41, 42, 56, 69,  93, 91,105, 126, 119, 119, 146, 147,161, 178, 173, 173, 198,199,212,234, 228, 228, 254,255,269, 286, 281, 281, 306,306,320,342, 336, 336, 362,363,377, 394, 413, 414,414};
	int bbTops [] =     {22,  33, 22,  0, 28,  40, 22,  0,  22,  32,  42,  54,  22,  0,  33,  42,  53,  64, 22,  0, 22,  -3, -13, -26, 22,  0,  33,  42,  53, -15, 22,  0, 22,  -3, -13, -26, 22,  0,  33,  44, -15, 22};
	int bbBottoms [] =  { 0,  26,  0,  0,  0,  33,  0,  0,   0,  24,  34,  47,   0,  0, -10,  34,  44,  57,  0,  0,  0, -11, -21, -32,  0,  0, -10,  34,  44, -21,  0,  0,  0, -11, -21, -32,  0,  0, -10,  38, -21, 0};
	
	// Each group = glyph-index, base, number of attached, glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,0,	1,1,0,	2,2,0,		3,3,0,		4,4,0,		5,5,0,				6,6,0,		7,7,0,
		8,8,2,9,10,		9,8,0,		10,8,0,		11,11,0,	12,12,0,			13,13,0,	14,14,2,15,16,
		15,14,0,		16,14,0,	17,17,0,	18,18,0,	19,19,0,		20,20,2,21,22,	21,20,0,
		22,20,0,		23,23,0,	24,24,0,	25,25,0,		26,26,2,27,28,	27,26,0,	28,26,0,
		29,29,0,		30,30,0,	31,31,0,	32,32,2,33,34,	33,32,0,		34,32,0,	35,35,0,
		36,36,0,		37,37,0,	38,38,0,	39,39,0,		40,40,0,		41,41,0
	};
	int attCount = sizeof(attachments) / sizeof(int);

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
	const int clickTestCnt = 6;
	int clickStuff[] = {
		109, 25,    9, false,   10, 32, 105,   30, 56, 125,	// first grave on 4th a
		178, 26,   15, true,    19, 66, 176,    3, 21, 159,	// top of 1st esh, right side
		220, 84,   23, false,   68, 89, 210,   65, 83, 228,	// first lower bridge diac
		271, 40,   26, false,    0, 72, 266,  kAbsent, kAbsent, kAbsent,  // second esh, left side
		271, 65,   26, false,    0, 72, 266,  kAbsent, kAbsent, kAbsent,  // just below second esh, left side
		271, 68,   29, false,   57, 78, 262,    0, 17, 280		// lower bridge diac under esh, left side
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
	SetAttachedClusters(attachments, attCount);
	SetClickTests(clickTestCnt, clickStuff);
}

/*----------------------------------------------------------------------------------------------
	A set of tests for handling trailing whitespace.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupNoWhiteSpace()
{
	m_testName = "No white space";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	gid16 glyphList[] =	{55, 75, 72,  3,  3, 84, 88, 76, 70, 78,  3, 69, 85, 82, 90, 81};
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
	SetBBs(NULL, NULL, NULL, NULL);
	SetInsPtFlags(insPtFlags);
	SetClickTests(0, NULL);
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupNoWhiteSpaceNoSeg()
{
	m_testName = "No white space - no segment";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
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
	SetBBs(NULL, NULL, NULL, NULL);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
}

/*----------------------------------------------------------------------------------------------
	A set of tests of cross-line contextualization.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupCrossLine1()
{
	m_testName = "Cross-line 1";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
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
	SetBBs(NULL, NULL, NULL, NULL);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupCrossLine2()
{
	m_testName = "Cross-line 2";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
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
	SetBBs(NULL, NULL, NULL, NULL);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
	SetInputContextBlock(contextBlockInSize, contextBlockIn);
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupCrossLine3()
{
	m_testName = "Cross-line 3";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
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
	SetBBs(NULL, NULL, NULL, NULL);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
	SetInputContextBlock(contextBlockInSize, contextBlockIn);
	SetOutputContextBlock(contextBlockOutSize, contextBlockOut);
}

void TestCase::SetupCrossLine4()
{
	m_testName = "Cross-line 4";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
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
	SetBBs(NULL, NULL, NULL, NULL);
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
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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

	// Each group = glyph-index, base, number of attached, glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,0,		1,1,1,2,	2,1,0,		3,3,4,4,5,6,7,	4,3,0,		5,3,0,		6,3,0,		7,3,0,
		8,8,0,		9,9,4,10,11,12,13,		10,9,0,			11,9,0,		12,9,0,		13,9,0,		14,14,0,
		15,15,0,	16,16,1,17,	17,16,0,	18,18,1,19,		19,18,0		// etc
	};
	int attCount = sizeof(attachments) / sizeof(int);
	SetAttachedClusters(attachments, attCount);

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
	int clickStuff[] = {
		199, 13,    37, true,    0, 35,196,   kAbsent, kAbsent, kAbsent,
		396, 13,     3, false,   8, 26,399,   10, 19, 397,
		396,  8,     6, true,    3, 15,391,    9, 22, 394,
		222,  5,    32, true,    0, 35,225,   kAbsent, kAbsent, kAbsent,
		217,  5,    31, false,   0, 35,215,    0, 35, 246,
	};
	SetClickTests(4, clickStuff);
}

void TestCase::SetupArabic2()
{
	m_testName = "Arabic 2";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

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
	SetBBs(NULL, NULL, NULL, NULL);
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

/*----------------------------------------------------------------------------------------------
	A set of tests that uses Tai Viet script to test positioning.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupTaiViet1()
{
	m_testName = "Tai Viet Collisions";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Input:
	m_fontName = L"Graphite Test TaiViet";
	m_fontFile = "grtest_taiviet.ttf";
	m_fontSize = 36;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 2000;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_text = TaiVietText();

	m_fset[0].id = 2001;	m_fset[0].value = 2;	// vowel position = final consonant
	m_fset[1].id = 1051;	m_fset[1].value = 0;	// diacritic selection = off
	m_fset[2].id = 2102;	m_fset[2].value = 0;	// collision avoidance = off
	m_fset[3].id = 0;

	//	Output:
	m_segWidth = 946;			// physical width of segment

	const int charCnt = 46;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true,  true,  false, true,  true,  true,  false, true,  true,  true,
		false, false, true,  true,  false, true,  true,  false, false, true,
		true,  false, false, true,  true,  false, false, true,  true,  false,
		false, true,  true,  false, true,  true,  false, false, true,  true,
		false, true,  true,  true,  false, false
	};

	const int glyphCnt = 46;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	//                    0                                          10                                            20                                            30                                            40     
	gid16 glyphList[] =	{59, 70, 71, 65, 59,  70, 71, 23,  32, 175,  70,  65,  55, 70,  73,  55, 175, 184,185, 55, 76, 184,185,  55, 77, 70, 185,  41,  77,  70, 23, 56,175,  65,  27,  93, 70,185,  53,  69,  81, 50, 27,175, 70, 23};
	int xPositions[] =  { 0, 72, 47, 72,101, 174,149,174, 206, 281, 283, 251, 283,352, 316, 352, 418, 425,386,425,493, 495,458, 495,556,559, 528, 559, 623, 623,591,623,706, 676, 706, 776,777,738, 777, 836, 816,843,880,944,946,912};
	int yPositions[] =  { 0,  0,  0,  0,  0,   0,  0,  0,   0,   0,  15,   0,   0,  0,   0,   0,   5,  13,  0,  0,  5,  22,  0,   0,  0,  5,   0,   0,   0,   0,  0,  0,  0,   0,   0,   5, 19,  0,   0,   8,   0,  0,  0,  5, 21,  0};
	int advWidths[] =   {47,  0, 24, 29, 47,   0, 24, 32,  44,   0,   0,  29,  33,  0,  36,  33,   0,   0, 26, 33,  0,   0, 26,  33,  0,  0,  26,  32,   0,   0, 32, 52,  0,  29,  32,   0,  0, 26,  39,   0,  26, 36, 32,  0,  0, 32};

	int bbLefts[] =     { 5, 45, 52, 73,106, 147,154,177, 209, 254, 257, 252, 286,326, 322, 355, 391, 414,390,428,463, 484, 462,498,540,532, 532, 562, 607, 597,594,627,679, 677, 709, 744,751,742, 784, 825, 820,849,883,917,919,915};
	int bbRights[] =    {72, 72, 63, 97,174, 174,165,211, 277, 281, 283, 276, 349,352, 355, 419, 418, 422,409,491,494, 491, 481,561,553,559, 551, 621, 620, 623,628,681,706, 701, 756, 776,777,761, 835, 833, 840,882,930,944,946,949};
	int bbTops [] =     {60, 50, 25, 25, 60,  50, 25, 41,  60,  49,  66,  25,  60, 50,  40,  60,  55,  65, 36, 60, 56,  74,  36, 60, -5, 55,  36,  60,  -5,  50, 41, 40, 49,  25,  55,  56, 69, 36,  55,  61,  41, 40, 55, 55, 71, 41};
	int bbBottoms [] =  { 0, 35,  0,  0,  0,  35,  0,  0,   0,  34,  50,   0,   0, 35,   0,   0,  39,  51,  0,  0, 40,  60,   0,  0,-27, 40,   0,   0, -27,  35,  0,  0, 34,   0,   0,  40, 54,  0,   0,  45,   0, 0,   0, 39, 56,  0};
	
	// Each group = glyph-index, base, number of attached, glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,0,		1,2,0,		2,2,1,1,		3,3,0,	4,4,0,	5,6,0,	6,6,1,5,	7,7,0,
		8,8,0,		9,11,0,		10,11,0,		11,11,2,9,10,	12,12,0,		13,14,0,	14,14,1,13,
		15,15,0,	16,18,0,	17,18,0,		18,18,2,16,17,	19,19,0,		20,22,0,	21,22,0,
		22,22,2,20,21,	23,23,0,		24,26,0,	25,26,0,	26,26,2,24,25,	27,27,0,	28,30,0,
		29,30,0,		30,30,2,28,29,	31,31,0,	32,33,0,	33,33,1,32,		34,34,0,	35,37,0,
		36,37,0,		37,37,2,35,36,	38,38,0,	39,40,0,	40,40,1,39,		41,41,0,	42,42,0,
		43,45,0,		44,45,0,		45,45,2,43,44
	};
	int attCount = sizeof(attachments) / sizeof(int);

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel Top, prim sel bottom, prim sel left,
	//		sec sel Top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
	const int clickTestCnt = 4;
	int clickStuff[] = {
		 55, 40,   1, true,   0,100,  46,  kAbsent, kAbsent, kAbsent,
		 65, 20,   1, false,  0,100,  46,  kAbsent, kAbsent, kAbsent,	// adjust to the left to get a valid IP?
		 95, 84,   4, true,   0,100, 100,  kAbsent, kAbsent, kAbsent,
		104, 40,   4, false,  0,100, 100,  kAbsent, kAbsent, kAbsent
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
	SetAttachedClusters(attachments, attCount);
	SetClickTests(clickTestCnt, clickStuff);
}

/*----------------------------------------------------------------------------------------------
	A set of tests that uses Tai Viet script to test positioning.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupTaiViet2()
{
	m_testName = "Tai Viet No Collisions";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Input:
	m_fontName = L"Graphite Test TaiViet";
	m_fontFile = "grtest_taiviet.ttf";
	m_fontSize = 36;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 2000;			// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_text = TaiVietText();

	m_fset[0].id = 2001;	m_fset[0].value = 2;	// vowel position = final consonant
	m_fset[1].id = 1051;	m_fset[1].value = 1;	// diacritic selection = on
	m_fset[2].id = 2102;	m_fset[2].value = 1;	// collision avoidance = off
	m_fset[3].id = 0;

	//	Output:
	m_segWidth = 947;			// physical width of segment

	const int charCnt = 46;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true, true, true, true,
		true, true, true, true, true, true, true, true, true, true,
		true, true, true, true, true, true, true, true, true, true,
		true, true, true, true, true, true, true, true, true, true,
		true, true, true, true, true, true
	};

	const int glyphCnt = 46;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	//                    0                                          10                                            20                                            30                                            40     
	gid16 glyphList[] =	{59, 70, 71, 65, 59,  70, 71, 23,  32, 175,  70,  65,  55, 70,  73,  55, 175, 184,185, 55, 76, 184,185,  55, 77, 70, 185,  41,  77,  70, 23, 56,175,  65,  27,  93, 70,185,  53,  69,  81, 50, 27,175, 70, 23};
	int xPositions[] =  { 0, 82, 47, 72,101, 184,149,174, 206, 281, 283, 251, 283,359, 316, 352, 418, 431,386,425,496, 508,458, 497,559,578, 530, 561, 626, 635,593,626,709, 678, 708, 778,780,740, 780, 853, 819,846,882,942,947,914};
	int yPositions[] =  { 0, -3,  0,  0,  0,  -3,  0,  0,   0,   0,  22,   0,   0,  0,   0,   0,   0,   9,  0,  0,  2,  20,  0,   0,  0,  1,   0,   0,   0,   0,  0,  0, -4,   0,   0,   5, 25,  0,   0,   8,   0,  0,  0,  0, 23,  0};
	int advWidths[] =   {47,  0, 24, 29, 47,   0, 24, 32,  44,   0,   0,  29,  33,  0,  36,  33,   0,   0, 26, 33,  0,   0, 26,  33,  0,  0,  26,  32,   0,   0, 32, 52,  0,  29,  32,   0,  0, 26,  39,   0,  26, 36, 32,  0,  0, 32};

	int bbLefts[] =     { 5, 56, 52, 73,106, 158,154,177, 209, 254, 257, 252, 286,333, 322, 355, 391, 420,390,428,466, 497, 462,500,542,552, 534, 564, 609, 609,597,629,682, 679, 711, 747,753,744, 786, 842, 823,851,885,916,921,918};
	int bbRights[] =    {72, 82, 63, 97,174, 184,165,211, 277, 281, 283, 276, 349,359, 355, 419, 418, 427,409,491,496, 504, 481,564,555,578, 553, 624, 622, 635,630,684,709, 703, 758, 778,780,763, 837, 850, 842,885,932,942,947,951};
	int bbTops [] =     {60, 46, 25, 25, 60,  46, 25, 41,  60,  49,  73,  25,  60, 50,  40,  60,  50,  60, 36, 60, 54,  72,  36, 60, -5, 52,  36,  60,  -5,  50, 41, 40, 45,  25,  55,  56, 75, 36,  55,  61,  41, 40, 55, 50, 73, 41};
	int bbBottoms [] =  { 0, 31,  0,  0,  0,  31,  0,  0,   0,  34,  57,   0,   0, 35,   0,   0,  34,  46,  0,  0, 37,  57,   0,  0,-27, 36,   0,   0, -27,  35,  0,  0, 29,   0,   0,  40, 60,  0,   0,  45,   0, 0,   0, 34, 58,  0};
	
	// Each group = glyph-index, base, number of attached, glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,0,		1,2,0,		2,2,1,1,		3,3,0,	4,4,0,	5,6,0,	6,6,1,5,	7,7,0,
		8,8,0,		9,11,0,		10,11,0,		11,11,2,9,10,	12,12,0,		13,14,0,	14,14,1,13,
		15,15,0,	16,18,0,	17,18,0,		18,18,2,16,17,	19,19,0,		20,22,0,	21,22,0,
		22,22,2,20,21,	23,23,0,		24,26,0,	25,26,0,	26,26,2,24,25,	27,27,0,	28,30,0,
		29,30,0,		30,30,2,28,29,	31,31,0,	32,33,0,	33,33,1,32,		34,34,0,	35,37,0,
		36,37,0,		37,37,2,35,36,	38,38,0,	39,40,0,	40,40,1,39,		41,41,0,	42,42,0,
		43,45,0,		44,45,0,		45,45,2,43,44
	};
	int attCount = sizeof(attachments) / sizeof(int);

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel top, prim sel bottom, prim sel left,
	//		sec sel top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
	const int clickTestCnt = 4;
	int clickStuff[] = {
		 55, 40,   2, false, 35, 65,  49,       14, 39, 82,
		 65, 20,   1, false,  0,100,  50,  kAbsent, kAbsent, kAbsent,
		 95, 84,   4, true,   0,100, 100,  kAbsent, kAbsent, kAbsent,
		104, 40,   4, false,  0,100, 100,  kAbsent, kAbsent, kAbsent
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
	SetAttachedClusters(attachments, attCount);
	SetClickTests(clickTestCnt, clickStuff);
}

std::wstring TestCase::TaiVietText()
{
	std::wstring strRet;
	wchar_t charData[] = {
		0xe00f,0xe042,0xe031,0xe02b,0xe00f,0xe042,0xe031,0xe025,0xe021,0xe033,
		0xe042,0xe02b,0xe01c,0xe042,0xe03e,0xe01c,0xe033,0xe040,0xe009,0xe01c,
		0xe039,0xe040,0xe009,0xe01c,0xe035,0xe042,0xe009,0xe024,0xe035,0xe042,
		0xe025,0xe01b,0xe033,0xe02b,0xe00a,0xe030,0xe042,0xe009,0xe01e,0xe040,
		0xe03b,0xe019,0xe00a,0xe033,0xe042,0xe025,0x0000
	};
	strRet.assign(charData);
	return strRet;
}

/*----------------------------------------------------------------------------------------------
	A set of tests that uses the new Burmese encoding, specifically to test rules that
	calculate the "position" slot attribute on attached glyphs...tricky stuff.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupBurmesePositioning1()
{
	m_testName = "Burmese Positioning LineBreakSeg";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Input:
	m_fontName = L"Graphite Test Burmese Two";
	m_fontFile = "grtest_burmese2.ttf";
	m_fontSize = 36;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight--irrelevant
	m_availWidth = 2000;			// width available for segment--make a LineBreakSegment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_text = BurmesePositioningText();

	//	Output:
	m_segWidth = 193;			// physical width of segment

	const int charCnt = 12;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, false, false, false, true, true, true, true, true, false,
		true, true, true
	};

	const int glyphCnt = 11;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	//                    0                                          10                                            20                                            30                                            40     
	gid16 glyphList[] =	{169,239,213,219,230, 99,  3,238,  99, 205, 220};
	int xPositions[] =  {  0, 43, 44, 51, 53, 61,108,125, 133, 178, 180};
	int yPositions[] =  {  0,  0, -1,  0,  0,  0,  0,  0,   0,   0,   0};
	int advWidths[] =   { 46,  9,  0,  0, 55, 46, 17, 55,  46,   0,  12};

	int bbLefts[] =     {  2, 24, 26, 44, 56, 63,108,129, 136, 159, 183};
	int bbRights[] =    { 43, 41, 41, 51,105,105,125,178, 178, 175, 190};
	int bbTops [] =     { 21, -3, 39, -7, 44, 21,  0, 44,  21,  42,  21};
	int bbBottoms [] =  {  0,-21, 26,-14,-21,  0,  0,-21,   0,  25,   0};
	
	// Each group = glyph-index, base, number of attached glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,3,1,2,3,	1,0,0,		2,0,0,		3,0,0,		4,4,1,5,	5,4,0,	6,6,0,
		7,7,2,8,9,		8,7,0,		9,7,0,		10,10,0
	};
	int attCount = sizeof(attachments) / sizeof(int);

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
	SetAttachedClusters(attachments, attCount);
//	SetClickTests(clickTestCnt, clickStuff);
}

void TestCase::SetupBurmesePositioning2()
{
	m_testName = "Burmese Positioning RangeSeg";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Input:
	m_fontName = L"Graphite Test Burmese Two";
	m_fontFile = "grtest_burmese2.ttf";
	m_fontSize = 36;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight--irrelevant
	m_availWidth = 10000;			// width available for segment--make a RangeSegment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_text = BurmesePositioningText();

	//	Output:
	m_segWidth = 193;			// physical width of segment

	const int charCnt = 12;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, false, false, false, true, true, true, true, true, false,
		true, true, true
	};

	const int glyphCnt = 11;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	//                    0                                          10                                            20                                            30                                            40     
	gid16 glyphList[] =	{169,239,213,219,230, 99,  3,238,  99, 205, 220};
	int xPositions[] =  {  0, 43, 44, 51, 53, 61,108,125, 133, 178, 180};
	int yPositions[] =  {  0,  0, -1,  0,  0,  0,  0,  0,   0,   0,   0};
	int advWidths[] =   { 46,  9,  0,  0, 55, 46, 17, 55,  46,   0,  12};

	int bbLefts[] =     {  2, 24, 26, 44, 56, 63,108,129, 136, 159, 183};
	int bbRights[] =    { 43, 41, 41, 51,105,105,125,178, 178, 175, 190};
	int bbTops [] =     { 21, -3, 39, -7, 44, 21,  0, 44,  21,  42,  21};
	int bbBottoms [] =  {  0,-21, 26,-14,-21,  0,  0,-21,   0,  25,   0};
	
	// Each group = glyph-index, base, number of attached glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,3,1,2,3,	1,0,0,		2,0,0,		3,0,0,		4,4,1,5,	5,4,0,	6,6,0,
		7,7,2,8,9,		8,7,0,		9,7,0,		10,10,0
	};
	int attCount = sizeof(attachments) / sizeof(int);

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
	SetAttachedClusters(attachments, attCount);
//	SetClickTests(clickTestCnt, clickStuff);
}


std::wstring TestCase::BurmesePositioningText()
{
	std::wstring strRet;
	wchar_t charData[] = {
		0x1018,0x103d,0x1032,0x1037,0x1000,0x103c,0x0020,0x1000,0x103c,0x102d,
		0x102f,0x1038,0x0000
	};
	strRet.assign(charData);
	return strRet;
}

/*----------------------------------------------------------------------------------------------
	Test pass constraints.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupBurmeseFeature1()
{
	m_testName = "Burmese Pass Constraint Fails";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Note: this font has the table rule-version set very high (8.0) in order to test the 
	//	CheckRuleValidity routine.

	//	Input:
	m_fontName = L"Graphite Test Burmese Three";
	m_fontFile = "grtest_burmese3.ttf";
	m_fontSize = 36;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight--irrelevant
	m_availWidth = 2000;			// width available for segment--make a LineBreakSegment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_text = BurmeseFeatureText();

	m_fset[0].id = 0x646F7463;	m_fset[0].value = 0;	// 0x646F7463 = "dotc"
	m_fset[1].id = 0;

	//	Output:
	m_segWidth = 147;			// physical width of segment

	const int charCnt = 7;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true
	};

	const int glyphCnt = 7;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{180,  3,210,137,  3,213,156};
	int xPositions[] =  {  0, 45, 62, 76,103,120,120};
	int yPositions[] =  {  0,  0,  0,  0,  0,  0,  0};
	int advWidths[] =   { 45, 17, 13, 26, 17,  0, 27};

	int bbLefts[] =     {  2, 45, 64, 79,103,103,123};
	int bbRights[] =    { 42, 62, 77,100,120,118,145};
	int bbTops [] =     { 21,  0, 20, 21,  0, 41, 21};
	int bbBottoms [] =  {  0,  0,-21,-21,  0, 28,  0};
	
	// Each group = glyph-index, base, number of attached glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,0,	1,1,0,		2,2,0,		3,3,0
	};
	int attCount = sizeof(attachments) / sizeof(int);

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
	SetAttachedClusters(attachments, attCount);
//	SetClickTests(clickTestCnt, clickStuff);
}

void TestCase::SetupBurmeseFeature2()
{
	m_testName = "Burmese Pass Constraint Succeeds";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Note: this font has the table rule-version set very high (8.0) in order to test the 
	//	CheckRuleValidity routine.

	//	Input:
	m_fontName = L"Graphite Test Burmese Three";
	m_fontFile = "grtest_burmese3.ttf";
	m_fontSize = 36;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight--irrelevant
	m_availWidth = 2000;			// width available for segment--make a LineBreakSegment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_text = BurmeseFeatureText();

	m_fset[0].id = 0x646F7463;	m_fset[0].value = 1;	// 0x646F7463 = "dotc"
	m_fset[1].id = 0;

	//	Output:
	m_segWidth = 193;			// physical width of segment

	const int charCnt = 7;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true
	};

	const int glyphCnt = 9;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays: 
	gid16 glyphList[] =	{180,  3,361,211,137,  3,361,213,156};
	int xPositions[] =  {  0, 45, 62, 84, 92,119,136,163,166};
	int yPositions[] =  {  0,  0,  0,  0,  0,  0,  0, -1,  0};
	int advWidths[] =   { 45, 17, 29,  0, 26, 17, 29,  0, 27};

	int bbLefts[] =     {  2, 45, 65, 72, 95,119,139,145,169};
	int bbRights[] =    { 42, 62, 89, 86,116,136,164,160,191};
	int bbTops [] =     { 21,  0, 23, -4, 21,  0, 23, 39, 21};
	int bbBottoms [] =  {  0,  0, -1,-21,-21,  0, -1, 26,  0};
	
	// Each group = glyph-index, base, number of attached glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,0,	1,1,0,	2,2,1,3, 3,2,0,	4,4,0,	5,5,0
	};
	int attCount = sizeof(attachments) / sizeof(int);

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
	SetAttachedClusters(attachments, attCount);
//	SetClickTests(clickTestCnt, clickStuff);
}

std::wstring TestCase::BurmeseFeatureText()
{
	std::wstring strRet;
	wchar_t charData[] = {
		0x101c,0x0020,0x1030,0x100c,0x0020,0x1032,0x1013,0x0000
	};
	strRet.assign(charData);
	return strRet;
}


/*----------------------------------------------------------------------------------------------
	Another pass constraint test.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupCharisPConstraint()
{
	m_testName = "Charis PConstraint";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Input:
	m_fontName = L"Graphite Text Charis PConstr";
	m_fontFile = "grtest_pconstraints.ttf";
	m_fontSize = 20;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 2000;			// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	wchar_t charData[] = {
		0x0073,0x0061,0x0067,0x0065,0x0000
	};
	m_text.assign(charData);

	m_fset[0].id = 1032;	m_fset[0].value = 1;	// 1032 = literacy alternates
	m_fset[1].id = 0;

	//	Output:
	m_segWidth = 53;			// physical width of segment

	const int charCnt = 4;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true
	};

	const int glyphCnt = 4;	// number of glyphs in the segment

	gid16 glyphList[] = { 86,320,1056, 72};
	int xPositions[] =  {  0, 10,  25, 40};
	int yPositions[] =  {  0,  0,   0,  0};
	int advWidths[] =   { 10, 15,  14, 13};

	int bbLefts[] =     {  0, 11,  26, 41};
	int bbRights[] =    {  9, 24,  38, 52};
	int bbTops [] =     { 13, 13,  13, 13};
	int bbBottoms [] =  {  0,  0,  -6,  0};
	
	// Each group = glyph-index, base, number of attached glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,0,	1,1,0,	2,2,0, 3,3,0
	};
	int attCount = sizeof(attachments) / sizeof(int);

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
	SetAttachedClusters(attachments, attCount);
//	SetClickTests(clickTestCnt, clickStuff);

}

/*----------------------------------------------------------------------------------------------
	A set of tests that uses Devanagari script.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupDevanagari()
{
	m_testName = "Devanagari 1";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Input:
	m_fontName = L"Graphite Test Devanagari";
	m_fontFile = "grtest_devanagari.ttf";
	m_fontSize = 36;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 340;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_text = DevanagariText();

	//m_fset[0].id = 2001;	m_fset[0].value = 2;	// vowel position = final consonant
	//m_fset[1].id = 1051;	m_fset[1].value = 1;	// diacritic selection = on
	//m_fset[2].id = 2102;	m_fset[2].value = 1;	// collision avoidance = off
	//m_fset[3].id = 0;

	//	Output:
	m_segWidth = 336;			// physical width of segment

	const int charCnt = 35;		// number of characters in the segment (line break happened)

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, false, true, true, true, false, true, true, true, true,
		true, true, true, true, true, true, true, false, true, true,
		false, true, true, true, false, true, false, true, true, true,
		true, false, true, true
	};

	const int glyphCnt = 23;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	//                    0                                       10                                  19                           
	gid16 glyphList[] =	{740,  3,622,655,656,809,304,622,  3,304,474,280,  3,526,303,654,  3,725,301,  3,699,328,  3};
	int xPositions[] =  {  0, 30, 44, 65, 70, 71, 71, 84,110,125,138,167,168,182,217,232,232,247,272,285,299,320,336};
	int yPositions[] =  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0, -4,  0};
	int advWidths[] =   { 30, 14, 26,  0,  0,  0, 13, 26, 14, 13, 26,  0, 14, 34, 13,  0, 14, 25, 13, 14, 36,  0, 14};

	int bbLefts[] =     { -2, 30, 42, 54, 58, 71, 69, 82,110,123,136,149,168,186,204,221,232,245,260,285,297,308,336};
	int bbRights[] =    { 32, 44, 73, 72, 69, 71, 97,112,125,150,166,167,182,219,236,225,247,274,287,299,338,330,350};
	int bbTops [] =     { 30,  0, 30, -2, 45,  0, 42, 30,  0, 42, 30, 46,  0, 31, 45, -4,  0, 30, 45,  0, 30, -5,  0};
	int bbBottoms [] =  {-11,  0, -2,-15, 30,  0,  0, -2,  0,  0,  0, 34,  0,  0,  0, -8,  0,-10,  0,  0, -4,-19,  0};
	
	// Each group = glyph-index, base, number of attached, glyphs, attached-glyph-indices
	int attachments[] = {
		0,0,0,		1,1,0,		2,2,2,3,4,	3,2,0,		4,2,0,		5,5,0,		6,6,0,		7,7,0,
		8,8,0,		9,9,0,		10,10,1,11,	11,10,0,	12,12,0,	13,13,0,	14,14,1,15,	15,14,0,
		16,16,0,	17,17,0,	18,18,0,	19,19,0,	20,20,1,21,	21,20,0,	22,22,0
	};
	int attCount = sizeof(attachments) / sizeof(int);

	// Each group = char-index, number of glyphs, glyph-indices.
	int charsToGlyphs[] = {
		0, 1, 0,	1, 1, 0,	2, 1, 0,	3, 1, 1,	4, 1, 4,	5, 1, 4,	6, 1, 2,	7, 1, 3,	8, 1, 5,
		9, 1, 7,	10, 1, 6,	11, 1, 8,	12, 1, 10,	13, 1, 9,	14, 1, 11,	15, 1, 12,	16, 1, 14,	17, 1, 14,
		18, 1, 13,	19, 1, 14,	20, 1, 15,	21, 1, 14,	22, 1, 16,	23, 1, 18,	24, 1, 18,	25, 1, 17,	26, 1, 17,
		27, 1, 17,	28, 1, 18,	29, 1, 19,	30, 1, 20,	31, 1, 20,	32, 1, 20,	33, 1, 21,	34, 1, 22
	};
	int c2gCount = sizeof(charsToGlyphs) / sizeof(int);

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel top, prim sel bottom, prim sel left,
	//		sec sel top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
	// TODO: fix these
	const int clickTestCnt = 3;
	int clickStuff[] = {
		  333, 14,  32, true,  13, 33, 332,       32, 50,299,	// top half of ligature
		  311, 40,  32, false, 30, 50, 298,		  13, 32,334,	// bottom half of ligature
		  311, 61,  33, false, 46, 66, 305,       32, 50,334	// lower attached ukar
	};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
	SetAttachedClusters(attachments, attCount);
	SetCharsToGlyphs(charsToGlyphs, c2gCount);
	SetClickTests(clickTestCnt, clickStuff);
}

std::wstring TestCase::DevanagariText()
{
	std::wstring strRet;
	wchar_t charData[] = {
		0x092e,0x094d,0x0932,0x0020,0x0930,0x094d,0x0939,0x094d,0x200c,0x0939,
		0x093f,0x0020,0x091f,0x093f,0x0901,0x0020,0x0930,0x094d,0x0927,0x094c,
		0x093c,0x0901,0x0020,0x0930,0x094d,0x091f,0x094d,0x0920,0x094c,0x0020,
		0x0915,0x094d,0x0915,0x0942,0x0020,0x0921,0x093f,0x0000

	};
	strRet.assign(charData);
	return strRet;
}

/*----------------------------------------------------------------------------------------------
	Test kerning that uses a large number of glyph attributes and therefore needs version 2
	of the Glat table.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupKernGlat2()
{
	m_testName = "Kern Glat2";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Input:
	m_fontName = L"Graphite Test Kern Glat2";
	m_fontFile = "grtest_kernglat2.ttf";
	m_fontSize = 36;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 1000;			// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_text = L"ch AW ox";	// text to render

	//	Output:
	m_segWidth = 181;			// physical width of segment

	const int charCnt = 8;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true, true
	};

	const int glyphCnt = 8;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:                             
	gid16 glyphList[] =	{ 70, 75,  3, 36, 58,  3, 82, 91};
	int xPositions[] =  {  0, 20, 46, 57, 83,123,134,157};
	int yPositions[] =  {  0,  0,  0,  0,  0,  0,  0,  0};
	int advWidths[] =   { 20, 25, 10, 28, 37, 10, 24, 23};

	int bbLefts[] =     {  1, 21, 46, 57, 83,123,136,158};
	int bbRights[] =    { 19, 46, 57, 85,122,134,156,181};
	int bbTops [] =     { 22, 36,  0, 30, 29,  0, 22, 21};
	int bbBottoms [] =  {  0,  0,  0,  0,  0,  0,  0,  0};
	
	// These tests are not particularly relevant:

	// Each group = glyph-index, base, number of attached, glyphs, attached-glyph-indices
	//int attachments[] = {
	//	0,0,0,		1,1,0,		2,2,2,3,4,	3,2,0,		4,2,0,		5,5,0,		6,6,0,		7,7,0,
	//	8,8,0,		9,9,0,		10,10,1,11,	11,10,0,	12,12,0,	13,13,0,	14,14,1,15,	15,14,0,
	//	16,16,0,	17,17,0,	18,18,0,	19,19,0,	20,20,1,21,	21,20,0,	22,22,0
	//};
	//int attCount = sizeof(attachments) / sizeof(int);

	// Each group = char-index, number of glyphs, glyph-indices.
	//int charsToGlyphs[] = {
	//	0, 1, 0,	1, 1, 0,	2, 1, 0,	3, 1, 1,	4, 1, 4,	5, 1, 4,	6, 1, 2,	7, 1, 3,	8, 1, 5,
	//	9, 1, 7,	10, 1, 6,	11, 1, 8,	12, 1, 10,	13, 1, 9,	14, 1, 11,	15, 1, 12,	16, 1, 14,	17, 1, 14,
	//	18, 1, 13,	19, 1, 14,	20, 1, 15,	21, 1, 14,	22, 1, 16,	23, 1, 18,	24, 1, 18,	25, 1, 17,	26, 1, 17,
	//	27, 1, 17,	28, 1, 18,	29, 1, 19,	30, 1, 20,	31, 1, 20,	32, 1, 20,	33, 1, 21,	34, 1, 22
	//};
	//int c2gCount = sizeof(charsToGlyphs) / sizeof(int);

	//	Each line in clickStuff represents one click test with the following items:
	//		click x-coord, click y-coord, char index, assoc-prev,
	//		prim sel top, prim sel bottom, prim sel left,
	//		sec sel top, sec sel bottom, sec sel left
	//  Y-coordinates are offsets from segment top; ie, (0,0) is segment top-left.
	// TODO: fix these
	//const int clickTestCnt = 3;
	//int clickStuff[] = {
	//	  333, 14,  32, true,  13, 33, 332,       32, 50,299,	// top half of ligature
	//	  311, 40,  32, false, 30, 50, 298,		  13, 32,334,	// bottom half of ligature
	//	  311, 61,  33, false, 46, 66, 305,       32, 50,334	// lower attached ukar
	//};

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
//	SetAttachedClusters(attachments, attCount);
//	SetCharsToGlyphs(charsToGlyphs, c2gCount);
//	SetClickTests(clickTestCnt, clickStuff);
}

/*----------------------------------------------------------------------------------------------
	Set up a test where the font is bad and we revert to dumb rendering
----------------------------------------------------------------------------------------------*/
void TestCase::SetupDumbFallback1()
{
	m_testName = "Dumb Fallback 1";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Input:
	m_fontName = L"GrErr BadVersion";
	m_fontFile = "grtest_badVersion.ttf";
	m_text = RomanText();			// text to render
	m_fontSize = 12;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 500;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_dumbFallback = true;

	//	Output:
	m_badFont = true;
	m_segWidth = 196;			// physical width of segment

	const int charCnt = 26;		// number of characters in the segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true, true, true, true, true, true, true, true, true, true,
		true, true, true, true, true, true, true, true, true, true,
		true, true,	true, true, true, true
	};

	const int glyphCnt = 26;	// number of glyphs in the segment

	// need glyphCnt elements in these arrays:
	//                    0                                      10                                      20
	gid16 glyphList[] =	{71,  0,  0, 82,  0, 85, 73,  0,  0,  0,  0, 67,  0,  0, 75,  0, 43,  0, 79,  0,  0,  0,  0, 72, 72, 75};
	int xPositions[] =  { 0,  7, 15, 23, 31, 39, 46, 54, 62, 70, 78, 86, 93,101,109,114,122,128,136,149,157,165,173,181,186,192};
	int yPositions[] =  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
	int advWidths[] =   { 7,  8,  8,  8,  8,  6,  8,  8,  8,  8,  8,  7,  8,  8,  4,  8,  5,  8, 12,  8,  8,  8,  8,  5,  5,  4};

	const int clickTestCnt = 0;
	int * clickStuff = NULL;

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetBBs(NULL, NULL, NULL, NULL);
	SetInsPtFlags(insPtFlags);
	SetClickTests(clickTestCnt, clickStuff);
}

/*----------------------------------------------------------------------------------------------
	Now make sure we will get a crash when we turn dumb rendering off for the same font.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupDumbFallback2()
{
	m_testName = "Dumb Fallback 2";
	//m_debug = true;
	//m_traceLog = true;
	//m_skip = true;

	//	Input:
	m_fontName = L"GrErr BadVersion";
	m_fontFile = "grtest_badVersion.ttf";
	m_text = L"This is a test.";	// text to render
	m_fontSize = 12;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 500;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_dumbFallback = false;

	//	Output:
	m_badFont = true;
	m_noSegment = true;
	m_segWidth = 0;			// physical width of segment

	const int charCnt = 0;		// number of characters in the segment

	const int glyphCnt = 0;	// number of glyphs in the segment

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
}

/*----------------------------------------------------------------------------------------------
	Now make sure we will get a crash when we turn dumb rendering off for the same font.
	Also produces a memory leak.
----------------------------------------------------------------------------------------------*/
void TestCase::SetupBadFont()
{
	m_testName = "Bad Font";
	//m_debug = true;
	//m_traceLog = true;
	m_skip = true; /////////////////////////

	//	Input:
	//	The font has been corrupted so that the size of the cmap in the directory is invalid.
	m_fontName = L"Graphite Test Roman";
	m_fontFile = "grtest_badCmap.ttf";
	m_text = L"This is a test.";	// text to render
	m_fontSize = 12;				// font size in points
	m_prefBreak = klbWordBreak;		// preferred break-weight
	m_availWidth = 500;				// width available for segment
	m_bold = false;
	m_italic = false;
	m_rtl = false;
	m_backtrack = false;
	m_dumbFallback = true;	// wants to do dumb fall-back, but can't because the font is totally invalid

	//	Output:
	m_badFont = true;
	m_noSegment = true;
	m_segWidth = 0;			// physical width of segment

	const int charCnt = 0;		// number of characters in the segment

	const int glyphCnt = 0;	// number of glyphs in the segment

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
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
	m_skip = false;

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
	m_dumbFallback = true;

	m_badFont = false;
	m_noSegment = false;			// yes, a segment should be generated
	m_charCount = 0;
	m_glyphCount = 0;
	m_glyphArray = NULL;
	m_xPositions = NULL;
	m_yPositions = NULL;
	m_advWidths = NULL;
	m_bbLefts = NULL;
	m_bbRights = NULL;
	m_bbTops = NULL;
	m_bbBottoms = NULL;
	m_insPointFlags = NULL;
	m_charsToGlyphs = NULL;
	m_c2gCount = 0;
	m_attGlyphs = NULL;
	m_attGCount = 0;
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
	delete[] m_bbLefts;
	delete[] m_bbRights;
	delete[] m_bbTops;
	delete[] m_bbBottoms;
	delete[] m_insPointFlags;
	delete[] m_charsToGlyphs;
	delete[] m_attGlyphs;
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
	m_bbLefts = new int[glyphCount];
	m_bbRights = new int[glyphCount];
	m_bbTops = new int[glyphCount];
	m_bbBottoms = new int[glyphCount];
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

void TestCase::SetBBs(int * bbLefts, int * bbRights, int * bbTops, int * bbBottoms)
{
	if (bbLefts == NULL) // no bb tests
	{
		delete[] m_bbLefts;
		delete[] m_bbRights;
		delete[] m_bbTops;
		delete[] m_bbBottoms;
		m_bbLefts = NULL;
		m_bbRights = NULL;
		m_bbTops = NULL;
		m_bbBottoms = NULL;
		return;
	}

	for (int i = 0; i < m_glyphCount; i++)
	{
		m_bbLefts[i] = bbLefts[i];
		m_bbRights[i] = bbRights[i];
		m_bbTops[i] = bbTops[i];
		m_bbBottoms[i] = bbBottoms[i];
	}
}

void TestCase::SetInsPtFlags(bool * flags)
{
	for (int i = 0; i < m_charCount; i++)
		m_insPointFlags[i] = flags[i];
}

void TestCase::SetCharsToGlyphs(int * stuff, int count)
{
	m_c2gCount = count;
	m_charsToGlyphs = new int[count];
	for (int i = 0; i < count; i++)
		m_charsToGlyphs[i] = stuff[i];
}

void TestCase::SetAttachedClusters(int * stuff, int count)
{
	m_attGCount = count;
	m_attGlyphs = new int[count];
	for (int i = 0; i < count; i++)
		m_attGlyphs[i] = stuff[i];
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


/****************************************************************************/

// Temporary methods for testing bugs.

void TestCase::SetupBugTest()
{
	m_testName = "Bug Test";
	m_traceLog = true;
	m_debug = true;
	m_skip = false;

	//	Input:
	//m_fontName = L"Padauk";
	//m_fontFile = "grtest_infinity.ttf";
	//wchar_t charData[] = { 0x101e, 0x1032, 0x1015, 0x103c, 0x103d, 0x103e, 0x102d, 0x1038, 0x0000 };
	//m_text.assign(charData);
	//int charCnt = 8;

	m_fontName = L"SILDoulos Kern Test Temp";
	///m_fontFile = "kern_temp_gr.ttf";
	m_fontFile = "kern_glat2_gr.ttf";
	wchar_t charData[] = {
		0x003E, 0x0061,						// >a
		0x003E, 0x003E, 0x003E, 0x0062,		// >>>b
		0x003C, 0x0063,						// <c
		0x003C, 0x003C, 0x0041,				// <<A
		0x003E, 0x003E, 0x0042,				// >>B
		0x0000 };			
	m_text.assign(charData);
	int charCnt = 14;

	m_fontSize = 20;				// font size in points
	m_prefBreak = klbWsBreak;		// preferred break-weight
	m_worstBreak = klbHyphenBreak;	// worst-case break-weight
	m_availWidth = 1000;			// width available for segment
	m_bold = false;
	m_italic = false;
	m_backtrack = false;
	m_rtl = false;

	//	Output:
	m_noSegment = false;
	m_segWidth = 52;			// physical width of segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true,  false,  true, true, false, false,  false, true
	};

	int glyphCnt = 7;
	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{105,174,158,202,231,162,231};
	int xPositions[] =  {  0, 17, 20, 22, 28, 29, 38};
	int yPositions[] =  {  0,  0,  0,  0,  0,  0,  0};
	int advWidths[] =   { 17,  2,  9,  4,  0, 10,  0};

	int bbLefts[] =     {  0, 10, 21, 23, 23, 30, 33};
	int bbRights[] =    { 16, 19, 28, 25, 28, 39, 38};
	int bbTops[] =      {  7,  7,  7, -1, 15,  7, 15};
	int bbBottoms[] =   {  0, -7, -1, -7,  9,  0,  9};

	// Each group = char-index, number of glyphs, glyph-indices.
	int charsToGlyphs[] = {
		0, 1, 0,	1, 1, 1,	2, 1, 3,	3, 1, 2,	4, 1, 4,	5, 0,	6, 1, 5,	7, 1, 6
	};
	int c2gCount = sizeof(charsToGlyphs) / sizeof(int);

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetCharsToGlyphs(charsToGlyphs, c2gCount);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
}


void TestCase::SetupBugTest_GentiumNfc()
{
	m_testName = "Bug Test";
	m_traceLog = true;
	m_debug = false;
	m_skip = true;

	//	Input:
	m_fontName = L"GentiumPlus";
	m_fontFile = "gentium_r_gr.ttf";

	wchar_t charData[] = {
//		0x1EBF,		// e-circum-acute 769 (x301)  "g1ebf"
//		0x1EA5,		// a-circum-acute 233 (xE9)	   "g1ea5"
		0x0105,		// a-ogonek
		0x01eb,		// o-ogonek  1741  "g01eb"
		0x01ed,		// o-ogonek-macron
		0x0000 };
	m_text.assign(charData);
	int charCnt = 3;

	m_fontSize = 20;				// font size in points
	m_prefBreak = klbWsBreak;		// preferred break-weight
	m_worstBreak = klbHyphenBreak;	// worst-case break-weight
	m_availWidth = 1000;			// width available for segment
	m_bold = false;
	m_italic = false;
	m_backtrack = false;
	m_rtl = false;

	m_fset[0].id = 1058;	m_fset[0].value = 1;	// small caps
	m_fset[1].id = 1029;	m_fset[1].value = 1;	// viet
	m_fset[2].id = 1043;	m_fset[2].value = 1;	// ogonek
	m_fset[3].id = 0;

	//	Output:
	m_noSegment = false;
	m_segWidth = 52;			// physical width of segment

	// need charCnt elements in this array:
	bool insPtFlags[] = {
		true,  false,  true, true, false, false,  false, true
	};

	int glyphCnt = 3;
	// need glyphCnt elements in these arrays:
	gid16 glyphList[] =	{105,174,158,202,231,162,231};
	int xPositions[] =  {  0, 17, 20, 22, 28, 29, 38};
	int yPositions[] =  {  0,  0,  0,  0,  0,  0,  0};
	int advWidths[] =   { 17,  2,  9,  4,  0, 10,  0};

	int bbLefts[] =     {  0, 10, 21, 23, 23, 30, 33};
	int bbRights[] =    { 16, 19, 28, 25, 28, 39, 38};
	int bbTops[] =      {  7,  7,  7, -1, 15,  7, 15};
	int bbBottoms[] =   {  0, -7, -1, -7,  9,  0,  9};

	// Each group = char-index, number of glyphs, glyph-indices.
	int charsToGlyphs[] = {
		0, 1, 0,	1, 1, 1,	2, 1, 3,	3, 1, 2,	4, 1, 4,	5, 0,	6, 1, 5,	7, 1, 6
	};
	int c2gCount = sizeof(charsToGlyphs) / sizeof(int);

	//	Finish setting up test case.
	SetCharCount(charCnt);
	SetGlyphCount(glyphCnt);
	SetGlyphList(glyphList);
	SetXPositions(xPositions);
	SetYPositions(yPositions);
	SetAdvWidths(advWidths);
	SetCharsToGlyphs(charsToGlyphs, c2gCount);
	SetBBs(bbLefts, bbRights, bbTops, bbBottoms);
	SetInsPtFlags(insPtFlags);
}
