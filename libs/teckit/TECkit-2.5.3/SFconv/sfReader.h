#include <string>
#include <cstdio>

#include "ushort_chartraits.h"

#define END_OF_FILE		-1
#define BODY_TEXT		0
#define SFM				1
#define INLINE_START	2
#define INLINE_END		3
#define INLINE_MARKER	4

template<class C>
class sfReader
{
public:
	typedef std::basic_string<C>			stringT;
	typedef std::char_traits<C>				traitsT;

							sfReader(std::FILE* inFile, char inForm = kForm_Bytes);
							~sfReader();

	long					escapeChar;
	long					inlineEscapeChar;
	long					startInline;
	long					endInline;
	
	stringT					sfmChars;
	stringT					inlineChars;
	
	int						next(bool checkInlineEnd);
	
	stringT					text;
	
protected:
	long					get();
	void					putback(long c);

	long					fSavedChar;

	char					inForm;

	std::FILE*				inFile;
};

template<class C>
sfReader<C>::sfReader(std::FILE* f, char i)
	: inFile(f)
	, inForm(i)
{
	escapeChar			= '\\';
	inlineEscapeChar	= -1;
	startInline			= -1;
	endInline			= -1;
	
	text.reserve(10000);
	fSavedChar = -1;
}

template<class C>
sfReader<C>::~sfReader()
{
}

template<> long
sfReader<char>::get()
{
	long	rval = fSavedChar;
	if (rval == -1)
		rval = getc(inFile);
	else
		fSavedChar = -1;
	return rval;
}

static char bytesFromUTF8[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5};
static unsigned long offsetsFromUTF8[6] =
	{0x00000000UL, 0x00003080UL, 0x000E2080UL, 
 	 0x03C82080UL, 0xFA082080UL, 0x82082080UL};
/*
const unsigned long kReplacementCharacter =	0x0000FFFDUL;
const unsigned long kMaximumUCS2 =			0x0000FFFFUL;
const unsigned long kMaximumUTF16 =			0x0010FFFFUL;
const unsigned long kMaximumUCS4 =			0x7FFFFFFFUL;
*/
const int halfShift							= 10;
const unsigned long halfBase				= 0x0010000UL;
const unsigned long halfMask				= 0x3FFUL;
const unsigned long kSurrogateHighStart		= 0xD800UL;
const unsigned long kSurrogateHighEnd		= 0xDBFFUL;
const unsigned long kSurrogateLowStart		= 0xDC00UL;
const unsigned long kSurrogateLowEnd		= 0xDFFFUL;

template<> long
sfReader<UniChar>::get()
{
	if (fSavedChar != -1) {
		long	rval = fSavedChar;
		fSavedChar = -1;
		return rval;
	}
	long	c1 = getc(inFile);
	if (c1 == -1)
		return -1;
	else {
		if (inForm == kForm_UTF8) {
			long	t1, t2 = -1;
			register unsigned long ch = c1;
			register unsigned short extraBytes = bytesFromUTF8[c1];
			register long	c2;
			switch(extraBytes) {	/* note: code falls through cases! */
				case 5:	c2 = getc(inFile); if (c2 == -1) return -1; ch <<= 6; ch += c2;
				case 4:	c2 = getc(inFile); if (c2 == -1) return -1; ch <<= 6; ch += c2;
				case 3:	c2 = getc(inFile); if (c2 == -1) return -1; ch <<= 6; ch += c2;
				case 2:	c2 = getc(inFile); if (c2 == -1) return -1; ch <<= 6; ch += c2;
				case 1:	c2 = getc(inFile); if (c2 == -1) return -1; ch <<= 6; ch += c2;
				case 0:	;
			};
			ch -= offsetsFromUTF8[extraBytes];
			if (ch <= UNI_MAX_BMP) {
				t1 = ch;
			} else if (ch > UNI_MAX_UTF16) {
				t1 = UNI_REPLACEMENT_CHAR;
			} else {
				ch -= halfBase;
				t1 = (ch >> halfShift) + kSurrogateHighStart;
				t2 = (ch & halfMask) + kSurrogateLowStart;
			}
			fSavedChar = t2;
			return t1;
		}
		else {
			long	c2 = getc(inFile);
			if (c2 == -1)
				return -1;
			if (inForm == kForm_UTF16BE)
				return (c1 << 8) + c2;
			else
				return (c2 << 8) + c1;
		}
	}
}

template<class C>
void
sfReader<C>::putback(long c)
{
	fSavedChar = c;
}

template<class C>
int
sfReader<C>::next(bool checkInlineEnd)
{
	long	c = get();
	if (c == traitsT::eof())
		return END_OF_FILE;

	text.clear();
	
	if (c == escapeChar) {
		while (1) {
			c = get();
			if (c == traitsT::eof()) {
				return SFM;
			}
			if (sfmChars.find(c, 0) != stringT::npos) {
				text.append(1, c);
			}
			else if (text.length() == 0) {
				text.append(1, c);
				return SFM;
			}
			else {
				putback(c);
				return SFM;
			}
		}
	}

	if (c == inlineEscapeChar) {
		while (1) {
			c = get();
			if (c == traitsT::eof()) {
				return INLINE_MARKER;
			}
			if (inlineChars.find(c, 0) != stringT::npos)
				text.append(1, c);
			else if (text.length() == 0) {
				text.append(1, c);
				c = get();
				if (c == startInline)
					return INLINE_START;
				else {
					putback(c);
					return INLINE_MARKER;
				}
			}
			else {
				if (c == startInline)
					return INLINE_START;
				else {
					putback(c);
					return INLINE_MARKER;
				}
			}
		}
	}
	
	if (checkInlineEnd && c == endInline) {
		return INLINE_END;
	}

	// must be "junk" before the first SFM
	text.append(1, c);

	while (1) {
		c = get();
		if (c == traitsT::eof() || c == escapeChar || c == inlineEscapeChar || (checkInlineEnd && c == endInline)) {
			putback(c);
			return BODY_TEXT;
		}
		text.append(1, c);
	}
}
