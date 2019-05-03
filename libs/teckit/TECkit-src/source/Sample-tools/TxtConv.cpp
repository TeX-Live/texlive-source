/*
	TxtConv.c
	Copyright (c) 2002-2016 SIL International.

	2008-01-23  jk  revised endian-ness stuff to allow Universal build
	 5-May-2005		jk	added include <stdlib.h> and <string.h> to keep gcc happy
	10-Mar-2004		jk	added -u option to control handling of unmappable input
*/

#include "TECkit_Engine.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if __MWERKS__
#if __dest_os == __mac_os
#include "SIOUX.h"
#include "console.h"
#endif
#endif

#ifdef HAVE_CONFIG_H
#	include "config.h"	/* a Unix-ish setup where we have config.h available */
#endif

#if	(defined(__dest_os) && (__dest_os == __win32_os)) || defined(WIN32)	/* Windows target: little-endian */
#	undef WORDS_BIGENDIAN
#endif

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#if defined(TARGET_RT_BIG_ENDIAN)	/* the CodeWarrior prefix files or Apple TargetConditionals.h sets this */
#	if TARGET_RT_BIG_ENDIAN
#		undef WORDS_BIGENDIAN
#		define WORDS_BIGENDIAN 1
#	else
#		undef WORDS_BIGENDIAN
#	endif
#endif

#define	kInBufLen	4096
#define kOutBufLen	(kInBufLen * 4)

static int
doConversion(TECkit_Converter cnv, FILE* inFile, FILE* outFile, UInt32 opts)
{
	UInt32			inBase;
	UInt32			inLength;
	char			inBuffer[kInBufLen];
	char			outBuffer[kOutBufLen];
	UInt32			savedLen = 0;
	UInt32			offset = 0;
	TECkit_Status	status = kStatus_NeedMoreInput;

	inBase = ftell(inFile);
	fseek(inFile, 0, SEEK_END);
	inLength = ftell(inFile) - inBase;
	fseek(inFile, inBase, SEEK_SET);

	while (1) {
		UInt32	inUsed = 0, outUsed = 0, lookahead = 0;
		UInt32	amountToRead = kInBufLen - savedLen;
		char*	inPtr = inBuffer;
		UInt32	inAvail = savedLen;
		
		if (offset + amountToRead > inLength)
			amountToRead = inLength - offset;
		
		if (amountToRead > 0) {
			amountToRead = fread(inBuffer + savedLen, 1, amountToRead, inFile);
			offset += amountToRead;
			inAvail += amountToRead;
		}

		if (inAvail > 0) {
			status = TECkit_ConvertBufferOpt(cnv, reinterpret_cast<Byte*>(inPtr), inAvail, &inUsed,
											reinterpret_cast<Byte*>(outBuffer), kOutBufLen, &outUsed, opts, &lookahead);
			fwrite(outBuffer, 1, outUsed, outFile);
			
			switch (status & kStatusMask_Basic) {
				case kStatus_OutputBufferFull:
				case kStatus_NeedMoreInput:
					if (inUsed < inAvail) {
						savedLen = inAvail - inUsed;
						memcpy(inBuffer, inBuffer + inAvail - savedLen, savedLen);
					}
					else
						savedLen = 0;
					continue;

				case kStatus_UnmappedChar:
					fprintf(stderr, "processing aborted at unmappable character, within %lu characters before file offset %lu\n",
								static_cast<unsigned long>(lookahead), static_cast<unsigned long>(offset - amountToRead + inUsed));
					break;

				default:
					fprintf(stderr, "bad returned status from TECkit_ConvertBuffer: %ld\n", status);
					break;
			}
		}

		if ((status & kStatusMask_Basic) != kStatus_UnmappedChar) {
			// flush the converter
			do {
				status = TECkit_FlushOpt(cnv, reinterpret_cast<Byte*>(outBuffer), kOutBufLen, &outUsed, opts, &lookahead);
				fwrite(outBuffer, 1, outUsed, outFile);
				savedLen -= inUsed;
				inPtr += inUsed;
				inUsed = 0;
			} while ((status & kStatusMask_Basic) == kStatus_OutputBufferFull);

			if ((status & kStatusMask_Basic) == kStatus_UnmappedChar)
				fprintf(stderr, "processing aborted at unmappable character, within %lu characters before end of input\n", static_cast<unsigned long>(lookahead));
			else if ((status & kStatusMask_Basic) != kStatus_NoError)
				fprintf(stderr, "bad returned status from TECkit_Flush: %ld\n", status);
		}
		
		if ((status & kStatusMask_Warning) == kStatus_UsedReplacement)
			fprintf(stderr, "warning: unmapped input replaced by default replacement character\n");

		break;
	}
	
	return ((status & kStatusMask_Basic) != 0)
		? 2
		: (
			((status & kStatusMask_Warning) != 0)
				? 1
				: 0
		  );
}

static int
stringArg(int* pargc, char*** pargv, char** pstr)
{
	int	err = 0;
	if (*pargc == 1) {
		fprintf(stderr, "missing value after %s\n", **pargv);
		err = 1;
	}
	if (*pstr != 0) {
		fprintf(stderr, "repeated argument %s\n", **pargv);
		err = 1;
	}
	*pstr = *++*pargv;
	--*pargc;
	return err;
}

int
main(int argc, char** argv)
{
#if __MWERKS__ && __dest_os == __mac_os
	SIOUXSettings.asktosaveonclose = 0;
	argc = ccommand(&argv);
#endif
	char*	progName = argv[0];
	char*	tecFileName = 0;
	char*	inFileName = 0;
	char*	outFileName = 0;
	char*	nameID = 0;
	char	forward = 1;
	char*	unmappedOption = 0;
	UInt16	inForm = kForm_Unspecified;
	UInt16	outForm = kForm_Unspecified;
	char	cmdLineErr = 0;
	UInt16	normForm = 0;
	char	noBOM = 0;
	UInt32	opts = kOptionsUnmapped_UseReplacementCharSilently;
	int		rval;
	
	UInt32	len = 0;
	char*	table = 0;
	FILE*	tecFile;
	FILE*	inFile;
	FILE*	outFile;
	UInt32	sourceFlags, targetFlags;
	TECkit_Status	status;
	TECkit_Converter	cnv;

	if (TECkit_GetVersion() != kCurrentTECkitVersion) {
		fprintf(stderr, "incorrect TECkit library version\n");
		return 1;
	}

	while (--argc) {
		char*	arg = *++argv;
		if (arg[0] == '-') {
			if (strlen(arg + 1) == 1) {
				switch (arg[1]) {
					case 'n':
						cmdLineErr += stringArg(&argc, &argv, &nameID);
						break;
					case 't':
						cmdLineErr += stringArg(&argc, &argv, &tecFileName);
						break;
					case 'i':
						cmdLineErr += stringArg(&argc, &argv, &inFileName);
						break;
					case 'o':
						cmdLineErr += stringArg(&argc, &argv, &outFileName);
						break;
					case 'u':
						cmdLineErr += stringArg(&argc, &argv, &unmappedOption);
						break;
					case 'r':
						forward = 0;
						break;
				}
			}
			else if (strcmp(arg + 1, "if") == 0) {
				++argv;
				--argc;
				if (strcmp(*argv, "bytes") == 0)
					inForm = kForm_Bytes;
				else if (strcmp(*argv, "utf8") == 0)
					inForm = kForm_UTF8;
				else if (strcmp(*argv, "utf16") == 0)
#ifdef WORDS_BIGENDIAN
					inForm = kForm_UTF16BE;
#else
					inForm = kForm_UTF16LE;
#endif
				else if (strcmp(*argv, "utf16be") == 0)
					inForm = kForm_UTF16BE;
				else if (strcmp(*argv, "utf16le") == 0)
					inForm = kForm_UTF16LE;
				else if (strcmp(*argv, "utf32") == 0)
#ifdef WORDS_BIGENDIAN
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
#ifdef WORDS_BIGENDIAN
					outForm = kForm_UTF16BE;
#else
					outForm = kForm_UTF16LE;
#endif
				else if (strcmp(*argv, "utf16be") == 0)
					outForm = kForm_UTF16BE;
				else if (strcmp(*argv, "utf16le") == 0)
					outForm = kForm_UTF16LE;
				else if (strcmp(*argv, "utf32") == 0)
#ifdef WORDS_BIGENDIAN
					outForm = kForm_UTF32BE;
#else
					outForm = kForm_UTF32LE;
#endif
				else if (strcmp(*argv, "utf32be") == 0)
					outForm = kForm_UTF32BE;
				else if (strcmp(*argv, "utf32le") == 0)
					outForm = kForm_UTF32LE;
			}
			else if (strcmp(arg + 1, "nfc") == 0)
				normForm = kForm_NFC;
			else if (strcmp(arg + 1, "nfd") == 0)
				normForm = kForm_NFD;
			else if (strcmp(arg + 1, "nobom") == 0)
				noBOM = 1;
			else {
				fprintf(stderr, "unknown option %s\n", arg);
				++cmdLineErr;
			}
		}
		else {
			fprintf(stderr, "missing option flag at %s\n", arg);
			++cmdLineErr;
		}
	}
	
	if (unmappedOption != 0) {
		if (strlen(unmappedOption) != 1 || *unmappedOption < '0' || *unmappedOption > '2') {
			fprintf(stderr, "unknown value '%s' for -u option (expected 0-2)\n", unmappedOption);
			++cmdLineErr;
		}
		else {
			switch (*unmappedOption) {
				case '0':
					opts = kOptionsUnmapped_UseReplacementCharSilently;
					break;
				case '1':
					opts = kOptionsUnmapped_UseReplacementCharWithWarning;
					break;
				case '2':
					opts = kOptionsUnmapped_DontUseReplacementChar;
					break;
			}
		}
	}

	if (cmdLineErr != 0 || inFileName == 0 || outFileName == 0) {
		fprintf(stderr, "\
Usage: %s -i inFile -o outFile [-t tecFile] [-r] [-if inForm] [-of outForm] [-nobom] [-nf[cd]] [-u n]\n\
    Required arguments:\n\
        -i <file>   input file\n\
        -o <file>   output file\n\
    Optional arguments:\n\
        -t <file>   compiled TECkit mapping (.tec) file\n\
        -r          reverse (RHS->LHS, or Unicode->Byte) mapping\n\
        -if <form>  input encoding form\n\
        -of <form>  output encoding form\n\
        -nobom      don't write a BOM to Unicode output\n\
        -nf[cd]     apply NFC or NFD normalization to Unicode output\n\
        -u <n>      handling of unmappable input:\n\
                        0 = use replacement character\n\
                        1 = use replacement but generate warning\n\
                        2 = stop conversion\n\
    Encoding forms:\n\
        bytes utf8 utf16be utf16le utf16 utf32be utf32le utf32\n\
", progName);
		exit(cmdLineErr != 0);
	}

	// examine the mapping (if any) to determine the input and output code spaces
	if (tecFileName != 0) {
		tecFile = fopen(tecFileName, "rb");

		if (tecFile == 0) {
			fprintf(stderr, "unable to load mapping table file %s\n", tecFileName);
			return 1;
		}

		fseek(tecFile, 0, SEEK_END);
		len = ftell(tecFile);
		fseek(tecFile, 0, SEEK_SET);

		table = static_cast<char*>(malloc(len));
		if (table == 0) {
			fprintf(stderr, "out of memory!\n");
			fclose(tecFile);
			return 1;
		}

		len = fread(table, 1, len, tecFile);
		fclose(tecFile);

		status = forward
			? TECkit_GetMappingFlags(reinterpret_cast<Byte*>(table), len, &sourceFlags, &targetFlags)
			: TECkit_GetMappingFlags(reinterpret_cast<Byte*>(table), len, &targetFlags, &sourceFlags);
		if (status != kStatus_NoError) {
			fprintf(stderr, "couldn't get encoding flags from mapping\n");
			return 1;
		}
	}
	else {
		sourceFlags = kFlags_Unicode;
		targetFlags = kFlags_Unicode;
	}

	inFile = fopen(inFileName, "rb");
	if (inFile == 0) {
		fprintf(stderr, "unable to open input file \"%s\"\n", inFileName);
		return 1;
	}
	
	if (sourceFlags & kFlags_Unicode) {
		// if the input is supposed to be Unicode, and the inForm is unspecified, try to guess it
		// or skip over the BOM if one is found that matches the specified inForm
		unsigned char	sig[4];
		size_t sigLen;
		if (inForm == kForm_Bytes) {
			fprintf(stderr, "improper input encoding form for this mapping");
			return 1;
		}
		
		switch (inForm) {
			case kForm_Unspecified:
				sigLen = fread(sig, 1, 4, inFile);
				if (sigLen >= 3 && sig[0] == 0xef && sig[1] == 0xbb && sig[2] == 0xbf) {
					inForm = kForm_UTF8;
					fseek(inFile, 3, SEEK_SET);
				}
				else if (sigLen >= 2 && sig[0] == 0xfe && sig[1] == 0xff) {
					inForm = kForm_UTF16BE;
					fseek(inFile, 2, SEEK_SET);
				}
				else if (sigLen >= 4 && sig[0] == 0xff && sig[1] == 0xfe && sig[2] == 0x00 && sig[3] == 0x00) {
					inForm = kForm_UTF32LE;
				/*	fseek(inFile, 4, SEEK_SET);	*/
				}
				else if (sigLen >= 2 && sig[0] == 0xff && sig[1] == 0xfe) {
					inForm = kForm_UTF16LE;
					fseek(inFile, 2, SEEK_SET);
				}
				else if (sigLen >= 4 && sig[0] == 0x00 && sig[1] == 0x00 && sig[2] == 0xfe && sig[3] == 0xff) {
					inForm = kForm_UTF32BE;
				/*	fseek(inFile, 4, SEEK_SET);	*/
				}
				else {
					inForm = kForm_UTF8;
					fseek(inFile, 0, SEEK_SET);
				}
				break;
		
			case kForm_UTF8:
				sigLen = fread(sig, 1, 3, inFile);
				if (sigLen >= 3 && !(sig[0] == 0xef && sig[1] == 0xbb && sig[2] == 0xbf))
					fseek(inFile, 0, SEEK_SET);
				break;

			case kForm_UTF16BE:
				sigLen = fread(sig, 1, 2, inFile);
				if (sigLen >= 2 && !(sig[0] == 0xfe && sig[1] == 0xff))
					fseek(inFile, 0, SEEK_SET);
				break;

			case kForm_UTF16LE:
				sigLen = fread(sig, 1, 2, inFile);
				if (sigLen >= 2 && !(sig[0] == 0xff && sig[1] == 0xfe))
					fseek(inFile, 0, SEEK_SET);
				break;

			case kForm_UTF32BE:
				sigLen = fread(sig, 1, 4, inFile);
				if (sigLen >= 4 && !(sig[0] == 0x00 && sig[1] == 0x00 && sig[2] == 0xfe && sig[3] == 0xff))
					fseek(inFile, 0, SEEK_SET);
				break;

			case kForm_UTF32LE:
				sigLen = fread(sig, 1, 4, inFile);
				if (sigLen >= 4 && !(sig[0] == 0xff && sig[1] == 0xfe && sig[2] == 0x00 && sig[3] == 0x00))
					fseek(inFile, 0, SEEK_SET);
				break;
		}
	}
	else {
		// if the input is not Unicode, the inputForm had better be Bytes
		switch (inForm) {
			case kForm_Unspecified:
				inForm = kForm_Bytes;
				break;
			case kForm_Bytes:
				break;
			default:
				fprintf(stderr, "improper input encoding form for this mapping");
				return 1;
		}
	}

	// choose an output encoding form (if not specified) and write BOM to Unicode output file
	outFile = fopen(outFileName, "wb");
	if (outFile == 0) {
		fprintf(stderr, "unable to open output file \"%s\"\n", outFileName);
		return 1;
	}

	if (targetFlags & kFlags_Unicode) {
		if (outForm == kForm_Bytes) {
			fprintf(stderr, "improper output encoding form for this mapping");
			return 1;
		}
		if (outForm == kForm_Unspecified) {
			if (inForm == kForm_Bytes)
				outForm = kForm_UTF8;
			else
				outForm = inForm;
		}
		if (!noBOM) {
			switch (outForm) {
				case kForm_UTF8:
					fwrite("\xef\xbb\xbf", 1, 3, outFile);
					break;

				case kForm_UTF16BE:
					fwrite("\xfe\xff", 1, 2, outFile);
					break;

				case kForm_UTF16LE:
					fwrite("\xff\xfe", 1, 2, outFile);
					break;

				case kForm_UTF32BE:
					fwrite("\x00\x00\xfe\xff", 1, 4, outFile);
					break;

				case kForm_UTF32LE:
					fwrite("\xff\xfe\x00\x00", 1, 4, outFile);
					break;
			}
		}
	}
	else {
		switch (outForm) {
			case kForm_Unspecified:
				outForm = kForm_Bytes;
				break;
			case kForm_Bytes:
				break;
			default:
				fprintf(stderr, "improper output encoding form for this mapping");
				return 1;
		}
	}

	// OK, we have figured out the input and output encoding forms we want to use;
	// now at last we can instantiate a converter
	status = TECkit_CreateConverter(reinterpret_cast<Byte*>(table), len, forward, inForm, outForm | normForm, &cnv);
	if (table != 0)
		free(table);
	if (status != kStatus_NoError) {
		fprintf(stderr, "bad returned status from TECkit_CreateConverter: %ld\n", status);
		return status;
	}

	// do the actual conversion
	rval = doConversion(cnv, inFile, outFile, opts);
	
	fclose(inFile);
	fclose(outFile);

	status = TECkit_DisposeConverter(cnv);
	if (status != kStatus_NoError) {
		fprintf(stderr, "bad returned status from TECkit_DisposeConverter: %ld\n", status);
		return status;
	}

	return rval;
}
