#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <string>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

#include "TECkit_Engine.h"
#include "ConvertUTF.h"
#include "sfReader.h"

#ifndef platformUTF16
#ifdef __MWERKS__
#if __dest_os==__win32_os
#define	platformUTF16	kForm_UTF16LE
#else
#define	platformUTF16	kForm_UTF16BE
#endif
#endif
#endif

#ifndef platformUTF16
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_RT_BIG_ENDIAN
#define	platformUTF16	kForm_UTF16BE
#else
#define	platformUTF16	kForm_UTF16LE
#endif
#endif
#endif

#ifndef platformUTF16
#include "config.h"
#if WORDS_BIGENDIAN
#define	platformUTF16	kForm_UTF16BE
#else
#define	platformUTF16	kForm_UTF16LE
#endif
#endif

#if HAVE_LIBEXPAT
#include <expat.h>
#else
#include "expat/xmlparse/xmlparse.h"
#endif


char*					gMappingDirectory;

typedef basic_string<UniChar>	ustring;

map<string,string>		sfmMappings;
map<string,string>		inlineMappings;

map<string,string>*		mappings;

string					defaultMapping;
string					sfmMapping;
string					inlineMapping;

ustring					sfmCharsU;
ustring					inlineCharsU;
long					escapeCharU			= 0x5c;
long					inlineEscapeCharU	= -1;
long					startInlineU		= -1;
long					endInlineU			= -1;

const char*				defaultMarkerChars	= "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

map<string,TECkit_Converter>	converters;

enum dir_enum {
	kDirection_Unspecified = 0,
	kDirection_8_U,
	kDirection_U_8
} direction = kDirection_Unspecified;

int	inForm = kForm_Unspecified;
int	outForm = kForm_Unspecified;

static int	sDepth;
static int	sError;

static inline bool
streq(const char* s, const char* t)
{
	return (strcmp(s, t) == 0);
}

static ustring
Utf8ToString(const char* s)
{
	int	len = strlen(s);
	UniChar*	buf = new UniChar[len];
	const Byte*	sourceStart = (Byte*)s;
	UniChar*	targetStart = buf;
	int	status = ConvertUTF8toUTF16(&sourceStart, sourceStart + len, &targetStart, targetStart + len, lenientConversion);
	if (status != conversionOK) {
		fprintf(stderr, "error %d converting UTF-8 to UTF-16\n", status);
		exit(1);
	}
	ustring		ustr(buf, targetStart - buf);
	delete[] buf;
	return ustr;
}

static void
startElement(void* /*userData*/, const XML_Char *name, const XML_Char **atts)
{
	switch (sDepth) {
		case 0:
			if (streq(name, "sfConversion")) {
				while (*atts) {
					const XML_Char*	n = *atts++;
					const XML_Char*	v = *atts++;
					if (streq(n, "defaultMapping")) {
						defaultMapping = v;
					}
					else {
						fprintf(stderr, "unrecognized attribute \"%s\" for <sfConversion>\n", n);
						sError = 1;
						break;
					}
				}
				if (defaultMapping.length() == 0) {
					fprintf(stderr, "<sfConversion> requires \"defaultMapping\" attribute\n");
					sError = 1;
					break;
				}
			}
			else {
				fprintf(stderr, "expected <sfConversion>, not <%s>, as top-level element in control file\n", name);
				sError = 1;
				break;
			}
			break;

		case 1:
			if (streq(name, "sfMarkers")) {
				while (*atts) {
					const XML_Char*	n = *atts++;
					const XML_Char*	v = *atts++;
					if (streq(n, "escape")) {
						ustring	u = Utf8ToString(v);
						if (u.length() != 1) {
							fprintf(stderr, "\"escape\" attribute of <sfMarkers> must be a single character\n");
							sError = 1;
							break;
						}
						escapeCharU = u[0];
					}
					else if (streq(n, "chars")) {
						sfmCharsU = Utf8ToString(v);
					}
					else if (streq(n, "mapping")) {
						sfmMapping = v;
					}
					else {
						fprintf(stderr, "unrecognized attribute \"%s\" for <sfMarkers>\n", n);
						sError = 1;
						break;
					}
				}
				if (sfmMapping.length() == 0)
					sfmMapping = defaultMapping;
				if (sfmCharsU.length() == 0) {
					sfmCharsU.reserve(96);
					for (const char* cp = defaultMarkerChars; *cp; ++cp)
						sfmCharsU.append(1, *cp);
				}
				mappings = &sfmMappings;
			}
			else if (streq(name, "inlineMarkers")) {
				while (*atts) {
					const XML_Char*	n = *atts++;
					const XML_Char*	v = *atts++;
					if (streq(n, "escape")) {
						ustring	u = Utf8ToString(v);
						if (u.length() != 1) {
							fprintf(stderr, "\"escape\" attribute of <inlineMarkers> must be a single character\n");
							sError = 1;
							break;
						}
						inlineEscapeCharU = u[0];
					}
					else if (streq(n, "start")) {
						ustring	u = Utf8ToString(v);
						if (u.length() != 1) {
							fprintf(stderr, "\"start\" attribute of <inlineMarkers> must be a single character\n");
							sError = 1;
							break;
						}
						startInlineU = u[0];
					}
					else if (streq(n, "end")) {
						ustring	u = Utf8ToString(v);
						if (u.length() != 1) {
							fprintf(stderr, "\"end\" attribute of <inlineMarkers> must be a single character\n");
							sError = 1;
							break;
						}
						endInlineU = u[0];
					}
					else if (streq(n, "chars")) {
						inlineCharsU = Utf8ToString(v);
					}
					else if (streq(n, "mapping")) {
						inlineMapping = v;
					}
					else {
						fprintf(stderr, "unrecognized attribute \"%s\" for <inlineMarkers>\n", n);
						sError = 1;
						break;
					}
				}
				if (inlineMapping.length() == 0)
					inlineMapping = defaultMapping;
				if (inlineCharsU.length() == 0) {
					inlineCharsU.reserve(96);
					for (const char* cp = defaultMarkerChars; *cp; ++cp)
						inlineCharsU.append(1, *cp);
				}
				mappings = &inlineMappings;
			}
			else {
				fprintf(stderr, "unrecognized element <%s> in <sfConversion>\n", name);
				sError = 1;
				break;
			}
			break;

		case 2:
			if (streq(name, "marker")) {
				string	marker;
				string	mapping;
				while (*atts) {
					const XML_Char*	n = *atts++;
					const XML_Char*	v = *atts++;
					if (streq(n, "name")) {
						marker = v;
					}
					else if (streq(n, "mapping")) {
						mapping = v;
					}
					else {
						fprintf(stderr, "unrecognized attribute \"%s\" for <marker>\n", n);
						sError = 1;
						break;
					}
				}
				if (marker.length() == 0 || mapping.length() == 0) {
					fprintf(stderr, "<marker> requires \"name\" and \"mapping\" attributes\n");
					sError = 1;
					break;
				}
				if ((*mappings).find(marker) != (*mappings).end()) {
					fprintf(stderr, "duplicate entry for marker \"%s\"\n", marker.c_str());
					sError = 1;
					break;
				}
				(*mappings)[marker] = mapping;
			}
			else {
				fprintf(stderr, "unrecognized element <%s>\n", name);
				sError = 1;
				break;
			}
			break;

		default:
			fprintf(stderr, "control file elements nested improperly at <%s>\n", name);
			sError = 1;
			break;
	}
	
	++sDepth;
}

static void
endElement(void* /*userData*/, const XML_Char */*name*/)
{
	--sDepth;
}

bool	bom = false;

static TECkit_Converter
makeConverter(const string& mappingName, int direction)
{
	string	mapFileName;
	if (gMappingDirectory)
		mapFileName += gMappingDirectory;
	mapFileName += mappingName;
	mapFileName += ".tec";

	FILE*	mapFile = fopen(mapFileName.c_str(), "rb");
	if (mapFile == 0) {
		fprintf(stderr, "unable to read mapping file for %s (file %s)\n", mappingName.c_str(), mapFileName.c_str());
		exit(1);
	}
	
	fseek(mapFile, 0, SEEK_END);
	long	fileSize = ftell(mapFile);
	fseek(mapFile, 0, SEEK_SET);
	
	unsigned char*	buf = (unsigned char*)malloc(fileSize);
	if (buf == 0) {
		fprintf(stderr, "unable to read mapping file for %s (file %s)\n", mappingName.c_str(), mapFileName.c_str());
		exit(1);
	}
	fread(buf, 1, fileSize, mapFile);
	fclose(mapFile);
	
	TECkit_Converter	converter;
	TECkit_Status		status = TECkit_CreateConverter(buf, fileSize,
														(direction == kDirection_8_U),
														(direction == kDirection_8_U) ? kForm_Bytes	: platformUTF16,
														(direction == kDirection_8_U) ? outForm		: kForm_Bytes,
														&converter);
	if (status != kStatus_NoError) {
		fprintf(stderr, "failed to create converter for %s (file %s)\n", mappingName.c_str(), mapFileName.c_str());
		exit(1);
	}
	
	free(buf);
	
	return converter;
}

static bool
read_control_file(const char* controlFile)
{
	FILE*	ctlFile = fopen(controlFile, "r");
	if (ctlFile == 0) {
		fprintf(stderr, "unable to open control file %s\n", controlFile);
		return false;
	}

	char buf[BUFSIZ];
	XML_Parser parser = XML_ParserCreate(0);
	int done;
	
	XML_SetElementHandler(parser, startElement, endElement);
	
	int	status = 0;
	do {
		long	len = fread(buf, 1, sizeof(buf), ctlFile);
		done = len < sizeof(buf);
		if (!XML_Parse(parser, buf, len, done)) {
			fprintf(stderr, "XML parse error: %s at line %d\n", XML_ErrorString(XML_GetErrorCode(parser)), XML_GetCurrentLineNumber(parser));
			status = 1;
		}
	} while (!status && !done);

	fclose(ctlFile);

	XML_ParserFree(parser);
	
	if (status != 0)
		return false;
	
	converters[defaultMapping] = makeConverter(defaultMapping, direction);

	if (converters.find(sfmMapping) == converters.end())
		converters[sfmMapping] = makeConverter(sfmMapping, direction);
	if (inlineMapping.length() > 0 && converters.find(inlineMapping) == converters.end())
		converters[sfmMapping] = makeConverter(inlineMapping, direction);

	for (map<string,string>::const_iterator	i = sfmMappings.begin(); i != sfmMappings.end(); ++i) {
		if (converters.find(i->second) == converters.end())
			converters[i->second] = makeConverter(i->second, direction);
	}
	for (map<string,string>::const_iterator	i = inlineMappings.begin(); i != inlineMappings.end(); ++i) {
		if (converters.find(i->second) == converters.end())
			converters[i->second] = makeConverter(i->second, direction);
	}
	return true;
}

static void
write_converted(const Byte* data, long nBytes, TECkit_Converter converter, FILE* outFile)
{
static Byte*	convBuffer = 0;
static UInt32	bufferSize = 0;

	UInt32	reqSpace = nBytes * 4 + 256;	// probably plenty of space
	UInt32	sourceUsed, destUsed;
	int		status;

// do the conversion

	while (1) {
		if (bufferSize < reqSpace) {
			if (convBuffer != 0)
				delete[] convBuffer;
			bufferSize = reqSpace;
			convBuffer = new Byte[bufferSize];
		}
		status = TECkit_ConvertBuffer(
			converter,
			const_cast<Byte*>(data),
			nBytes,
			&sourceUsed,
			convBuffer,
			bufferSize,
			&destUsed,
			true);
		if (status == kStatus_OutputBufferFull) {
			reqSpace *= 2;		// output didn't fit, enlarge buffer and try again
			continue;
		}
		if (status != kStatus_NoError) {
			fprintf(stderr, "error %d in TECkit_Convert\n", status);
			exit(1);
		}
		UInt32	destUsed2;
		status = TECkit_Flush(
			converter,
			convBuffer + destUsed,
			bufferSize - destUsed,
			&destUsed2);
		if (status == kStatus_OutputBufferFull) {
			reqSpace *= 2;
			continue;
		}
		TECkit_ResetConverter(converter);
		if (status != kStatus_NoError) {
			fprintf(stderr, "error %d in TECkit_Flush\n", status);
			exit(1);
		}
		nBytes = destUsed + destUsed2;
		break;
	}

	fwrite(convBuffer, 1, nBytes, outFile);
}

static void
convertMarker(const ustring& marker, TECkit_Converter converter, string& cnvMarker)
{
	int	status;
	if (cnvMarker.size() < marker.length() * 4)
		cnvMarker.resize(marker.length() * 4 + 32);
	while (1) {
		UInt32	sourceUsed, destUsed, destUsed2;
		status = TECkit_ConvertBuffer(converter,
					(Byte*)marker.data(),
					marker.size() * 2,
					&sourceUsed,
					(Byte*)cnvMarker.data(),
					cnvMarker.size(),
					&destUsed,
					true);
		if (status == kStatus_OutputBufferFull) {
			cnvMarker.resize(cnvMarker.size() * 2);
			continue;
		}
		status = TECkit_Flush(converter,
					(Byte*)cnvMarker.data() + destUsed,
					cnvMarker.size() - destUsed,
					&destUsed2);
		if (status == kStatus_OutputBufferFull) {
			cnvMarker.resize(cnvMarker.size() * 2);
			continue;
		}
		cnvMarker.resize(destUsed + destUsed2);
		TECkit_ResetConverter(converter);
		if (status != kStatus_NoError) {
			fprintf(stderr, "error %d converting SFM from Unicode\n", status);
			exit(1);
		}
		break;
	}
}

static long
convertSingleChar(UniChar inChar, TECkit_Converter converter)
{
	int		status;
	Byte	buf[32];
	UInt32	sourceUsed, destUsed, destUsed2;
	
	status = TECkit_ConvertBuffer(converter,
				(Byte*)&inChar,
				2,
				&sourceUsed,
				&buf[0],
				sizeof(buf),
				&destUsed,
				true);
	if (status == kStatus_OutputBufferFull || destUsed > 1) {
		fprintf(stderr, "marker characters must map to single byte values");
		exit(1);
	}
	status = TECkit_Flush(converter,
				&buf[0] + destUsed,
				sizeof(buf) - destUsed,
				&destUsed2);
	if (status == kStatus_OutputBufferFull || destUsed + destUsed2 != 1) {
		fprintf(stderr, "marker characters must map to single byte values");
		exit(1);
	}
	TECkit_ResetConverter(converter);

	if (status != kStatus_NoError) {
		fprintf(stderr, "error %d converting marker characters from Unicode\n", status);
		exit(1);
	}

	return buf[0];
}

static void
process(const char* inputFile, const char* outputFile)
{
	TECkit_Converter	defaultConverter	= converters[defaultMapping];
	TECkit_Converter	sfmConverter		= converters[sfmMapping];
	TECkit_Converter	inlineConverter		= converters[inlineMapping];
	
	FILE*	outFile = fopen(outputFile, "wb");
	if (!outFile) {
		fprintf(stderr, "unable to open output file %s\n", outputFile);
		exit(1);
	}

	FILE*	inFile = fopen(inputFile, "rb");
	if (!inFile) {
		fprintf(stderr, "unable to open input file %s\n", inputFile);
		exit(1);
	}

	if (direction == kDirection_8_U) {
		// *** Byte to Unicode conversion
		if (outForm == kForm_Unspecified)
			outForm = kForm_UTF8;

		if (bom) {
			if (outForm == kForm_UTF8) {
				Byte	bom[] = "\xEF\xBB\xBF";
				fwrite(bom, 3, 1, outFile);
			}
			else if (outForm == kForm_UTF16BE) {
				Byte	bom[] = "\xFE\xFF";
				fwrite(&bom, 2, 1, outFile);
			}
			else {
				Byte	bom[] = "\xFF\xFE";
				fwrite(&bom, 2, 1, outFile);
			}
		}

		sfReader<char>		reader(inFile);

		TECkit_Converter	markerMapping = makeConverter(sfmMapping, kDirection_U_8);
		
		reader.escapeChar		= convertSingleChar(escapeCharU, markerMapping);
		for (int i = 0; i < sfmCharsU.length(); ++i)
			reader.sfmChars.append(1, convertSingleChar(sfmCharsU[i], markerMapping));

		if (inlineEscapeCharU != -1) {
			if (inlineMapping != sfmMapping) {
				TECkit_DisposeConverter(markerMapping);
				markerMapping = makeConverter(inlineMapping, kDirection_U_8);
			}
			reader.inlineEscapeChar	= convertSingleChar(inlineEscapeCharU, markerMapping);
			reader.startInline		= convertSingleChar(startInlineU, markerMapping);
			reader.endInline		= convertSingleChar(endInlineU, markerMapping);
				for (int i = 0; i < inlineCharsU.length(); ++i)
					reader.inlineChars.append(1, convertSingleChar(inlineCharsU[i], markerMapping));
		}
		
		TECkit_DisposeConverter(markerMapping);
		
		vector<TECkit_Converter>	converterStack;
		converterStack.assign(1, defaultConverter);
		int		dataType;
		while ((dataType = reader.next(converterStack.size() > 1)) != END_OF_FILE) {
			map<string,string>::const_iterator	i;
			switch (dataType) {
				case BODY_TEXT:
					write_converted((Byte*)reader.text.data(), reader.text.length(), converterStack.back(), outFile);
					break;

				case SFM:
					i = sfmMappings.find(reader.text);
					if (i == sfmMappings.end())
						converterStack.assign(1, defaultConverter);
					else
						converterStack.assign(1, converters[i->second]);
					reader.text.insert(reader.text.begin(), reader.escapeChar);
					write_converted((Byte*)reader.text.data(), reader.text.length(), sfmConverter, outFile);
					break;

				case INLINE_MARKER:
					i = inlineMappings.find(reader.text);
					if (i == inlineMappings.end())
						converterStack.assign(1, converterStack.back());
					else
						converterStack.assign(1, converters[i->second]);
					reader.text.insert(reader.text.begin(), reader.escapeChar);
					write_converted((Byte*)reader.text.data(), reader.text.length(), inlineConverter, outFile);
					break;

				case INLINE_START:
					i = inlineMappings.find(reader.text);
					if (i == inlineMappings.end())
						converterStack.push_back(converterStack.back());
					else
						converterStack.push_back(converters[i->second]);
					reader.text.insert(reader.text.begin(), reader.inlineEscapeChar);
					reader.text.insert(reader.text.end(), reader.startInline);
					write_converted((Byte*)reader.text.data(), reader.text.length(), inlineConverter, outFile);
					break;

				case INLINE_END:
					reader.text.insert(reader.text.end(), reader.endInline);
					write_converted((Byte*)reader.text.data(), reader.text.length(), inlineConverter, outFile);
					converterStack.pop_back();
					break;
			}
		}
	}
	else {
		// *** Unicode to Byte conversion
		Byte	bom[3];
		long	pos = ftell(inFile);
		if (fread(bom, 3, 1, inFile)) {
			if (bom[0] == 0xef && bom[1] == 0xbb && bom[2] == 0xbf) {
				inForm = kForm_UTF8;
			}
			else if (bom[0] == 0xfe && bom[1] == 0xff) {
				inForm = kForm_UTF16BE;
				fseek(inFile, pos + 2, SEEK_SET);
			}
			else if (bom[0] == 0xff && bom[1] == 0xfe) {
				inForm = kForm_UTF16LE;
				fseek(inFile, pos + 2, SEEK_SET);
			}
			else {
				if (inForm == kForm_Unspecified) {
					if (bom[0] == 0)
						inForm = kForm_UTF16BE;
					else if (bom[1] == 0)
						inForm = kForm_UTF16LE;
					else
						inForm = kForm_UTF8;
				}
				fseek(inFile, pos, SEEK_SET);
			}
		}
		else
			fseek(inFile, pos, SEEK_SET);

		sfReader<UniChar>	reader(inFile, inForm);

		reader.escapeChar = escapeCharU;
		reader.sfmChars = sfmCharsU;

		if (inlineEscapeCharU != -1) {
			reader.inlineEscapeChar = inlineEscapeCharU;
			reader.startInline = startInlineU;
			reader.endInline = endInlineU;
			reader.inlineChars = inlineCharsU;
		}

		vector<TECkit_Converter>	converterStack;
		converterStack.assign(1, defaultConverter);
		int		dataType;
		while ((dataType = reader.next(converterStack.size() > 1)) != END_OF_FILE) {
			map<string,string>::const_iterator	i;
			static string	cnvMarker;
			switch (dataType) {
				case BODY_TEXT:
					write_converted((Byte*)reader.text.data(), reader.text.length() * 2, converterStack.back(), outFile);
					break;

				case SFM:
					convertMarker(reader.text, sfmConverter, cnvMarker);
					i = sfmMappings.find(cnvMarker);
					if (i == sfmMappings.end())
						converterStack.assign(1, defaultConverter);
					else
						converterStack.assign(1, converters[i->second]);
					reader.text.insert(reader.text.begin(), reader.escapeChar);
					write_converted((Byte*)reader.text.data(), reader.text.length() * 2, sfmConverter, outFile);
					break;

				case INLINE_MARKER:
					convertMarker(reader.text, inlineConverter, cnvMarker);
					i = inlineMappings.find(cnvMarker);
					if (i == inlineMappings.end())
						converterStack.assign(1, converterStack.back());
					else
						converterStack.assign(1, converters[i->second]);
					reader.text.insert(reader.text.begin(), reader.inlineEscapeChar);
					write_converted((Byte*)reader.text.data(), reader.text.length() * 2, inlineConverter, outFile);
					break;

				case INLINE_START:
					convertMarker(reader.text, inlineConverter, cnvMarker);
					i = inlineMappings.find(cnvMarker);
					if (i == inlineMappings.end())
						converterStack.push_back(converterStack.back());
					else
						converterStack.push_back(converters[i->second]);
					reader.text.insert(reader.text.begin(), reader.inlineEscapeChar);
					reader.text.insert(reader.text.end(), reader.startInline);
					write_converted((Byte*)reader.text.data(), reader.text.length() * 2, inlineConverter, outFile);
					break;

				case INLINE_END:
					reader.text.insert(reader.text.end(), reader.endInline);
					write_converted((Byte*)reader.text.data(), reader.text.length() * 2, inlineConverter, outFile);
					converterStack.pop_back();
					break;
			}
		}
	}

	fclose(inFile);
	fclose(outFile);
}

#ifdef __MWERKS__
#if (__dest_os == __mac_os)
#include <console.h>
#endif
#endif

int
main(
	int		argc,
	char**	argv)
{
#ifdef __MWERKS__
#if (__dest_os == __mac_os)
	argc = ccommand(&argv);
#endif
#endif

	char*	controlFile = 0;
	char*	inputFile = 0;
	char*	outputFile = 0;
		
	bool	cmdLineErr = (argc < 2);
	
	char	unicodeFormat = kForm_UTF8;
	
	int		normForm = 0;
	
	while (--argc) {
		char	*arg = *++argv;
		if (arg[0] == '-') {
			if (strlen(arg + 1) == 1) {
				switch (arg[1]) {
					case 'c':
						if (controlFile != 0) {
							fprintf(stderr, "repeated argument -c\n");
							cmdLineErr = true;
							continue;
						}
						if (argc == 0) {
							fprintf(stderr, "missing file name after -c\n");
							cmdLineErr = true;
							continue;
						}
						controlFile = *++argv;
						--argc;
						continue;
					case 'd':
						if (gMappingDirectory != 0) {
							fprintf(stderr, "repeated argument -d\n");
							cmdLineErr = true;
							continue;
						}
						if (argc == 0) {
							fprintf(stderr, "missing directory path after -d\n");
							cmdLineErr = true;
							continue;
						}
						gMappingDirectory = *++argv;
						--argc;
						continue;
					case 'i':
						if (inputFile != 0) {
							fprintf(stderr, "repeated argument -i\n");
							cmdLineErr = true;
							continue;
						}
						if (argc == 0) {
							fprintf(stderr, "missing file name after -i\n");
							cmdLineErr = true;
							continue;
						}
						inputFile = *++argv;
						--argc;
						continue;
					case 'o':
						if (outputFile != 0) {
							fprintf(stderr, "repeated argument -o\n");
							cmdLineErr = true;
							continue;
						}
						if (argc == 0) {
							fprintf(stderr, "missing file name after -o\n");
							cmdLineErr = true;
							continue;
						}
						outputFile = *++argv;
						--argc;
						continue;
					case 'h':
						cmdLineErr = true;	// to get "usage" message
						continue;
				}
			}
			else if (strcmp(arg + 1, "utf8") == 0)
				unicodeFormat = kForm_UTF8;
			else if (strcmp(arg + 1, "be") == 0)
				unicodeFormat = kForm_UTF16BE;
			else if (strcmp(arg + 1, "le") == 0)
				unicodeFormat = kForm_UTF16LE;
			else if (strcmp(arg + 1, "bom") == 0)
				bom = true;
			else if (strcmp(arg + 1, "u8") == 0)
				direction = kDirection_U_8;
			else if (strcmp(arg + 1, "8u") == 0)
				direction = kDirection_8_U;
			else if (strcmp(arg + 1, "nfc") == 0)
				normForm = kForm_NFC;
			else if (strcmp(arg + 1, "nfd") == 0)
				normForm = kForm_NFD;
			else {
				fprintf(stderr, "Unknown option: %s\n", arg);
				cmdLineErr = true;
			}
		}
		else {
			cmdLineErr = true;
		}
	}
	
	if (cmdLineErr || direction == kDirection_Unspecified || unicodeFormat == kForm_Unspecified) {
		fprintf(stderr, "\
8-bit to Unicode:\n\
    SFconv -8u [-utf8|-be|-le] [-bom] -c ControlFile [-d MappingDirectory] -i InFile -o OutFile\n\
Unicode to 8-bit:\n\
    SFconv -u8 [-utf8|-be|-le]        -c ControlFile [-d MappingDirectory] -i InFile -o OutFile\n");
		return 1;
	}

	if (direction == kDirection_8_U) {
		inForm = kForm_Bytes;
		outForm = unicodeFormat + normForm;
	}
	else {
		inForm = unicodeFormat;
		outForm = kForm_Bytes;
	}

	if (!read_control_file(controlFile))
		exit(1);

	process(inputFile, outputFile);

	return 0;
}
