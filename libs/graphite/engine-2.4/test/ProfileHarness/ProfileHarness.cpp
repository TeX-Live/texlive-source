/*-----------------------------------------------------------------------------
Copyright (C) 2007 SIL International

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: 
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
A simple console app that creates a segment using FileFont and dumps a 
diagnostic table of the resulting glyph vector to the console. 
If graphite has been built with -DTRACING then it will also produce a
diagnostic log of the segment creation in grSegmentLog.txt
-----------------------------------------------------------------------------*/

//#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstring>

#include <graphite/GrClient.h>
#include <graphite/ITextSource.h>
///#include <graphite/IGrJustifier.h>
#include <graphite/IGrEngine.h>
#include <graphite/SegmentAux.h>
#include <graphite/Font.h>
#include <graphite/Segment.h>
#include <graphite/GraphiteProcess.h>
#include <graphite/FileFont.h>

#include "GrUtfTextSrc.h"

typedef std::pair< gr::GlyphIterator, gr::GlyphIterator > GlyphRange;

#ifndef HAVE_STRTOF
float strtof(char * text, char ** /*ignore*/)
{
  return static_cast<float>(atof(text));
}
#endif

#ifndef HAVE_STRTOL
long strtol(char * text, char ** /*ignore*/)
{
  return atol(text);
}
#endif

struct Parameters
{
	const char * textFile;
	const char * fontFile;
	float pointSize;
	int dpi;
	bool lineStart;
	bool lineEnd;
	bool useLineFill;
	bool useCodes;
	float width;
	int textArgIndex;
	gr::utf16 * pText16;
	gr::utf32 * pText32;
	size_t charLength;
	size_t offset;
};

#ifdef HAVE_ICONV
void
convertUtf8ToUtf32(char* pText, Parameters & parameters)
{
	int length = strlen(reinterpret_cast<char*>(pText));
	//gr::utf8 * pText = reinterpret_cast<gr::utf8*>(argv[2]);
	// convert text to utf32 using iconv because its easier to debug string placements
	size_t bytesLeft = length;
	size_t outBytesLeft = (length + 1) * sizeof(gr::utf32);
	size_t outBufferSize = outBytesLeft;
	gr::utf32 * text32 = new gr::utf32[length + 1];
	iconv_t utf8to32 = iconv_open("utf32","utf8");
	assert(utf8to32 != (iconv_t)(-1)); 
	char * pText32 = reinterpret_cast<char*>(&text32[0]);
	size_t convertLength = iconv(utf8to32, &pText, &bytesLeft, &pText32, &outBytesLeft);
	if (convertLength == size_t(-1)) perror("iconv failed:");
	size_t charLength = (outBufferSize - outBytesLeft) / sizeof(gr::utf32);
	assert(convertLength != size_t(-1));
	// size_t offset = 0;
	// offset by 1 to avoid bom
	if (text32[0] == 0xfeff)
	{
		parameters.offset = 1;
		charLength--;
	}
	std::cout << "String has " << charLength << " characters (" << length << " bytes)" << std::endl;
	size_t ci;
	for (ci = 0; ci < 10 && ci < charLength; ci++)
	{
		std::cout << std::setw(4) << ci << '\t';
	}
	std::cout << std::endl;
	for (ci = 0; ci < charLength; ci++)
	{
		std::cout << std::setw(4) << std::hex 
		          << text32[ci+parameters.offset] << '\t';
		if (((ci + 1) % 10) == 0)  std::cout << std::endl;
	}
	std::cout << std::endl;
	parameters.charLength = charLength;
	parameters.pText32 = text32;
	iconv_close(utf8to32);
}
#endif

bool parseArgs(int argc, char *argv[], Parameters & parameters)
{
	int mainArgOffset = 0;
	bool argError = false;
	char* pText = NULL;
	typedef enum 
	{
		NONE,
		POINT_SIZE,
		DPI,
		LINE_START,
		LINE_END,
		LINE_FILL,
		CODES
	} TestOptions;
	TestOptions option = NONE;
	char * pIntEnd = NULL;
	char * pFloatEnd = NULL;
	long lTestSize = 0;
	float fTestSize = 0.0f;
	for (int a = 1; a < argc; a++)
	{
		switch (option)
		{
		case DPI:
			pIntEnd = NULL;
			lTestSize = strtol(argv[a],&pIntEnd, 10);
			if (lTestSize > 0 && lTestSize < INT_MAX && lTestSize != LONG_MAX)
			{
				parameters.dpi = lTestSize;
			}
			else
			{
				fprintf(stderr,"Invalid dpi %s\n", argv[a]);
			}
			option = NONE;
			break;
		case POINT_SIZE:
			pFloatEnd = NULL;
			fTestSize = strtof(argv[a],&pFloatEnd);
			// what is a reasonable maximum here
			if (fTestSize > 0 && fTestSize < 5000.0f)
			{
				parameters.pointSize = fTestSize;
			}
			else
			{
				fprintf(stderr,"Invalid point size %s\n", argv[a]);
				argError = true;
			}
			option = NONE;
			break;
		case LINE_FILL:
			pFloatEnd = NULL;
			fTestSize = strtof(argv[a],&pFloatEnd);
			// what is a good max width?
			if (fTestSize > 0 && fTestSize < 10000)
			{
				parameters.width = fTestSize;
			}
			else
			{
				fprintf(stderr,"Invalid line width %s\n", argv[a]);
				argError = true;
			}
			option = NONE;
			break;
		default:
			option = NONE;
			if (argv[a][0] == '-')
			{
				if (strcmp(argv[a], "-pt") == 0)
				{
					option = POINT_SIZE;
				}
				else if (strcmp(argv[a], "-dpi") == 0)
				{
					option = DPI;
				}
				else if (strcmp(argv[a], "-ls") == 0)
				{
					option = NONE;
					parameters.lineStart = true;
				}
				else if (strcmp(argv[a], "-le") == 0)
				{
					option = NONE;
					parameters.lineEnd = true;
				}
				else if (strcmp(argv[a], "-linefill") == 0)
				{
					option = LINE_FILL;
					parameters.useLineFill = true;
				}
				else if (strcmp(argv[a], "-codes") == 0)
				{
					option = NONE;
					parameters.useCodes = true;
					// must be less than argc
					parameters.pText32 = new gr::utf32[argc];
					printf("\nText codes\n");
				}
				else
				{
					argError = true;
					fprintf(stderr,"Unknown option %s\n",argv[a]);
				}
			}
			else if (mainArgOffset == 0)
			{
				parameters.textFile = argv[a];
				mainArgOffset++;
			}
			else if (mainArgOffset == 1)
			{
				parameters.fontFile = argv[a];
				mainArgOffset++;
			}
			else if (parameters.useCodes)
			{
				pIntEnd = NULL;
				mainArgOffset++;
				gr::utf32 code = strtol(argv[a],&pIntEnd, 16);
				if (code > 0)
				{
					parameters.pText32[parameters.charLength++] = code;
					if (parameters.charLength % 10 == 0)
						printf("%4x\n",code);
					else
						printf("%4x\t",code);
				}
				else
				{
					fprintf(stderr,"Invalid dpi %s\n", argv[a]);
				}
			}
			else if (mainArgOffset == 1)
			{
				mainArgOffset++;
				pText = argv[a];
				parameters.textArgIndex = a;
			}
			else
			{
				argError = true;
				fprintf(stderr,"too many arguments %s\n",argv[a]);
			}
		}
	}
	if (mainArgOffset < 2) argError = true;
	else 
	{
		if (!parameters.useCodes && pText != NULL)
		{
#ifdef HAVE_ICONV
			convertUtf8ToUtf32(pText, parameters);
#else
			fprintf(stderr,"Only the -codes option is supported on Win32\r\n");
			argError = true;
#endif
		}
		else 
		{
			///parameters.pText32[parameters.charLength] = 0;
			printf("\n");
		}
	}
	return (argError) ? false : true;
}


void initParameters(Parameters & parameters)
{
	parameters.textFile = "";
	parameters.fontFile = "";
	parameters.pointSize = 12.0f;
	parameters.dpi = 72;
	parameters.lineStart = false;
	parameters.lineEnd = false;
	parameters.useLineFill = false;
	parameters.useCodes = false;
	parameters.width = 100.0f;
	parameters.pText16 = NULL;
	parameters.pText32 = NULL;
	parameters.textArgIndex = 0;
	parameters.charLength = 0;
	parameters.offset = 0;
}



int runGraphite(Parameters parameters)
{
	int returnCode = 0;
	FILE	* file = fopen(parameters.fontFile, "rb");
	gr::FileFont * fileFont = NULL;

	// Put the text into an array of string, each string is one paragraph.
	std::ifstream textFileStrm(parameters.textFile);
	std::cout << "FILE: " << parameters.textFile << "\n";
	std::string stringArray[100];

	std::stringbuf buf;
	textFileStrm >> &buf;
	std::cout << "read " << buf.str().length() << " characters" << std::endl;
	std::cout << "====BEGIN===" << buf.str() << "====END====" << std::endl;
	const std::string strTemp = buf.str();
	stringArray[0] = strTemp;
	int cpara = 1;

	try
    {
		if (file)
		{
			fileFont = new gr::FileFont(file, parameters.pointSize, parameters.dpi);
			if (!fileFont || !fileFont->isValid())
			{
				fprintf(stderr,"graphitejni:Invalid font!");
				delete fileFont;
				fileFont = NULL;
				return 2;
			}
			printf("Font: %s bold=%d italic=%d %4.1fpt dpi %d\n", 
				parameters.fontFile, 
				fileFont->bold(), fileFont->italic(), 
				parameters.pointSize, parameters.dpi);
			bool isGraphite = fileFont->fontHasGraphiteTables();
			if (!isGraphite)
			{
				fprintf(stderr,"graphitejni: %s does not have graphite tables",
						parameters.fontFile);
				delete fileFont;
				fileFont = NULL;
				return 3;
			}
		}
		else
		{
			fprintf(stderr, "Failed to open %s\n", parameters.fontFile);
			return 4;
		}

		for (int ipara = 0; ipara < cpara; ipara++)
		{
			GrUtfTextSrc textSrc;
			gr::LayoutEnvironment layout;
			
			textSrc.setText(stringArray[ipara].data(), stringArray[ipara].length());
			textSrc.setPointSize(parameters.pointSize);
			textSrc.setFont(fileFont);

			layout.setStartOfLine(parameters.lineStart);
			layout.setEndOfLine(parameters.lineEnd);
			layout.setDumbFallback(true);
			layout.setJustifier(NULL);
			
			gr::Segment * pSegment = NULL;
			//try
			//{
			if (parameters.useLineFill)
			{
				pSegment = new gr::LineFillSegment(fileFont, &textSrc, &layout, 
													0, stringArray[ipara].length(), 
													parameters.width);
				printf("LineFillSegment overing char %d - %d\nline start=%d line end=%d\n", 
						pSegment->startCharacter(), pSegment->stopCharacter(),
						parameters.lineStart, parameters.lineEnd);
			}
			else
			{
				pSegment = new gr::RangeSegment(fileFont, &textSrc, &layout, 
									0, stringArray[ipara].length());
				printf("RangeSegment covering char %d - %d\nline start=%d line end=%d\n", 
						pSegment->startCharacter(), pSegment->stopCharacter(),
						parameters.lineStart, parameters.lineEnd);
			}
			//}
			//catch (...)
			//{
			//	printf("Exception occurred while creating segment\n");
			//	returnCode = 5;
			//	throw;
			//}

			if (!pSegment) return returnCode;

			// for justifier testing
			/*
			gr::GrJustifier justifier;
			pSegment->SetJustifier(&justifier);
			gr::Segment * justified = gr::Segment::JustifiedSegment(*pSegment, 2.0f * advanceWidth);
			delete justified;
			*/
			
			delete pSegment;
		}
	}
	catch (...)
	{
		printf("Exception occurred\n");
		returnCode = 5;
	}

	delete fileFont;

	return returnCode;
}

#ifdef _MSC_VER

int _tmain(int argc, _TCHAR* argv[])
{
	Parameters parameters;
	initParameters(parameters);

	if (!parseArgs(argc, argv, parameters))
	{
		fprintf(stderr,"Usage: %s [options] textfile fontfile \n",argv[0]);
		return 1;
	}
	// UTF16 arguments
	//if (parameters.textArgIndex > 0)
	//{
	//	parameters.pText16 = reinterpret_cast<wchar_t*>(argv[parameters.textArgIndex]);
	//	std::wstring text(parameters.pText16);
	//	parameters.charLength = text.size();
	//	for (int i = 0; i < text.size(); i++)
	//	{
	//		if (i % 10 == 0)
	//			printf("\r\n%4x", text[i]);
	//		else
	//			printf("\t%4x", text[i]);
	//	}
	//}
	//else 
	//{
	//	assert(parameters.pText32);
	//}

	return runGraphite(parameters);
}

#else

int main(int argc, char *argv[])
{
	
	Parameters parameters;
	initParameters(parameters);
	
	if (!parseArgs(argc, argv, parameters))
	{
		fprintf(stderr,"Usage: %s [options] textfile fontfile \n",argv[0]);
		return 1;
	}
	return runGraphite(parameters);
}

#endif
