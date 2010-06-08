%{
#ifndef lint
static char	rcsid[] = "$Header: /p/src/local/bin/detex/RCS/detex.l,v 2.22 2007/03/01 20:22:01 trinkle Exp trinkle $";
#endif

/*
 * Copyright (c) 1986-2007 Purdue University
 * All rights reserved.
 * 
 * Developed by:  Daniel Trinkle
 *                Department of Computer Science, Purdue University
 *                http://www.cs.purdue.edu/
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal with the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * o Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimers.
 * 
 * o Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimers in the
 *   documentation and/or other materials provided with the distribution.
 * 
 * o Neither the names of Daniel Trinkle, Purdue University, nor the
 *   names of its contributors may be used to endorse or promote products
 *   derived from this Software without specific prior written
 *   permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
 */


/*
 * detex [-e environment-list] [-c] [-l] [-n] [-s] [-t] [-w] [file[.tex] ]
 *
 *	This program is used to remove TeX or LaTeX constructs from a text
 *	file.
 */

#include "detex.h"
#ifdef HAVE_STRING_H
#include <string.h>
#define	index	strchr
#define	rindex	strrchr
#else
#include <strings.h>
#endif
#ifndef MAXPATHLEN
#include <sys/param.h>
#endif
#ifdef OS2
#include <stdlib.h>
#endif

#define	LaBEGIN		if (fLatex) BEGIN
#define	IGNORE		if (fSpace && !fWord) putchar(' ')
#define	SPACE		if (!fWord) putchar(' ')
#define	NEWLINE		if (!fWord) putchar('\n')
#define	LATEX		fLatex=!fForcetex
#define KILLARGS(x)	cArgs=x; LaBEGIN LaMacro
#define STRIPARGS(x)	cArgs=x; LaBEGIN LaMacro2
#define	CITE(x)		if (fLatex && !fCite) KILLARGS(x)

char	*SafeMalloc();
#ifndef NO_MALLOC_DECL
char	*malloc();
#endif
#ifdef OS2
void	yyless(int);
#endif

char	*rgsbEnvIgnore[MAXENVS];	/* list of environments ignored */
char	*rgsbIncList[MAXINCLIST];	/* list of includeonly files */
char	*rgsbInputPaths[MAXINPUTPATHS];	/* list of input paths in order */
char	sbCurrentEnv[CCHMAXENV];	/* current environment being ignored */
char	*sbProgName;			/* name we were invoked with */
FILE	*rgfp[NOFILE+1];		/* stack of input/include files */
int	cfp = 0;			/* count of files in stack */
int	cOpenBrace = 0;			/* count of `{' in <LaMacro> and <LaMacro2> */
int	cArgs = 0;			/* argument connt in <LaArgs> */
int	csbEnvIgnore;			/* count of environments ignored */
int	csbIncList = 0;			/* count of includeonly files */
int	csbInputPaths;			/* count of input paths */
int	fLatex = 0;			/* flag to indicated delatex */
int	fWord = 0;			/* flag for -w option */
int	fFollow = 1;			/* flag to follow input/include */
int	fCite = 0;			/* flag to echo \cite and \ref args */
int	fSpace = 0;			/* flag to replace \cs with space */
int	fForcetex = 0;			/* flag to inhibit latex mode */

#ifdef FLEX_SCANNER
/* flex has contexts for buffers that need to be switched when file changes
 * otherwise output contains imported files in reverse order.  Weird, but
 * true.
 */
YY_BUFFER_STATE rgsb[NOFILE + 1]; /* flex context stack */
int             csb = 0;		 /* depth of flex context stack */
#endif /* FLEX_SCANNER */

%}

S	[ \t\n]*
W	[a-zA-Z]+
N	[+-]?(([0-9]+(\.[0-9]+)?)|(\.[0-9]+))
U	pt|pc|in|bp|cm|mm|dd|cc|sp
D	{S}(({N}{S}{U})|(\\{W})){S}
Z	\*?

%Start Define Display IncludeOnly Input Math Normal Control
%Start LaBegin LaDisplay LaEnd LaEnv LaFormula LaInclude
%Start LaMacro LaOptArg LaMacro2 LaOptArg2 LaVerbatim 
%start LaBreak LaPicture

%%
<Normal>"%".*		/* ignore comments */	;

<Normal>"\\begin"{S}"{"{S}"document"{S}"}"	{LATEX; IGNORE;}

<Normal>"\\begin"     /* environment start */	{LaBEGIN LaBegin; IGNORE;}

<LaBegin>{S}"{"{S}"verbatim"{S}"}"		{   if (BeginEnv("verbatim"))
							BEGIN LaEnv;
						    else
							BEGIN LaVerbatim;
						    IGNORE;
						}

<LaVerbatim>"\\end"{S}"{"{S}"verbatim"{S}"}" /* verbatim mode */	{BEGIN Normal; IGNORE;}
<LaVerbatim>[^\\]+				ECHO;
<LaVerbatim>.					ECHO;

<LaBegin>{S}"{"{S}"minipage"{S}"}"		{ KILLARGS(1);
						  if (BeginEnv("minpage"))
							BEGIN LaEnv;
						  else
							BEGIN Normal;
						  IGNORE;
						}

<LaBegin>{W}					{   if (BeginEnv(yytext))
							BEGIN LaEnv;
						    else
							BEGIN Normal;
						    IGNORE;
						}
<LaBegin>"\n"					NEWLINE;
<LaBegin>.					;

<LaEnv>"\\end"  /* absorb some environments */	{LaBEGIN LaEnd; IGNORE;}
<LaEnv>"\n"					NEWLINE;
<LaEnv>.					;

<LaEnd>{W}		 /* end environment */	{   if (EndEnv(yytext))
							BEGIN Normal;
						    IGNORE;
						}
<LaEnd>"}"					{BEGIN LaEnv; IGNORE;}
<LaEnd>"\n"					NEWLINE;
<LaEnd>.					;

<Normal>"\\vskip"{D}				;
<Normal>"\\vspace"{Z}{S}"{"{D}"}"		;
<Normal>"\\hskip"{D}				;
<Normal>"\\hspace"{Z}{S}"{"{D}"}"		;
<Normal>"\\addvspace"{S}"{"{D}"}"		;

<Normal>"\\newlength"				{ KILLARGS(1); }
<Normal>"\\setlength"				{ KILLARGS(2); }
<Normal>"\\addtolength"				{ KILLARGS(2); }
<Normal>"\\settowidth"				{ KILLARGS(2); }
<Normal>"\\settoheight"				{ KILLARGS(2); }
<Normal>"\\settodepth"				{ KILLARGS(2); }
<Normal>"\\newsavebox"				{ KILLARGS(1); }
<Normal>"\\sbox"				{ KILLARGS(1); }
<Normal>"\\savebox"				{ KILLARGS(2); }
<Normal>"\\usebox"				{ KILLARGS(1); }
<Normal>"\\raisebox"				{ STRIPARGS(2); }
<Normal>"\\parbox"				{ KILLARGS(1); }
<Normal>"\\scalebox"				{ STRIPARGS(2); }
<Normal>"\\resizebox"{Z}			{ KILLARGS(2); }
<Normal>"\\reflectbox"				;
<Normal>"\\rotatebox"				{ KILLARGS(1); }
<Normal>"\\includegraphics"[^{]*		{ LaBEGIN LaPicture; }

<LaPicture>"{"					;
<LaPicture>[^{}]+				{ printf("<Picture %s>", yytext); }
<LaPicture>"\}"					BEGIN Normal;

<Normal>"\\definecolor"				{ KILLARGS(3); }
<Normal>"\\color"				{ KILLARGS(1); }
<Normal>"\\textcolor"				{ KILLARGS(2); }
<Normal>"\\colorbox"				{ KILLARGS(2); }
<Normal>"\\fcolorbox"				{ KILLARGS(3); }
<Normal>"\\pagecolor"				{ KILLARGS(1); }
<Normal>"\\foilhead"				{ STRIPARGS(1); }

<Normal>"\\part"{Z}				;
<Normal>"\\section"{Z}				;
<Normal>"\\subsection"{Z}			;
<Normal>"\\subsubsection"{Z}			;
<Normal>"\\paragraph"{Z}			;
<Normal>"\\sunparagraph"{Z}			;

<Normal>"\\bibitem"	    /* ignore args  */	{KILLARGS(1); IGNORE;}
<Normal>"\\bibliography"    /* of these \cs */	{KILLARGS(1); IGNORE;}
<Normal>"\\bibstyle"				{KILLARGS(1); IGNORE;}
<Normal>"\\cite"				{CITE(1); IGNORE;}
<Normal>"\\documentstyle"			{LATEX; KILLARGS(1); IGNORE;}
<Normal>"\\documentclass"			{LATEX; KILLARGS(1); IGNORE;}
<Normal>"\\usepackage"				{KILLARGS(1); IGNORE;}
<Normal>"\\end"					{KILLARGS(1); IGNORE;}
<Normal>"\\index"				{KILLARGS(1); SPACE;}
<Normal>"\\footnote"				{KILLARGS(1); SPACE;}
<Normal>"\\label"				{KILLARGS(1); IGNORE;}
<Normal>"\\pageref"				{CITE(1); IGNORE;}
<Normal>"\\pagestyle"				{KILLARGS(1); IGNORE;}
<Normal>"\\ref"					{CITE(1); IGNORE;}
<Normal>"\\setcounter"				{KILLARGS(1); IGNORE;}
<Normal>"\\verb" /* ignore \verb<ch>...<ch> */	{   if (fLatex) {
						 	char verbchar, c;
						 	verbchar = input();
						 	while ((c = input()) != verbchar)
							    if (c == '\n')
								NEWLINE;
						    }
						    IGNORE;
						}

<Normal>"\\newcommand"				{ LATEX; KILLARGS(2); }
<Normal>"\\renewcommand"			{ LATEX; KILLARGS(2); }
<Normal>"\\newenvironment"			{ LATEX; KILLARGS(3); }

<Normal>"\\def"		/* ignore def begin */	{BEGIN Define; IGNORE;}
<Define>"{"					BEGIN Normal;
<Define>"\n"					NEWLINE;
<Define>.					;

<Normal>"\\("		/* formula mode */	{LaBEGIN LaFormula; IGNORE;}
<LaFormula>"\\)"				BEGIN Normal;
<LaFormula>"\n"					NEWLINE;
<LaFormula>.					;

<Normal>"\\["		/* display mode */	{LaBEGIN LaDisplay; IGNORE;}
<LaDisplay>"\\]"				BEGIN Normal;
<LaDisplay>"\n"					NEWLINE;
<LaDisplay>.					;

<Normal>"$$"		/* display mode */	{BEGIN Display; IGNORE;}
<Display>"$$"					BEGIN Normal;
<Display>"\n"					NEWLINE;
<Display>.					;

<Normal>"$"		/* math mode */		{BEGIN Math; IGNORE;}
<Math>"$"					BEGIN Normal;
<Math>"\n"					NEWLINE;
<Math>"\\$"					;
<Math>.						;

<Normal>"\\include"	/* process files */	{LaBEGIN LaInclude; IGNORE;}
<LaInclude>[^{ \t\n}]+				{   IncludeFile(yytext);
						    BEGIN Normal;
						}
<LaInclude>"\n"					NEWLINE;
<LaInclude>.					;

<Normal>"\\includeonly"				{BEGIN IncludeOnly; IGNORE;}
<IncludeOnly>[^{ \t,\n}]+			AddInclude(yytext);
<IncludeOnly>"}"				{   if (csbIncList == 0)
							rgsbIncList[csbIncList++] = '\0';
						    BEGIN Normal;
						}
<IncludeOnly>"\n"				NEWLINE;
<IncludeOnly>.					;

<Normal>"\\input"				{BEGIN Input; IGNORE;}
<Input>[^{ \t\n}]+				{   InputFile(yytext);
						    BEGIN Normal;
						}
<Input>"\n"					NEWLINE;
<Input>.					;

<Normal>\\(aa|AA|ae|AE|oe|OE|ss)[ \t]*[ \t\n}] /* handle ligatures */	{(void)printf("%.2s", yytext+1);}
<Normal>\\[OoijLl][ \t]*[ \t\n}]		{(void)printf("%.1s", yytext+1);}
<Normal>"\\linebreak"				BEGIN LaBreak;

<Normal>\\[a-zA-Z@]+	/* ignore other \cs */	{BEGIN Control; IGNORE;}
<Normal>"\\ "					SPACE;
<Normal>"\\\\"{Z}(\[[^\]]*\])?			NEWLINE;
<Normal>\\.					IGNORE;
<LaBreak>[0-4]?					{  if (yytext==NULL || strlen(yytext)==0
							|| atoi(yytext)==4)
							NEWLINE;
						BEGIN Normal;
						}

<Control>\\[a-zA-Z@]+				IGNORE;
<Control>[a-zA-Z@0-9]*[-'=`][^ \t\n{]*		IGNORE;
<Control>"\n"					{BEGIN Normal; NEWLINE;}
<Control>[ \t]*[{]*				{BEGIN Normal; IGNORE;}
<Control>.					{yyless(0);BEGIN Normal;}

<Normal>[{}\\|]	/* special characters */	IGNORE;
<Normal>[!?]"`"					IGNORE;
<Normal>~					SPACE;

<Normal>{W}[']*{W}				{   if (fWord)
							(void)printf("%s\n", yytext);
						    else
							ECHO;
						}
<Normal>[0-9]+					if (!fWord) ECHO;
<Normal>(.|\n)					if (!fWord) ECHO;

<LaMacro>"\["					{ BEGIN LaOptArg; }
<LaMacro>"{"					{ cOpenBrace++; }
<LaMacro>"}"					{   cOpenBrace--;
						    if (cOpenBrace == 0)
						    {
							if (--cArgs==0)
							BEGIN Normal;
						    }
						}
<LaMacro>.					;
<LaOptArg>"\]"					BEGIN LaMacro;	
<LaOptArg>[^\]]*				;

<LaMacro2>"\["					{ BEGIN LaOptArg2; }
<LaMacro2>"{"				        { if (cOpenBrace == 0)
						    {
							if (--cArgs==0)
							{
							    BEGIN Normal;
							    cOpenBrace--;
							}
						    }
						  cOpenBrace++;
						}
<LaMacro2>"}"					{   cOpenBrace--; }
<LaMacro2>.					;
<LaOptArg2>"\]"					BEGIN LaMacro2;	
<LaOptArg2>.					;
%%
/******
** main --
**	Set sbProgName to the base of arg 0.
**	Set the input paths.
**	Check for options
**		-c		echo LaTeX \cite, \ref, and \pageref values
**		-e <env-list>	list of LaTeX environments to ignore
**		-l		force latex mode
**		-n		do not follow \input and \include
**		-s		replace control sequences with space
**		-t		force tex mode
**		-w		word only output
**	Set the list of LaTeX environments to ignore.
**	Process each input file.
**	If no input files are specified on the command line, process stdin.
******/

main(cArgs,rgsbArgs)
int	cArgs;
char	*rgsbArgs[];
{
	char	*pch, *sbEnvList = DEFAULTENV, sbBadOpt[2];
	FILE	*TexOpen();
	int	fSawFile = 0, iArgs = 1;
	
	/* get base name and decide what we are doing, detex or delatex */
#ifdef OS2
	char drive[_MAX_DRIVE], dir[_MAX_DIR];
	char fname[_MAX_FNAME], ext[_MAX_EXT];
#ifdef __EMX__
	_wildcard(&cArgs, &rgsbArgs);
	_response(&cArgs, &rgsbArgs);
#endif
	_splitpath (rgsbArgs[0], drive, dir, fname, ext);
	sbProgName = strlwr(fname);
#else
	if ((sbProgName = rindex(rgsbArgs[0], '/')) != NULL)
	    sbProgName++;
	else
	    sbProgName = rgsbArgs[0];
#endif
	if (strcmp("delatex",sbProgName) == 0)
	    fLatex = 1;
	
	/* set rgsbInputPaths for use with TexOpen() */
	SetInputPaths();

	/* process command line options */
	while (iArgs < cArgs && *(pch = rgsbArgs[iArgs]) == CHOPT) {
		while (*++pch)
		    switch (*pch) {
		    case CHCITEOPT:
			fCite = 1;
			break;
		    case CHENVOPT:
			if (++iArgs >= cArgs) {
				ErrorExit("-e option requires and argument");
			}
			sbEnvList = rgsbArgs[iArgs];
			break;
		    case CHLATEXOPT:
			fLatex = 1;
			break;
		    case CHNOFOLLOWOPT:
			fFollow = 0;
			break;
		    case CHSPACEOPT:
			fSpace = 1;
			break;
		    case CHTEXOPT:
			fForcetex = 1;
			break;
		    case CHWORDOPT:
			fWord = 1;
			break;
		    default:
#ifdef OS2
			OS2UsageExit();
#else
			sbBadOpt[0] = *pch;
			sbBadOpt[1] = '\0';
			Warning("unknown option ignored -", sbBadOpt);
#endif
		    }
		iArgs++;
	}
	SetEnvIgnore(sbEnvList);

	/* process input files */
	for (; iArgs < cArgs; iArgs++) {
	    fSawFile++;
	    if ((yyin = TexOpen(rgsbArgs[iArgs])) == NULL) {
		Warning("can't open file", rgsbArgs[iArgs]);
		continue;;
	    }
	    BEGIN Normal;
	    (void)yylex();
	}

	/* if there were no input files, assume stdin */
	if (!fSawFile) {
	    yyin = stdin;
#ifdef OS2
	    if (isatty(fileno(stdin)))
		OS2UsageExit();
#endif
	    BEGIN Normal;
	    (void)yylex();
	}
#ifndef FLEX_SCANNER
	if (YYSTATE != Normal)
	    ErrorExit("input contains an unterminated mode or environment");
#endif
	return(0);
}

#ifdef FLEX_SCANNER
#undef yywrap
#endif

/******
** yywrap -- handles EOF for lex.  Check to see if the stack of open files
**	has anything on it.  If it does, set yyin to the to value.  If not
**	return the termination signal for lex.
******/

yywrap()
{
	(void)fclose(yyin);
#ifdef FLEX_SCANNER
        /* Pop context state */
	if (csb > 0) {
		yy_delete_buffer( YY_CURRENT_BUFFER );
		yy_switch_to_buffer( rgsb[--csb] );
	}
#endif /* FLEX_SCANNER */
	if (cfp > 0) {
	    yyin = rgfp[--cfp];
	    return(0);
	}
	return(1);
}

#ifdef OS2

/******
** yyless -- return characters to the input stream.  Some systems don't have
**	a yyless routine
******/

void yyless(n)
int n;
{
	int 	i = strlen(yytext);

	while (i > n) unput(yytext[--i]);
	yytext[yyleng = n] = '\0';
}
#endif

/******
** SetEnvIgnore -- sets rgsbEnvIgnore to the values indicated by the
**	sbEnvList.
******/

SetEnvIgnore(sbEnvList)
char	*sbEnvList;
{
	char *sb;

	sb = SafeMalloc(strlen(sbEnvList) + 1, "malloc for SetEnvIgnore failed");
	(void) strcpy(sb, sbEnvList);
	csbEnvIgnore = SeparateList(sb, rgsbEnvIgnore, CHENVSEP, MAXENVS);
	if (csbEnvIgnore == ERROR)
	    ErrorExit("The environtment list contains too many environments");
}

/******
** BeginEnv -- checks to see if sbEnv is in the list rgsbEnvIgnore.  If it
**	is, sbCurrentEnv is set to sbEnv.
******/

BeginEnv(sbEnv)
char	*sbEnv;
{
	int	i;

	if (!fLatex) return(0);
	for (i = 0; i < csbEnvIgnore; i++)
	    if (strcmp(sbEnv, rgsbEnvIgnore[i]) == 0) {
		(void)strcpy(sbCurrentEnv, sbEnv);
		return(1);
	    }
	return(0);
}

/******
** EndEnv -- checks to see if sbEnv is the current environment being ignored.
******/

EndEnv(sbEnv)
char	*sbEnv;
{
	if (!fLatex) return(0);
	if (strcmp(sbEnv, sbCurrentEnv) == 0)
	    return(1);
	return(0);
}

/******
** InputFile -- push the current yyin and open sbFile.  If the open fails,
**	the sbFile is ignored.
******/

InputFile(sbFile)
char	*sbFile;
{
	FILE	*TexOpen();

	if (!fFollow)
	    return;
	rgfp[cfp++] = yyin;
	if ((yyin = TexOpen(sbFile)) == NULL) {
	    Warning("can't open \\input file", sbFile);
	    yyin = rgfp[--cfp];
            return;
	} 
#ifdef FLEX_SCANNER
        rgsb[csb++] = YY_CURRENT_BUFFER;
        yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
#endif /* FLEX_SCANNER */
}

/******
** IncludeFile -- if sbFile is not in the rgsbIncList, push current yyin
**	and open sbFile.  If the open fails, the sbFile is ignored.
******/

IncludeFile(sbFile)
char	*sbFile;
{
	FILE	*TexOpen();

	if (!fFollow)
	    return;
	if (!InList(sbFile))
	    return;
	rgfp[cfp++] = yyin;
	if ((yyin = TexOpen(sbFile)) == NULL) {
	    Warning("can't open \\include file", sbFile);
	    yyin = rgfp[--cfp];
            return;
	}
#ifdef FLEX_SCANNER
        rgsb[csb++] = YY_CURRENT_BUFFER;
        yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
#endif /* FLEX_SCANNER */
}

/******
** AddInclude -- adds sbFile to the rgsbIncList and increments csbIncList.
**	If the include list is too long, sbFile is ignored.
******/

AddInclude(sbFile)
char	*sbFile;
{
	if (!fFollow)
	    return;
	if (csbIncList >= MAXINCLIST)
	    Warning("\\includeonly list is too long, ignoring", sbFile);
	rgsbIncList[csbIncList] = SafeMalloc(strlen(sbFile) + 1, "malloc for AddInclude failed");
	(void)strcpy(rgsbIncList[csbIncList++], sbFile);
}

/******
** InList -- checks to see if sbFile is in the rgsbIncList.  If there is
**	no list, all files are assumed to be "in the list".
******/

InList(sbFile)
char	*sbFile;
{
	char	*pch, sbBase[MAXPATHLEN];
	int	i;

	if (csbIncList == 0)	/* no list */
	    return(1);
	(void)strcpy(sbBase, sbFile);
	if ((pch = rindex(sbBase, '.')) != NULL)
	    *pch = '\0';
	i = 0;
	while ((i < csbIncList) && rgsbIncList[i])
	    if (strcmp(rgsbIncList[i++], sbBase) == 0)
	        return(1);
	return(0);
}

/******
** SetInputPaths -- sets rgsbInputPaths to the values indicated by the
**	TEXINPUTS environment variable if set or else DEFAULTINPUTS.  If
**	the user's TEXINPUTS has a leading ':' prepend the DEFAULTINPUTS
**	to the path, if there is a trailing ':' append the DEFAULTINPUTS.
**	This is consistent with the most recent TeX.  However, this
**	routine does not honor the '//' construct (expand subdirs).
******/

SetInputPaths()
{
	char *sb, *sbPaths, *getenv();
	int cchDefaults, cchPaths;

	cchDefaults = strlen(DEFAULTINPUTS);
#ifdef OS2
	if ((sb = getenv("TEXINPUT")) == NULL)
#endif
	    if ((sb = getenv("TEXINPUTS")) == NULL)
		sb = DEFAULTINPUTS;
	cchPaths = strlen(sb);
	if (sb[0] == CHPATHSEP)
	    cchPaths += cchDefaults;
	if (sb[strlen(sb) - 1] == CHPATHSEP)
	    cchPaths += cchDefaults;
	sbPaths = SafeMalloc(cchPaths + 1, "malloc for SetInputPaths failed");
	sbPaths[0] = '\0';
	if (sb[0] == CHPATHSEP)
	    (void)strcat(sbPaths, DEFAULTINPUTS);
	(void)strcat(sbPaths, sb);
	if (sb[strlen(sb) - 1] == CHPATHSEP)
	    (void)strcat(sbPaths, DEFAULTINPUTS);

	csbInputPaths = SeparateList(sbPaths, rgsbInputPaths, CHPATHSEP, MAXINPUTPATHS);
	if (csbInputPaths == ERROR)
#ifdef OS2
	    ErrorExit("TEXINPUT(S) environment variable has too many paths");
#else
	    ErrorExit("TEXINPUTS environment variable has too many paths");
#endif
}

/******
** SeparateList -- takes a chSep separated list sbList, replaces the
**	chSep's with NULLs and sets rgsbList[i] to the beginning of
**	the ith word in sbList.  The number of words is returned.  A
**	ERROR is returned if there are more than csbMax words.
******/

SeparateList(sbList, rgsbList, chSep, csbMax)
char	*sbList, *rgsbList[], chSep;
int	csbMax;
{
	int	csbList = 0;

	while (sbList && *sbList && csbList < csbMax) {
	    rgsbList[csbList++] = sbList;
	    if (sbList = index(sbList, chSep))
		*sbList++ = '\0';
	}
	return(sbList && *sbList ? ERROR : csbList);
}

/******
** TexOpen -- tries to open sbFile in each of the rgsbInputPaths in turn.
**	For each input path the following order is used:
**		file.tex - must be as named, if not there go to the next path
**		file.ext - random extension, try it
**		file     - base name, add .tex and try it
**		file     - try it as is
**	Notice that if file exists in the first path and file.tex exists in
**	one of the other paths, file in the first path is what is opened.
**	If the sbFile begins with a '/', no paths are searched.
******/

FILE *
TexOpen(sbFile)
char	*sbFile;
{
	char	*pch, *sbNew;
	FILE	*fp;
	int	iPath;
	static char	sbFullPath[MAXPATHLEN];

	for (iPath = 0; iPath < csbInputPaths; iPath++) {
#ifdef OS2
	    if (*sbFile == '/' || *sbFile == '\\' || strchr(sbFile, ':')) {	/* absolute path */
#else
	    if (*sbFile == '/') {	/* absolute path */
#endif
		(void)sprintf(sbFullPath, "%s", sbFile);
		iPath = csbInputPaths;	/* only check once */
	    } else
		(void)sprintf(sbFullPath, "%s/%s", rgsbInputPaths[iPath], sbFile);
#ifdef OS2
	    pch = sbFullPath;
	    while (pch = strchr(pch, '\\'))
		*pch = '/';
#endif

	    /* If sbFile ends in .tex then it must be there */
	    if ((pch = rindex(sbFullPath, '.')) != NULL
			&& (strcmp(pch, ".tex") == 0))
		if ((fp = fopen(sbFullPath, "r")) != NULL)
		    return(fp);
		else
		    continue;

	    /* if .<ext> then try to open it.  the '.' represents   */
	    /* the beginning of an extension if it is not the first */
	    /* character and it does not follow a '.' or a '/'      */
	    if (pch != NULL && pch > &(sbFullPath[0])
                    && *(pch - 1) != '.' && *(pch - 1) != '/'
		    && (fp = fopen(sbFullPath, "r")) != NULL)
		return(fp);

	    /* just base name, add .tex to the name */
	    sbNew = SafeMalloc(strlen(sbFullPath) + 5, "malloc for TexOpen failed");
	    (void)strcpy(sbNew, sbFullPath);
	    (void)strcat(sbNew, ".tex");
	    if ((fp = fopen(sbNew, "r")) != NULL)
		return(fp);

	    /* try sbFile regardless */
	    if ((fp = fopen(sbFullPath, "r")) != NULL)
		return(fp);
	}
	return((FILE *)NULL);
}

/******
** SafeMalloc -- wrapper around malloc() to check for failure.
******/

char *
SafeMalloc(cch, sbMessage)
int cch;
char *sbMessage;
{
	char *sb;

	if ((sb = (char *)malloc((unsigned)cch)) == NULL)
	    ErrorExit(sbMessage);
	return(sb);
}

/******
** Warning -- print a warning message preceded by the program name.
******/

Warning(sb1, sb2)
char	*sb1, *sb2;
{
	(void)fprintf(stderr, "%s: warning: %s %s\n", sbProgName, sb1, sb2);
}

/******
** ErrorExit -- print an error message preceded by the program name.
**	Stdout is flushed and detex exits.
******/

ErrorExit(sb1)
char	*sb1;
{
	(void)fflush(stdout);
	(void)fprintf(stderr, "%s: error: %s\n", sbProgName, sb1);
	exit(1);
}
#ifdef OS2

/******
** OS2UsageExit -- print OS/2 usage message and exit.
******/

OS2UsageExit()
{
	(void)printf("\n%s [ -clnstw ] [ -e environment-list ] [ filename[.tex] ... ]\n",
		sbProgName);
	puts("  -c  echo LaTeX \\cite, \\ref, and \\pageref values\n  \
-e  <env-list> list of LaTeX environments to ignore\n  \
-l  force latex mode\n  \
-n  do not follow \\input and \\include\n  \
-s  replace control sequences with space\n  \
-t  force tex mode\n  \
-w  word only output");
	exit(0);
}
#endif
