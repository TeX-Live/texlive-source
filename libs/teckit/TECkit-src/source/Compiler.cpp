/*------------------------------------------------------------------------
Copyright (C) 2002-2016 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: Compiler.cpp
Responsibility: Jonathan Kew
Last reviewed: Not yet.

Description:
    Implements the TECkit mapping compiler.
-------------------------------------------------------------------------*/

/*
	2008-11-17	jk			include <cstdio> (Debian bug 505693)
	2006-06-19	jk			added new APIs to look up Unicode names
	2006-01-12	jk			removed multi-char constants, use FOUR_CHAR_CODE to define UInt32 values instead
							(no functional change, just to avoid compiler warnings)
    2005-07-07  jk  2.1.5   changed to use WORDS_BIGENDIAN rather than TARGET_RT_BIG_ENDIAN
    2005-06-20  jk  2.1.4   added lhsDefault/rhsDefault attributes to <pass> elem in xml output
	23-May-2005		changes for 64-bit compilation, from Ulrik P
	21-May-2005		changes based on Ulrik Petersen's patch for MS VC++ 6
    2004-11-11  jk  2.1.3   added support for XML export
	2004-07-21	jk	2.1.2	removed trailing spaces from 2 names in UnicodeNames.cpp
	2004-06-16	jk	2.1.1	fixed bug of ignoring char after '_'
	2004-03-12	jk	2.1		updated for version 2.1 with ...Opt APIs
							modified compiler to accept Unicode source text
*/

#include "Compiler.h"

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>

#include "zlib.h"

const UInt32	kInvalidChar	= 0xfffffffdUL;

static UInt32
offsetsFromUTF8[6] =	{
	0x00000000UL,
	0x00003080UL,
	0x000E2080UL, 
	0x03C82080UL,
	0xFA082080UL,
	0x82082080UL
};

static UInt8
bytesFromUTF8[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

static UInt8
firstByteMark[7] = {
	0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

const int halfShift					= 10;
const UInt32 halfBase				= 0x0010000UL;
const UInt32 kSurrogateHighStart	= 0xD800UL;
const UInt32 kSurrogateHighEnd		= 0xDBFFUL;
const UInt32 kSurrogateLowStart		= 0xDC00UL;
const UInt32 byteMask				= 0x000000BFUL;
const UInt32 byteMark				= 0x00000080UL;

#define FOUR_CHAR_CODE(a,b,c,d)	UInt32((a << 24) + (b << 16) + (c << 8) + d)

const UInt32 kCode_Byte	= FOUR_CHAR_CODE('B','y','t','e');
const UInt32 kCode_BU	= FOUR_CHAR_CODE('B','-','>','U');
const UInt32 kCode_UB	= FOUR_CHAR_CODE('U','-','>','B');
const UInt32 kCode_Unic	= FOUR_CHAR_CODE('U','n','i','c');
const UInt32 kCode_NFCf	= FOUR_CHAR_CODE('N','F','C','f');
const UInt32 kCode_NFCr	= FOUR_CHAR_CODE('N','F','C','r');
const UInt32 kCode_NFC	= FOUR_CHAR_CODE('N','F','C',' ');
const UInt32 kCode_NFDf	= FOUR_CHAR_CODE('N','F','D','f');
const UInt32 kCode_NFDr	= FOUR_CHAR_CODE('N','F','D','r');
const UInt32 kCode_NFD	= FOUR_CHAR_CODE('N','F','D',' ');

Compiler::Keyword
Compiler::keywords[] = {
	{ "Pass",				tok_Pass,		0						},
	{ "Byte",				tok_PassType,	kCode_Byte				},
	{ "Byte_Unicode",		tok_PassType,	kCode_BU					},
	{ "Unicode_Byte",		tok_PassType,	kCode_UB					},
	{ "Unicode",			tok_PassType,	kCode_Unic				},
	{ "NFC_fwd",			tok_PassType,	kCode_NFCf				},
	{ "NFC_rev",			tok_PassType,	kCode_NFCr				},
	{ "NFC",				tok_PassType,	kCode_NFC				},
	{ "NFD_fwd",			tok_PassType,	kCode_NFDf				},
	{ "NFD_rev",			tok_PassType,	kCode_NFDr				},
	{ "NFD",				tok_PassType,	kCode_NFD				},
	{ "Class",				tok_Class,		0						},
	{ "ByteClass",			tok_Class,		'B'						},
	{ "UniClass",			tok_Class,		'U'						},
	{ "ByteDefault",		tok_Default,	'B'						},
	{ "UniDefault",			tok_Default,	'U'						},
	{ "EncodingName",		tok_Name,		kNameID_LHS_Name		},
	{ "DescriptiveName",	tok_Name,		kNameID_LHS_Description	},
	{ "Name",				tok_Name,		0xffffffff				},
	{ "LHSName",			tok_Name,		kNameID_LHS_Name		},
	{ "LHSDescription",		tok_Name,		kNameID_LHS_Description	},
	{ "RHSName",			tok_Name,		kNameID_RHS_Name		},
	{ "RHSDescription",		tok_Name,		kNameID_RHS_Description	},
	{ "Version",			tok_Name,		kNameID_Version			},
	{ "Contact",			tok_Name,		kNameID_Contact			},
	{ "RegistrationAuthority",	tok_Name,	kNameID_RegAuthority	},
	{ "RegistrationName",	tok_Name,		kNameID_RegName			},
	{ "Copyright",			tok_Name,		kNameID_Copyright		},
	{ "LHSFlags",			tok_Flags,		'S'						},
	{ "RHSFlags",			tok_Flags,		'T'						},
	{ "ExpectsNFC",			tok_FlagValue,	kFlags_ExpectsNFC		},
	{ "ExpectsNFD",			tok_FlagValue,	kFlags_ExpectsNFD		},
	{ "GeneratesNFC",		tok_FlagValue,	kFlags_GeneratesNFC		},
	{ "GeneratesNFD",		tok_FlagValue,	kFlags_GeneratesNFD		},
	{ "VisualOrder",		tok_FlagValue,	kFlags_VisualOrder		},
	{ "Define",				tok_Define,		0						},
	{ 0,					tok_Identifier,	0						}
};

UInt32
WINAPI
TECkit_GetCompilerVersion()
{
	return kCurrentTECkitVersion;
}

TECkit_Status
WINAPI
TECkit_Compile(char* txt, UInt32 len, Byte doCompression, TECkit_ErrorFn errFunc, void* userData, Byte** outTable, UInt32* outLen)
{
	TECkit_Status	result = kStatus_CompilationFailed;
	try {
		Compiler*	cmp = new Compiler(txt, len, kForm_Unspecified, bool(doCompression), false, errFunc, userData);
		cmp->GetCompiledTable(*outTable, *outLen);
		if (*outTable == 0)
			result = kStatus_CompilationFailed;
		else {
			cmp->DetachCompiledTable();
			result = kStatus_NoError;
		}
		delete cmp;
	}
	catch (...) {
		result = kStatus_Exception;
	}
	return result;
}

TECkit_Status
WINAPI
TECkit_CompileOpt(char* txt, UInt32 len, TECkit_ErrorFn errFunc, void* userData, Byte** outTable, UInt32* outLen, UInt32 opts)
{
	TECkit_Status	result = kStatus_CompilationFailed;
	try {
		Compiler*	cmp = new Compiler(txt, len, (opts & kCompilerOpts_FormMask),
			(opts & kCompilerOpts_Compress) != 0, (opts & kCompilerOpts_XML) != 0, errFunc, userData);
		cmp->GetCompiledTable(*outTable, *outLen);
		if (*outTable == 0)
			result = kStatus_CompilationFailed;
		else {
			cmp->DetachCompiledTable();
			result = kStatus_NoError;
		}
		delete cmp;
	}
	catch (...) {
		result = kStatus_Exception;
	}
	return result;
}

void
WINAPI
TECkit_DisposeCompiled(Byte* table)
{
	if (table != 0)
		free(table);
}

const char*
WINAPI
TECkit_GetUnicodeName(UInt32 usv)
{
	const CharName	*c = &gUnicodeNames[0];
	while (c->name != 0)
		if (c->usv == usv)
			return const_cast<char*>(c->name);
		else
			++c;
	return NULL;
}

char*
WINAPI
TECkit_GetTECkitName(UInt32 usv)
{
	static char	buffer[256];
	const char*	name = TECkit_GetUnicodeName(usv);
	if (name == NULL)
		sprintf(buffer, "U+%04X", usv);
	else {
		char* cp = &buffer[0];
		while (*name && (cp - buffer < 255)) {
			if ((*name < '0') || (*name > '9' && *name < 'A') || (*name > 'Z'))
				*cp++ = '_';
			else
				*cp++ = *name | 0x20;
			++name;
		}
		*cp = 0;
	}
	return buffer;
}

static int
unicodeNameCompare(const char* uniName, const char* idStr, UInt32 len)
{ // idStr could be either a "real" unicode name or a teckit identifier
  // when this is used by the TECkit_GetUnicodeValue API
	while (*uniName || len != 0) {
		if (len == 0)
			return 1;
		char	u = *uniName++;
		char	i = *idStr++;
		--len;
		if ((i >= 'a') && (i <= 'z'))
			i &= ~0x20;
		if (u == i)
			continue;
		if ((u < '0') || (u > '9' && u < 'A') || (u > 'Z'))
			u = '_';
		if (u == i)
			continue;
		return u < i ? -1 : 1;
	}
	return 0;
}

int
WINAPI
TECkit_GetUnicodeValue(char* name)
{
	const CharName	*c = &gUnicodeNames[0];
	size_t	len = strlen(name);
	while (c->name != 0)
		if (unicodeNameCompare(c->name, name, len) == 0)
			return static_cast<int>(c->usv);
		else
			++c;
	return -1;
}

inline UInt8
READ(const UInt8 p)
{
	return p;
}

inline UInt16
READ(const UInt16 p)
{
#ifdef WORDS_BIGENDIAN
	return p;
#else
	return (p >> 8) + (p << 8);
#endif
}

inline UInt32
READ(const UInt32 p)
{
#ifdef WORDS_BIGENDIAN
	return p;
#else
	return (p >> 24) + ((p >> 8) & 0x0000ff00) + ((p << 8) & 0x00ff0000) + (p << 24);
#endif
}

template<class T>
inline void
WRITE(T& t, UInt32 v)
{
	t = READ(T(v));
}

void
Compiler::appendToTable(string& s, const char* ptr, UInt32 len)
{
#ifdef WORDS_BIGENDIAN
	s.append(ptr, len);
#else
	ptr += len;
	while (len-- > 0)
		s.append(1, *--ptr);
#endif
}

static inline bool
isIDstart(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static inline bool
isIDcont(char c)
{
	return isIDstart(c) || (c >= '0' && c <= '9');
}

static bool
strmatch(const char* str, const char* txt, UInt32 len)
{
	while (*str || len != 0) {
		if (len == 0)
			return false;
		if ((*str++ | 0x20) != (*txt++ | 0x20))
			return false;
		--len;
	}
	return true;
}

static const char*
getClassName(const map<string,UInt32>& nameMap, UInt32 index)
{
	for (map<string,UInt32>::const_iterator i = nameMap.begin(); i != nameMap.end(); ++i) {
		if (i->second == index) {
			return i->first.c_str();
		}
	}
	return "[UNKNOWN]";
}

static const char*
asHex(UInt32 val, short digits)
{
	static char	str[16];
	sprintf(str, "%0*X", digits, val);
	return str;
}

static const char*
asDec(UInt32 val)
{
	static char	str[16];
	sprintf(str, "%d", val);
	return str;
}

void
Compiler::xmlOut(const char* s)
{
	xmlRepresentation += s;
}

void
Compiler::xmlOut(const string& s)
{
	xmlRepresentation += s;
}

void
Compiler::xmlOut(char c)
{
	xmlRepresentation += c;
}

string
Compiler::getContextID(const vector<Item>& ctx, bool isUnicode)
{
	string	contextString = xmlString(ctx.begin(), ctx.end(), isUnicode);
	string	contextID = currentPass.xmlContexts[contextString];
	if (contextID.length() == 0) {
		contextID = isUnicode ? "uctx_" : "bctx_";
		contextID += asDec(currentPass.xmlContexts.size());
		currentPass.xmlContexts[contextString] = contextID;
	}
	return contextID;
}

string
Compiler::xmlString(vector<Item>::const_iterator b, vector<Item>::const_iterator e, bool isUnicode)
{
	string	rval;
	if (b == e)
		return rval;
	for (vector<Item>::const_iterator i = b; i != e; ++i) {
		switch (i->type) {
			case 0:
				rval += "<ch n=\"";
				rval += asHex(i->val, isUnicode ? 4 : 2);
				rval += "\"";
				break;
			case kMatchElem_Type_EOS:
				rval += "<eot";
				break;
			case kMatchElem_Type_ANY:
				rval += "<any";
				break;
			case kMatchElem_Type_BGroup:
				{
					vector<Item>::const_iterator j = i;
					int nesting = 0;
					bool	alt = false;
					string	groupStr;
					++i;
					while (++j != e) {
						if (j->type == kMatchElem_Type_BGroup)
							++nesting;
						else if (j->type == kMatchElem_Type_EGroup) {
							if (nesting == 0) {
								if (alt && i < j - 1)
									groupStr += "<group>\n";
								groupStr += xmlString(i, j, isUnicode);
								if (alt && i < j - 1)
									groupStr += "</group>\n";
								break;
							}
							else
								--nesting;
						}
						else if (j->type == kMatchElem_Type_OR && nesting == 0) {
							if (i < j - 1)
								groupStr += "<group>\n";
							groupStr += xmlString(i, j, isUnicode);
							if (i < j - 1)
								groupStr += "</group>\n";
							i = j + 1;
							alt = true;
						}
					}
					i = j;
					rval += "<group";
					if (alt)
						rval += " alt=\"1\"";
					if ((i->repeatMin != 1) && (i->repeatMin != 255)) {
						rval += " min=\"";
						rval += asDec(i->repeatMin);
						rval += "\"";
					}
					if ((i->repeatMax != 1) && (i->repeatMax != 255)) {
						rval += " max=\"";
						rval += asDec(i->repeatMax);
						rval += "\"";
					}
					if (i->tag.length() > 0) {
						if (i->type != kMatchElem_Type_Copy) {
							rval += " id=\"";
							rval += i->tag;
							rval += "\"";
						}
					}
					rval += ">\n";
					rval += groupStr;
					rval += "</group>\n";
					continue;
				}
				break;
			case kMatchElem_Type_OR:
				rval += "<OR/>\n";
				continue;
				break;
			case kMatchElem_Type_EGroup:
				rval += "<END-GROUP/>\n";
				continue;
				break;
			case kMatchElem_Type_Class:
				{
					rval += "<class-ref name=\"";
					const map<string,UInt32>&	classes = isUnicode ? currentPass.uniClassNames : currentPass.byteClassNames;
					rval += isUnicode ? "u_" : "b_";
					rval += getClassName(classes, i->val);
					rval += "\"";
				}
				break;
			case kMatchElem_Type_Copy:
				rval += "<copy-ref id=\"";
				rval += i->tag;
				rval += "\"";
				break;
			default:
				rval += "<UNKNOWN type=\"";
				rval += asHex(i->type, 1);
				break;
		}
		if (i->negate)
			rval += " neg=\"1\"";
		if ((i->repeatMin != 1) && (i->repeatMin != 255)) {
			rval += " min=\"";
			rval += asDec(i->repeatMin);
			rval += "\"";
		}
		if ((i->repeatMax != 1) && (i->repeatMax != 255)) {
			rval += " max=\"";
			rval += asDec(i->repeatMax);
			rval += "\"";
		}
		if (i->tag.length() > 0) {
			if (i->type != kMatchElem_Type_Copy) {
				rval += " id=\"";
				rval += i->tag;
				rval += "\"";
			}
		}
		rval += "/>";
	}
	return rval;
}

Compiler::Compiler(const char* txt, UInt32 len, char inForm, bool cmp, bool genXML, TECkit_ErrorFn errFunc, void* userData)
{
	compiledTable = 0;
	compiledSize = 0;
	usedExtStringRules = false;

	textPtr = reinterpret_cast<const unsigned char*>(txt);
	textEnd = textPtr + len;
	
	ungotten = kInvalidChar;
	inputForm = inForm;
	
	generateXML = genXML;
	
	lineNumber = 1;
	errorState = false;
	errorCount = 0;

	errorFunction = errFunc;
	errFuncUserData = userData;
	
	names.clear();
	fwdTables.clear();
	revTables.clear();
	currentPass.clear();
	buildVars.clear();
	currentRule.clear();
	
	lhsFlags = 0;
	rhsFlags = 0;
	
	char	classType;
	
	ruleState = notInRule;
	int	nestingLevel = 0;

	defIter = defEnd;

	while (inputForm == kForm_Unspecified) {
		// attempt to determine input encoding
		if (len >= 4) {
			// check for UTF32 BOM or 3 nulls in first 4 bytes
			if (strncmp(txt, "\0\0\376\377", 4) == 0) {
				inputForm = kForm_UTF32BE;
				break;
			}
			if (strncmp(txt, "\377\376\0\0", 4) == 0) {
				inputForm = kForm_UTF32LE;
				break;
			}
			if (strncmp(txt, "\0\0\0", 3) == 0) {
				inputForm = kForm_UTF32BE;
				break;
			}
			if (strncmp(txt+1, "\0\0\0", 3) == 0) {
				inputForm = kForm_UTF32LE;
				break;
			}
		}
		if (len >= 3) {
			// check for UTF8 signature
			if (strncmp(txt, "\357\273\277", 3) == 0) {
				inputForm = kForm_UTF8;
				break;
			}
		}
		if (len >= 2) {
			// check for UTF16 BOM or null byte
			if (strncmp(txt, "\376\377", 2) == 0) {
				inputForm = kForm_UTF16BE;
				break;
			}
			if (strncmp(txt, "\377\376", 2) == 0) {
				inputForm = kForm_UTF16LE;
				break;
			}
			if (txt[0] == '\0') {
				inputForm = kForm_UTF16BE;
				break;
			}
			if (txt[1] == '\0') {
				inputForm = kForm_UTF16LE;
				break;
			}
		}
		inputForm = kForm_Bytes;
	};

	if (inputForm != kForm_Bytes) {
		// discard initial BOM if present
		UInt32 currCh = getChar();
		if (currCh != 0xfeff)
			ungetChar(currCh);
	}

	while (GetNextToken()) {
		// on error, skip to next newline
	GOT_TOKEN:
		if (errorState) {
			if (ruleState != notInRule) {
				ruleState = notInRule;
				nestingLevel = 0;
				currentRule.clear();
			}
			if (tok.type != tok_Newline)
				continue;
		}
		errorState = false;

		string32::const_iterator i;
		switch (int(tok.type)) {
			default:
				Error("this can't happen!");
				break;
				
			case tok_Unknown:
				Error("unexpected character", string(1, tok.val).c_str());
				break;

			case tok_Identifier:
				Error("unexpected identifier", asUTF8(tok.strval).c_str());
				break;

			case tok_Define:
				if (!ExpectToken(tok_Identifier, "expected identifier after Define"))
					break;
				{
					string		defName(asUTF8(tok.strval));
					tokListT	defToks;
					while (GetNextToken()) {
						if (tok.type == tok_Newline)
							break;
						if (tok.type == tok_Unknown)
							Error("unexpected character in Define text", string(1, tok.val).c_str());
						else
							defToks.push_back(tok);
					}
					defines[defName] = defToks;
				}
				break;

			case tok_Newline:
				switch (ruleState) {
					default:
						break;

					case inLHSString:
					case inLHSPreContext:
					case inLHSPostContext:
						Error("no mapping operator found");
						goto GOT_TOKEN;

					case inRHSString:
					case inRHSPreContext:
					case inRHSPostContext:
						if (nestingLevel > 0) {
							Error("unmatched opening parenthesis");
							break;
						}
						if (ruleType == 0 || ruleType == '>') {
							currentPass.fwdRules.push_back(Rule(currentRule.lhsString,
								reverseContext(currentRule.lhsPreContext), currentRule.lhsPostContext,
								currentRule.rhsString, currentRule.startingLine));
						}
						if (ruleType == 0 || ruleType == '<') {
							currentPass.revRules.push_back(Rule(currentRule.rhsString,
								reverseContext(currentRule.rhsPreContext), currentRule.rhsPostContext,
								currentRule.lhsString, currentRule.startingLine));
						}
						if (generateXML) {
							// create an XML representation of the rule and append to currentPass.xmlRules/xmlContexts
							bool	sourceUni = (currentPass.passType == kCode_UB) || (currentPass.passType == kCode_Unic);
							bool	targetUni = (currentPass.passType == kCode_BU) || (currentPass.passType == kCode_Unic);

							string	xmlRule;
							xmlRule += "<a";
							xmlRule += " line=\"";
							xmlRule += asDec(currentRule.startingLine);
							xmlRule += "\"";
							if (ruleType == '>')
								xmlRule += " dir=\"fwd\"";
							else if (ruleType == '<')
								xmlRule += " dir=\"rev\"";
							xmlRule += ">\n";
							
							string	contextID;
							xmlRule += "<l";
							if (currentRule.lhsPreContext.size() != 0) {
								contextID = getContextID(currentRule.lhsPreContext, sourceUni);
								xmlRule += " preCtx=\"";
								xmlRule + contextID;
								xmlRule += "\"";
							}
							if (currentRule.lhsPostContext.size() != 0) {
								contextID = getContextID(currentRule.lhsPostContext, sourceUni);
								xmlRule += " postCtx=\"";
								xmlRule += contextID;
								xmlRule += "\"";
							}
							xmlRule += ">";
							xmlRule += xmlString(currentRule.lhsString.begin(), currentRule.lhsString.end(), sourceUni);
							xmlRule += "</l>\n";

							xmlRule += "<r";
							if (currentRule.rhsPreContext.size() != 0) {
								contextID = getContextID(currentRule.rhsPreContext, targetUni);
								xmlRule += " preCtx=\"";
								xmlRule += contextID;
								xmlRule += "\"";
							}
							if (currentRule.rhsPostContext.size() != 0) {
								contextID = getContextID(currentRule.rhsPostContext, targetUni);
								xmlRule += " postCtx=\"";
								xmlRule += contextID;
								xmlRule += "\"";
							}
							xmlRule += ">";
							xmlRule += xmlString(currentRule.rhsString.begin(), currentRule.rhsString.end(), targetUni);
							xmlRule += "</r>\n";

							xmlRule += "</a>\n";
							currentPass.xmlRules.push_back(xmlRule);
						}
						currentRule.clear();
						ruleState = notInRule;
						break;
				}
				break;

			case tok_Number:
				AppendLiteral(tok.val);
				break;

			case tok_USV:
				AppendUSV(tok.val);
				break;
				
			case tok_String:
				if (inputForm == kForm_Bytes && charLimit() != 0xff) {
					Error("can't use quoted string for Unicodes in 8-bit source text");
					break;
				}
				if (inputForm != kForm_Bytes && charLimit() == 0xff) {
					Error("can't use quoted string for Bytes in Unicode source text");
					break;
				}
				for (i = tok.strval.begin(); i != tok.strval.end(); ++i)
					AppendLiteral(*i);
				break;

			case '^':
				// negation can only apply to a few things:
				GetNextToken();
				switch (int(tok.type)) {
					case tok_Number:
						AppendLiteral(tok.val, true);
						break;
					case tok_USV:
						AppendUSV(tok.val, true);
						break;
					case '[':
						if (!ExpectToken(tok_Identifier, "expected CLASS-NAME after opening bracket"))
							break;
						AppendClass(asUTF8(tok.strval), true);
						if (!ExpectToken(']', "expected closing bracket after CLASS-NAME"))
							break;
						break;
					case '#':
						AppendSpecial(kMatchElem_Type_EOS, true);
						break;
					case '.':
						AppendSpecial(kMatchElem_Type_ANY, true);
						break;
					default:
						Error("invalid use of negation");
						break;
				}
				break;
			
			case '#':
				AppendSpecial(kMatchElem_Type_EOS);
				break;
			
			case '.':
				AppendSpecial(kMatchElem_Type_ANY);
				break;
			
			case '(':
				AppendSpecial(kMatchElem_Type_BGroup);
				++nestingLevel;
				break;
				
			case ')':
				if (nestingLevel == 0) {
					Error("unmatched closing parenthesis");
					break;
				}
				--nestingLevel;
				AppendSpecial(kMatchElem_Type_EGroup);
				break;
				
			case '|':
				if (nestingLevel == 0) {
					Error("alternation only permitted within parentheses");
					break;
				}
				AppendSpecial(kMatchElem_Type_OR);
				break;
			
			case tok_Map:
			case '>':
			case '<':
				if (nestingLevel > 0) {
					Error("unmatched opening parenthesis");
					break;
				}
				switch (ruleState) {
					default:
						Error("not within a mapping rule");
						break;
					case inLHSString:
					case inLHSPostContext:
						ruleState = inRHSString;
						ruleType = (tok.type == tok_Map ? 0 : (tok.type == '>' ? '>' : '<'));
						break;
					case inLHSPreContext:
						Error("no underscore found in context");
						break;
					case inRHSString:
					case inRHSPreContext:
					case inRHSPostContext:
						Error("extra mapping operator in rule");
						break;
				}
				break;

			case '/':
				if (nestingLevel > 0) {
					Error("unmatched opening parenthesis");
					break;
				}
				switch (ruleState) {
					default:
						Error("not within a mapping rule");
						break;
					case inLHSString:
						ruleState = inLHSPreContext;
						break;
					case inRHSString:
						ruleState = inRHSPreContext;
						break;
					case inLHSPreContext:
					case inLHSPostContext:
					case inRHSPreContext:
					case inRHSPostContext:
						Error("extra slash in rule");
						break;
				}
				break;

			case '_':
				if (nestingLevel > 0) {
					Error("unmatched opening parenthesis");
					break;
				}
				switch (ruleState) {
					default:
						Error("not within a mapping rule");
						break;
					case inLHSPreContext:
						ruleState = inLHSPostContext;
						break;
					case inRHSPreContext:
						ruleState = inRHSPostContext;
						break;
					case inLHSString:
					case inRHSString:
						Error("underscore only allowed in context");
						break;
					case inLHSPostContext:
					case inRHSPostContext:
						Error("extra underscore in context");
						break;
				}
				break;

			case '[':
				if (!ExpectToken(tok_Identifier, "expected CLASS-NAME after opening bracket"))
					break;
				AppendClass(asUTF8(tok.strval));
				if (!ExpectToken(']', "expected closing bracket after CLASS-NAME"))
					break;
				break;
			
			case ']':
				Error("unmatched closing bracket");
				break;
			
			case '=':
				if (!ExpectToken(tok_Identifier, "expected tag name after '='"))
					break;
				AssignTag(asUTF8(tok.strval));
				break;

			case '@':
				if (!ExpectToken(tok_Identifier, "expected tag name after '@'"))
					break;
				AppendSpecial(kMatchElem_Type_Copy);
				AssignTag(asUTF8(tok.strval));
				break;
			
			case '?':
				SetMinMax(0, 1);
				break;
			
			case '*':
				SetMinMax(0, 15);
				break;
			
			case '+':
				SetMinMax(1, 15);
				break;
			
			case '{':
				{
					UInt32 repeatMin = 0;
					UInt32 repeatMax = 15;
					GetNextToken();
					if (tok.type == tok_Number) {
						repeatMin = repeatMax = tok.val;
						GetNextToken();
						if (tok.type == ',') {
							GetNextToken();
							if (tok.type == tok_Number) {
								repeatMax = tok.val;
								if (!ExpectToken('}', "expected closing brace after repeat counts"))
									break;
							}
							else if (tok.type == '}')
								repeatMax = 15;
							else {
								Error("expected repeat count or closing brace after comma");
								break;
							}
						}
						else if (tok.type != '}') {
							Error("expected comma or closing brace after repeat count");
							break;
						}
					}
					else if (tok.type == ',') {
						GetNextToken();
						if (tok.type == tok_Number)
							repeatMax = tok.val;
						else {
							Error("expected repeat count");
							break;
						}
						if (!ExpectToken('}', "expected closing brace after repeat count"))
							break;
					}
					else {
						Error("expected repeat counts within braces");
						break;
					}
					SetMinMax(repeatMin, repeatMax);
				}
				break;
	
			case '}':
				Error("unmatched closing brace");
				break;

			case tok_Name:
				if (tok.val == 0xffffffff) {
					if (!ExpectToken('(', "expected (NUMBER) STRING after Name"))
						break;
					if (!ExpectToken(tok_Number,  "expected (NUMBER) STRING after Name"))
						break;
					UInt16 nameID = tok.val;
					if (!ExpectToken(')', "expected (NUMBER) STRING after Name"))
						break;
					ReadNameString(nameID);
				}
				else
					ReadNameString(tok.val);
				goto GOT_TOKEN;	// ReadNameString has already read the newline

			case tok_Flags:
				{
					if (!ExpectToken('(', "expected (FLAG-LIST) after SourceFlags/TargetFlags"))
						break;
					UInt32	flagValue = 0;
					char	whichFlags = tok.val;
					while (1) {
						GetNextToken();
						if (tok.type == tok_FlagValue)
							flagValue |= tok.val;
						else
							break;
					}
					if (tok.type != ')') {
						Error("expected (FLAG-LIST) after SourceFlags/TargetFlags");
						break;
					}
					if (whichFlags == 'S')
						lhsFlags = flagValue;
					else
						rhsFlags = flagValue;
				}
				ExpectToken(tok_Newline, "junk at end of line");
				break;

			case tok_Pass:
				FinishPass();
				currentPass.setLineNo(lineNumber);
				if (!ExpectToken('(', "expected (PASS-TYPE) after Pass"))
					break;
				GetNextToken();
				if (tok.type == tok_PassType)
					currentPass.passType = tok.val;
				else
					Error("unrecognized pass type");
				if (!ExpectToken(')', "expected (PASS-TYPE) after Pass"))
					break;
				ExpectToken(tok_Newline, "junk at end of line");
				goto  GOT_TOKEN;
			
			case tok_Default:
				StartDefaultPass();
				if (currentPass.passType != kCode_BU && currentPass.passType != kCode_UB) {
					Error("defaults are only used in Byte_Unicode and Unicode_Byte passes");
					break;
				}
				{
					char	whichDefault = tok.val;
					GetNextToken();
					switch (tok.type) {
						case tok_String:
							if (tok.strval.length() != 1)
								Error("default can only be a single character, not a multi-character string");
							else if (whichDefault == 'U') {
								if (inputForm == kForm_Bytes)
									Error("UniDefault cannot use quoted character in 8-bit source text");
								else
									currentPass.uniDefault = tok.strval[0];
							}
							else {
								if (inputForm != kForm_Bytes)
									Error("ByteDefault cannot use quoted character in Unicode source text");
								else
									currentPass.byteDefault = tok.strval[0];
							}
							break;
						case tok_Number:
							if (whichDefault == 'U')
								currentPass.uniDefault = tok.val;
							else
								currentPass.byteDefault = tok.val;
							break;
						case tok_USV:
							if (whichDefault == 'U')
								currentPass.uniDefault = tok.val;
							else
								Error("can't use Unicode value in byte encoding");
							break;
						default:
							Error("expected character code after ByteDefault/UniDefault");
							break;
					}
				}
				break;
			
			case tok_Class:
				StartDefaultPass();
				classLine = lineNumber;
				if (tok.val == 0) {
					if (currentPass.passType == kCode_Byte)
						classType = 'B';
					else if (currentPass.passType == kCode_Unic)
						classType = 'U';
					else {
						Error("must use ByteClass or UniClass to define classes in this pass");
						break;
					}
				}
				else {
					classType = tok.val;
					if (classType == 'B' && currentPass.passType == kCode_Unic)
						Error("can't use ByteClass in this pass");
					else if (classType == 'U' && currentPass.passType == kCode_Byte)
						Error("can't use UniClass in this pass");
				}
				{
					UInt32	classLimit = (classType == 'U' ? 0x10ffff : 0xff);
					if (!ExpectToken('[', "expected [CLASS-NAME] after Class/ByteClass/UniClass"))
						break;
					if (!ExpectToken(tok_Identifier, "expected [CLASS-NAME] after Class/ByteClass/UniClass"))
						break;
					string	className(asUTF8(tok.strval));
					if (!ExpectToken(']', "expected [CLASS-NAME] after Class/ByteClass/UniClass"))
						break;
					if (!ExpectToken('=', "expected =(CHARACTER-CODE-LIST) after Class/ByteClass/UniClass[CLASS-NAME]"))
						break;
					if (!ExpectToken('(', "expected =(CHARACTER-CODE-LIST) after Class/ByteClass/UniClass[CLASS-NAME]"))
						break;
					vector<UInt32>	classMembers;
					bool	ellipsis = false;
					bool	ellipsisOK = false;
					while (tok.type != ')' && tok.type != tok_Newline) {
						GetNextToken();
						switch (int(tok.type)) {
							case tok_USV:
								if (classType == 'B') {
									Error("can't use Unicode value in byte encoding");
									break;
								}
								// fall through
							case tok_Number:
								if (tok.val > classLimit) {
									Error("class element outside valid range");
									break;
								}
								if (ellipsis) {
									ellipsis = false;
									ellipsisOK = false;
									UInt32	lastVal = classMembers.back();
									if (tok.val < lastVal) {
										Error("range out of order");
										break;
									}
									while (++lastVal <= tok.val)
										classMembers.push_back(lastVal);
								}
								else {
									classMembers.push_back(tok.val);
									ellipsisOK = true;
								}
								if (classMembers.back() > 0x0000ffff)
									currentPass.supplementaryChars = true;
								break;
								
							case tok_String:
								if (classType == 'U' && inputForm == kForm_Bytes) {
									Error("can't use quoted string for Unicode class in 8-bit source text");
									break;
								}
								if (classType == 'B' && inputForm != kForm_Bytes) {
									Error("can't use quoted string for Byte class in Unicode source text");
									break;
								}
								if (ellipsis) {
									ellipsis = false;
									ellipsisOK = false;
									if (tok.strval.length() != 1) {
										Error("can only use single-character string with ..");
										break;
									}
									UInt32	lastVal = classMembers.back();
									if (tok.strval[0] < lastVal) {
										Error("range out of order");
										break;
									}
									while (++lastVal <= tok.strval[0])
										classMembers.push_back(lastVal);
									break;
								}
								ellipsisOK = (tok.strval.length() == 1);
								for (i = tok.strval.begin(); i < tok.strval.end(); ++i)
									classMembers.push_back(*i);
								break;
								
							case tok_Ellipsis:
								if (ellipsisOK) {
									ellipsisOK = false;
									ellipsis = true;
								}
								else
									Error("illegal .. in class");
								break;
								
							case '[':
								{
									if (ellipsis) {
										Error("can't use [CLASS-NAME] after ..");
										break;
									}
									ellipsis = false;
									ellipsisOK = false;
									// get the referenced class and copy in its members
									if (ExpectToken(tok_Identifier, "expected [CLASS-NAME]")) {
										string	refName(asUTF8(tok.strval));
										if (classType == 'U') {
											map<string,UInt32>::const_iterator	c = currentPass.uniClassNames.find(refName);
											if (c == currentPass.uniClassNames.end()) {
												Error("undefined class used", refName.c_str());
												break;
											}
											Class	uc = currentPass.uniClassMembers[c->second];
											for (Class::const_iterator i = uc.begin(); i != uc.end(); ++i)
												classMembers.push_back(*i);
										}
										else {
											map<string,UInt32>::const_iterator	c = currentPass.byteClassNames.find(refName);
											if (c == currentPass.byteClassNames.end()) {
												Error("undefined class used", refName.c_str());
												break;
											}
											Class	bc = currentPass.byteClassMembers[c->second];
											for (Class::const_iterator i = bc.begin(); i != bc.end(); ++i)
												classMembers.push_back(*i);
										}
										if (!ExpectToken(']', "expected closing bracket after CLASS-NAME"))
											break;
									}
								}
								break;
								
							case ')':
								if (ellipsis)
									Error("trailing .. in class");
								break;
								
							case tok_Newline:
								Error("unexpected end of line within class");
								break;
								
							case tok_Identifier:
								Error("unexpected identifier within class", asUTF8(tok.strval).c_str());
								break;

							default:
								Error("unexpected token within class", string(reinterpret_cast<const char *>(tokStart), reinterpret_cast<const char *>(textPtr - tokStart)).c_str());
								break;
						}
					}
					if (tok.type != tok_Newline)
						if (!ExpectToken(tok_Newline, "junk at end of line"))
							break;
					// ok, we've got the class name and members; save it
					if (classType == 'U') {
						if (currentPass.uniClassNames.find(className) != currentPass.uniClassNames.end()) {
							Error("class already defined", className.c_str());
							break;
						}
						currentPass.uniClassNames[className] = currentPass.uniClassMembers.size();
						currentPass.uniClassMembers.push_back(classMembers);
						currentPass.uniClassLines.push_back(classLine);
					}
					else {
						if (currentPass.byteClassNames.find(className) != currentPass.byteClassNames.end()) {
							Error("class already defined", className.c_str());
							break;
						}
						currentPass.byteClassNames[className] = currentPass.byteClassMembers.size();
						currentPass.byteClassMembers.push_back(classMembers);
						currentPass.byteClassLines.push_back(classLine);
					}
					goto GOT_TOKEN;
				}
				break;
		}
	}
	FinishPass();

	// Do we have names for both LHS and RHS? If not, is LHS legacy and RHS Unicode?
	if (names.find(kNameID_LHS_Name) == names.end()) {
		Error("EncodingName or LHSName must be specified");
	}
	const string&   lhs = names[kNameID_LHS_Name];
	if (lhs.find("(REG_ID)") != lhs.npos) {
		Error("Draft mappings generated by Encore2Unicode MUST be reviewed before use");
	}
	if (names.find(kNameID_RHS_Name) == names.end()) {
		if ((lhsFlags & kFlags_Unicode) == 0 || (rhsFlags & kFlags_Unicode) != 0) {
			names[kNameID_RHS_Name] = "UNICODE";
		}
		else {
			Error("RHSName must be specified for non-Legacy/Unicode mapping table");
		}
	}

	if (errorCount == 0) {
		if (generateXML) {
			string	header;
			header += "<?xml version=\"1.0\"?>\n";
			header += "<teckitMapping\n";
	
#define doName(att,name_id)								\
			if (names.find(name_id) != names.end()) {	\
				header += " ";							\
				header += att;							\
				header += "=\"";						\
				header += names[name_id];				\
				header += "\"\n";						\
			}
	
			doName("lhsName", kNameID_LHS_Name);
			doName("rhsName", kNameID_RHS_Name);
			doName("lhsDescription", kNameID_LHS_Description);
			doName("rhsDescription", kNameID_RHS_Description);
			doName("version", kNameID_Version);
			doName("contact", kNameID_Contact);
			doName("registrationAuthority", kNameID_RegAuthority);
			doName("registrationName", kNameID_RegName);
			doName("copyright", kNameID_Copyright);

			if (lhsFlags & kFlags_ExpectsNFC)
				header += " lhsExpects=\"NFC\"\n";
			else if (lhsFlags & kFlags_ExpectsNFD)
				header += " lhsExpects=\"NFD\"\n";
			if (rhsFlags & kFlags_ExpectsNFC)
				header += " rhsExpects=\"NFC\"\n";
			else if (rhsFlags & kFlags_ExpectsNFD)
				header += " rhsExpects=\"NFD\"\n";

			header += ">\n";

			string	trailer("</teckitMapping>\n");
			
			compiledSize = header.length() + xmlRepresentation.length() + trailer.length();
			compiledTable = static_cast<Byte*>(malloc(compiledSize + 1));
			if (compiledTable == NULL)
				throw bad_alloc();
			
			memcpy(compiledTable, header.data(), header.length());
			memcpy(compiledTable + header.length(), xmlRepresentation.data(), xmlRepresentation.length());
			memcpy(compiledTable + header.length() + xmlRepresentation.length(), trailer.data(), trailer.length());
			compiledTable[compiledSize] = 0;

			xmlRepresentation.erase(xmlRepresentation.begin(), xmlRepresentation.end());
		}
		else {
			// assemble the complete compiled file
			FileHeader	fh;
			WRITE(fh.type, kMagicNumber);
			WRITE(fh.version, usedExtStringRules ? kCurrentFileVersion : kFileVersion2_1);
			WRITE(fh.headerLength, 0);	// to be filled in later, once names and table counts are known
			WRITE(fh.formFlagsLHS, lhsFlags);
			WRITE(fh.formFlagsRHS, rhsFlags);
	
			WRITE(fh.numFwdTables, fwdTables.size());
			WRITE(fh.numRevTables, revTables.size());
			WRITE(fh.numNames, names.size());
			
			string	offsets;
			UInt32	offset = sizeof(FileHeader) + (names.size() + fwdTables.size() + revTables.size()) * sizeof(UInt32);
			UInt32	prevLength = 0;
			
			// sort the name IDs into ascending order
			vector<UInt16>	nameIDs;
			nameIDs.reserve(names.size());
			for (map<UInt16,string>::const_iterator n = names.begin(); n != names.end(); ++n) {
				nameIDs.push_back(n->first);
			}
			sort(nameIDs.begin(), nameIDs.end());
			
			// pack all the name records
			string	namesData;
			for (vector<UInt16>::const_iterator i = nameIDs.begin(); i != nameIDs.end(); ++i) {
				appendToTable(offsets, reinterpret_cast<const char*>(&offset), sizeof(offset));
				NameRec	r;
				WRITE(r.nameID, *i);
				WRITE(r.nameLength, names[*i].length());
				namesData.append(reinterpret_cast<const char*>(&r), sizeof(r));
				namesData.append(names[*i]);
				if ((namesData.length() & 1) != 0)
					namesData.append(1, '\0');
				offset += namesData.length() - prevLength;
				prevLength = namesData.length();
			}
			if ((namesData.length() & 2) != 0)
				namesData.append(2, '\0');
			offset += namesData.length() - prevLength;
			
			// pack the offsets to the actual mapping tables
			vector<string>::const_iterator t;
			for (t = fwdTables.begin(); t != fwdTables.end(); ++t) {
				appendToTable(offsets, reinterpret_cast<const char*>(&offset), sizeof(offset));
				offset += t->size();
			}
			for (t = revTables.end(); t != revTables.begin(); ) {
				--t;
				appendToTable(offsets, reinterpret_cast<const char*>(&offset), sizeof(offset));
				offset += t->size();
			}
			
			WRITE(fh.headerLength, sizeof(fh) + offsets.length() + namesData.length());
	
			if (errorCount == 0) {
				// calculate total size of compiled table, malloc() it, and copy everything into it
				compiledSize = sizeof(fh)
							+ offsets.length()
							+ namesData.length();
				for (t = fwdTables.begin(); t != fwdTables.end(); ++t)
					compiledSize += t->length();
				for (t = revTables.begin(); t != revTables.end(); ++t)
					compiledSize += t->length();
	
				compiledTable = static_cast<Byte*>(malloc(compiledSize));
				if (compiledTable != 0) {
					Byte*	cp = compiledTable;
					memcpy(cp, &fh, sizeof(fh));
					cp += sizeof(fh);
					memcpy(cp, offsets.data(), offsets.length());
					cp += offsets.length();
					memcpy(cp, namesData.data(), namesData.length());
					cp += namesData.length();
					for (t = fwdTables.begin(); t != fwdTables.end(); ++t) {
						memcpy(cp, t->data(), t->length());
						cp += t->length();
					}
					for (t = revTables.end(); t != revTables.begin(); ) {
						--t;
						memcpy(cp, t->data(), t->length());
						cp += t->length();
					}
					if (compiledTable + compiledSize != cp)
						cerr << "error!" << endl;
				}
				else
					throw bad_alloc();
			}
			
			if (errorCount == 0 && cmp) {
				// do the compression...
				unsigned long	destLen = compiledSize * 11 / 10 + 20;
				Byte*	dest = static_cast<Byte*>(malloc(destLen + 8));
				if (dest != 0) {
					int	result = compress2(dest + 8, &destLen, compiledTable, compiledSize, Z_BEST_COMPRESSION);
					if (result == Z_OK) {
						destLen += 8;
						dest = static_cast<Byte*>(realloc(dest, destLen)); // shrink dest to fit
						WRITE(reinterpret_cast<FileHeader*>(dest)->type, kMagicNumberCmp);
						WRITE(reinterpret_cast<FileHeader*>(dest)->version, compiledSize);
						free(compiledTable);
						compiledTable = dest;
						compiledSize = destLen;
					}
					else
						free(dest);
				}
			}
		}
	}
}

Compiler::~Compiler()
{
	if (compiledTable != 0)
		free(compiledTable);
}

void
Compiler::GetCompiledTable(Byte*& table, UInt32& len) const
{
	table = compiledTable;
	len = compiledSize;
}

void
Compiler::DetachCompiledTable()
{
	compiledTable = 0;
	compiledSize = 0;
}

string
Compiler::asUTF8(const string32 s)
{
	string	rval;
	string32::const_iterator i;
	for (i = s.begin(); i != s.end(); ++i) {
		UInt32	c = *i;
		int	bytesToWrite;
		if (c < 0x80) {				bytesToWrite = 1;
		} else if (c < 0x800) {		bytesToWrite = 2;
		} else if (c < 0x10000) {	bytesToWrite = 3;
		} else if (c < 0x200000) {	bytesToWrite = 4;
		} else {					bytesToWrite = 2;
									c = 0x0000fffd;
		};
		rval.append(size_t(bytesToWrite), 0);
		size_t index = rval.length();
		switch (bytesToWrite) {	/* note: code falls through cases! */
			case 4:	rval[--index] = static_cast<char>((c | byteMark) & byteMask); c >>= 6;
			case 3:	rval[--index] = static_cast<char>((c | byteMark) & byteMask); c >>= 6;
			case 2:	rval[--index] = static_cast<char>((c | byteMark) & byteMask); c >>= 6;
			case 1:	rval[--index] =  c | firstByteMark[bytesToWrite];
		};
	}
	return rval;
}

void
Compiler::ReadNameString(UInt16 nameID)
{
	if (ExpectToken(tok_String, "expected STRING after name keyword")) {
		if (inputForm == kForm_Bytes) {
			names[nameID].erase(names[nameID].begin(), names[nameID].end());
			for (string32::const_iterator i = tok.strval.begin(); i != tok.strval.end(); ++i)
				names[nameID].append(1, *i);
		}
		else
			names[nameID] = asUTF8(tok.strval);
		ExpectToken(tok_Newline, "junk at end of line");
	}
}

void
Compiler::FinishPass()
{
	if (currentPass.passType == 0)
		return;

	if ((currentPass.passType & 0xFFFF0000) == (FOUR_CHAR_CODE('N','F','_','_') & 0xFFFF0000)) {
		while (errorCount == 0) {
			if (fwdTables.size() == 0)
				lhsFlags |= kFlags_Unicode;
			else {
				if ((rhsFlags & kFlags_Unicode) == 0) {
					Error("normalization only supported in Unicode space");
					break;
				}
			}
			rhsFlags |= kFlags_Unicode;
			string	normTable((currentPass.passType & 0x0000FF00) == (FOUR_CHAR_CODE('_','_','C','_') & 0x0000FF00) 
								? "NFC " : "NFD ");
			if ((currentPass.passType & 0x000000FF) != 'r')
				fwdTables.push_back(normTable);
			if ((currentPass.passType & 0x000000FF) != 'f')
				revTables.push_back(normTable);
			if (generateXML) {
				xmlOut("<pass lhs=\"unicode\" rhs=\"unicode\" line=\"");
				xmlOut(asDec(currentPass.startingLine));
				xmlOut("\">\n");
				xmlOut("<normalize form=\"");
				xmlOut(normTable[2]);
				if ((currentPass.passType & 0x000000FF) == 'f')
					xmlOut(" dir=\"fwd\"");
				else if ((currentPass.passType & 0x000000FF) == 'r')
					xmlOut(" dir=\"rev\"");
				xmlOut("\">\n");
				xmlOut("</pass>\n");
			}
			break;
		}
	}
	else {
		while (errorCount == 0) {
			// not really a loop; just so we can use 'break' to exit early
			bool	sourceUni = (currentPass.passType == kCode_UB) || (currentPass.passType == kCode_Unic);
			bool	targetUni = (currentPass.passType == kCode_BU) || (currentPass.passType == kCode_Unic);

			if (generateXML) {
				// pass header
				xmlOut("<pass lhs=\"");
				xmlOut(sourceUni ? "unicode" : "bytes");
				xmlOut("\" rhs=\"");
				xmlOut(targetUni ? "unicode" : "bytes");
				if (sourceUni != targetUni) {
					xmlOut("\" lhsDefault=\"");
					xmlOut(sourceUni ? asHex(currentPass.uniDefault, 4) : asHex(currentPass.byteDefault, 2));
					xmlOut("\" rhsDefault=\"");
					xmlOut(targetUni ? asHex(currentPass.uniDefault, 4) : asHex(currentPass.byteDefault, 2));
				}
				xmlOut("\" line=\"");
				xmlOut(asDec(currentPass.startingLine));
				xmlOut("\">\n");
				
				// class definitions
				if (currentPass.byteClassMembers.size() > 0 || currentPass.uniClassMembers.size() > 0) {
					xmlOut("<classes>\n");
					unsigned int i;
					for (i = 0; i < currentPass.byteClassMembers.size(); ++i) {
						xmlOut("<class size=\"bytes\" name=\"b_");
						xmlOut(getClassName(currentPass.byteClassNames, i));
						xmlOut("\" line=\"");
						xmlOut(asDec(currentPass.byteClassLines[i]));
						xmlOut("\">");
						for (Class::const_iterator ci = currentPass.byteClassMembers[i].begin(); ci != currentPass.byteClassMembers[i].end(); ++ci) {
							xmlOut(ci == currentPass.byteClassMembers[i].begin() ? "\n" : " ");
							xmlOut(asHex(*ci, 2));
						}
						xmlOut("\n</class>\n");
					}
					for (i = 0; i < currentPass.uniClassMembers.size(); ++i) {
						xmlOut("<class size=\"unicode\" name=\"u_");
						xmlOut(getClassName(currentPass.uniClassNames, i));
						xmlOut("\" line=\"");
						xmlOut(asDec(currentPass.uniClassLines[i]));
						xmlOut("\">");
						for (Class::const_iterator ci = currentPass.uniClassMembers[i].begin(); ci != currentPass.uniClassMembers[i].end(); ++ci) {
							xmlOut(ci == currentPass.uniClassMembers[i].begin() ? "\n" : " ");
							xmlOut(asHex(*ci, 4));
						}
						xmlOut("\n</class>\n");
					}
					xmlOut("</classes>\n");
				}
				
				if (currentPass.xmlContexts.size() > 0) {
					xmlOut("<contexts>\n");
					for (map<string,string>::const_iterator i = currentPass.xmlContexts.begin();
							i != currentPass.xmlContexts.end(); ++i) {
						xmlOut("<context id=\"");
						xmlOut(i->second);
						xmlOut("\">");
						xmlOut(i->first);
						xmlOut("</context>\n");
					}
					xmlOut("</contexts>\n");
				}
				
				xmlOut("<assignments>\n");
				for (vector<string>::const_iterator i = currentPass.xmlRules.begin();
						i != currentPass.xmlRules.end(); ++i) {
					xmlOut(*i);
				}
				xmlOut("</assignments>\n");
				
				// end pass
				xmlOut("</pass>\n");
			}

			if (fwdTables.size() == 0) {
				if (sourceUni)
					lhsFlags |= kFlags_Unicode;
			}
			else {
				if (sourceUni != ((rhsFlags & kFlags_Unicode) != 0)) {
					Error("code space mismatch");
					break;
				}
			}
			rhsFlags &= ~kFlags_Unicode;
			if (targetUni)
				rhsFlags |= kFlags_Unicode;

			// deal with COPY on LHS, and set up class/copy replacement index fields
			associateItems(currentPass.fwdRules, sourceUni, targetUni);
			if (errorCount > 0)
				break;

			setGroupPointers(currentPass.fwdRules);

			// sort rules by length (also propagates repeat counts from EGroup back to BGroup items)
			sortRules(currentPass.fwdRules);
			if (errorCount > 0)
				break;

			// build the forward table
			fwdTables.push_back(string());
			buildTable(currentPass.fwdRules, sourceUni, targetUni, fwdTables.back());
			buildVars.clear();
			if (errorCount > 0)
				break;
			
			// build the reverse table
			associateItems(currentPass.revRules, targetUni, sourceUni);
			if (errorCount > 0)
				break;
			setGroupPointers(currentPass.revRules);
			sortRules(currentPass.revRules);
			if (errorCount > 0)
				break;
			revTables.push_back(string());
			buildTable(currentPass.revRules, targetUni, sourceUni, revTables.back());
			buildVars.clear();
			break;
		}
	}
	currentPass.clear();
	currentPass.setLineNo(lineNumber);
}

void
Compiler::SkipSpaces(void)
{
	while (textPtr < textEnd) {
		UInt32 currCh = getChar();
		if (currCh != ' ' && currCh != '\t') {
			ungetChar(currCh);
			break;
		}
	}
}

Compiler::tokenType
Compiler::IDlookup(const char* str, UInt32 len)
{
	const Keyword	*k = &keywords[0];
	while (k->keyword != 0)
		if (strmatch(k->keyword, str, len)) {
			tok.val = k->refCon;
			return k->token;
		}
		else
			++k;

	// try for a macro
	map<string,tokListT>::const_iterator	i = defines.find(string(str, len));
	if (i != defines.end()) {
		defIter = i->second.begin();
		defEnd = i->second.end();
		tok = *defIter;
		defIter++;
		return tok.type;
	}

	// didn't find the identifier as a keyword; try as a Unicode char name
	// NOTE: the names are now sorted (by Unicode name), so we could use a binary
	//  search here if anyone complains about compilation time when using names :)
	const CharName	*c = &gUnicodeNames[0];
	while (c->name != 0)
		if (unicodeNameCompare(c->name, str, len) == 0) {
			tok.val = c->usv;
			return tok_USV;
		}
		else
			++c;

#ifdef __MWERKS__
	tok.strval.clear();
#else
	tok.strval.erase(tok.strval.begin(), tok.strval.end());
#endif
	while (len-- > 0)
		tok.strval.append(1, static_cast<UInt32>(*str++));
	return tok_Identifier;
}

UInt32
Compiler::getChar()
{
	UInt32	rval = 0;

	if (ungotten != kInvalidChar) {
		rval = ungotten;
		ungotten = kInvalidChar;
		return rval;
	}

#define CHECK_AVAIL(x)				\
	if (textPtr + (x) > textEnd) {	\
			textPtr = textEnd;		\
			return kInvalidChar;	\
	}
	
	switch (inputForm) {
		case kForm_Bytes:
			rval = *textPtr++;
			break;
			
		case kForm_UTF8:
			{
				UInt16 extraBytes = bytesFromUTF8[*textPtr];
				CHECK_AVAIL(extraBytes + 1);
				switch (extraBytes) {	// note: code falls through cases!
					case 5:	rval += *textPtr++; rval <<= 6;
					case 4:	rval += *textPtr++; rval <<= 6;
					case 3:	rval += *textPtr++; rval <<= 6;
					case 2:	rval += *textPtr++; rval <<= 6;
					case 1:	rval += *textPtr++; rval <<= 6;
					case 0:	rval += *textPtr++;
				};
				rval -= offsetsFromUTF8[extraBytes];
			}
			break;

		case kForm_UTF16BE:
			CHECK_AVAIL(2);
			rval = static_cast<UInt32>(*textPtr++) << 8;
			rval += *textPtr++;
			if (rval >= kSurrogateHighStart && rval <= kSurrogateHighEnd) {
				// check that 2 more bytes are available
				CHECK_AVAIL(2);
				UInt32	low = static_cast<UInt32>(*textPtr++) << 8;
				low += *textPtr++;
				rval = ((rval - kSurrogateHighStart) << halfShift) + (low - kSurrogateLowStart) + halfBase;
			}
			break;

		case kForm_UTF16LE:
			CHECK_AVAIL(2);
			rval = *textPtr++;
			rval += static_cast<UInt32>(*textPtr++) << 8;
			if (rval >= kSurrogateHighStart && rval <= kSurrogateHighEnd) {
				CHECK_AVAIL(2);
				UInt32	low = *textPtr++;
				low += static_cast<UInt32>(*textPtr++) << 8;
				rval = ((rval - kSurrogateHighStart) << halfShift) + (low - kSurrogateLowStart) + halfBase;
			}
			break;

		case kForm_UTF32BE:
			CHECK_AVAIL(4);
			rval = static_cast<UInt32>(*textPtr++) << 24;
			rval += static_cast<UInt32>(*textPtr++) << 16;
			rval += static_cast<UInt32>(*textPtr++) << 8;
			rval += *textPtr++;
			break;

		case kForm_UTF32LE:
			CHECK_AVAIL(4);
			rval = *textPtr++;
			rval += static_cast<UInt32>(*textPtr++) << 8;
			rval += static_cast<UInt32>(*textPtr++) << 16;
			rval += static_cast<UInt32>(*textPtr++) << 24;
			break;
	}

	return rval;
}

void
Compiler::ungetChar(UInt32 c)
{
	ungotten = c;
}

bool
Compiler::GetNextToken()
{
	UInt32	currCh;
	
	if (defIter != defEnd) {
		tok = *defIter;
		defIter++;
		return true;
	}

	if (textPtr == textEnd) {
		++textPtr;
		tok.type = tok_Newline;
		return true;	
	}

	if (textPtr >= textEnd)
		return false;

	while (true) {
		SkipSpaces();
		
		tokStart = textPtr;
		
		if (textPtr == textEnd) {
			++textPtr;
			tok.type = tok_Newline;
			++lineNumber;
			return true;
		}
		
		if (textPtr > textEnd)
			return false;
		
		currCh = getChar();
		switch (currCh) {
			case '\r':
				if (textPtr < textEnd) {
					currCh = getChar();
					if (currCh != '\n')
						ungetChar(currCh);
				}
				tok.type = tok_Newline;
				++lineNumber;
				return true;
	
			case '\n':
				if (textPtr < textEnd) {
					currCh = getChar();
					if (currCh != '\r')
						ungetChar(currCh);
				}
				tok.type = tok_Newline;
				++lineNumber;
				return true;
	
			case '\\':
				if (textPtr < textEnd) {
					currCh = getChar();
					if (currCh == '\r' || currCh == '\n') {
						if (textPtr < textEnd) {
							UInt32 nextCh = getChar();
							if (!((currCh == '\r' && nextCh == '\n') || (currCh == '\n' && nextCh == '\r')))
								ungetChar(nextCh);
						}
						++lineNumber;
						continue;
					}
					ungetChar(currCh);
				}
				goto DEFAULT;
	
			case '"':
			case '\'':
				{
					UInt32	delimiter = currCh;
#ifdef __MWERKS__
					tok.strval.clear();
#else
					tok.strval.erase(tok.strval.begin(), tok.strval.end());
#endif
					while ((textPtr < textEnd) && ((currCh = getChar()) != delimiter) && (currCh != '\r') && (currCh != '\n'))
						tok.strval.append(1, currCh);
					tok.type = tok_String;
					if (currCh == '\r' || currCh == '\n')
						ungetChar(currCh);
				}
				return true;
	
			case '^':
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case ',':
			case '+':
			case '*':
			case '?':
			case '>':
			case '#':
			case '|':
			case '/':
			case '=':
			case '@':
				tok.type = tokenType(currCh);
				return true;
	
			case '<':
				tok.type = tokenType('<');
				if (textPtr < textEnd) {
					if ((currCh = getChar()) == '>')
						tok.type = tok_Map;
					else
						ungetChar(currCh);
				}
				return true;
	
			case '.':
				tok.type = tokenType('.');
				if (textPtr < textEnd) {
					if ((currCh = getChar()) == '.')
						tok.type = tok_Ellipsis;
					else
						ungetChar(currCh);
				}
				return true;
			
			case '_':
				if (textPtr < textEnd) {
					currCh = getChar();
					ungetChar(currCh);
					if (isIDcont(currCh)) {
						currCh = '_';
						goto DEFAULT;
					}
				}
				tok.type = tokenType('_');
				return true;
			
			case '0':
				if (textPtr < textEnd) {
					currCh = getChar();
					if (currCh == 'x' || currCh == 'X') {
						tok.type = tok_Number;
						tok.val = 0;
						while (textPtr < textEnd) {
							currCh = getChar();
							if (currCh >= '0' && currCh <= '9')
								tok.val = tok.val * 16 + currCh - '0';
							else if (currCh >= 'a' && currCh <= 'f')
								tok.val = tok.val * 16 + currCh - 'a' + 10;
							else if (currCh >= 'A' && currCh <= 'F')
								tok.val = tok.val * 16 + currCh - 'A' + 10;
							else {
								ungetChar(currCh);
								break;
							}
						}
						return true;
					}
					ungetChar(currCh);
					currCh = '0';
				}
				// else fall through
				
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				tok.type = tok_Number;
				tok.val = currCh - '0';
				while (textPtr < textEnd) {
					currCh = getChar();
					if (currCh >= '0' && currCh <= '9')
						tok.val = tok.val * 10 + currCh - '0';
					else {
						ungetChar(currCh);
						break;
					}
				}
				return true;
	
			case ';':
                {
                	bool	continuation = false;
					while (textPtr < textEnd) {
						continuation = (currCh == '\\');
						currCh = getChar();
						if (currCh == '\r' || currCh == '\n')
							break;
					}
					if (textPtr < textEnd) {
						UInt32	nextCh = getChar();
						if (!((currCh == '\r' && nextCh == '\n') || (currCh == '\n' && nextCh == '\r')))
							ungetChar(nextCh);
					}
					++lineNumber;
					if (continuation)
						continue;
					else {
						tok.type = tok_Newline;
						return true;
					}
	            }
						
			case 'U':
				// check for U+xxxx USV
				if (textPtr < textEnd) {
					currCh = getChar();
					if (currCh == '+') {
						tok.type = tok_USV;
						tok.val = 0;
						int	digitCount = 0;
						while (textPtr < textEnd) {
							currCh = getChar();
							if (currCh >= '0' && currCh <= '9')
								tok.val = tok.val * 16 + currCh - '0';
							else if (currCh >= 'a' && currCh <= 'f')
								tok.val = tok.val * 16 + currCh - 'a' + 10;
							else if (currCh >= 'A' && currCh <= 'F')
								tok.val = tok.val * 16 + currCh - 'A' + 10;
							else {
								ungetChar(currCh);
								break;
							}
							++digitCount;
						}
						if (digitCount < 4 || digitCount > 6) {
							Error("Unicode value (U+xxxx) must have 4-6 hex digits");
							tok.val = 0;
						}
						return true;
					}
					else
						ungetChar(currCh);
				}
				currCh = 'U';
				goto DEFAULT;
						
				// read an identifier or some other 'unknown' character
			default:
			DEFAULT:
				if (isIDstart(currCh)) {
					idBuffer[0] = currCh;
					tok.val = 1;
					while (textPtr < textEnd) {
						currCh = getChar();
						if (!isIDcont(currCh)) {
							ungetChar(currCh);
							break;
						}
						if (tok.val < 256)
							idBuffer[tok.val++] = currCh;
					}
					tok.type = IDlookup(&idBuffer[0], tok.val);
					return true;
				}
				tok.type = tok_Unknown;
				tok.val = currCh;
				return true;
		}
	}
}

bool
Compiler::ExpectToken(tokenType type, const char* errMsg)
{
	if (!GetNextToken() || tok.type != type) {
		Error(errMsg);
		return false;
	}
	return true;
}

void
Compiler::Error(const char* msg, const char* s, UInt32 line)
{
	if (line == 0xffffffff)
		line = lineNumber;
	if (errorFunction == 0) {
		cout << "Error: " << msg;
		if (s != 0)
			cout << ": \"" << s << '"';
		cout << " at line " << line << endl;
	}
	else
		(*errorFunction)(errFuncUserData, const_cast<char*>(msg), const_cast<char*>(s), line);
	errorState = true;
	++errorCount;
}

void
Compiler::StartDefaultPass()
{
	if ((currentPass.passType & 0xFFFF0000) == (FOUR_CHAR_CODE('N','F','_','_') & 0xFFFF0000)) {
		Error("normalization pass cannot contain any other rules");
		currentPass.passType = kCode_Unic;
	}
	if (currentPass.passType == 0) {
		currentPass.clear();	// should already be clear!
		currentPass.passType = kCode_BU;
		currentPass.setLineNo(lineNumber);
	}
}

void
Compiler::AppendToRule(const Item& item)
{
	StartDefaultPass();
	switch (ruleState) {
		case notInRule:
			ruleState = inLHSString;
			currentRule.setLineNo(lineNumber);
		case inLHSString:
			currentRule.lhsString.push_back(item);
			break;
		case inLHSPreContext:
			currentRule.lhsPreContext.push_back(item);
			break;
		case inLHSPostContext:
			currentRule.lhsPostContext.push_back(item);
			break;
		case inRHSString:
			currentRule.rhsString.push_back(item);
			break;
		case inRHSPreContext:
			currentRule.rhsPreContext.push_back(item);
			break;
		case inRHSPostContext:
			currentRule.rhsPostContext.push_back(item);
			break;
	}
}

UInt32
Compiler::charLimit()
{
	UInt32	limit;
	switch (ruleState) {
		case inRHSString:
		case inRHSPreContext:
		case inRHSPostContext:
			limit = (currentPass.passType == kCode_BU || currentPass.passType == kCode_Unic ? 0x10ffff : 0xff);
			break;
		default:
			limit = (currentPass.passType == kCode_UB || currentPass.passType == kCode_Unic ? 0x10ffff : 0xff);
			break;
	}
	return limit;
}

void
Compiler::AppendLiteral(UInt32 val, bool negate)
{
	StartDefaultPass();
	if (val > charLimit()) {
		Error("literal value out of range");
		return;
	}
	Item	item;
	item.type = 0;
	item.negate = negate ? 1 : 0;
	item.repeatMin = 0xff;
	item.repeatMax = 0xff;
	item.val = val;
	AppendToRule(item);
}

void
Compiler::AppendUSV(UInt32 val, bool negate)
{
	StartDefaultPass();
	if (charLimit() == 0xff) {
		Error("can't use Unicode character in byte encoding");
		return;
	}
	AppendLiteral(val, negate);
}

void
Compiler::AppendSpecial(UInt8 type, bool negate)
{
	Item	item;
	item.type = type;
	item.negate = negate ? 1 : 0;
	item.repeatMin = 0xff;
	item.repeatMax = 0xff;
	item.val = 0;
	item.start = item.next = item.after = item.index = 0xff;
	AppendToRule(item);
}

void
Compiler::AppendClass(const string& className, bool negate)
{
	StartDefaultPass();
	Item	item;
	item.type = kMatchElem_Type_Class;
	item.negate = negate ? 1 : 0;
	item.repeatMin = 0xff;
	item.repeatMax = 0xff;
	item.val = 0;
	const map<string,UInt32>*	classNames;
	switch (ruleState) {
		case inRHSString:
		case inRHSPreContext:
		case inRHSPostContext:
			classNames = (currentPass.passType == kCode_Byte || currentPass.passType == kCode_UB)
							? &currentPass.byteClassNames : &currentPass.uniClassNames;
			break;
		default:
			classNames = (currentPass.passType == kCode_Byte || currentPass.passType == kCode_BU)
							? &currentPass.byteClassNames : &currentPass.uniClassNames;
			break;
	}
	map<string,UInt32>::const_iterator	i;
	i = classNames->find(className);
	if (i == classNames->end())
		Error("undefined class", className.c_str());
	else
		item.val = i->second;
	AppendToRule(item);
}

bool
Compiler::tagExists(bool rhs, const string& tag)
{
	if (rhs) {
		if (   (findTag(tag, currentRule.rhsString) != -1)
			|| (findTag(tag, currentRule.rhsPreContext) != -1)
			|| (findTag(tag, currentRule.rhsPostContext) != -1))
			return true;
	}
	else {
		if (   (findTag(tag, currentRule.lhsString) != -1)
			|| (findTag(tag, currentRule.lhsPreContext) != -1)
			|| (findTag(tag, currentRule.lhsPostContext) != -1))
			return true;
	}
	return false;
}

void
Compiler::AssignTag(const string& tag)
{
	if (currentPass.passType == 0 || ruleState == notInRule) {
		Error("item tag doesn't seem to be attached to a rule item", tag.c_str());
		return;
	}
	Item*	item = NULL;
	switch (ruleState) {
		default:
			Error("this can't happen (AssignTag)");
			return;
		case inLHSString:
			if (tagExists(false, tag))
				break;
			item = &currentRule.lhsString.back();
			break;
		case inLHSPreContext:
			if (tagExists(false, tag))
				break;
			item = &currentRule.lhsPreContext.back();
			break;
		case inLHSPostContext:
			if (tagExists(false, tag))
				break;
			item = &currentRule.lhsPostContext.back();
			break;
		case inRHSString:
			if (tagExists(true, tag))
				break;
			item = &currentRule.rhsString.back();
			break;
		case inRHSPreContext:
			if (tagExists(true, tag))
				break;
			item = &currentRule.rhsPreContext.back();
			break;
		case inRHSPostContext:
			if (tagExists(true, tag))
				break;
			item = &currentRule.rhsPostContext.back();
			break;
	}
	if (item == NULL) {
		Error("duplicate tag (ignored)", tag.c_str());
		return;
	}
	if (item->tag.length() > 0) {
		Error("rule item already has a tag", tag.c_str());
		return;
	}
	switch (item->type) {
		case 0:
		case kMatchElem_Type_Class:
		case kMatchElem_Type_EGroup:
		case kMatchElem_Type_ANY:
		case kMatchElem_Type_Copy:
			item->tag = tag;
			break;
			
		default:
			Error("invalid use of item tag", tag.c_str());
			break;
	}
}

void
Compiler::SetMinMax(UInt32 repeatMin, UInt32 repeatMax)
{
	Item*	item = 0;
	switch (ruleState) {
		default:
			Error("invalid use of repeat count");
			break;
		case inLHSString:
			item = &currentRule.lhsString.back();
			break;
		case inLHSPreContext:
			item = &currentRule.lhsPreContext.back();
			break;
		case inLHSPostContext:
			item = &currentRule.lhsPostContext.back();
			break;
		case inRHSString:
			item = &currentRule.rhsString.back();
			break;
		case inRHSPreContext:
			item = &currentRule.rhsPreContext.back();
			break;
		case inRHSPostContext:
			item = &currentRule.rhsPostContext.back();
			break;
	}
	if (item) {
		switch (item->type) {
			case 0:
			case kMatchElem_Type_Class:
			case kMatchElem_Type_ANY:
			case kMatchElem_Type_EGroup:
				if (repeatMin > repeatMax || repeatMax < 1 || repeatMax > 15)
					Error("invalid repeat counts (0-15 allowed)");
				else if (item->repeatMin != 0xff)
					Error("multiple repeat counts on item");
				else {
					item->repeatMin = repeatMin;
					item->repeatMax = repeatMax;
				}
				break;
			default:
				Error("invalid use of repeat count");
				break;
		}
	}
}

void
Compiler::setGroupPointers(vector<Item>::iterator b, vector<Item>::iterator e, int startIndex, bool isReversed)
{
// set up the fwd and back pointers on bgroup/or/egroup
// and propagate repeat counts from egroup to bgroup
	vector<Item>::iterator	base = b;
	vector<Item>::iterator	altStart = startIndex > 0 ? base - 1 : e;
	bool altSeen = false;
	while (b != e) {
		if (b->repeatMin == 0xff)
			b->repeatMin = 1;
		if (b->repeatMax == 0xff)
			b->repeatMax = 1;
		switch (b->type) {
			case 0:	// literal
			case kMatchElem_Type_Class:
			case kMatchElem_Type_ANY:
			case kMatchElem_Type_EOS:
				break;
			
			case kMatchElem_Type_OR:
				// if startIndex > 0, then initial altStart will be valid
				if ((startIndex > 0 || altSeen) && (altStart->type == kMatchElem_Type_OR || altStart->type == kMatchElem_Type_BGroup))
					altStart->next = startIndex + (b - base);
				else {
					Error("this can't happen (setGroupPointers 1)");
					return;
				}
				altStart = b;
				altStart->start = startIndex - 1;
				altSeen = true;
				break;

			case kMatchElem_Type_EGroup:
				Error("this can't happen (setGroupPointers 2)");
				return;

			case kMatchElem_Type_BGroup:
				{
					// need to find corresponding EGroup and copy repeat counts from there
					// (or vice versa if this is reversed context)
					vector<Item>::iterator subGroupStart = b++;
					subGroupStart->next = 0;
					int	nestingLevel = 0;
					while (b->type != kMatchElem_Type_EGroup || nestingLevel > 0) {
						if (b->type == kMatchElem_Type_BGroup)
							++nestingLevel;
						else if (b->type == kMatchElem_Type_EGroup)
							--nestingLevel;
						++b;
					}
					if (isReversed) {
						b->repeatMin = subGroupStart->repeatMin;
						b->repeatMax = subGroupStart->repeatMax;
					}
					else {
						if (b->repeatMin == 0xff)
							b->repeatMin = 1;
						if (b->repeatMax == 0xff)
							b->repeatMax = 1;
						subGroupStart->repeatMin = b->repeatMin;
						subGroupStart->repeatMax = b->repeatMax;
					}
					setGroupPointers(subGroupStart + 1, b, startIndex + (subGroupStart - base + 1), isReversed);
					subGroupStart->after = startIndex + (b - base + 1);
					b->start = startIndex + (subGroupStart - base);
				}
				break;
		}
		++b;
	}
	if (altSeen)
		altStart->next = startIndex + (b - base);	// set NEXT pointer of last OR
	if (startIndex > 0) {	// we were handling a group, so set pointers of EGroup
		if (b->type == kMatchElem_Type_EGroup)
			b->start = startIndex - 1;
		else {
			Error("this can't happen (setGroupPointers 3)");
			return;
		}
	}
}

void
Compiler::setGroupPointers(vector<Rule>& rules)
{
	for (vector<Rule>::iterator i = rules.begin(); i != rules.end(); ++i) {
		setGroupPointers(i->matchStr.begin(), i->matchStr.end(), 0);
		setGroupPointers(i->preContext.begin(), i->preContext.end(), 0, true);
		setGroupPointers(i->postContext.begin(), i->postContext.end(), 0);
	}
}

int
Compiler::calcMaxLen(vector<Item>::iterator b, vector<Item>::iterator e)
{
	int	len = 0;
	int	maxLen = 0;
	while (b != e) {
		switch (b->type) {
			case 0:	// literal
			case kMatchElem_Type_Class:
			case kMatchElem_Type_ANY:
			case kMatchElem_Type_EOS:
				len += b->repeatMax;
				break;
			
			case kMatchElem_Type_OR:
				if (len > maxLen)
					maxLen = len;
				len = 0;
				break;

			case kMatchElem_Type_EGroup:
				Error("this can't happen (calcMaxLen)");
				return 0;

			case kMatchElem_Type_BGroup:
				{
					// need to find corresponding EGroup
					vector<Item>::iterator subGroupStart = b++;
					int	nestingLevel = 0;
					while (b->type != kMatchElem_Type_EGroup || nestingLevel > 0) {
						if (b->type == kMatchElem_Type_BGroup)
							++nestingLevel;
						else if (b->type == kMatchElem_Type_EGroup)
							--nestingLevel;
						++b;
					}
					len += subGroupStart->repeatMax * calcMaxLen(subGroupStart + 1, b);
				}
				break;
		}
		if (b == e)
			break;	// this can happen with nested groups that end together
		++b;
	}
	if (len > maxLen)
		maxLen = len;
	return maxLen;
}

int
Compiler::calcMaxOutLen(Rule& rule)
{
	int	len = 0;
	for (vector<Item>::const_iterator i = rule.replaceStr.begin(); i != rule.replaceStr.end(); ++i) {
		switch (i->type) {
			case 0:
				++len;
				break;
		
			case kMatchElem_Type_Class:
				++len;
				break;
			
			case kMatchElem_Type_Copy:
				{
					vector<Item>::iterator	b = rule.matchStr.begin() + i->index;
					if (b->type == kMatchElem_Type_BGroup)
// 2002-10-01 bugfix for "this can't happen (calcMaxLen)" reported by Bob Eaton
// Note that b->after is absolute index, not relative to b
//						len += calcMaxLen(b + 1, b + b->after - 2);
// 2002-03-19 bugfix: added multiplication by b->repeatMax, as engine copies entire repeated group
//					and corrected ending pointer (-1, not -2)
						len += b->repeatMax * calcMaxLen(b + 1, rule.matchStr.begin() + b->after - 1);
//						b->next - i->index;	// FIXME: this is actually an overestimate of the possible length
					else
						len += b->repeatMax;
				}
				continue;
			
			default:
				cerr << "bad rep elem type: " << i->type << endl;
				break;
		}
	}
	return len;
}

void
Compiler::associateItems(vector<Rule>& rules, bool fromUni, bool toUni)
{
	/*
	handle associations between match and replacement class/copy items:
		1. any COPY items on LHS need to be exchanged with the corresponding sequence from RHS
		2. any CLASS and COPY items on RHS need their /index/ field set according to the
			corresponding LHS item:
			a.	if /tag/ is non-null, look for matching tag
			b.	else use position, checking that corresponding LHS item is valid
				- note that if LHS item is EGroup, we actually need to point at the corresponding BGroup
	*/
	
	for (vector<Rule>::iterator i = rules.begin(); i != rules.end(); ++i) {
		// Deal with COPY items on LHS
		vector<Item>&	m = i->matchStr;
		vector<Item>&	r = i->replaceStr;
		for (UInt32 j = 0; j < m.size(); ++j) {
			if (m[j].type == kMatchElem_Type_Copy) {
				if (m[j].tag.length() == 0) {
					Error("COPY item must have association tag", 0, i->lineNumber);
					continue;
				}
				for (UInt32 k = 0; k < r.size(); ++k) {
					if (r[k].tag == m[j].tag) {
						Item	t = m[j];
						switch (r[k].type) {
							case kMatchElem_Type_EGroup:
								{
									// find corresponding BGroup, and replace m[j] with the entire group
									UInt32	b = k;
									int	nestingLevel = 0;
									while (b != 0) {
										--b;
										if (r[b].type == kMatchElem_Type_EGroup)
											++nestingLevel;
										else if (r[b].type == kMatchElem_Type_BGroup) {
											if (nestingLevel > 0)
												--nestingLevel;
											else
												break;
										}
									}
									if (r[b].type == kMatchElem_Type_BGroup && nestingLevel == 0) {
										m.erase(m.begin() + j);
										m.insert(m.begin() + j, r.begin() + b, r.begin() + k + 1);
										r.erase(r.begin() + b, r.begin() + k + 1);
										r.insert(r.begin() + b, t);
									}
									else {
										Error("can't find complete tagged group for COPY item", r[k].tag.c_str(), i->lineNumber);
										break;
									}
								}
								break;
							
							case 0:
							case kMatchElem_Type_Class:
							case kMatchElem_Type_ANY:
								m[j] = r[k];
								r[k] = t;
								break;
								
							default:
								Error("invalid COPY item in match", r[k].tag.c_str(), i->lineNumber);
								break;
						}
						break;
					}
				}
			}
		}

		// Set up associations
		for (vector<Item>::iterator ir = r.begin(); ir != r.end(); ++ir) {
			int	matchIndex = ir->tag.length() > 0 ? findTag(ir->tag, m) : ir - r.begin();
			if (matchIndex == -1) {
				Error("tag not found", ir->tag.c_str(), i->lineNumber);
				continue;
			}
			if (errorCount > 0)
				break;
			ir->index = matchIndex;
			vector<Item>::const_iterator im;
			if (ir->index < m.end() - m.begin())
				im = m.begin() + ir->index;
			else
				im = m.end();
			switch (ir->type) {
				case kMatchElem_Type_Class:
					if (im == m.end()) {
						Error("class in replacement does not have corresponding match item", 0, i->lineNumber);
						break;
					}
					switch (im->type) {
						case kMatchElem_Type_Class:
							{
								// check that class sizes correspond
								Class&	mc = fromUni ? currentPass.uniClassMembers[im->val] : currentPass.byteClassMembers[im->val];
								Class&	rc = toUni   ? currentPass.uniClassMembers[ir->val] : currentPass.byteClassMembers[ir->val];
								if (mc.size() != rc.size())
									Error("class size mismatch", 0, i->lineNumber);
							}
							break;
						default:
							Error("type mismatch for replacement class item", 0, i->lineNumber);
							break;
					}
					break;
					
				case kMatchElem_Type_Copy:
					// COPY can correspond to anything except COPY on LHS
					if (im == m.end()) {
						Error("COPY in replacement does not have corresponding match item", 0, i->lineNumber);
						break;
					}
					switch (im->type) {
						case kMatchElem_Type_Copy:	
							Error("can't associate COPY elements", 0, i->lineNumber);
							break;
						case kMatchElem_Type_EGroup:
							// change replacement item to point to the BGroup instead
							{
								int	nestingLevel = 0;
								while (1) {
									if (im == m.begin()) {
										Error("this can't happen (associate)", 0, i->lineNumber);
										break;
									}
									--im;
									if (im->type == kMatchElem_Type_EGroup)
										++nestingLevel;
									else if (im->type == kMatchElem_Type_BGroup) {
										if (nestingLevel > 0)
											--nestingLevel;
										else {
											ir->index = im - m.begin();
											break;
										}
									}
								}
							}
							break;
					}
					break;

				case kMatchElem_Type_ANY:
					// ANY on RHS can only correspond with LITERAL or ANY on LHS
					if (im == m.end()) {
						Error("ANY in replacement does not have corresponding match item", 0, i->lineNumber);
						break;
					}
					switch (im->type) {
						case 0:
						case kMatchElem_Type_ANY:
							break;
						default:
							Error("invalid ANY element in replacement", 0, i->lineNumber);
							break;
					}
					break;

				case 0:
					break;
			}
		}
		
		// filter out stuff on the RHS that doesn't belong
		for (UInt32 f = r.size(); f > 0; ) {
			--f;
			switch (r[f].type) {
				case kMatchElem_Type_EOS:
					Error("can't use EOS in replacement", 0, i->lineNumber);
					break;
				case kMatchElem_Type_BGroup:
				case kMatchElem_Type_EGroup:
				case kMatchElem_Type_OR:
					r.erase(r.begin() + f);
					break;
			}
		}
		
		if (errorCount > 0)
			break;
	}
}

int
Compiler::findTag(const string& tag, const vector<Item>& str)
{
	for (vector<Item>::const_iterator i = str.begin(); i != str.end(); ++i)
		if (i->tag == tag)
			return i - str.begin();
	return -1;
}

int
Compiler::ruleKeyComp(const Rule& a, const Rule& b)
{
	if (a.sortKey > b.sortKey)	// higher sortKey comes first
		return -1;
	else if (a.sortKey < b.sortKey)
		return 1;
	else if (a.lineNumber < b.lineNumber)	// lower line number comes first
		return -1;
	else if (a.lineNumber > b.lineNumber)
		return 1;
	else
		return 0;
}

void
Compiler::sortRules(vector<Rule>& rules)
{
	// calculate sort keys based on match string length and context length
	for (vector<Rule>::iterator i = rules.begin(); i != rules.end(); ++i) {
		int	maxMatch = calcMaxLen(i->matchStr.begin(), i->matchStr.end());
		int	maxPre = calcMaxLen(i->preContext.begin(), i->preContext.end());
		int	maxPost = calcMaxLen(i->postContext.begin(), i->postContext.end());
		if (maxMatch + maxPre + maxPost > 255)
			Error("rule too long", 0, i->lineNumber);
		i->sortKey = (maxMatch << 8) + maxPre + maxPost;
		if (maxMatch > buildVars.maxMatch)
			buildVars.maxMatch = maxMatch;
		if (maxPre > buildVars.maxPre)
			buildVars.maxPre = maxPre;
		if (maxPost > buildVars.maxPost)
			buildVars.maxPost = maxPost;
		
		// calculate maxOutput and track highest value
		int	maxOutput = calcMaxOutLen(*i);
		if (maxOutput > 255)
			Error("output too long", 0, i->lineNumber);
		if (maxOutput > buildVars.maxOutput)
			buildVars.maxOutput = maxOutput;
	}
#if 0 // __MWERKS__
	sort(rules.begin(), rules.end(), ruleKeyComp);
#else
	// std library /sort/ crashes in Project Builder version, so do a bubble sort ourselves instead
/*
	for (vector<Rule>::iterator a = rules.begin(); a != rules.end(); ++a)
		for (vector<Rule>::iterator b = rules.end() - 1; b != a; --b)
			if (ruleKeyComp(*(b - 1), *b) > 0)
				swap(*b, *(b - 1));
*/
	// bubble-sorting the actual rules is really expensive; create an index and sort that instead
	vector<UInt32>	ruleIndex;
	for (UInt32 i = 0; i < rules.size(); ++i)
		ruleIndex.push_back(i);
	for (vector<UInt32>::iterator a = ruleIndex.begin(); a != ruleIndex.end(); ++a)
		for (vector<UInt32>::iterator b = ruleIndex.end() - 1; b != a; --b)
			if (ruleKeyComp(rules[*(b - 1)], rules[*b]) > 0)
				swap(*b, *(b - 1));
	vector<Rule>	sortedRules;
	for (vector<UInt32>::iterator s = ruleIndex.begin(); s != ruleIndex.end(); ++s)
		sortedRules.push_back(rules[*s]);
	rules = sortedRules;
#endif
}

bool
Compiler::findInitialItems(const Rule& rule, vector<Item>::const_iterator b, vector<Item>::const_iterator e, vector<Item>& initialItems)
{
// return true if we find a non-optional item, false if we could match null string
	while (b != e) {
		switch (b->type) {
			case 0:
			case kMatchElem_Type_Class:
			case kMatchElem_Type_ANY:
			case kMatchElem_Type_EOS:
				initialItems.push_back(*b);
				if (b->repeatMin > 0)
					return true;
				++b;
				break;
			
			case kMatchElem_Type_BGroup:
				{
					vector<Item>::const_iterator	groupStart = b;
					vector<Item>::const_iterator	altStart = b + 1;
					int		nestingLevel = 0;
					bool	optional = false;
					while (++b != e) {
						switch (b->type) {
							case kMatchElem_Type_BGroup:
								++nestingLevel;
								break;
							case kMatchElem_Type_OR:
								if (nestingLevel == 0) {
									if (!findInitialItems(rule, altStart, b, initialItems))
										optional = true;
									altStart = b + 1;
								}
								break;
							case kMatchElem_Type_EGroup:
								if (nestingLevel == 0) {
									if (!findInitialItems(rule, altStart, b, initialItems))
										optional = true;
								}
								--nestingLevel;
								break;
						}
						if (nestingLevel < 0)
							break;
					}
					if (!optional && groupStart->repeatMin > 0)
						return true;
					++b;
				}
				break;
	
			case kMatchElem_Type_Copy:
				Error("can't use copy item (@tag) on match side of rule", 0, rule.lineNumber);
				++b;
				break;
	
			default:
				Error("this can't happen (findInitialItems)", 0, rule.lineNumber);
				++b;
				break;
		}
	}
	return false;
}

void
Compiler::findInitialItems(const Rule& rule, vector<Item>& initialItems)
{
	bool	foundNonOpt = false;
	if (rule.matchStr.size() > 0)
		foundNonOpt = findInitialItems(rule, rule.matchStr.begin(), rule.matchStr.end(), initialItems);
	if (!foundNonOpt && rule.postContext.size() > 0)
		foundNonOpt = findInitialItems(rule, rule.postContext.begin(), rule.postContext.end(), initialItems);
	if (!foundNonOpt)
		Error("rule must have non-null match string or post-context", 0, rule.lineNumber);
}

long
Compiler::classIndex(UInt32 charCode, const Class& classMembers)
{
	for (Class::const_iterator i = classMembers.begin(); i != classMembers.end(); ++i)
		if (*i == charCode)
			return i - classMembers.begin();
	return -1;
}

bool
Compiler::isSingleCharRule(const Rule& rule)
{
	if (rule.preContext.size() == 0 && rule.postContext.size() == 0 && rule.matchStr.size() == 1) {
		const Item&	item = rule.matchStr.front();
		if (item.repeatMin == 1 && item.repeatMax == 1)
			if (item.type == 0 || item.type == kMatchElem_Type_Class || item.type == kMatchElem_Type_ANY)
				return true;
	}
	return false;
}

void
Compiler::appendMatchElem(string& packedRule, Item& item, unsigned int index,
							vector<MatClass>& matchClasses)
{
	MatchElem	m;
	WRITE(m.value.usv.data, 0);
	WRITE(m.flags.repeat, (static_cast<UInt32>(item.repeatMin) << 4) + item.repeatMax);
	if (item.negate)
		WRITE(m.flags.type, kMatchElem_Negate);
	else
		WRITE(m.flags.type, 0);
	switch (item.type) {
		case 0:
			WRITE(m.value.usv.data, READ(m.value.usv.data) | item.val);
			break;

		case kMatchElem_Type_Class:
			{
				WRITE(m.flags.type, READ(m.flags.type) | (kMatchElem_NonLit + kMatchElem_Type_Class));
				UInt32 i;
				for (i = 0; i < matchClasses.size(); ++i)
					if (matchClasses[i].membersClass == item.val)
						break;
				if (i == matchClasses.size())
					matchClasses.push_back(MatClass(item.val));
				WRITE(m.value.cls.index, i);
			}
			break;

		case kMatchElem_Type_BGroup:
			WRITE(m.flags.type, READ(m.flags.type) | (kMatchElem_NonLit + kMatchElem_Type_BGroup));
			WRITE(m.value.bgroup.dNext, item.next - index);
			WRITE(m.value.bgroup.dAfter, item.after - index);
			break;

		case kMatchElem_Type_EGroup:
			WRITE(m.flags.type, READ(m.flags.type) | (kMatchElem_NonLit + kMatchElem_Type_EGroup));
			WRITE(m.value.egroup.dStart, index - item.start);
			break;

		case kMatchElem_Type_OR:
			WRITE(m.flags.type, READ(m.flags.type) | (kMatchElem_NonLit + kMatchElem_Type_OR));
			WRITE(m.value.egroup.dNext, item.next - index);
			WRITE(m.value.egroup.dStart, index - item.start);
			break;

		case kMatchElem_Type_ANY:
			WRITE(m.flags.type, READ(m.flags.type) | (kMatchElem_NonLit + kMatchElem_Type_ANY));
			break;

		case kMatchElem_Type_EOS:
			WRITE(m.flags.type, READ(m.flags.type) | (kMatchElem_NonLit + kMatchElem_Type_EOS));
			break;
	}
	packedRule.append(reinterpret_cast<const char*>(&m), sizeof(m));
}

void
Compiler::appendReplaceElem(string& packedRule, Item& item, vector<Item>& matchStr, vector<RepClass>& repClasses)
{
	RepElem	r;
	WRITE(r.value, 0);
	switch (item.type) {
		case 0:
			WRITE(r.value, item.val);
			break;
		
		case kRepElem_Class:
			{
				WRITE(r.flags.type, kRepElem_Class);
				WRITE(r.flags.matchIndex, item.index);
				Item&	mItem = matchStr[item.index];
				if (mItem.type != kMatchElem_Type_Class) {
					cerr << "this can't happen (appendReplaceElem)\n";
					exit(1);
				}
				UInt32 i;
				for (i = 0; i < repClasses.size(); ++i)
					if (repClasses[i].membersClass == item.val && repClasses[i].sortLikeClass == mItem.val)
						break;
				if (i == repClasses.size())
					repClasses.push_back(RepClass(item.val, mItem.val));
				WRITE(r.flags.repClass, i);
			}
			break;
		
		case kRepElem_Copy:
			WRITE(r.flags.type, kRepElem_Copy);
			WRITE(r.flags.matchIndex, item.index);
			break;

		case kRepElem_Unmapped:
			WRITE(r.flags.type, kRepElem_Unmapped);
			break;
	}
	packedRule.append(reinterpret_cast<const char*>(&r), sizeof(r));
}

vector<Compiler::Item>
Compiler::reverseContext(const vector<Item>& ctx)
{
	vector<Item>	rval;
	for (vector<Item>::const_iterator i = ctx.begin(); i != ctx.end(); ++i) {
		rval.insert(rval.begin(), *i);
		switch (i->type) {
			case kMatchElem_Type_BGroup:
				rval.front().type = kMatchElem_Type_EGroup;
				break;

			case kMatchElem_Type_EGroup:
				rval.front().type = kMatchElem_Type_BGroup;
				break;
		}
	}
	return rval;
}

void
Compiler::addToCharMap(UInt32 ch, UInt16 index)
{
	UInt8	plane = ch >> 16;
	UInt8	page = (ch & 0x00ffff) >> 8;
	if (buildVars.planeMap.size() <= plane)
		buildVars.planeMap.resize(plane + 1, '\xff');
	if (UInt8(buildVars.planeMap[plane]) == 0xff) {
		buildVars.planeMap[plane] = buildVars.pageMaps.size();
		buildVars.pageMaps.resize(buildVars.pageMaps.size() + 1);
		buildVars.pageMaps.back().resize(256, '\xff');
	}
	UInt8	planeIndex = static_cast<UInt8>(buildVars.planeMap[plane]);
	string&	pageMap = buildVars.pageMaps[planeIndex];
	if (UInt8(pageMap[page]) == 0xff) {
		pageMap[page] = buildVars.charMaps.size();
		buildVars.charMaps.resize(buildVars.charMaps.size() + 1);
		buildVars.charMaps.back().resize(256);
	}
	vector<UInt16>&	charMap = buildVars.charMaps[UInt8(pageMap[page])];
	charMap[ch & 0x0000ff] = index;
}

void
Compiler::align(string& table, string::size_type alignment)
{
	const string::size_type length = table.size();
	const string::size_type remainder = length % alignment;
	if (remainder != 0)
		table.resize(length + alignment - remainder);
}

class Member {
public:
			Member(UInt32 v, UInt32 k)
				: value(v), key(k)
					{ }
	UInt32	value;
	UInt32	key;
	bool	operator<(const Member& rhs) const
					{ return key < rhs.key; }
};

void
Compiler::buildTable(vector<Rule>& rules, bool fromUni, bool toUni, string& table)
{
	TableHeader	th;
	WRITE(th.type, fromUni ? (toUni ? kTableType_UU : kTableType_UB) : (toUni ? kTableType_BU : kTableType_BB));
	WRITE(th.version, kCurrentTableVersion);
	WRITE(th.length, 0);
	WRITE(th.flags, 0);
	WRITE(th.pageBase, 0);
	WRITE(th.lookupBase, 0);
	WRITE(th.matchClassBase, 0); 
	WRITE(th.repClassBase, 0);
	WRITE(th.stringListBase, 0);
	WRITE(th.stringRuleData, 0);
	WRITE(th.maxMatch, buildVars.maxMatch);
	WRITE(th.maxPre, buildVars.maxPre);
	WRITE(th.maxPost, buildVars.maxPost);
	WRITE(th.maxOutput, buildVars.maxOutput);
	WRITE(th.replacementChar, toUni ? currentPass.uniDefault : currentPass.byteDefault);
	
	map<UInt32,UInt16>			charToIndex;
	map<UInt16,UInt32>			indexToChar;
	vector< vector<UInt32> >	rulesForIndex;
	vector<UInt32>				stringRuleLists;
	string						stringRuleData;
	vector<MatClass>			matchClasses;
	vector<RepClass>			repClasses;
#if 0
	UInt32						terminatorRule = 0;
	bool						terminatorCreated = false;
#endif

	if (fromUni) {
		rulesForIndex.resize(1);	// index 0 will be unmapped char
		for (UInt32 i = 0; i != rules.size(); ++i) {
			vector<Item>	initialItems;
			findInitialItems(rules[i], initialItems);
			for (vector<Item>::iterator j = initialItems.begin(); j != initialItems.end(); ++j) {
				UInt32	index;
				switch (j->type) {
					case 0:	// literal
						if (j->negate) {
							Error("can't start with negated Unicode literal", 0, rules[i].lineNumber);
							break;
						}
						if (charToIndex.find(j->val) == charToIndex.end()) {
							// add char to charToIndex mapping
							charToIndex[j->val] = rulesForIndex.size();
							indexToChar[rulesForIndex.size()] = j->val;
							rulesForIndex.resize(rulesForIndex.size() + 1);
							addToCharMap(j->val, charToIndex[j->val]);
						}
						index = charToIndex[j->val];
						if (rulesForIndex[index].size() == 0 || rulesForIndex[index].back() != i) {
							rulesForIndex[index].push_back(i);
							if (rulesForIndex[index].size() == 0x3ff) {
								Error("too many matches with same initial character", 0, rules[i].lineNumber);
							}
						}
						break;
					
					case kMatchElem_Type_Class:
						{
							if (j->negate) {
								Error("can't start with negated Unicode class", 0, rules[i].lineNumber);
								break;
							}
							Class&	uc = currentPass.uniClassMembers[j->val];
							Class::const_iterator u;
							for (u = uc.begin(); u != uc.end(); ++u) {
								if (charToIndex.find(*u) == charToIndex.end()) {
									charToIndex[*u] = rulesForIndex.size();
									indexToChar[rulesForIndex.size()] = *u;
									rulesForIndex.resize(rulesForIndex.size() + 1);
									addToCharMap(*u, charToIndex[*u]);
								}
								index = charToIndex[*u];
								if (rulesForIndex[index].size() == 0 || rulesForIndex[index].back() != i)
									rulesForIndex[index].push_back(i);
							}
						}
						break;
					
					case kMatchElem_Type_ANY:
					case kMatchElem_Type_EOS:
						Error("can't start with ANY or EOS in Unicode match", 0, rules[i].lineNumber);
						break;
					
					default:
						Error("this can't happen (buildTable 1)");
						break;
				}
			}
		}
	}
	else {
		rulesForIndex.resize(256);
		for (UInt32 i = 0; i != rules.size(); ++i) {
			vector<Item>	initialItems;
			findInitialItems(rules[i], initialItems);
			for (vector<Item>::iterator j = initialItems.begin(); j != initialItems.end(); ++j) {
				unsigned int c;
				switch (j->type) {
					case 0:	// literal
						if (j->negate) {
							// add rule to every char except the literal!
							for (c = 0; c < 256; ++c)
								if (c != j->val)
									if (rulesForIndex[c].size() == 0 || rulesForIndex[c].back() != i)
										rulesForIndex[c].push_back(i);
						}
						else
							if (rulesForIndex[j->val].size() == 0 || rulesForIndex[j->val].back() != i)
								rulesForIndex[j->val].push_back(i);
						break;
					
					case kMatchElem_Type_Class:
						{
							Class&	bc = currentPass.byteClassMembers[j->val];
							for (c = 0; c < 256; ++c)
								if ((classIndex(c, bc) != -1) != j->negate)
									if (rulesForIndex[c].size() == 0 || rulesForIndex[c].back() != i)
										rulesForIndex[c].push_back(i);
						}
						break;
					
					case kMatchElem_Type_ANY:
					case kMatchElem_Type_EOS:
						if (j->negate) {
							Error("rule can't start with negated ANY or EOS", 0, rules[i].lineNumber);
							break;
						}
						for (c = 0; c < 256; ++c)
							if (rulesForIndex[c].size() == 0 || rulesForIndex[c].back() != i)
								rulesForIndex[c].push_back(i);
						break;
					
					default:
						Error("this can't happen (buildTable 2)");
						break;
				}
			}
		}
	}

	vector<Lookup>	lookup;
	lookup.resize(rulesForIndex.size());
	
	UInt32 i;
	for (i = 0; errorCount == 0 && i < rulesForIndex.size(); ++i) {
		WRITE(lookup[i].usv, 0);	// initialize the lookup to all zero bits

		if (rulesForIndex[i].size() == 0) {
			WRITE(lookup[i].rules.type, kLookupType_Unmapped);
			continue;
		}

		if (rulesForIndex[i].size() == 1) {
			const Rule&	rule = rules[rulesForIndex[i].front()];
			if (isSingleCharRule(rule)) {
				if (toUni) {
					// mapping to Unicode: direct lookup can only support one Unicode character
					if (rule.replaceStr.size() == 1) {
						const Item&	rep = rule.replaceStr.front();
						vector<Item>::size_type tag;
						if (rep.tag.length() > 0) {
							const int result = findTag(rep.tag, rule.matchStr);
							if (result < 0) {
								Error("tag not found", rep.tag.c_str(), rule.lineNumber);
								continue;
							}
							tag = static_cast<vector<Item>::size_type>(result);
						} else {
							tag = 0;
						}
						switch (rep.type) {
							case 0:
								WRITE(lookup[i].usv, rep.val);
								continue;
						
							case kMatchElem_Type_Class:
								{
									const Item&	mat = rule.matchStr[tag];
									if (mat.type != kMatchElem_Type_Class) {
										Error("improper use of class as target of mapping", 0, rule.lineNumber);
										continue;
									}
									Class&	rc = currentPass.uniClassMembers[rep.val];
									if (fromUni) {
										Class&	mc = currentPass.uniClassMembers[mat.val];
										if (mc.size() != rc.size()) {
											Error("class size mismatch", 0, rule.lineNumber);
											continue;
										}
										WRITE(lookup[i].usv, rc[classIndex(indexToChar[i], mc)]);
									}
									else {
										Class&	mc = currentPass.byteClassMembers[mat.val];
										if (mc.size() != rc.size()) {
											Error("class size mismatch", 0, rule.lineNumber);
											continue;
										}
										WRITE(lookup[i].usv, rc[classIndex(i, mc)]);
									}
								}
								continue;
							
							case kMatchElem_Type_Copy:
								// should only occur in UU table
								if (tag > rule.matchStr.size()) {
									Error("no corresponding item for copy", 0, rule.lineNumber);
									goto ERR_FOUND;
								}
								WRITE(lookup[i].usv, indexToChar[i]);
								continue;
							
							default:
								goto STRING_RULE_NEEDED;
						}
					}
				}
				else {
					// mapping to bytes: we can put up to three bytes into the direct lookup
					if (rule.replaceStr.size() <= 3) {
						WRITE(lookup[i].bytes.count, rule.replaceStr.size());
							// this will get overwritten by lookup[i].rules.type if string rules turn out to be needed
						for (UInt32 j = 0; j < rule.replaceStr.size(); ++j) {
							const Item&	rep = rule.replaceStr[j];
							vector<Item>::size_type tag;
							if (rep.tag.length() > 0) {
								const int result = findTag(rep.tag, rule.matchStr);
								if (result < 0) {
									Error("tag not found", rep.tag.c_str(), rule.lineNumber);
									goto ERR_FOUND;
								}
								tag = static_cast<vector<Item>::size_type>(result);
							} else {
								tag = j;
							}
							switch (rep.type) {
								case 0:	// literal
									WRITE(lookup[i].bytes.data[j], rep.val);
									break;

								case kMatchElem_Type_Class:
									if (tag > rule.matchStr.size()) {
										Error("no corresponding item for class replacement", 0, rule.lineNumber);
										goto ERR_FOUND;
									}
									else {
										const Item&	mat = rule.matchStr[tag];
										if (mat.type != kMatchElem_Type_Class) {
											Error("improper use of class as target of mapping", 0, rule.lineNumber);
											goto ERR_FOUND;
										}
										Class&	rc = currentPass.byteClassMembers[rep.val];
										if (fromUni) {
											Class&	mc = currentPass.uniClassMembers[mat.val];
											if (mc.size() != rc.size()) {
												Error("class size mismatch", 0, rule.lineNumber);
												goto ERR_FOUND;
											}
											WRITE(lookup[i].bytes.data[j], rc[classIndex(indexToChar[i], mc)]);
										}
										else {
											Class&	mc = currentPass.byteClassMembers[mat.val];
											if (mc.size() != rc.size()) {
												Error("class size mismatch", 0, rule.lineNumber);
												goto ERR_FOUND;
											}
											WRITE(lookup[i].bytes.data[j], rc[classIndex(i, mc)]);
										}
									}
									break;

								case kMatchElem_Type_Copy:
									// should only occur in BB table
									if (tag > rule.matchStr.size()) {
										Error("no corresponding item for copy", 0, rule.lineNumber);
										goto ERR_FOUND;
									}
									WRITE(lookup[i].bytes.data[j], i);
									break;

								default:
									goto STRING_RULE_NEEDED;
							}
						}
					ERR_FOUND:
						continue;
					}
				}
			}
		}
		
	STRING_RULE_NEEDED:
#if 0
		// decide whether we need to add a default terminating rule
		const Rule&	finalRule = rules[rulesForIndex[i].back()];
		if (!isSingleCharRule(finalRule)) {
			if (!terminatorCreated) {
				currentRule.clear();
				Item	item;
				item.type = kMatchElem_Type_ANY;
				item.negate = 0;
				item.repeatMin = 1;
				item.repeatMax = 1;
				item.val = 0;
				currentRule.lhsString.push_back(item);
				item.type = kRepElem_Unmapped;
				currentRule.rhsString.push_back(item);
				terminatorRule = rules.size();
				rules.push_back(Rule(currentRule.lhsString,
						currentRule.lhsPreContext, currentRule.lhsPostContext,
						currentRule.rhsString, finalRule.lineNumber));
				terminatorCreated = true;
				currentRule.clear();
			}
			rulesForIndex[i].push_back(terminatorRule);
		}
#endif		
		// set the Lookup fields
		if (rulesForIndex[i].size() > 255) {
			WRITE(lookup[i].rules.type, kLookupType_ExtStringRules + (rulesForIndex[i].size() >> 8));
			WRITE(lookup[i].rules.ruleCount, rulesForIndex[i].size() & 0xff);
			usedExtStringRules = true;
		}
		else {
			WRITE(lookup[i].rules.type, kLookupType_StringRules);
			WRITE(lookup[i].rules.ruleCount, rulesForIndex[i].size());
		}
		WRITE(lookup[i].rules.ruleIndex, stringRuleLists.size());
		
		// construct the rule list
		if (errorCount == 0) {
			for (unsigned int j = 0; j < rulesForIndex[i].size(); ++j) {
				Rule&	r = rules[rulesForIndex[i][j]];
				if (r.offset == kInvalidRuleOffset) {
					r.offset = stringRuleData.size();
					stringRuleData.append(1, r.matchStr.size());
					stringRuleData.append(1, r.postContext.size());
					stringRuleData.append(1, r.preContext.size());
					stringRuleData.append(1, r.replaceStr.size());
					unsigned int k;
					for (k = 0; k < r.matchStr.size(); ++k)
						appendMatchElem(stringRuleData, r.matchStr[k], k, matchClasses);
					for (k = 0; k < r.postContext.size(); ++k)
						appendMatchElem(stringRuleData, r.postContext[k], k, matchClasses);
					for (k = 0; k < r.preContext.size(); ++k)
						appendMatchElem(stringRuleData, r.preContext[k], k, matchClasses);
					for (k = 0; k < r.replaceStr.size(); ++k)
						appendReplaceElem(stringRuleData, r.replaceStr[k], r.matchStr, repClasses);
				}
				stringRuleLists.push_back(r.offset);
			}
		}
	}

	if (fromUni && buildVars.planeMap.size() > 1)
		currentPass.supplementaryChars = true;

	UInt32	headerOffset = table.size();
	table.append(reinterpret_cast<const char*>(&th), sizeof(th));

	if (fromUni) {
		WRITE(th.pageBase, table.size());
		UInt32	i, j;
		if (currentPass.supplementaryChars) {
			buildVars.planeMap.resize(17, '\xff');
			for (i = 0; i < buildVars.planeMap.size(); ++i)
				appendToTable(table, buildVars.planeMap[i]);
			appendToTable(table, UInt8(buildVars.pageMaps.size()));
			align(table, 4);
		}

		for (i = 0; i < buildVars.pageMaps.size(); ++i)
			for (j = 0; j < buildVars.pageMaps[i].size(); ++j)
				appendToTable(table, UInt8(buildVars.pageMaps[i][j]));
		align(table, 4);

		for (i = 0; i < buildVars.charMaps.size(); ++i)
			for (j = 0; j < buildVars.charMaps[i].size(); ++j)
				appendToTable(table, buildVars.charMaps[i][j]);
		align(table, 4);
	}

	WRITE(th.lookupBase, table.size());
	for (i = 0; i < lookup.size(); ++i)
		appendToTable(table, READ(lookup[i].usv));
	align(table, 4);
	
	WRITE(th.stringListBase, table.size());
	for (i = 0; i < stringRuleLists.size(); ++i)
		appendToTable(table, stringRuleLists[i]);
	align(table, 4);
	
	WRITE(th.stringRuleData, table.size());
	for (i = 0; i < stringRuleData.size(); ++i)
		appendToTable(table, stringRuleData[i]);
	align(table, 4);
	
	// sort and output the match classes
	{
		WRITE(th.matchClassBase, table.size() - headerOffset);
		vector<UInt32>	classOffsets;
		classOffsets.resize(matchClasses.size());
		UInt32	classOffset = matchClasses.size() * sizeof(UInt32);
		string	classes;
		
		for (i = 0; i < matchClasses.size(); ++i) {
			classOffsets[i] = classOffset + classes.size();
			Class	sortedClass = fromUni
					? currentPass.uniClassMembers[matchClasses[i].membersClass]
					: currentPass.byteClassMembers[matchClasses[i].membersClass];

			if (sortedClass.size() > 0) {
				sort(sortedClass.begin(), sortedClass.end());
				for (UInt32 j = sortedClass.size() - 1; j > 0; --j)
					if (sortedClass[j] == sortedClass[j - 1])
						sortedClass.erase(sortedClass.begin() + j);
			}

			appendToTable(classes, UInt32(sortedClass.size()));
			if (fromUni)
				if (currentPass.supplementaryChars)
					for (Class::iterator x = sortedClass.begin(); x != sortedClass.end(); ++x)
						appendToTable(classes, *x);
				else
					for (Class::iterator x = sortedClass.begin(); x != sortedClass.end(); ++x)
						appendToTable(classes, UInt16(*x));
			else
				for (Class::iterator x = sortedClass.begin(); x != sortedClass.end(); ++x)
					appendToTable(classes, UInt8(*x));
			align(classes, 4);
		}
		// copy the real classOffsets into the table
		for (i = 0; i < classOffsets.size(); ++i)
			appendToTable(table, classOffsets[i]);
		
		// now append the actual classes
		table.insert(table.end(), classes.begin(), classes.end());
	}
	
	// sort and output the replacement classes
	{
		WRITE(th.repClassBase, table.size());
		vector<UInt32>	classOffsets;
		classOffsets.resize(repClasses.size());
		UInt32	classOffset = repClasses.size() * sizeof(UInt32);
		string	classes;

		for (i = 0; i < repClasses.size(); ++i) {
			classOffsets[i] = classOffset + classes.size();
			vector<Member>	sortedClass;
			const Class&	values = toUni
									? currentPass.uniClassMembers[repClasses[i].membersClass]
									: currentPass.byteClassMembers[repClasses[i].membersClass];
			const Class&	keys = fromUni
									? currentPass.uniClassMembers[repClasses[i].sortLikeClass]
									: currentPass.byteClassMembers[repClasses[i].sortLikeClass];
			for (UInt32 j = 0; j < values.size(); ++j)
				sortedClass.push_back(Member(values[j], keys[j]));

			if (sortedClass.size() > 0) {
				sort(sortedClass.begin(), sortedClass.end());
				for (UInt32 j = sortedClass.size() - 1; j > 0; --j)
					if (sortedClass[j].key == sortedClass[j - 1].key)
						sortedClass.erase(sortedClass.begin() + j);
			}

			appendToTable(classes, UInt32(sortedClass.size()));
			if (toUni)
				if (currentPass.supplementaryChars)
					for (vector<Member>::iterator x = sortedClass.begin(); x != sortedClass.end(); ++x)
						appendToTable(classes, x->value);
				else
					for (vector<Member>::iterator x = sortedClass.begin(); x != sortedClass.end(); ++x)
						appendToTable(classes, UInt16(x->value));
			else
				for (vector<Member>::iterator x = sortedClass.begin(); x != sortedClass.end(); ++x)
					appendToTable(classes, UInt8(x->value));
			align(classes, 4);
		}
		// copy the real classOffsets into the table
		for (i = 0; i < classOffsets.size(); ++i)
			appendToTable(table, classOffsets[i]);
		
		// now append the actual classes
		table.insert(table.end(), classes.begin(), classes.end());
	}
	
	// stuff the real header values into the beginning of the table
	if (currentPass.supplementaryChars)
		WRITE(th.flags, READ(th.flags) | kTableFlags_Supplementary);
	WRITE(th.length, table.size());
	table.replace(0, sizeof(th), reinterpret_cast<const char*>(&th), sizeof(th));
}

void
Compiler::Pass::clear()
{
	fwdRules.clear();
	revRules.clear();
	xmlRules.clear();
	xmlContexts.clear();
	
	byteClassNames.clear();
	uniClassNames.clear();

	byteClassMembers.clear();
	uniClassMembers.clear();

	uniDefault = 0xfffd;	// REPLACEMENT CHARACTER
	byteDefault = '?';
	passType = 0;
	supplementaryChars = false;
	startingLine = 0;
}

void
Compiler::Pass::setLineNo(UInt32 lineNo)
{
	if (startingLine == 0)
		startingLine = lineNo;
}

void
Compiler::BuildVars::clear()
{
	planeMap.erase(planeMap.begin(), planeMap.end());
	pageMaps.clear();
	charMaps.clear();
	maxMatch = 1;
	maxPre = 0;
	maxPost = 0;
	maxOutput = 0;
}

void
Compiler::CurrRule::clear()
{
	lhsString.clear();
	lhsPreContext.clear();
	lhsPostContext.clear();
	rhsString.clear();
	rhsPreContext.clear();
	rhsPostContext.clear();
	startingLine = 0;
}

void
Compiler::CurrRule::setLineNo(UInt32 lineNo)
{
	if (startingLine == 0)
		startingLine = lineNo;
}
