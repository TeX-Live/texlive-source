#include "TECkit_Engine.h"
#include "Compiler.h"

#include <iostream>
#include <fstream>
#include <string>

#if __MWERKS__
#if __dest_os == __mac_os
#include "SIOUX.h"
#include "console.h"
#endif
//#include "Profiler.h"
#endif

#ifdef HAVE_CONFIG_H
#	include "config.h"	/* a Unix-ish setup where we have config.h available */
#else
#	if	(defined __dest_os && (__dest_os == __win32_os)) || defined WIN32	/* Windows target: little-endian */
#		undef WORDS_BIGENDIAN
#	else
#		if (defined TARGET_RT_BIG_ENDIAN)	/* the CodeWarrior prefix files set this */
#			if TARGET_RT_BIG_ENDIAN
#				define WORDS_BIGENDIAN 1
#			else
#				undef WORDS_BIGENDIAN
#			endif
#		else
#			error Unsure about endianness!
#		endif
#	endif
#endif

using namespace std;

TECkit_Status
makeConverter(const Byte* table, UInt32 len, bool fwd, UInt16& inForm, UInt16& outForm, TECkit_Converter* cnv)
{
	UInt32	lhsFlags, rhsFlags;
	TECkit_Status	status;
	
	status = fwd
		? TECkit_GetMappingFlags((Byte*)table, len, &lhsFlags, &rhsFlags)
		: TECkit_GetMappingFlags((Byte*)table, len, &rhsFlags, &lhsFlags);
	if (status != kStatus_NoError)
		return status;
	
	if (lhsFlags & kFlags_Unicode) {
		switch (inForm & kForm_EncodingFormMask) {
			case kForm_Unspecified:
				inForm = kForm_UTF8;
				break;
			case kForm_Bytes:
				status = kStatus_InvalidForm;
				break;
		}
	}
	else {
		switch (inForm & kForm_EncodingFormMask) {
			case kForm_Unspecified:
				inForm = kForm_Bytes;
				break;
			case kForm_Bytes:
				break;
			default:
				status = kStatus_InvalidForm;
				break;
		}
	}
	
	if (rhsFlags & kFlags_Unicode) {
		switch (outForm & kForm_EncodingFormMask) {
			case kForm_Unspecified:
				if ((inForm & kForm_EncodingFormMask) == kForm_Bytes)
					outForm |= kForm_UTF8;
				else
					outForm |= (inForm & kForm_EncodingFormMask);
				break;
			case kForm_Bytes:
				status = kStatus_InvalidForm;
				break;
		}
	}
	else {
		switch (outForm & kForm_EncodingFormMask) {
			case kForm_Unspecified:
				outForm |= kForm_Bytes;
				break;
			case kForm_Bytes:
				break;
			default:
				status = kStatus_InvalidForm;
				break;
		}
	}
	
	if (status != kStatus_NoError)
		return status;
	
	status = TECkit_CreateConverter((Byte*)table, len, fwd, inForm, outForm, cnv);
	return status;
}

static int
doConversion(TECkit_Converter cnv, istream& is, ostream& os, UInt32 opt, UInt32& inputLoc, UInt32& lookahead)
{
	UInt32	inBase = is.tellg();
	is.seekg(0, ios::end);
	UInt32	inLength = (UInt32)is.tellg() - inBase;
	is.seekg(inBase, ios::beg);

	UInt32	inBufLen = 10240;
	char*	inBuffer = new char[inBufLen];

	UInt32	outBufLen = 40960;
	char*	outBuffer = new char[outBufLen];

	UInt32	savedLen = 0;
	UInt32	offset = 0;

	TECkit_Status	status = kStatus_NoError;
	while (1) {
		UInt32		inUsed, outUsed;
		UInt32		amountToRead = inBufLen - savedLen;
		const char*	inPtr = inBuffer;

		if (offset + amountToRead > inLength)
			amountToRead = inLength - offset;
		
		UInt32		inAvail = savedLen + amountToRead;
		if (inAvail > 0) {
			is.read(inBuffer + savedLen, amountToRead);
			offset += amountToRead;

			status = TECkit_ConvertBufferOpt(cnv, (const Byte*)inPtr, inAvail, &inUsed,
											(Byte*)outBuffer, outBufLen, &outUsed, opt, &lookahead);
			os.write(outBuffer, outUsed);
			
			switch (status & kStatusMask_Basic) {
				case kStatus_OutputBufferFull:
				case kStatus_NeedMoreInput:
					if (inUsed < inAvail) {
						savedLen = inAvail - inUsed;
						std::memcpy(inBuffer, inBuffer + inAvail - savedLen, savedLen);
					}
					else
						savedLen = 0;
					continue;
				
				case kStatus_UnmappedChar:
					inputLoc = offset - amountToRead + inUsed;
					break;

				default:
					cerr << "bad returned status from TECkit_ConvertBuffer: " << hex << status << endl;
					break;
			}
		}

		// flush the converter
		if ((status & kStatusMask_Basic) != kStatus_UnmappedChar) {
			inputLoc = inLength;
			do {
				status = TECkit_FlushOpt(cnv, (Byte*)outBuffer, outBufLen, &outUsed, opt, &lookahead);
				os.write(outBuffer, outUsed);
			} while ((status & kStatusMask_Basic) == kStatus_OutputBufferFull);
			if ((status & kStatusMask_Basic) != kStatus_NoError)
				cerr << "bad returned status from TECkit_Flush: " << hex << status << endl;
		}

		break;
	}
	
	delete[] inBuffer;
	delete[] outBuffer;
	
	return status;
}

int
main(int argc, char** argv)
{
#if __MWERKS__ && __dest_os == __mac_os
	SIOUXSettings.asktosaveonclose = 0;
	argc = ccommand(&argv);
#endif

	char*	mapFileName = 0;
	char*	tecFileName = 0;
	char*	inFileName = 0;
	char*	outFileName = 0;
	bool	forward = true;
	UInt16	inForm = kForm_Unspecified;
	UInt16	outForm = kForm_Unspecified;
	bool	compress = false;
	UInt16	norm = 0;
	UInt32	opt = kOptionsUnmapped_DontUseReplacementChar;
	bool	bom = true;

	while (--argc) {
		char*	arg = *++argv;
		if (arg[0] == '-') {
			if (strlen(arg + 1) == 1) {
				switch (arg[1]) {
					case 'm':
						mapFileName = *++argv;
						--argc;
						break;
					case 't':
						tecFileName = *++argv;
						--argc;
						break;
					case 'z':
						compress = true;
						tecFileName = *++argv;
						--argc;
						break;
					case 'i':
						inFileName = *++argv;
						--argc;
						break;
					case 'o':
						outFileName = *++argv;
						--argc;
						break;
					case 'r':
						forward = false;
						break;
					case 'u':
						opt = atoi(*++argv);
						--argc;
						break;
				}
			}
			else if (strcmp(arg + 1, "nobom") == 0)
				bom = false;
			else if (strcmp(arg + 1, "nfc") == 0)
				norm = kForm_NFC;
			else if (strcmp(arg + 1, "nfd") == 0)
				norm = kForm_NFD;
			else if (strcmp(arg + 1, "if") == 0) {
				++argv;
				--argc;
				if (strcmp(*argv, "bytes") == 0)
					inForm = kForm_Bytes;
				else if (strcmp(*argv, "utf8") == 0)
					inForm = kForm_UTF8;
				else if (strcmp(*argv, "utf16") == 0)
#if TARGET_RT_BIG_ENDIAN
					inForm = kForm_UTF16BE;
#else
					inForm = kForm_UTF16LE;
#endif
				else if (strcmp(*argv, "utf16be") == 0)
					inForm = kForm_UTF16BE;
				else if (strcmp(*argv, "utf16le") == 0)
					inForm = kForm_UTF16LE;
				else if (strcmp(*argv, "utf32") == 0)
#if TARGET_RT_BIG_ENDIAN
					inForm = kForm_UTF32BE;
#else
					inForm = kForm_UTF32LE;
#endif
				else if (strcmp(*argv, "utf32be") == 0)
					inForm = kForm_UTF32BE;
				else if (strcmp(*argv, "utf32le") == 0)
					inForm = kForm_UTF32LE;
			}
			else if (strcmp(arg + 1, "of") == 0) {
				++argv;
				--argc;
				if (strcmp(*argv, "bytes") == 0)
					outForm = kForm_Bytes;
				else if (strcmp(*argv, "utf8") == 0)
					outForm = kForm_UTF8;
				else if (strcmp(*argv, "utf16") == 0)
#if TARGET_RT_BIG_ENDIAN
					outForm = kForm_UTF16BE;
#else
					outForm = kForm_UTF16LE;
#endif
				else if (strcmp(*argv, "utf16be") == 0)
					outForm = kForm_UTF16BE;
				else if (strcmp(*argv, "utf16le") == 0)
					outForm = kForm_UTF16LE;
				else if (strcmp(*argv, "utf32") == 0)
#if TARGET_RT_BIG_ENDIAN
					outForm = kForm_UTF32BE;
#else
					outForm = kForm_UTF32LE;
#endif
				else if (strcmp(*argv, "utf32be") == 0)
					outForm = kForm_UTF32BE;
				else if (strcmp(*argv, "utf32le") == 0)
					outForm = kForm_UTF32LE;
			}
		}
	}

	Byte*	compiledTable = 0;
	UInt32	compiledSize = 0;
	TECkit_Status	status = kStatus_NoError;
		
	if (mapFileName != 0) {
		// compile the mapping
		ifstream	is(mapFileName, ios::in | ios::binary);
		is.seekg(0, ios::end);
		UInt32	len = is.tellg();
		is.seekg(0, ios::beg);
		
		char*	txt = new char[len];
		is.read(txt, len);
		is.close();
		
		status = TECkit_CompileOpt(txt, len, 0, 0, &compiledTable, &compiledSize, compress ? kCompilerOpts_Compress : 0);
		delete[] txt;
		
		if (status == kStatus_NoError) {
			if (tecFileName != 0) {
				// save the compiled mapping
				ofstream	os(tecFileName, ios::out | ios::trunc | ios::binary);
				os.write((char*)compiledTable, compiledSize);
				os.close();
			}
		}
		else {
			cerr << "compilation failed" << endl;
			return 1;
		}
		
//		return 0;
	}

	if (inFileName != 0 && outFileName != 0) {
		// open input file and (if inForm is unspecified) try to guess the encoding form,
		// or skip over the BOM if present
		ifstream	is(inFileName, ios::in | ios::binary);
		char	c1, c2, c3, c4;
		switch (inForm) {
			case kForm_Unspecified:
				is >> c1;
                                is >> c2;
                                is >> c3;
                                is >> c4;
				if (c1 == (char)0xef && c2 == (char)0xbb && c3 == (char)0xbf) {
					inForm = kForm_UTF8;
					is.seekg(3, ios::beg);
				}
				else if (c1 == (char)0xfe && c2 == (char)0xff) {
					inForm = kForm_UTF16BE;
					is.seekg(2, ios::beg);
				}
				else if (c1 == (char)0x00 && c2 == (char)0x00 && c3 == (char)0xfe && c4 == (char)0xff) {
					inForm = kForm_UTF32BE;
				//	is.seekg(4, ios::beg);
				}
				else if (c1 == (char)0xff && c2 == (char)0xfe && c3 == (char)0x00 && c4 == (char)0x00) {
					inForm = kForm_UTF32LE;
				//	is.seekg(4, ios::beg);
				}
				else if (c1 == (char)0xff && c2 == (char)0xfe) {
					inForm = kForm_UTF16LE;
					is.seekg(2, ios::beg);
				}
				else {
					is.seekg(0, ios::beg);
				}
				break;
		
			case kForm_UTF8:
				is >> c1 >> c2 >> c3;
				if (!(c1 == (char)0xef && c2 == (char)0xbb && c3 == (char)0xbf))
					is.seekg(0, ios::beg);
				break;

			case kForm_UTF16BE:
				is >> c1 >> c2;
				if (!(c1 == (char)0xfe && c2 == (char)0xff))
					is.seekg(0, ios::beg);
				break;

			case kForm_UTF16LE:
				is >> c1 >> c2;
				if (!(c1 == (char)0xff && c2 == (char)0xfe))
					is.seekg(0, ios::beg);
				break;

			case kForm_UTF32BE:
				is >> c1 >> c2 >> c3 >> c4;
				if (!(c1 == (char)0x00 && c2 == (char)0x00 && c3 == (char)0xfe && c4 == (char)0xff))
					is.seekg(0, ios::beg);
				break;

			case kForm_UTF32LE:
				is >> c1 >> c2 >> c3 >> c4;
				if (!(c1 == (char)0xff && c2 == (char)0xfe && c3 == (char)0x00 && c4 == (char)0x00))
					is.seekg(0, ios::beg);
				break;
		}
		outForm |= norm;

		TECkit_Converter	cnv;
		if (compiledTable != 0)
			status = makeConverter(compiledTable, compiledSize, forward, inForm, outForm, &cnv);
		else if (tecFileName != 0) {
			// load and use a compiled mapping
			ifstream	is(tecFileName, ios::in | ios::binary);
			is.seekg(0, ios::end);
			UInt32	len = is.tellg();
			is.seekg(0, ios::beg);

			char*	table = new char[len];
			is.read(table, len);
			is.close();
			status = makeConverter((Byte*)table, len, forward, inForm, outForm, &cnv);
			delete[] table;
		}
		else {
			if ((outForm & kForm_EncodingFormMask) == kForm_Unspecified)
				outForm |= inForm;
			status = TECkit_CreateConverter(0, 0, 0, inForm, outForm, &cnv);
		}
		
		if (status != kStatus_NoError)
			cerr << "bad returned status from TECkit_CreateConverter: " << status << endl;
		else {
			ofstream	os(outFileName, ios::out | ios::trunc | ios::binary);
			if (bom) {
				switch (outForm & kForm_EncodingFormMask) {
					case kForm_UTF8:
						os << (char)0xef << (char)0xbb << (char)0xbf;
						break;

					case kForm_UTF16BE:
						os << (char)0xfe << (char)0xff;
						break;

					case kForm_UTF16LE:
						os << (char)0xff << (char)0xfe;
						break;

					case kForm_UTF32BE:
						os << (char)0x00 << (char)0x00 << (char)0xfe << (char)0xff;
						break;

					case kForm_UTF32LE:
						os << (char)0xff << (char)0xfe << (char)0x00 << (char)0x00;
						break;
				}
			}
			UInt32	offs, la;
			status = doConversion(cnv, is, os, opt, offs, la);
			if ((status & kStatusMask_Basic) == kStatus_UnmappedChar)
				cerr << "halted at unmappable character, within " << la << " characters before offset " << offs << endl;
			else if ((status & kStatusMask_Warning) == kStatus_UsedReplacement)
				cerr << "warning: unmappable input replaced by default replacement character" << endl;
			status = TECkit_DisposeConverter(cnv);
			if (status != kStatus_NoError)
				cerr << "bad returned status from TECkit_DisposeConverter: " << status << endl;
		}
	}
	
	TECkit_DisposeCompiled(compiledTable);
	return status;
}
