//
//  SimplePSInterpreter.m
//  T1Wrap
//
//  Created by Massimiliano Gubinelli on 10/02/2005.
//  Copyright 2005 Massimiliano Gubinelli. All rights reserved.
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

#import "SimplePSInterpreter.h"

static NSString *SimplePSInterpreterException = @"SimplePSInterpreterException";

@interface NSMutableArray (Stack)
- pop;
@end

@implementation NSMutableArray (Stack)
- pop
{ 
  id value = [[[self lastObject] retain] autorelease];
  [self removeLastObject];
  return value;
}
@end

@interface SimplePSInterpreter (Private)
- (void) execProc:(NSArray *)proc;
- (void) execObj:(id)obj;
- (void) execName:(NSString *)identifier;

// PS operators
- (void) opString;
- (void) opReadstring;
- (void) opDef;
- (void) opNot;
- (void) opDict;
- (void) opBegin;
- (void) opEnd;
- (void) opPop;
- (void) opDup;
- (void) opNop;
- (void) opGet;
- (void) opPut;
- (void) opGetinterval;
- (void) opPutinterval;
- (void) opArray;
- (void) opMark;
- (void) opCleartomark;
- (void) opExch;
- (void) opIndex;
- (void) opFor;
- (void) opKnown;
- (void) opIf;
- (void) opIfelse;
- (void) opCurrentdict;
- (void) opCurrentfile;
- (void) opClosefile;
- (void) opDefinefont;
- (void) opEexec;
- (void) opExec;
- (void) opNe;
- (void) opEq;
- (void) initializeInterpreter;
@end // interface

@implementation SimplePSInterpreter

static NSString *StackMarkObject = @" MARK ";
static NSString *NullPSObject = @"[NIL]";

- (void) opString
{
  int n = 0;
  id obj = [stack pop];
  if (![obj isKindOfClass:[NSNumber class]])
	[NSException raise:SimplePSInterpreterException format:@"string parameter is not a number"];
  else n = [obj intValue];
#if 0
  NSMutableString *emptyString = [NSMutableString stringWithCapacity:[(NSNumber*)obj intValue]];
  while (n--) [emptyString appendString:@" "];
  [stack addObject:emptyString];	
#else
//  char *ss = alloca(n+1);
  char *ss = alloca(n);
//  ss[n] = 0x0;
  int i = n;
  while (i) ss[--i] = ' ';
//  [stack addObject:[NSString stringWithCString:ss length:n]];
  [stack addObject:[[NSString alloc] initWithBytes:ss length:n encoding:NSMacOSRomanStringEncoding]];
#endif
  
}

- (void) opReadstring
{
  id stringobj = [stack pop];
  id fileobj = [stack pop];
  if (![fileobj isKindOfClass:[NSString class]] || ![stringobj isKindOfClass:[NSString class]] )
	[NSException raise:SimplePSInterpreterException format:@"wrong arguments in readstring"];
  if (![(NSString*)fileobj isEqualToString:@" <FILE> "])
	[NSException raise:SimplePSInterpreterException format:@"cannot read string from file %@",fileobj];
  int len = [(NSString*)stringobj length];

  NSString *dataString = [inputStream substringWithRange:NSMakeRange([sc scanLocation]+1,len)];
  [sc setScanLocation:[sc scanLocation]+1+len]; // skip whitespace & data string
  [stack addObject:dataString];
  [stack addObject:[NSNumber numberWithInt:1]];
}

- (void) opDef
{
  id obj = [stack pop];
  id nameobj = [stack pop];
  if (![nameobj isKindOfClass:[NSString class]]) 
	[NSException raise:SimplePSInterpreterException format:@"defining non name"];
  else {
	NSString *name = nameobj;
	if ([name characterAtIndex:0] != '/') NSLog(@"Warning: defining non literal name");
	[(NSMutableDictionary*)[dicts lastObject] setObject:obj forKey:name];
	[(NSMutableArray*)[(NSDictionary*)[dicts lastObject] objectForKey:@"[KEY ORDER]"] addObject:name];
  }
}

- (void) opNot
{
  id myBool = [stack pop];
  if ([myBool isKindOfClass:[NSNumber class]]) {
	[stack addObject:[NSNumber numberWithInt:([(NSNumber*)myBool intValue]? 0 : 1)]];
  }
  else [NSException raise:SimplePSInterpreterException format:@"cannot apply <not> to non-boolean object"];
}

- (void) opNe
{
  id a = [stack pop];
  id b = [stack pop];
  [stack addObject:[NSNumber numberWithInt:([a isEqualTo:b] ? 0 : 1 )]];
}


- (void) opEq
{
  id a = [stack pop];
  id b = [stack pop];
  [stack addObject:[NSNumber numberWithInt:([a isEqualTo:b] ? 1 : 0 )]];
}

- (void) execProc:(NSArray *)procwrapper
{
  NSArray *proc = [procwrapper objectAtIndex:1];
  NSEnumerator *en = [proc objectEnumerator];
  id obj;
  while ((obj = [en nextObject])) {
	[self execObj:obj];
  }
}

- (void) execObj:(id)obj
{
  if ([obj isKindOfClass:[NSInvocation class]])
  {
	[(NSInvocation*)obj setTarget:self];
	[(NSInvocation*)obj invoke];
  } else if ([obj isKindOfClass:[NSArray class]])
  {
#if 1
	NSString *type = [(NSArray*)obj objectAtIndex:0];
	if ([type isEqualToString:@"proc"]) [self execProc:obj];
	else
#endif
	  [stack addObject:obj];
	  
  }  else if ([obj isKindOfClass:[NSString class]])
  {
	NSString *identifier = obj;
	if ([identifier characterAtIndex:0]=='/')
	  [stack addObject:identifier];
	else
	  [self execName:identifier];
  } else
	[stack addObject:obj];
}

- (void) execName:(NSString *)identifier
{
  NSEnumerator *en = [dicts reverseObjectEnumerator];
  NSDictionary *d;
  id value = nil;
  while (d = [en nextObject]) {
	if (!(value = [d objectForKey:identifier])) value = [d objectForKey:[@"/" stringByAppendingString:identifier]];
	if (value) break;
  }
  
  if (value) {
	[self execObj: value];
  }
  else {
	[stack addObject:identifier];
  }
}

- (void) opDict
{
  id obj = [stack pop];
  if (![obj isKindOfClass:[NSNumber class]]) 
	[NSException raise:SimplePSInterpreterException format:@"wrong type argument for dict"];
  NSMutableDictionary *newDict = [NSMutableDictionary dictionaryWithCapacity:[(NSNumber*)obj intValue]+1];
  [newDict setObject:[NSMutableArray arrayWithCapacity:[(NSNumber*)obj intValue]] 
			  forKey:@"[KEY ORDER]"]; // KEY ORDER auxiliary array
  [stack addObject:newDict];
}


- (void) opBegin
{
  id obj = [stack pop];
  if (![obj isKindOfClass:[NSDictionary class]]) 
	[NSException raise:SimplePSInterpreterException format:@"wrong type argument for begin, expecting a dictionary"];
  [dicts addObject:obj];
}


- (void) opEnd
{
  if ([dicts count]==0)
	[NSException raise:SimplePSInterpreterException format:@"dictionary stack underflow"];
  [dicts removeLastObject];
}

- (void) opPop
{
  [stack removeLastObject];
}

- (void) opDup
{
#if 0
  id aCopy = [[stack lastObject] copy];
  [stack addObject:aCopy];
  [aCopy release];
#else
  id aCopy = [stack lastObject];
  [stack addObject:aCopy];
#endif
}

- (void) opNop
{
}

- (void) opGet
{
  id key = [stack pop];
  id target = [stack pop];
  if ([target isKindOfClass:[NSArray class]])
  {
	if (![key isKindOfClass:[NSNumber class]])
	  [NSException raise:SimplePSInterpreterException 
				  format:@"wrong type argument for get, key should be a number"];
	  
	[stack addObject:[(NSArray*)[(NSArray*)target objectAtIndex:1] objectAtIndex:[(NSNumber*)key intValue]]];	
  } else if ([target isKindOfClass:[NSDictionary class]]) {
	if (![key isKindOfClass:[NSString class]])
	  [NSException raise:SimplePSInterpreterException 
				  format:@"wrong type argument for get, key should be a number"];
	if ([(NSString*)key characterAtIndex:0] != '/')
	  [NSException raise:SimplePSInterpreterException 
				  format:@"expecting literal dictionary key for get"];	 
	id value = [(NSDictionary*)target objectForKey:key];
	if (value)
	  [stack addObject:value];
	else
	  [stack addObject:NullPSObject];
  } else
	[NSException raise:SimplePSInterpreterException 
				format:@"applying get to nonarray/dict"];  
}

- (void) opPut
{
  id value = [stack pop];
  id key = [stack pop];
  id target = [stack pop];
  if ([target isKindOfClass:[NSArray class]])
  {
	if (![key isKindOfClass:[NSNumber class]])
	  [NSException raise:SimplePSInterpreterException 
				  format:@"wrong type argument for put, key should be a number"];
	
	[(NSMutableArray*)[(NSArray*)target objectAtIndex:1] replaceObjectAtIndex:[(NSNumber*)key intValue] 
																   withObject:value];	
  } else if ([target isKindOfClass:[NSDictionary class]]) {
	if (![key isKindOfClass:[NSString class]])
	  [NSException raise:SimplePSInterpreterException 
				  format:@"wrong type argument for put, key should be a number"];
	if ([(NSString*)key characterAtIndex:0] != '/')
	  [NSException raise:SimplePSInterpreterException 
				  format:@"expecting literal dictionary key for put"];
	
	[(NSMutableDictionary*)target setObject:value forKey:key];
	[(NSMutableArray*)[(NSMutableDictionary*)target objectForKey:@"[KEY ORDER]"] 
		addObject:key];	
  } else
	[NSException raise:SimplePSInterpreterException 
				format:@"applying put to nonarray/dict"];  
  
}


- (void) opGetinterval
{
  id count = [stack pop];
  id index = [stack pop];
  id array = [stack pop];
  if ([array isKindOfClass:[NSArray class]]) {
	if ([count isKindOfClass:[NSNumber class]] && [index isKindOfClass:[NSNumber class]]) 
	  [stack addObject:[NSArray arrayWithObjects:@"array",[(NSArray*)[(NSArray*)array objectAtIndex:1] subarrayWithRange:
	   NSMakeRange([(NSNumber*)index intValue],[(NSNumber*)count intValue])],nil]];
	else
	  [NSException raise:SimplePSInterpreterException 
				  format:@"applying getinterval with nonnumber count or index"];	  
  }
  else
	[NSException raise:SimplePSInterpreterException 
				format:@"applying getinterval to nonarray"];
	
}

- (void) opPutinterval
{
  id array2 = [stack pop];
  id index = [stack pop];
  id array = [stack pop];
  if ([array isKindOfClass:[NSArray class]]) {
	if (![array2 isKindOfClass:[NSArray class]]) 
	  [NSException raise:SimplePSInterpreterException 
				  format:@"using putinterval with nonarray source"];
	if (![index isKindOfClass:[NSNumber class]]) 
	  [NSException raise:SimplePSInterpreterException 
				  format:@"using putinterval with nonnumber index"];
	NSArray *source = [(NSArray*)array2 objectAtIndex:1];
	int count = [array2 count];
	[(NSMutableArray*)[(NSArray*)array objectAtIndex:1]
	  replaceObjectsInRange:NSMakeRange([(NSNumber*)index intValue],count)
	   withObjectsFromArray:source
					  range:NSMakeRange(0,count)];
  }
  else
	[NSException raise:SimplePSInterpreterException 
				format:@"applying putinterval to nonarray"];  
}

- (void) opArray
{
  id countobj = [stack pop];
  if (![countobj isKindOfClass:[NSNumber class]]) 
	[NSException raise:SimplePSInterpreterException 
				format:@"using array with nonnumber count"];
  int count = [countobj intValue];
  NSMutableArray *array = [[NSMutableArray alloc] initWithCapacity:count];
  while(count--) [array addObject:NullPSObject];
  [stack addObject:[NSArray arrayWithObjects:@"array",array,nil]];
  [array release];
}

- (void) opMark
{
  [stack addObject:StackMarkObject];
}

- (void) opCleartomark
{
  id obj;
  while ((obj = [stack lastObject])) {
	if (obj == StackMarkObject) break;
	[stack removeLastObject];
  }
  if (obj) [stack removeLastObject];  
}

- (void) opExch
{
  id a = [stack pop];
  id b = [stack pop];
  [stack addObject:a];
  [stack addObject:b];
}

- (void) opIndex
{
  id index = [stack pop];
  if (![index isKindOfClass:[NSNumber class]]) 
	[NSException raise:SimplePSInterpreterException 
				format:@"using index with nonnumber count"];
  [stack addObject:[stack objectAtIndex:[stack count]-[(NSNumber*)index intValue]-1]];
}

- (void) opFor
{
  id proc = [stack pop];
  id limitobj = [stack pop];
  id stepobj = [stack pop];
  id startobj = [stack pop];
  if ([limitobj isKindOfClass:[NSNumber class]] && 
	  [stepobj isKindOfClass:[NSNumber class]] &&
	  [startobj isKindOfClass:[NSNumber class]] &&
	  [proc isKindOfClass:[NSArray class]] ) {
	int limit = [(NSNumber*)limitobj intValue];
	int step = [(NSNumber*)stepobj intValue];
	int start = [(NSNumber*)startobj intValue];
	for ( ;start <= limit;start += step) {
	  [stack addObject:[NSNumber numberWithInt:start]];
	  [self execProc: proc];	  
	};
  }
  else
	[NSException raise:SimplePSInterpreterException 
				format:@"wrong arguments in for"];
  
}


- (void) opKnown
{
  id key = [stack pop];
  id dict = [stack pop];
  [stack addObject:[NSNumber numberWithInt:([(NSDictionary*)dict objectForKey:key] ? 1 : 0)]];
}

- (void) opIf
{
  id trueproc = [stack pop];
  id condition = [stack pop];
  
  if (![condition isKindOfClass:[NSNumber class]])
	[NSException raise:SimplePSInterpreterException 
				format:@"condition in if is not a boolean"];
	
  if ([(NSNumber*)condition intValue]) [self execProc:trueproc];
}

- (void) opIfelse
{
  id falseproc = [stack pop];
  id trueproc = [stack pop];
  id condition = [stack pop];
  
  if (![condition isKindOfClass:[NSNumber class]])
	[NSException raise:SimplePSInterpreterException 
				format:@"condition in ifelse is not a boolean"];
  
  if ([(NSNumber*)condition intValue])
	[self execProc:trueproc];
  else  [self execProc:falseproc];
}

- (void) opCurrentdict
{
  [stack addObject:[dicts lastObject]];
}

- (void) opCurrentfile
{
  [stack addObject:@" <FILE> "];
}

- (void) opClosefile
{
  [stack pop];
}

- (void) opDefinefont
{
  id font = [stack pop];
  id key = [stack pop];
  [fontDir setObject:font forKey:key];
  [stack addObject:font];
}

NSString *decryptString(NSString *inString)
{
  unsigned const char *inbuf = (unsigned const char *)[inString cString];
  unsigned long insize = [inString cStringLength];
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
//  NSString *resString = [NSString stringWithCString:outbuf length:insize];  
  NSString *resString = [[NSString alloc] initWithData:result encoding:NSMacOSRomanStringEncoding];
  return resString;
}

- (void) opEexec
{
  id fileobj = [stack pop];
  [sc scanCharactersFromSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]
				 intoString:nil];
  int startLoc = [sc scanLocation];
  NSString *dataString;
  if ([sc scanUpToString:@"00000000000000000000" intoString:&dataString])
  {
	NSString *decryptedStream = decryptString(dataString); 
	[inputStream deleteCharactersInRange:NSMakeRange(startLoc,[inputStream length]-startLoc)];
	[inputStream appendString:[decryptedStream substringFromIndex:4]]; // skip first 4 characters
	[sc setScanLocation:startLoc];
  } else
	[NSException raise:SimplePSInterpreterException 
				format:@"cannot find end of stream in eexec"];
}



- (void) opExec
{
  id obj = [stack pop];
  if ([obj isKindOfClass:[NSString class]]) [self execName:obj];	
  if ([obj isKindOfClass:[NSArray class]] && [(NSString*)[(NSArray*)obj objectAtIndex:0] isEqualToString:@"proc"])
  {
	[self execProc:obj];
  }
  else [NSException raise:SimplePSInterpreterException 
			  format:@"unimplemented exec feature"];
}

- (void) opCount
{
  [stack addObject:[NSNumber numberWithInt:[stack count]]];
}

- (void) opType
{
  id obj = [stack pop];
  NSString *type = @"/nulltype";
  if ([obj isKindOfClass:[NSNumber class]])
	  type = @"/integertype";
  else if ([obj isKindOfClass:[NSDictionary class]])
	type = @"/dicttype";
  else if ([obj isKindOfClass:[NSArray class]]) {
	NSString *head = [(NSArray*)obj objectAtIndex:0];
	if ([head isEqualToString:@"array"])
	  type = @"/arraytype";
	else
	  type = @"/arraytype"; // not quite correct...	  
  }   else if ([obj isKindOfClass:[NSString class]]) {
	if ([obj characterAtIndex:0] == '/') type = @"/nametype" ;
	  else
		type = @"/stringtype";
  }

  [stack addObject:type];
}

- (NSInvocation *) makeInvocationForOp:(SEL) theSelector
{
  NSMethodSignature *aSignature;
  NSInvocation *anInvocation;
  aSignature = [[self class] instanceMethodSignatureForSelector:theSelector];
  anInvocation = [NSInvocation invocationWithMethodSignature:aSignature];
  [anInvocation setSelector:theSelector];
  return anInvocation;
}

- (void)initializeInterpreter
{
  if (stack) [stack release];
  if (dicts) [dicts release];
  if (fontDir) [fontDir release];
  if (inputStream) [inputStream release];
  if (sc) [sc release];
  
  stack = [[NSMutableArray arrayWithCapacity:10000] retain];
  dicts = [[NSMutableArray arrayWithCapacity:10] retain];
  fontDir = [[NSMutableDictionary dictionaryWithCapacity:10] retain];
  inputStream = nil;
  sc = nil;
  nStoringProc = 0;
  
  NSArray *standardEncoding = [NSArray arrayWithObjects:
	@"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", 
	@"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", 
	@"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", 
	@"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", 
	@"/space", @"/exclam", @"/quotedbl", @"/numbersign", @"/dollar", @"/percent", @"/ampersand", @"/quoteright",
	@"/parenleft", @"/parenright", @"/asterisk", @"/plus", @"/comma", @"/hyphen", @"/period", @"/slash", 
	@"/zero", @"/one", @"/two", @"/three", @"/four", @"/five", @"/six", @"/seven", 
	@"/eight", @"/nine", @"/colon", @"/semicolon", @"/less", @"/equal", @"/greater", @"/question", 
	@"/at", @"/A", @"/B", @"/C", @"/D", @"/E", @"/F", @"/G",
	@"/H", @"/I", @"/J", @"/K", @"/L", @"/M", @"/N", @"/O", 
	@"/P", @"/Q", @"/R", @"/S", @"/T", @"/U", @"/V", @"/W", 
	@"/X", @"/Y", @"/Z", @"/bracketleft", @"/backslash", @"/bracketright", @"/asciicircum", @"/underscore",
	@"/quoteleft", @"/a", @"/b", @"/c", @"/d", @"/e", @"/f", @"/g", 
	@"/h", @"/i", @"/j", @"/k", @"/l", @"/m", @"/n", @"/o",
	@"/p", @"/q", @"/r", @"/s", @"/t", @"/u", @"/v", @"/w",
	@"/x", @"/y", @"/z", @"/braceleft", @"/bar", @"/braceright", @"/asciitilde", @"/.notdef",
	@"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef",
	@"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef",
	@"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", 
	@"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef",
	@"/.notdef", @"/exclamdown", @"/cent", @"/sterling", @"/fraction", @"/yen", @"/florin", @"/section", 
	@"/currency", @"/quotesingle", @"/quotedblleft", @"/guillemotleft", @"/guilsinglleft", @"/guilsinglright", @"/fi", @"/fl", 
	@"/.notdef", @"/endash", @"/dagger", @"/daggerdbl", @"/periodcentered", @"/.notdef", @"/paragraph", 
	@"/bullet", @"/quotesinglbase", @"/quotedblbase", @"/quotedblright", @"/guillemotright", @"/ellipsis", @"/perthousand", @"/.notdef", @"/questiondown", 
	@"/.notdef", @"/grave", @"/acute", @"/circumflex", @"/tilde", @"/macron", @"/breve", @"/dotaccent", 
	@"/dieresis", @"/.notdef", @"/ring", @"/cedilla", @"/.notdef", @"/hungarumlaut", @"/ogonek", @"/caron", 
	@"/emdash", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", 
	@"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", 
	@"/.notdef", @"/AE", @"/.notdef", @"/ordfeminine", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", 
	@"/Lslash", @"/Oslash", @"/OE", @"/ordmasculine", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", 
	@"/.notdef", @"/ae", @"/.notdef", @"/.notdef", @"/.notdef", @"/dotlessi", @"/.notdef", @"/.notdef", 
	@"/lslash", @"/oslash", @"/oe", @"/germandbls", @"/.notdef", @"/.notdef", @"/.notdef", @"/.notdef", nil
	];
  
  NSMutableDictionary *userdict = [[NSDictionary dictionaryWithObjectsAndKeys:
	  fontDir, @"/FontDirectory",
	  [NSArray arrayWithObjects: @"array", standardEncoding, nil], @"/StandardEncoding",
	  nil	
  ] mutableCopy];
  
  NSMutableDictionary *systemdict = [NSMutableDictionary dictionaryWithCapacity:1000];
  NSMutableDictionary *errordict = [NSMutableDictionary dictionaryWithCapacity:10];

  NSDictionary *auxDict = [NSDictionary dictionaryWithObjectsAndKeys:
	[self makeInvocationForOp:@selector(opDef)], @"def",
	[self makeInvocationForOp:@selector(opDup)], @"dup",
	[self makeInvocationForOp:@selector(opIndex)], @"index",
	[self makeInvocationForOp:@selector(opExch)], @"exch",
	[self makeInvocationForOp:@selector(opFor)], @"for",
	[self makeInvocationForOp:@selector(opDict)], @"dict",
	[self makeInvocationForOp:@selector(opArray)], @"array",
	[self makeInvocationForOp:@selector(opMark)], @"mark",
	[self makeInvocationForOp:@selector(opCleartomark)], @"cleartomark",
 	[self makeInvocationForOp:@selector(opBegin)], @"begin",
	[self makeInvocationForOp:@selector(opEnd)], @"end",
	[self makeInvocationForOp:@selector(opExec)], @"exec",
	[self makeInvocationForOp:@selector(opGet)], @"get",
	[self makeInvocationForOp:@selector(opPut)], @"put",
	[self makeInvocationForOp:@selector(opGetinterval)], @"getinterval",
	[self makeInvocationForOp:@selector(opPutinterval)], @"putinterval",
	[self makeInvocationForOp:@selector(opPop)], @"pop",
	[self makeInvocationForOp:@selector(opString)], @"string",
	[self makeInvocationForOp:@selector(opReadstring)], @"readstring",
	[self makeInvocationForOp:@selector(opNop)], @"readonly",
	[self makeInvocationForOp:@selector(opNop)], @"executeonly",
	[self makeInvocationForOp:@selector(opNop)], @"noaccess",
	[self makeInvocationForOp:@selector(opNop)], @"bind",
	[self makeInvocationForOp:@selector(opIf)], @"if",
	[self makeInvocationForOp:@selector(opIfelse)], @"ifelse",
	[self makeInvocationForOp:@selector(opKnown)], @"known",
	[self makeInvocationForOp:@selector(opNot)], @"not",
	[self makeInvocationForOp:@selector(opCurrentdict)], @"currentdict",
	[self makeInvocationForOp:@selector(opCurrentfile)], @"currentfile",
	[self makeInvocationForOp:@selector(opClosefile)], @"closefile",
	[self makeInvocationForOp:@selector(opEexec)], @"eexec",
	[self makeInvocationForOp:@selector(opDefinefont)], @"definefont",
	/* more operators */
	[self makeInvocationForOp:@selector(opNe)], @"ne",
	[self makeInvocationForOp:@selector(opEq)], @"eq",
	[self makeInvocationForOp:@selector(opCount)], @"count",
	[self makeInvocationForOp:@selector(opType)], @"type",
    /* some variables */
	[NSNumber numberWithInt:1], @"true",
	[NSNumber numberWithInt:0], @"false",
	userdict, @"userdict",
	systemdict, @"systemdict",
	errordict, @"errordict",
	nil];
	
  [errordict setObject:[NSArray arrayWithObjects:@"proc", 
	[NSArray arrayWithObjects:@"pop",nil],nil]
				forKey:@"/invalidaccess"];
  
  [systemdict addEntriesFromDictionary:auxDict];

  [dicts addObject:systemdict];
  [dicts addObject:userdict];
  
}


- (void)doString:(NSString *)anInputString
{
  NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

  
  inputStream = [[anInputString mutableCopy] retain];
  
  NSString *identifierString;

  if (sc) [sc release];
  sc = [[NSScanner scannerWithString:inputStream] retain];

//  NSCharacterSet  *sCS= [NSCharacterSet whitespaceAndNewlineCharacterSet];
  NSCharacterSet  *sCS= [NSCharacterSet characterSetWithCharactersInString:@" \t\n\r"];
  NSCharacterSet  *idCS= [NSCharacterSet characterSetWithCharactersInString:@" \t\n\r[]{}()%/"];
  NSCharacterSet  *nlCS= [NSCharacterSet characterSetWithCharactersInString:@"\n\r"];
  NSCharacterSet  *parCS= [NSCharacterSet characterSetWithCharactersInString:@"()"];
  float tmpFloat;
  
  while (![sc isAtEnd]) {
	unichar ch;
	
	/* 
	   BUG? 
	   [sc scanCharactersFromSet:sCS intoString:nil]; does not scan through /r at
	   the beginning of the string. Actually it does not scan through ANY of the
	   characters in cCS!!!
	   we must do it by hand ...
	*/
	
	while (1) {
	[sc scanCharactersFromSet:sCS intoString:nil];
	ch =[inputStream characterAtIndex:[sc scanLocation]];
	if  (ch == '\n' || ch == '\r' || ch == ' ' || ch == '\t')
	  [sc setScanLocation:(int)[sc scanLocation]+1];
	  else break;
	};
	switch (ch)
	{
	  case '%'  : // comment
			 [sc scanUpToCharactersFromSet:nlCS intoString:nil];
			[sc setScanLocation:[sc scanLocation]+1];
			 continue;
	  case '/' : // literal
		    [sc scanString:@"/" intoString:nil];
			if ([sc scanUpToCharactersFromSet:idCS intoString:&identifierString]) {
			  [stack addObject:[@"/" stringByAppendingString:identifierString]];
			}
			continue;
	  case '('  : // string
			{
			  int nesting = 0;
			  NSMutableString *curString = [NSMutableString stringWithCapacity:100];
			  NSString *fragment;
		      [sc scanString:@"(" intoString:nil];
		      while (1) {
				[sc scanUpToCharactersFromSet:parCS intoString:&fragment];
				[curString appendString:fragment];
				if ([inputStream characterAtIndex:[sc scanLocation]] == '(')
				{
				  nesting++; [curString appendString:@"("]; [sc scanString:@"(" intoString:nil];
				}
				else {
				  [sc scanString:@")" intoString:nil];
				  if (nesting) {
					nesting--; [curString appendString:@")"]; 
				  }
				  else break;
				}
			  }
			  [stack addObject:curString];
			}	  
			continue;
	  case '{'  : // begin proc
			[sc scanString:@"{" intoString:nil];
		    nStoringProc++;
			[stack addObject:StackMarkObject];
		    continue;
	  case '}'  : // end proc
			{
		    [sc scanString:@"}" intoString:nil];
		    nStoringProc--;
			id obj;
			NSMutableArray *proc = [NSMutableArray arrayWithCapacity:100];
			while ((obj = [stack lastObject]) != StackMarkObject) {
			  [proc insertObject:obj atIndex:0];
			  [stack removeLastObject];
			}
			if (obj == StackMarkObject) [stack removeLastObject];
			[stack addObject:[NSArray arrayWithObjects:@"proc",proc,nil]];
			}
		    continue;
	  case '['  : // begin array
		    [sc scanString:@"[" intoString:nil];
			[stack addObject:StackMarkObject];
			continue;
	  case ']'  : // end array
	        {
		    [sc scanString:@"]" intoString:nil];
			id obj;
			NSMutableArray *array = [NSMutableArray arrayWithCapacity:100];
			while ((obj = [stack lastObject]) != StackMarkObject) {
			  [array insertObject:obj atIndex:0];
			  [stack removeLastObject];
			}
			if (obj == StackMarkObject) [stack removeLastObject];
			[stack addObject:[NSArray arrayWithObjects:@"array",array,nil]];
		    }
		    continue;

	  case '<' : // hex string		  
		[NSException raise:SimplePSInterpreterException format:@"hexstring not yet implemented!"];
		continue;
	
	default : // check if it is a number or a symbol
	  
	  if ([sc scanFloat:&tmpFloat]) {
		[stack addObject:[NSNumber numberWithFloat:tmpFloat]];
	  } else if ([sc scanUpToCharactersFromSet:idCS intoString:&identifierString]) {
		if (nStoringProc) {
		  [stack addObject:identifierString];
		} else {
		  [self execName:identifierString];
		}
	  }	else
		[NSException raise:SimplePSInterpreterException format:@"Exhausted possible cases. I'm lost!"];
	}
  }

  [sc release];
  [inputStream release];
  sc = nil;
  inputStream = nil;
  
  [pool release];
}


- init
{
  self = [super init];
  if (self) {
	[self initializeInterpreter];
  }
  return self;
}

- (void) dealloc
{
  [stack release];
  [fontDir release];
  [dicts release];
  
  [super dealloc];
}


- (NSDictionary*)fontDir
{
  return fontDir;
}

@end
