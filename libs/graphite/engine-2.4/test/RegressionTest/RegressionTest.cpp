/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2004 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: RegressionTest.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Main file for the Graphite regression test program.
-------------------------------------------------------------------------------*//*:End Ignore*/

#include "main.h"
#include "MemoryUsage.h"
#include <cstring>

//:>********************************************************************************************
//:>	Global variables
//:>********************************************************************************************
std::ofstream g_strmLog;	// log file output stream
std::ofstream g_strmTrace;	// debugger trace for selected tests
std::ofstream g_strmMemUsage;

int g_errorCount;

// HDC g_hdc;	// device-context for bogus window on which to do drawing

bool g_debugMode = false;
bool g_silentMode = false;

int g_itcaseStart = 0;  // adjust to skip to a certain test

SegmentMemoryUsage g_smu;
FontMemoryUsage g_fmu;

std::string g_fontPath = ".";
#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

// Forward defintions.
int WriteLog(int);
void CopyWstringToUtf16(std::wstring textStr, gr::utf16 * utf16Buf, int bufSize);

//:>********************************************************************************************
//:>	Functions
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Main function.
----------------------------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
#ifdef _MSC_VER
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif // WIN32

	int iargc = 1;
	while (iargc < argc)
	{
		if ((strcmp(argv[iargc], "/d") == 0) || (strcmp(argv[iargc], "-d") == 0))
		{
			g_debugMode = true;
		}
		if ((strcmp(argv[iargc], "/s") == 0) || (strcmp(argv[iargc], "-s") == 0))
		{
			g_silentMode = true;
		}
		if ((strcmp(argv[iargc], "/p") == 0) || (strcmp(argv[iargc], "-p") == 0))
		{
			iargc++;
			if (iargc < argc)
			{
				g_fontPath = argv[iargc];
			}
		}
		iargc++;
	}

	if (!g_silentMode)
	{
		std::cout << "Graphite Regression Test\n";
		std::cout << "Files path is " << g_fontPath << "\n";
		if (g_debugMode)
			std::cout << "In debug mode\n";

		int nMajorVersion;
		int nMinorVersion;
		int nBugFixVersion;
		gr::EngineVersion(&nMajorVersion, &nMinorVersion, &nBugFixVersion);
		std::cout << "Engine Version is " << nMajorVersion << "." << nMinorVersion << "." << nBugFixVersion << "\n\n";
	}

	//	Start a new log.
	g_strmLog.open("grregtest.log");
	if (g_strmLog.fail())
	{
		std::cout << "Unable to open log file.";
		return -1;
	}

	g_errorCount = 0;

//	g_hdc = ::GetDC(NULL);

	WriteToLog("Graphite Regression Test\n\n");

	// Initialize the tracelog file.
	g_strmTrace.open("tracelog.txt");
	g_strmTrace.close();

	TestCase * ptcaseList = NULL;
	int numberOfTests = TestCase::SetupTests(&ptcaseList);

	RunTests(numberOfTests, ptcaseList);

	WriteToLog("\n==============================================\n");
	g_strmLog << "\n\nTOTAL NUMBER OF ERRORS:  " << g_errorCount << "\n";
	if (!g_silentMode)
		std::cout << "\n\nTOTAL NUMBER OF ERRORS:  " << g_errorCount << "\n";

	g_strmLog.close();

	////EngineInstance::DeleteEngines();
	TestCase::DeleteTests();

//	::ReleaseDC(NULL, g_hdc);

	// Output segment memory usage information.
	//g_strmMemUsage.open("SegMemoryUsage.log");
	//g_smu.prettyPrint(g_strmMemUsage);
	//g_strmMemUsage.close();

	return g_errorCount;
}

/*----------------------------------------------------------------------------------------------
	Run the tests.
----------------------------------------------------------------------------------------------*/
void RunTests(int numberOfTests, TestCase * ptcaseList)
{
	Segment * psegPrev = NULL;
	RtTextSrc * ptsrcPrev = NULL;
	Segment * psegRet = NULL;
	RtTextSrc * ptsrcRet = NULL;

	for (int itcase = g_itcaseStart; itcase < numberOfTests; itcase++)
	{
		TestCase * ptcase = ptcaseList + itcase;
		WriteToLog("\n----------------------------------------------\n");
		WriteToLog("Test: ");

		if (!g_silentMode)
			std::cout << "Test " << ptcase->TestName() << "...";
		WriteToLog(ptcase->TestName());
		WriteToLog("\n");

		RunOneTestCase(ptcase, psegPrev, &psegRet, &ptsrcRet);

		delete psegPrev;
		delete ptsrcPrev;
		psegPrev = psegRet;
		ptsrcPrev = ptsrcRet;
		psegRet = NULL;
		ptsrcRet = NULL;
	}

	delete psegPrev;
	delete ptsrcPrev;
}

/*----------------------------------------------------------------------------------------------
	Run a single test case.
----------------------------------------------------------------------------------------------*/
int RunOneTestCase(TestCase * ptcase, Segment * psegPrev, Segment ** ppsegRet, RtTextSrc ** pptsrcRet)
{
	if (ptcase->Skip())
	{
		std::cout << "\nskipped\n";
		g_strmLog << "skipping\n";
		*ppsegRet = NULL;
		return 0;
	}

#ifdef _WIN32
	// Break into the debugger if requested.
	if (ptcase->RunDebugger() && ::IsDebuggerPresent())
	{
		::DebugBreak();
	}
#endif

	int errorCount = 0;

	gr::utf16 text[1024];
	CopyWstringToUtf16(ptcase->Text(), text, 1024);

	*ppsegRet = NULL;
	SegmentPainter * ppainter = NULL;
	std::pair<GlyphIterator, GlyphIterator> iterPair;
	GlyphIterator gitBegin;
	GlyphIterator gitEnd;

	//LOGFONT lf;
	//memset(&lf, '\0', sizeof(LOGFONT));
	//lf.lfCharSet = DEFAULT_CHARSET;
	//lf.lfHeight = (signed(ptcase->FontSize()) * -96) / 72;	// 96 = resolution, 72 = points per inch
	//lf.lfWeight = 400;
	//lf.lfItalic = FALSE;
	//wcscpy(lf.lfFaceName, ptcase->FontName().data());
	//HDC hdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	//HFONT hfont = CreateFontIndirect(&lf);
	//HFONT hfontOld = (HFONT)::SelectObject(hdc, hfont); // restore before destroying the DC.
	//WinFont winfont(hdc);

	FileFont font(g_fontPath + PATH_SEP + ptcase->FontFile(), float(signed(ptcase->FontSize())), 96, 96);

	if (!font.isValid())
	{
		OutputError(ptcase, std::string("ERROR: reading font file: \"") + ptcase->FontFile() + "\" at path \"" + g_fontPath + "\"; remaining tests skipped");
		*ppsegRet = NULL;
		errorCount++;
		return WriteLog(errorCount);
	}
	
	//	Set up the text source.
	*pptsrcRet = new RtTextSrc(&(text[0]));
	RtTextSrc * ptsrc = *pptsrcRet;
	(*pptsrcRet)->setFeatures(ptcase->Features());
	(*pptsrcRet)->setRightToLeft(ptcase->Rtl());

	//	Generate a segment.
	LayoutEnvironment layout;
	layout.setDumbFallback(ptcase->DumbFallback());
	layout.setStartOfLine(true);
	layout.setRightToLeft(ptcase->ParaRtl());
	if (ptcase->InitWithPrevSeg())
		layout.setPrevSegment(psegPrev);

	if (ptcase->TraceLog())
	{
		g_strmTrace.open("tracelog.txt", std::ios::app);
		g_strmTrace << "Test: " << ptcase->TestName() << "\n\n";
		layout.setLoggingStream(&g_strmTrace);
	}

	try
	{
		if (ptcase->AvailWidth() < 10000)
		{
			layout.setBestBreak(ptcase->PrefBreak());
			layout.setWorstBreak(ptcase->WorstBreak());
			layout.setTrailingWs(ptcase->Twsh());
			*ppsegRet = new LineFillSegment(&font, ptsrc, &layout,
				ptcase->FirstChar(), ptsrc->getLength(),
				(float)ptcase->AvailWidth(),
				ptcase->Backtrack());
		}
		else
		{
			*ppsegRet = new RangeSegment(&font, ptsrc, &layout);
		}
	}
	catch (...)
	{
		if (!ptcase->NoSegment())
		{
			if (ptcase->BadFont() && !ptcase->DumbFallback())
				// Weird situation.
				OutputError(ptcase, "ERROR: bad font with no fallback, yet a segment was expected???");
			else
				OutputError(ptcase, "ERROR: generating segment; remaining tests skipped");
			errorCount++;
		}
		// else segment creation failed as expected

		// NB: failure to create a segment due to throwing an exception seems to result in a memory
		// leak--the FontCache object FontFace::s_pFontCache does not get deleted.

		*ppsegRet = NULL;
		return WriteLog(errorCount);
	}

	if (ptcase->TraceLog())
	{
		g_strmTrace << "\n\n*********************************************************************************************************************\n\n";
		g_strmTrace.close();
	}

	if ((*ppsegRet) && (*ppsegRet)->segmentTermination() == kestNothingFit)
	{
		delete *ppsegRet;
		*ppsegRet = NULL;
	}

	if ((*ppsegRet) == NULL && !ptcase->NoSegment())
	{
		OutputError(ptcase, "ERROR: segment not created; remaining tests skipped");
		errorCount++;
		return WriteLog(errorCount);
	}
	else if (ptcase->NoSegment())
	{
		if (*ppsegRet)
		{
			OutputError(ptcase, "ERROR: segment created when none expected");
			errorCount++;
		}
		delete *ppsegRet;
		*ppsegRet = NULL;
		return WriteLog(errorCount);
	}

	Segment * pseg = *ppsegRet;

	pseg->calculateMemoryUsage(g_smu);

	// Calculate and output font memory usage.
	//g_fmu = Font::calculateMemoryUsage();
	//g_strmMemUsage.open("fontMemoryUsage.log");
	//g_fmu.prettyPrint(g_strmMemUsage);
	//g_strmMemUsage.close();	

	//	Test results.
	int segMin = pseg->startCharacter();
	int segLim = pseg->stopCharacter();
	if ((segLim - segMin) != ptcase->CharCount())
	{
		OutputErrorWithValues(ptcase, "ERROR: number of characters in segment", -1,
			(segLim - segMin), ptcase->CharCount());
		errorCount++;
	}

	int segWidth = (int)pseg->advanceWidth();
	if (segWidth != ptcase->SegWidth())
	{
		OutputErrorWithValues(ptcase, "ERROR: width of segment", -1,
			segWidth, ptcase->SegWidth());
		errorCount++;
	}

	iterPair = pseg->glyphs();
	gitBegin = iterPair.first;
	gitEnd   = iterPair.second;
	int cGlyphs = gitEnd - gitBegin;
	if (cGlyphs != ptcase->GlyphCount())
	{
		OutputErrorWithValues(ptcase, "ERROR: number of glyphs", -1,
			cGlyphs, ptcase->GlyphCount());
		errorCount++;
	}
	else
	{
		GlyphIterator gitThis  = gitBegin;
		for (int iglyph = 0; gitThis != gitEnd; ++gitThis, iglyph++)
		{
			if ((*gitThis).glyphID() != ptcase->GlyphID(iglyph))
			{
				OutputErrorWithValues(ptcase, "ERROR: glyph id ", iglyph,
					(*gitThis).glyphID(), ptcase->GlyphID(iglyph));
				errorCount++;
			}
			if (int((*gitThis).origin()) != ptcase->XPos(iglyph))
			{
				OutputErrorWithValues(ptcase, "ERROR: glyph x-position ", iglyph,
					int((*gitThis).origin()), ptcase->XPos(iglyph));
				errorCount++;
			}
			if (int((*gitThis).yOffset()) != ptcase->YPos(iglyph))
			{
				OutputErrorWithValues(ptcase, "ERROR: glyph y-position ", iglyph,
					int((*gitThis).yOffset()), ptcase->YPos(iglyph));
				errorCount++;
			}
			if (int((*gitThis).advanceWidth()) != ptcase->AdvWidth(iglyph))
			{
				OutputErrorWithValues(ptcase, "ERROR: advance width ", iglyph,
					int((*gitThis).advanceWidth()), ptcase->AdvWidth(iglyph));
				errorCount++;
			}
			if (ptcase->BbTests())
			{
				gr::Rect rectBb = (*gitThis).bb();
				if (int(rectBb.left) != ptcase->BbLeft(iglyph))
				{
					OutputErrorWithValues(ptcase, "ERROR: bb left ", iglyph,
						int(rectBb.left), ptcase->BbLeft(iglyph));
					errorCount++;
				}
				if (int(rectBb.right) != ptcase->BbRight(iglyph))
				{
					OutputErrorWithValues(ptcase, "ERROR: bb right ", iglyph,
						int(rectBb.right), ptcase->BbRight(iglyph));
					errorCount++;
				}
				if (int(rectBb.top) != ptcase->BbTop(iglyph))
				{
					OutputErrorWithValues(ptcase, "ERROR: bb top ", iglyph,
						int(rectBb.top), ptcase->BbTop(iglyph));
					errorCount++;
				}
				if (int(rectBb.bottom) != ptcase->BbBottom(iglyph))
				{
					OutputErrorWithValues(ptcase, "ERROR: bb bottom ", iglyph,
						int(rectBb.bottom), ptcase->BbBottom(iglyph));
					errorCount++;
				}
			}
		}
	}

	ppainter = new SegmentNonPainter(pseg);

	if (segLim == ptcase->CharCount())
	{
		for (int ichar = 0; ichar < segLim; ichar++)
		{
			LgIpValidResult ipvr = ppainter->isValidInsertionPoint(ichar);
			if ((ipvr == kipvrOK && !ptcase->InsPtFlag(ichar)) // TODO: handle kipvrUnknown
				|| (ipvr != kipvrOK && ptcase->InsPtFlag(ichar)))
			{
				OutputErrorWithValues(ptcase, "ERROR: valid insertion point ", ichar,
					(ipvr == kipvrOK), ptcase->InsPtFlag(ichar));
				errorCount++;
			}
		}
	}

	int c2gi = 0;
	while (c2gi < ptcase->CharToGlyphCount())
	{
		int ichar = ptcase->CharToGlyphItem(c2gi++);
		int glyphCount = ptcase->CharToGlyphItem(c2gi++);
		std::pair<gr::GlyphSetIterator, gr::GlyphSetIterator> glyphSet = pseg->charToGlyphs(ichar);
		gr::GlyphSetIterator gitStart = glyphSet.first;
		gr::GlyphSetIterator gitStop = glyphSet.second;
		if ((gitStop - gitStart) != glyphCount)
		{
			OutputErrorWithValues(ptcase, "ERROR: number of glyphs for char ", ichar,
				gitStop - gitStart, glyphCount);
			errorCount++;
			c2gi += glyphCount;
		}
		else
		{
			GlyphSetIterator glyphLp = glyphSet.first;
			for (int ig = 0; ig < glyphCount; ig++)
			{
				if (static_cast<int>((*glyphLp).logicalIndex()) != ptcase->CharToGlyphItem(c2gi))
				{
					OutputErrorWithValues(ptcase, "ERROR: glyph for char ", ichar,
						(*glyphLp).logicalIndex(), ptcase->CharToGlyphItem(c2gi));
					errorCount++;
				}
				c2gi++;
				glyphLp++;
			}
		}
	}

	int att = 0;
	while (att < ptcase->AttachedGlyphCount())
	{
		int iglyph = ptcase->AttachedGlyphItem(att++);
		if (iglyph > cGlyphs)
		{
			OutputError(ptcase, "ERROR: non-existent glyph in attachment test ", iglyph);
			errorCount++;
			att++;
			att += ptcase->AttachedGlyphItem(att) + 1;
			continue;
		}

		GlyphIterator gitThis = gitBegin + iglyph;
		GlyphIterator gitBase = gitThis->attachedClusterBase();
		int ibase = gitBase->logicalIndex();
		if (ibase != ptcase->AttachedGlyphItem(att))
		{
			OutputErrorWithValues(ptcase, "ERROR: attachment base for glyph ", iglyph,
				ibase, ptcase->AttachedGlyphItem(att));
			errorCount++;
		}
		att++;
		std::pair<gr::GlyphSetIterator, gr::GlyphSetIterator> glyphSet = gitThis->attachedClusterGlyphs();
		gr::GlyphSetIterator gitStart = glyphSet.first;
		gr::GlyphSetIterator gitStop = glyphSet.second;
		int glyphCount = ptcase->AttachedGlyphItem(att++);
		if ((gitStop - gitStart) != glyphCount)
		{
			OutputErrorWithValues(ptcase, "ERROR: number of attachments for glyph ", iglyph,
				gitStop - gitStart, glyphCount);
			errorCount++;
			att += glyphCount;
		}
		else
		{
			GlyphSetIterator glyphLp = glyphSet.first;
			for (int ig = 0; ig < glyphCount; ig++)
			{
				if (static_cast<int>((*glyphLp).logicalIndex()) != ptcase->AttachedGlyphItem(att))
				{
					OutputErrorWithValues(ptcase, "ERROR: attachment for glyph ", iglyph,
						(*glyphLp).logicalIndex(), ptcase->AttachedGlyphItem(att));
					errorCount++;
				}
				att++;
				glyphLp++;
			}
		}
	}

	for (int iclicktest = 0; iclicktest < ptcase->NumberOfClickTests(); iclicktest++)
	{
        gr::Point ptClick;
		ptClick.x = static_cast<float>(ptcase->XClick(iclicktest));
		ptClick.y = static_cast<float>(ptcase->YClick(iclicktest));
		int charIndex;
		bool assocPrev;
		ppainter->pointToChar(ptClick, &charIndex, &assocPrev);
		if (charIndex != ptcase->ClickCharIndex(iclicktest))
		{
			OutputErrorWithValues(ptcase, "ERROR: char index from click ", iclicktest,
				charIndex, ptcase->ClickCharIndex(iclicktest));
			errorCount++;
			continue;
		}
		if (assocPrev != ptcase->ClickAssocPrev(iclicktest))
		{
			OutputErrorWithValues(ptcase, "ERROR: assoc-prev from click ", iclicktest,
				assocPrev, ptcase->ClickAssocPrev(iclicktest));
			errorCount++;
		}
		gr::Rect rect1, rect2;
		ppainter->positionsOfIP(charIndex, assocPrev,
			false, &rect1, &rect2);
		if (ptcase->Sel1Top(iclicktest) == TestCase::kAbsent)
		{
			if (rect1.top != 0 || rect1.bottom != 0 || rect1.left != 0 || rect1.right != 0)
			{
				OutputError(ptcase, "ERROR: IP prim rect found ", iclicktest);
				errorCount++;
			}
		}
		else if (rect1.top == 0 && rect1.bottom == 0 && rect1.left == 0 && rect1.right == 0)
		{
			OutputError(ptcase, "ERROR: IP prim rect not found ", iclicktest);
			errorCount++;
		}
		else
		{
			if ((int)rect1.top != ptcase->Sel1Top(iclicktest))
			{
				OutputErrorWithValues(ptcase, "ERROR: IP prim rect top ", iclicktest,
					int(rect1.top), ptcase->Sel1Top(iclicktest));
				errorCount++;
			}
			if ((int)rect1.bottom != ptcase->Sel1Bottom(iclicktest))
			{
				OutputErrorWithValues(ptcase, "ERROR: IP prim rect bottom ", iclicktest,
					int(rect1.bottom), ptcase->Sel1Bottom(iclicktest));
				errorCount++;
			}
			if ((int)rect1.left != ptcase->Sel1Left(iclicktest))
			{
				OutputErrorWithValues(ptcase, "ERROR: IP prim rect left ", iclicktest,
					int(rect1.left), ptcase->Sel1Left(iclicktest));
				errorCount++;
			}
		}

		if (ptcase->Sel2Top(iclicktest) == TestCase::kAbsent)
		{
			if (rect2.top != 0 || rect2.bottom != 0 || rect2.left != 0 || rect2.right != 0)
			{
				OutputError(ptcase, "ERROR: IP sec rect found when not expected ", iclicktest);
				errorCount++;
			}
		}
		else if (rect2.top == 0 && rect2.bottom == 0 && rect2.left == 0 && rect2.right == 0)
		{
			OutputError(ptcase, "ERROR: IP sec rect not found ", iclicktest);
			errorCount++;
		}
		else
		{
			if ((int)rect2.top != ptcase->Sel2Top(iclicktest))
			{
				OutputErrorWithValues(ptcase, "ERROR: IP sec rect top ", iclicktest,
					int(rect2.top), ptcase->Sel2Top(iclicktest));
				errorCount++;
			}
			if ((int)rect2.bottom != ptcase->Sel2Bottom(iclicktest))
			{
				OutputErrorWithValues(ptcase, "ERROR: IP sec rect bottom ", iclicktest,
					int(rect2.bottom), ptcase->Sel2Bottom(iclicktest));
				errorCount++;
			}
			if ((int)rect2.left != ptcase->Sel2Left(iclicktest))
			{
				OutputErrorWithValues(ptcase, "ERROR: IP sec rect left ", iclicktest,
					int(rect2.left), ptcase->Sel2Left(iclicktest));
				errorCount++;
			}
		}
	}

	////if (contextBlockOut != ptcase->OutputContextBlockSize())
	////{
	////	OutputErrorWithValues(ptcase, "ERROR: output context block size ", -1,
	////		(int)contextBlockOut, (int)ptcase->OutputContextBlockSize());
	////	errorCount++;
	////}
	////else if (!ptcase->CompareContextBlock(pContextBlockOut))
	////{
	////	OutputError(ptcase, "ERROR: output context block");
	////	errorCount++;
	////}
	delete ppainter;

    return WriteLog(errorCount);
}


/*----------------------------------------------------------------------------------------------
	Write the error count to the log.
----------------------------------------------------------------------------------------------*/
int WriteLog(int errorCount)
{
	WriteToLog("\nError count = ");
	WriteToLog(errorCount);
	WriteToLog("\n");

	if (!g_silentMode)
	{
		if (errorCount == 0)
			std::cout << "ok\n";
		else
			std::cout << "FAILED\n";
	}


	//delete pseg; // don't delete these; they are passed back to the calling method
	//delete ptsrc;

	// Delete device context
	//DeleteObject(SelectObject(hdc, hfontOld));
	//DeleteDC(hdc);

	g_errorCount += errorCount;
	return errorCount;
}

/*----------------------------------------------------------------------------------------------
	Copy a std::wstring (whose bytes can be of various sizes on different platforms)
	to a buffer of UTF16.
----------------------------------------------------------------------------------------------*/
void CopyWstringToUtf16(std::wstring textStr, gr::utf16 * utf16Buf, int bufSize)
{
	std::fill_n(utf16Buf, bufSize, L'\0');
	int cc = textStr.length();
	for (int i = 0; i < cc; i++)
		utf16Buf[i] = textStr[i];
}

/*----------------------------------------------------------------------------------------------
	Output information about an error.
----------------------------------------------------------------------------------------------*/
void OutputError(TestCase * ptcase, std::string strErr, int i)
{
	OutputErrorAux(ptcase, strErr, i, false, 0, 0);
}

void OutputErrorWithValues(TestCase * ptcase, std::string strErr, int i,
	int valueFound, int valueExpected)
{
	OutputErrorAux(ptcase, strErr, i, true, valueFound, valueExpected);
}

void OutputErrorAux(TestCase * /*ptcase*/, std::string strErr, int i,
	bool showValues, int valueFound, int valueExpected)
{
//	if (g_debugMode)
//		::DebugBreak();

	if (!g_silentMode)
	{
		//std::cout << ptcase->TestName() << ": ";
		std::cout << strErr;
		if (i > -1)
		{
			std::cout << "[" << i << "]";
		}
		std::cout << "\n   ";
	}

	WriteToLog(strErr, i, showValues, valueFound, valueExpected);

	WriteToLog("\n");
}

/*----------------------------------------------------------------------------------------------
	Write some text to the log file.
----------------------------------------------------------------------------------------------*/
bool WriteToLog(std::string str, int i)
{
	if (g_strmLog.fail())
	{
		std::cout << "Error opening log file.";
		return false;
	}
	g_strmLog << str;
	if (i > -1)
		g_strmLog << "[" << i << "]";
	g_strmLog.flush();
	return true;
}

bool WriteToLog(std::string str, int i,
	bool showValues, int valueFound, int valueExpected)
{
	if (g_strmLog.fail())
	{
		std::cout << "Error opening log file.";
		return false;
	}
	g_strmLog << str;
	if (i > -1)
		g_strmLog << "[" << i << "]";
	if (showValues)
	{
		g_strmLog << "; found " << valueFound << " not " << valueExpected;
	}
	g_strmLog.flush();
	return true;
}


bool WriteToLog(int n)
{
	if (g_strmLog.fail())
	{
		std::cout << "Error opening log file.";
		return false;
	}
	g_strmLog << n;
	g_strmLog.flush();
	return true;
}

