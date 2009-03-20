//
//  Copyright 2005 Massimiliano Gubinelli.
//

// Based on a Perl script by Jonathan Kew, SIL International (February 2005)

/****************************************************************************\
Permission is hereby granted, free of charge, to any person obtaining  
a copy of this software and associated documentation files (the  
"Software"), to deal in the Software without restriction, including  
without limitation the rights to use, copy, modify, merge, publish,  
distribute, sublicense, and/or sell copies of the Software, and to  
permit persons to whom the Software is furnished to do so, subject to  
the following conditions:

The above copyright notice and this permission notice shall be  
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,  
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF  
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND  
NONINFRINGEMENT. IN NO EVENT SHALL MASSIMILIANO GUBINELLI OR
SIL INTERNATIONAL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
\****************************************************************************/

#import <Foundation/Foundation.h>
//#import <ATS/ATS.h> // you need ApplicationServices.framework

#import "SimplePSInterpreter.h"

#import <unistd.h> // for getopt()

#include "config.h"

static inline UInt16
SWAP16(UInt16 v)
{
#ifdef WORDS_BIGENDIAN
	return v;
#else
	return (v >> 8) | (v << 8);
#endif
}

static inline UInt32
SWAP32(UInt32 v)
{
#ifdef WORDS_BIGENDIAN
	return v;
#else
	return (v >> 24) | ((v >> 8) & 0x0000FF00) | ((v << 8) & 0x00FF0000) | (v << 24);
#endif
}

static NSString *T1WrapExceptionName = @"T1WrapException";

/* typ1 Table  */
enum {
  typ1TableTag           = 0x54595031 // 'TYP1'
};

typedef struct  {
  FourCharCode        version;
  UInt16 flags;
  UInt16 glyphCount;
  UInt32 totalLength;
  UInt32 asciiLength;
  UInt32 binaryLength;
  SInt32 subrMaxLength;
} typ1Header;

/* HFMX Table  */
enum {
  HFMXTableTag           = 0x48464D58 // 'HFMX'
};

typedef struct {
  Fixed Version;
  SInt16 Ascent;
  SInt16 Descent;
  SInt16 LineGap;
  SInt16 CaretSlopeRise;
  SInt16 CaretSlopeRun;
  SInt16 CaretOffset;
} HFMXHeader;

// format 6 subheader structure
struct sfntCMapSubHeader6 {
  UInt16              format;
  UInt16              length;
  UInt16              languageID;             /* base-1 */
  UInt16              firstCode;
  UInt16              entryCount;
};
typedef struct sfntCMapSubHeader6        sfntCMapSubHeader6;
enum {
  sizeof_sfntCMapSubHeader6      = 10
};


/* post Table  */
enum {
  postTableTag           = 0x706F7374 // 'post'
};

struct sfntPostHeader {
  Fixed version; /* format version number */
  Fixed italicAngle; /* italic angle in degrees */
  SInt16 underlinePosition;  /* underline position (in FUnits) */
  SInt16 underlineThickness; /* underline thickness (in FUnits) */
  UInt16 isFixedPitch;  /* font is monospaced */
  UInt16 reserved;
  UInt32 minMemType42; /*  Minimum memory usage when a TrueType font is downloaded */
  UInt32 maxMemType42; /* Maximum memory usage when a TrueType font is downloaded as a Type 1 font */
  UInt32 minMemType1; /* Minimum memory usage when a TrueType font is downloaded as a Type 1 font */
  UInt32 maxMemType1; /* Maximum memory usage when a TrueType font is downloaded as a Type 1 font */
//  UInt16 glyphCount; /* this is for version 2.0 post header */
}; 

typedef struct sfntPostHeader sfntPostHeader;
enum {
  sizeof_sfntPostHeader      = 32
};
/* BUG?: I do not know if it is a bug but it happens that sizeof(sfntPostHeader) = 36 !?! */
/* No, it's not a bug, that would be due to long-alignment of the structure.
   But the glyphCount field is not strictly part of the 'post' table header,
   it is part of the format-specific data for 'post' format 2.0.
   So I've removed that, and it is handled separately below. */

unsigned long getPfbLong(const unsigned char *ptr)
{
  unsigned long c0 = ptr[0];
  unsigned long c1 = ptr[1];
  unsigned long c2 = ptr[2];
  unsigned long c3 = ptr[3];
  
  return (c0 | (c1 << 8) | (c2 <<16) | (c3<<24));
}



NSData *decrypt(const unsigned char* inbuf, unsigned int insize)
{
  NSMutableData *result = [NSMutableData dataWithLength:insize];
  unsigned char *outbuf = [result mutableBytes];
  
  unsigned short R = 55665; /* key */
  unsigned short c1 = 52845;
  unsigned short c2 = 22719;
  unsigned long counter;
  
  for(counter = 0; counter < insize; counter++) {
	outbuf[counter] = (inbuf[counter] ^ (R>>8));
	R = (inbuf[counter] + R) * c1 + c2;
  }
  
  return result;  
}

/* provide strnstr here because it's not available on 10.2 */
char *strnstr(const char *big, const char *little, size_t bigLen)
{
	unsigned littleLen = strlen(little);
	if (littleLen > bigLen)
		return 0;

	unsigned n;
	for (n = bigLen - littleLen + 1; n-- > 0; ++big) {
		if (memcmp(big, little, littleLen) == 0)
			return (char*)big;
	}

	return 0;
}

NSData *digestPfbForSfnt(NSString *pfbPath, typ1Header *t1h)
{
  static char *target1 = "currentfile eexec";
  static char *target2 = "0000000000000000000000000000000000000000000000000000000000000000";
  static char *target3 = "mark currentfile closefile";
  /* despite what the "Type 1 sfnt" document says, these don't appear to be necessary */
  static char *replacement1 = "\nsystemdict begin\n";	
  static char *replacement2 = "\nend\n";
  
  
  NSData *pfbData = [NSData dataWithContentsOfFile:pfbPath];
  if (!pfbData) 
	[NSException raise:T1WrapExceptionName format:@"Cannot read file:%@",pfbPath];		  
  
  NSMutableData *outData = [NSMutableData dataWithCapacity:[pfbData length]];
  if (!outData)
	[NSException raise:T1WrapExceptionName format:@"Cannot allocate workspace"];		  
  
  const char *pfbBytes = [pfbData bytes];
  unsigned long chunkSize;
  unsigned long asciiSize = 0, binSize = 0;
  while (1) {
	if ((unsigned char)pfbBytes[0] != 0x80 )	
	  [NSException raise:T1WrapExceptionName format:@"Wrong PFB format"];
	
	if (pfbBytes[1] == 0x01)
	{
	  // read the ASCII chunk (before and after binary chunks)
	  chunkSize = getPfbLong((unsigned char*)pfbBytes+2);
	  
	  const char * p0 = pfbBytes+6;
	  char * p1, *p2;
	  p1 = strnstr(p0,target1,chunkSize);
	  if (p1) {
		if (p1 != p0+chunkSize-strlen(target1)-1) 
		  [NSException raise:T1WrapExceptionName format:@"I'm confused..."];
		[outData appendBytes:p0 length:p1-p0];
		[outData appendBytes:replacement1 length:strlen(replacement1)];
		asciiSize = p1-p0+strlen(replacement1);
	  } else {
		p2 = strnstr(p0,target2,chunkSize);
		if (p2) {
#if 0
		  /* the following test if false with: '\r00000...' */
		  if (p2 != p0) 
			[NSException raise:T1WrapExceptionName format:@"I'm confused..."];
#endif
		  ;
		  //[outData appendBytes:p0 length:chunkSize];
		}
		else
		  [NSException raise:T1WrapExceptionName format:@"Cannot find begin/end of encrypted portion"];		  
	  }
	}
	else if (pfbBytes[1] == 0x02)
	{
	  // read the binary chunk
	  chunkSize = getPfbLong((unsigned char*)pfbBytes+2);
	  NSData *decryptedData = decrypt((unsigned char*)pfbBytes+6,chunkSize);
	  const char *p0 = [decryptedData bytes];
	  const char *p1 = p0+chunkSize-(int)strlen(target3)-1;
	  if (strncmp(target3,p1,strlen(target3))!=0)
		[NSException raise:T1WrapExceptionName format:@"Cannot find internal end of encrypted portion"];		  
	  
	  [outData appendBytes:p0+4 length:p1-p0-4]; /* +4 is to skip YNDY */
	  [outData appendBytes:replacement2 length:strlen(replacement2)];
	  binSize = p1-p0-4+strlen(replacement2);
	}
	else if (pfbBytes[1] == 0x03)
	  break;
	else  
	  [NSException raise:T1WrapExceptionName format:@"Wrong PFB format"];
	pfbBytes += chunkSize+6;
  } // while (1)
  
  // fill the table header
  
  t1h->version = SWAP32(0x00010000);
  t1h->flags = SWAP16(0); 
  t1h->glyphCount = SWAP16(130); /* number of elements of CharStrings array/dict */
  t1h->totalLength = SWAP32(asciiSize+binSize);
  t1h->asciiLength = SWAP32(asciiSize);
  t1h->binaryLength = SWAP32(binSize);
  t1h->subrMaxLength = SWAP32(-1);
  
  // pad the data to long boundary
  {
	unsigned x = [outData length]%4; 
	unsigned long y = 0;
	if (x) [outData appendBytes:&y length:4-x];
  }
  
  return outData;
}



void fillTag(FourCharCode *tag, char *str)
{
  char * p = (char*)tag;
  p[0] = str[0];
  p[1] = str[1];
  p[2] = str[2];
  p[3] = str[3];
}

unsigned long calcTableChecksum(register unsigned long *table, unsigned long length)
{
  register unsigned long sum = 0L;
  register unsigned long *endptr = table+((length+3) & ~3) / sizeof(long);
  while (table < endptr)
	sum += SWAP32(*table++);
  return sum;
}

unsigned four_align(unsigned a) { return ((a+3)/4)*4; }


NSData *wrapPfb(NSString *fontPath, BOOL omitType1Data)
{
  
  unsigned totalSize = 0;
  
  // needed tables: HFMX, TYP1, cmap, fdsc, name
  // we add also a post table (suggested by Jonathan Kew)
  
  
  // TYP1 table
  
  typ1Header *t1h = calloc(1,sizeof(typ1Header));  
  NSData *type1Data = digestPfbForSfnt(fontPath,t1h);
  
  totalSize += sizeof(typ1Header) + [type1Data length];

  // parse pfb
  
  NSDictionary *fontDir = nil;

  {
	SimplePSInterpreter *psi = [[SimplePSInterpreter alloc] init];
//	[psi doString:[NSString stringWithCString:[type1Data bytes] length:[type1Data length]]];
    [psi doString:[[NSString alloc] initWithData:type1Data encoding:NSMacOSRomanStringEncoding]];
	fontDir = [[[psi fontDir] retain] autorelease];
	[psi release];
  }
  
  if ([fontDir count]!=1) 
	[NSException raise:T1WrapExceptionName format:@"expecting only one font definition in pfb (found many or none)"];		  

  NSDictionary *fontDict = (NSDictionary*)[[fontDir allValues] objectAtIndex:0];
  NSDictionary *fontInfo = [fontDict objectForKey:@"/FontInfo"];
  NSDictionary *charStrings = [fontDict objectForKey:@"/CharStrings"];
  
  int glyphCount = [charStrings count]-1; // subtract 1 for the [KEY ORDER] key added by the pseudo-interpreter
  
  t1h->glyphCount = SWAP16(glyphCount);
  
  NSMutableData *type1TableData = [NSMutableData dataWithCapacity:sizeof(typ1Header)+[type1Data length]];
  [type1TableData appendBytes:t1h length:sizeof(typ1Header)];
  [type1TableData appendData:type1Data];
  
  // create glyphNames array
  
  NSMutableArray *glyphNames = [NSMutableArray arrayWithCapacity:glyphCount];

  {
	NSEnumerator *en = [[charStrings objectForKey:@"[KEY ORDER]"] objectEnumerator];
	NSString *name;
	[glyphNames addObject:@"/.notdef"];
	while ((name = [en nextObject]))
	{
	  if (![name isEqualToString:@"/.notdef"]) [glyphNames addObject:name];
	}
  }
  
  // HFMX table
  
  unsigned HFMXTableSize = sizeof(HFMXHeader);
  HFMXHeader *hh = calloc(1,HFMXTableSize);
  
  hh->Version = SWAP32(0x00010000);
  hh->Ascent = SWAP16([(NSNumber*)[(NSArray*)[(NSArray*)[fontDict objectForKey:@"/FontBBox"] objectAtIndex:1] objectAtIndex:3] intValue]);
  hh->Descent = SWAP16(-[(NSNumber*)[(NSArray*)[(NSArray*)[fontDict objectForKey:@"/FontBBox"] objectAtIndex:1] objectAtIndex:1] intValue]);
  hh->LineGap = SWAP16(0);
  hh->CaretSlopeRise = SWAP16(1); 
  hh->CaretSlopeRun = SWAP16(0);
  hh->CaretOffset = SWAP16(0);
  
  totalSize += HFMXTableSize;
  
  // name table

  NSMutableArray *nameIdArray = [NSMutableArray arrayWithCapacity:10];
  NSMutableArray *stringsArray = [NSMutableArray arrayWithCapacity:10];
  NSString *string;
  
  [nameIdArray addObject:[NSNumber numberWithInt:kFontPostscriptName]];
  string = [fontDict objectForKey:@"/FontName"];
  if ([string characterAtIndex:0] == '/') string = [string substringFromIndex:1];
  [stringsArray addObject:string];
  
  string = [fontInfo objectForKey:@"/FullName"];
  if (string) {
	[nameIdArray addObject:[NSNumber numberWithInt:kFontFullName]];
	[stringsArray addObject:string];
  }
  
  string = [fontInfo objectForKey:@"/FamilyName"];
  if (string) {
    [nameIdArray addObject:[NSNumber numberWithInt:kFontFamilyName]];
	[stringsArray addObject:string];
  }

  string = [fontInfo objectForKey:@"/Copyright"];
  if (string) {
    [nameIdArray addObject:[NSNumber numberWithInt:kFontCopyrightName]];
	[stringsArray addObject:string];
  }

  string = [fontInfo objectForKey:@"/Notice"];
  if (string) {
    [nameIdArray addObject:[NSNumber numberWithInt:([fontDict objectForKey:@"/Copyright"] ? kFontCopyrightName : kFontDescriptionName)]];
	[stringsArray addObject:string];
  }

  string = [fontInfo objectForKey:@"/version"];
  if (string) {
    [nameIdArray addObject:[NSNumber numberWithInt:kFontVersionName]];
	[stringsArray addObject:string];
  }
  

  
  NSMutableData *nameRecs = [NSMutableData dataWithCapacity:100];
  NSMutableData *nameStrings = [NSMutableData dataWithCapacity:100];
  
  sfntNameRecord rec;
  int nameRecords = 0;
  rec.platformID = SWAP16(kFontMacintoshPlatform);
  rec.scriptID = SWAP16(kFontRomanScript);
  rec.languageID = SWAP16(kFontEnglishLanguage);

  NSEnumerator *nameIdEn = [nameIdArray objectEnumerator];
  NSEnumerator *stringsEn = [stringsArray objectEnumerator];
  NSNumber *nameIdObj;
  while ((nameIdObj = [nameIdEn nextObject]) && (string = [stringsEn nextObject]))
  {
	rec.nameID = SWAP16([nameIdObj intValue]);
	rec.length = SWAP16([string length]);
	rec.offset = SWAP16([nameStrings length]);
	[nameStrings appendBytes:[string cString] length:[string cStringLength]];
	[nameRecs appendBytes:&rec length:sizeof(sfntNameRecord)];
	nameRecords++;
  }
  
  unsigned nameTableSize = 	four_align(	sizeof_sfntNameHeader+
										[nameRecs length]+
										[nameStrings length]);
										
  
  sfntNameHeader nameh;
  nameh.format = SWAP16(0);
  nameh.count = SWAP16(nameRecords);
  nameh.stringOffset = SWAP16(sizeof_sfntNameHeader + [nameRecs length]);
  
  NSMutableData *nameTable = [NSMutableData dataWithCapacity:nameTableSize];
  [nameTable appendBytes:&nameh length:sizeof_sfntNameHeader];
  [nameTable appendData:nameRecs];
  [nameTable appendData:nameStrings];
  [nameTable increaseLengthBy:nameTableSize-[nameTable length]]; // pad with zeros
  
  totalSize += nameTableSize;
    
  // fdsc table
  
  unsigned fdscTableSize = four_align(sizeof_sfntDescriptorHeader+sizeof(sfntFontDescriptor)*2);
  sfntDescriptorHeader *dh = calloc(1,fdscTableSize);							
  dh->version = SWAP32(0x00010000);
  dh->descriptorCount = SWAP32(2);
	/* FIXME: could try to infer better values from known font names like "bold", "condensed", etc */
  dh->descriptor[0].name = SWAP32(0x77647468);  /* wdth */
  dh->descriptor[0].value = SWAP32(0x00010000); /* +1.0 */
  dh->descriptor[1].name = SWAP32(0x77676874);  /* wght */
  dh->descriptor[1].value = SWAP32(0x00010000); /* +1.0 */
  
  totalSize += fdscTableSize;
  
  // cmap table
  NSArray *encoding = [(NSArray*)[fontDict objectForKey:@"/Encoding"] objectAtIndex:1];
  unsigned encsize = [encoding count];
  
  unsigned cmapTableSize;
  sfntCMapHeader *cmaph;
  
  if (glyphCount > 256) {
	// more than 256 glyphs: use format 6 cmap
	cmapTableSize = four_align(sizeof_sfntCMapHeader+sizeof_sfntCMapEncoding+sizeof_sfntCMapSubHeader6+256*2);
	cmaph = calloc(1,cmapTableSize);	
	sfntCMapSubHeader6 *cmapsh = (sfntCMapSubHeader6*)(((char*)cmaph)+12);
	UInt16 *glyphIndexArray = (UInt16*)(((char*)cmaph)+12+sizeof_sfntCMapSubHeader6);
	cmaph->version = 0;
	cmaph->numTables = SWAP16(1);
	cmaph->encoding[0].platformID = SWAP16(kFontMacintoshPlatform);
	cmaph->encoding[0].scriptID = SWAP16(kFontRomanScript);
	cmaph->encoding[0].offset = SWAP32(12);
	cmapsh->format = SWAP16(6);
	cmapsh->length = SWAP16(10+256*2);
	cmapsh->languageID = SWAP16(0);
	cmapsh->firstCode = SWAP16(0);
	cmapsh->entryCount = SWAP16(256);
	{
	  int i;
	  NSString *name;
	  for (i=0; i<256; i++) {		
		if (i < encsize) {
		  name = [encoding objectAtIndex:i];
		  if ([name isEqualToString:@" <NULL> "]) name = @"/.notdef";		  
		} else name = @"/.notdef";		
		glyphIndexArray[i] = SWAP16([glyphNames indexOfObject:name]);
	  }
	}
  } else {
	// less than 256 glyphs: use format 0 cmap
	cmapTableSize = four_align(sizeof_sfntCMapHeader+sizeof_sfntCMapEncoding+sizeof_sfntCMapSubHeader+256);
	cmaph = calloc(1,cmapTableSize);	
	sfntCMapSubHeader *cmapsh = (sfntCMapSubHeader*)(((char*)cmaph)+12);
	UInt8 *glyphIndexArray = (UInt8*)(((char*)cmaph)+12+sizeof_sfntCMapSubHeader);
	cmaph->version = 0;
	cmaph->numTables = SWAP16(1);
	cmaph->encoding[0].platformID = SWAP16(kFontMacintoshPlatform);
	cmaph->encoding[0].scriptID = SWAP16(kFontRomanScript);
	cmaph->encoding[0].offset = SWAP32(12);
	cmapsh->format = SWAP16(0);
	cmapsh->length = SWAP16(6+256);
	cmapsh->languageID = SWAP16(0);
	{
	  int i;
	  NSString *name;
	  for (i=0; i<256; i++) {		
		if (i < encsize) {
		  name = [encoding objectAtIndex:i];
		  if ([name isEqualToString:@" <NULL> "]) name = @"/.notdef";		  
		} else name = @"/.notdef";		
		glyphIndexArray[i] = [glyphNames indexOfObject:name];
	  }
	}
  }
  
  totalSize += cmapTableSize;
  
  // post table

  NSArray *appleGlyphNames = [NSArray arrayWithObjects:
	  @".notdef",@".null",@"nonmarkingreturn",@"space",@"exclam",@"quotedbl",@"numbersign",@"dollar",
	  @"percent",@"ampersand",@"quotesingle",@"parenleft",@"parenright",@"asterisk",@"plus",@"comma",
	  @"hyphen",@"period",@"slash",@"zero",@"one",@"two",@"three",@"four",
	  @"five",@"six",@"seven",@"eight",@"nine",@"colon",@"semicolon",@"less",
	  @"equal",@"greater",@"question",@"at",@"A",@"B",@"C",@"D",
	  @"E",@"F",@"G",@"H",@"I",@"J",@"K",@"L",
	  @"M",@"N",@"O",@"P",@"Q",@"R",@"S",@"T",
	  @"U",@"V",@"W",@"X",@"Y",@"Z",@"bracketleft",@"backslash",
	  @"bracketright",@"asciicircum",@"underscore",@"grave",@"a",@"b",@"c",@"d",
	  @"e",@"f",@"g",@"h",@"i",@"j",@"k",@"l",
	  @"m",@"n",@"o",@"p",@"q",@"r",@"s",@"t",
	  @"u",@"v",@"w",@"x",@"y",@"z",@"braceleft",@"bar",
	  @"braceright",@"asciitilde",@"Adieresis",@"Aring",@"Ccedilla",@"Eacute",@"Ntilde",@"Odieresis",
	  @"Udieresis",@"aacute",@"agrave",@"acircumflex",@"adieresis",@"atilde",@"aring",@"ccedilla",
	  @"eacute",@"egrave",@"ecircumflex",@"edieresis",@"iacute",@"igrave",@"icircumflex",@"idieresis",
	  @"ntilde",@"oacute",@"ograve",@"ocircumflex",@"odieresis",@"otilde",@"uacute",@"ugrave",
	  @"ucircumflex",@"udieresis",@"dagger",@"degree",@"cent",@"sterling",@"section",@"bullet",
	  @"paragraph",@"germandbls",@"registered",@"copyright",@"trademark",@"acute",@"dieresis",@"notequal",
	  @"AE",@"Oslash",@"infinity",@"plusminus",@"lessequal",@"greaterequal",@"yen",@"mu",
	  @"partialdiff",@"summation",@"product",@"pi",@"integral",@"ordfeminine",@"ordmasculine",@"Omega",
	  @"ae",@"oslash",@"questiondown",@"exclamdown",@"logicalnot",@"radical",@"florin",@"approxequal",
	  @"Delta",@"guillemotleft",@"guillemotright",@"ellipsis",@"nonbreakingspace",@"Agrave",@"Atilde",@"Otilde",
	  @"OE",@"oe",@"endash",@"emdash",@"quotedblleft",@"quotedblright",@"quoteleft",@"quoteright",
	  @"divide",@"lozenge",@"ydieresis",@"Ydieresis",@"fraction",@"currency",@"guilsinglleft",@"guilsinglright",
	  @"fi",@"fl",@"daggerdbl",@"periodcentered",@"quotesinglbase",@"quotedblbase",@"perthousand",@"Acircumflex",
	  @"Ecircumflex",@"Aacute",@"Edieresis",@"Egrave",@"Iacute",@"Icircumflex",@"Idieresis",@"Igrave",
	  @"Oacute",@"Ocircumflex",@"apple",@"Ograve",@"Uacute",@"Ucircumflex",@"Ugrave",@"dotlessi",
	  @"circumflex",@"tilde",@"macron",@"breve",@"dotaccent",@"ring",@"cedilla",@"hungarumlaut",
	  @"ogonek",@"caron",@"Lslash",@"lslash",@"Scaron",@"scaron",@"Zcaron",@"zcaron",
	  @"brokenbar",@"Eth",@"eth",@"Yacute",@"yacute",@"Thorn",@"thorn",@"minus",
	  @"multiply",@"onesuperior",@"twosuperior",@"threesuperior",@"onehalf",@"onequarter",@"threequarters",@"franc",
	  @"Gbreve",@"gbreve",@"Idotaccent",@"Scedilla",@"scedilla",@"Cacute",@"cacute",@"Ccaron",
	  @"ccaron",@"dcroat", 
	  nil];

	
  NSMutableData *postTableStrings = [NSMutableData dataWithCapacity:100];
  UInt16 *postTableGlyphArray = calloc([glyphNames count],sizeof(UInt16));
  UInt16 nGlyphs = 0;
	{
	  NSEnumerator *gnEn = [glyphNames objectEnumerator];
	  int nStrings = 0;
	  NSString *string;
	  while ((string = [gnEn nextObject])) {
		if ([string characterAtIndex:0]=='/') string = [string substringFromIndex:1];
		unsigned index = [appleGlyphNames indexOfObject:string];
		if (index != NSNotFound) {
		  postTableGlyphArray[nGlyphs] = SWAP16(index);
		} else {
		  postTableGlyphArray[nGlyphs] = SWAP16(258+(nStrings++));
		  char strLength = [string cStringLength];
		  [postTableStrings appendBytes:&strLength length:1];
		  [postTableStrings appendBytes:[string cString] length:strLength];
		}
		nGlyphs++;
	  }
	}

	
  sfntPostHeader ph;
  ph.version = SWAP32(0x00020000);
  ph.italicAngle = SWAP32(0);
  ph.underlinePosition = SWAP16([(NSNumber*)[fontInfo objectForKey:@"/UnderlinePosition"] intValue]);
  ph.underlineThickness = SWAP16([(NSNumber*)[fontInfo objectForKey:@"/UnderlineThickness"] intValue]);
  ph.isFixedPitch = SWAP16(([(NSNumber*)[fontInfo objectForKey:@"/isFixedPitch"] intValue] ? 1 : 0));
  ph.reserved = 0;
  ph.minMemType42 = 0;
  ph.maxMemType42 = 0;
  ph.minMemType1 = 0;
  ph.maxMemType1 = 0;
//  ph.glyphCount = nGlyphs;
  
  NSMutableData *postTable = [NSMutableData dataWithCapacity:100];
  [postTable appendBytes:&ph length:sizeof_sfntPostHeader];
  nGlyphs = SWAP16(nGlyphs);
  [postTable appendBytes:&nGlyphs length:sizeof(UInt16)];
  nGlyphs = SWAP16(nGlyphs);
  [postTable appendBytes:postTableGlyphArray length:sizeof(UInt16)*nGlyphs];
  [postTable appendData:postTableStrings];
  [postTable increaseLengthBy:(4-([postTable length] % 4)) % 4]; // pad with zeros
  free(postTableGlyphArray);
  
	
  // sfnt directory
  
  unsigned sfntDirectorySize = sizeof_sfntDirectory+sizeof(sfntDirectoryEntry)*6;
  sfntDirectory *dir = calloc(1,sfntDirectorySize);
  fillTag(&(dir->format),"typ1");
  dir->numOffsets = SWAP16(6); /* number of tables */
  dir->searchRange = SWAP16(4*16); /* (max pow 2 <= numOffsets)*16 */
  dir->entrySelector = SWAP16(2);/* log2(max pow 2 <= numOffsets) */
  dir->rangeShift = SWAP16(32);  /* numOffsets*16-searchRange*/
	
  unsigned totalOffset = sfntDirectorySize;

  /* table directory entries must be sorted in ascending order by tag,
     but we want to put the TYP1 table last because we optionally omit the data
	 (but still include the table header) */
	
  dir->table[0].tableTag = SWAP32(HFMXTableTag);
  dir->table[0].checkSum = SWAP32(calcTableChecksum((void*)hh,HFMXTableSize));
  dir->table[0].offset = SWAP32(totalOffset);
  dir->table[0].length = SWAP32(HFMXTableSize);
  totalOffset += SWAP32(dir->table[0].length);
		
  dir->table[2].tableTag = SWAP32(cmapFontTableTag);
  dir->table[2].checkSum = SWAP32(calcTableChecksum((void*)cmaph,cmapTableSize));
  dir->table[2].offset = SWAP32(totalOffset);
  dir->table[2].length = SWAP32(cmapTableSize);
  totalOffset += SWAP32(dir->table[2].length);
	
  dir->table[3].tableTag = SWAP32(descriptorFontTableTag);
  dir->table[3].checkSum = SWAP32(calcTableChecksum((void*)dh,fdscTableSize));
  dir->table[3].offset = SWAP32(totalOffset);
  dir->table[3].length = SWAP32(fdscTableSize);
  totalOffset += SWAP32(dir->table[3].length);
	
  dir->table[4].tableTag = SWAP32(nameFontTableTag);
  dir->table[4].checkSum = SWAP32(calcTableChecksum((void*)[nameTable bytes], [nameTable length]));
  dir->table[4].offset = SWAP32(totalOffset);
  dir->table[4].length = SWAP32([nameTable length]);
  totalOffset += SWAP32(dir->table[4].length);
	
  dir->table[5].tableTag = SWAP32(postTableTag);
  dir->table[5].checkSum = SWAP32(calcTableChecksum((void*)[postTable bytes],[postTable length]));
  dir->table[5].offset = SWAP32(totalOffset);
  dir->table[5].length = SWAP32([postTable length]);
  totalOffset += SWAP32(dir->table[5].length);
  
  dir->table[1].tableTag = SWAP32(typ1TableTag);
  dir->table[1].checkSum = SWAP32(calcTableChecksum((void*)[type1TableData bytes],[type1TableData length]));
  dir->table[1].offset = SWAP32(totalOffset);
  dir->table[1].length = SWAP32([type1TableData length]);
//  totalOffset += dir->table[0].length;

  NSMutableData *sfntData = [NSMutableData dataWithCapacity:totalSize];
  [sfntData appendBytes:dir length:sfntDirectorySize];

  /* append all the tables, matching the order of directory entry creation so that the offsets will be right */
  [sfntData appendBytes:hh length:HFMXTableSize];
  [sfntData appendBytes:cmaph length:cmapTableSize];
  [sfntData appendBytes:dh length:fdscTableSize];
  [sfntData appendData:nameTable];
  [sfntData appendData:postTable];
  if (omitType1Data)
    [sfntData appendBytes:t1h length:sizeof(typ1Header)];
  else
    [sfntData appendData:type1TableData];
  free(t1h);

  free(dir); 
  
  free(cmaph); free(dh); free(hh);
	
  return sfntData;
}


void usage(const char* progName)
{
  fprintf(stderr, "# usage: %s [-t | -T] pfbFile\n\
#   writes sfnt-wrapped Type 1 font to stdout\n\
#   options:\n\
#       -t  omit TYP1 table from sfnt\n\
#       -T  only generate TYP1 data\n", progName);
  exit(1);
}

int main (int argc, char * const *argv) {
  NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

#if 1
  const char* progName = argv[0];
  int ch;
  BOOL omitTyp1 = NO;
  BOOL onlyTyp1 = NO;
  while ((ch = getopt(argc, argv, "tT")) != -1)
	switch (ch) {
	  case 't':
        omitTyp1 = YES;
        break;
	  case 'T':
        onlyTyp1 = YES;
        break;
      case '?':
      default:
        usage(progName);
    }
  argc -= optind;
  argv += optind;
  if (argc != 1 || (omitTyp1 && onlyTyp1))
    usage(progName);
  NSString *pfbFilename = [NSString stringWithCString:argv[0]]; 
  NS_DURING
    if (onlyTyp1) {
	  typ1Header *t1h = calloc(1,sizeof(typ1Header));  
	  NSData *type1Data = digestPfbForSfnt(pfbFilename, t1h);
      fwrite([type1Data bytes], 1, [type1Data length], stdout);
	}
	else {
      NSData *sfntData = wrapPfb(pfbFilename, omitTyp1);
      fwrite([sfntData bytes], 1, [sfntData length], stdout);
	}
  NS_HANDLER
	fprintf(stderr,"%s",[[NSString stringWithFormat:@"exception: %@  reason: %@\n",[localException name],[localException reason]] cString]);
  NS_ENDHANDLER	
//  [sfntData writeToFile:@"/Users/mgubi/Desktop/cmr10.sfnt" atomically:NO];
  
#else
  NSString *basePath = @"/usr/local/teTeX/share/texmf.tetex/fonts/type1";
  NSDirectoryEnumerator *direnum = [[NSFileManager defaultManager]
        enumeratorAtPath:basePath];
  NSString *pname;
  NSString *fullname;
  BOOL fOk = YES;

	
  while ((pname = [direnum nextObject])) {
	if ([[pname pathExtension] isEqualToString:@"pfb"]) {
	  fullname = [basePath stringByAppendingPathComponent:pname];
	  fprintf(stdout,"testing %s\n",[fullname fileSystemRepresentation]);
	  NS_DURING
	  NSData *sfntData = wrapPfb(fullname);
	  NS_HANDLER
		
		fOk = NO;
		fprintf(stderr,"%s",[[NSString stringWithFormat:@"exception: %@  reason: %@",[localException name],[localException reason]] cString]);
		
	  NS_ENDHANDLER	
	}
  }
  
  
  
  
#endif
  
  
  
  [pool release];
  return 0;
}

