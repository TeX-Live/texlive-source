%option never-interactive
%option nounput

%top{
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
 * detex [-e environment-list] [-c] [-l] [-n] [-s] [-t] [-w] [-1] [file[.tex] ]
 *
 *	This program is used to remove TeX or LaTeX constructs from a text
 *	file.
 */

#include "detex.h"

#ifdef KPATHSEA

#include <c-auto.h>
#include <kpathsea/c-auto.h>
#include <kpathsea/config.h>
#include <kpathsea/c-memstr.h>
#include <kpathsea/c-pathmx.h>
#include <kpathsea/c-std.h>
#include <kpathsea/tex-file.h>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#else /* KPATHSEA */

#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#define	strrchr rindex
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#include <sys/param.h>
#ifndef PATH_MAX /* for old BSD */
#define PATH_MAX MAXPATHLEN
#endif
#endif /* HAVE_LIMITS_H */

#ifdef OS2
#include <stdlib.h>
#endif

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#endif

#endif /* KPATHSEA */
}

%{
#undef IGNORE
#undef ECHO

#define	LaBEGIN		if (fLatex) BEGIN
#define	IGNORE		Ignore()
#define INCRLINENO	IncrLineNo()
#define ECHO		Echo()
#define NOUN		if (fSpace && !fWord && !fReplace) putchar(' '); else {if (fReplace) printf("noun");}
#define VERBNOUN		if (fReplace) printf(" verbs noun"); /* puts a verb and a noun to make grammar checking work */
#define	SPACE		if (!fWord) putchar(' ')
#define	NEWLINE		LineBreak()
#define	LATEX		fLatex=!fForcetex
#define KILLARGS(x)	cArgs=x; LaBEGIN LaMacro
#define STRIPARGS(x)	cArgs=x; LaBEGIN LaMacro2
#define	CITE(x)		if (fLatex && !fCite) KILLARGS(x)

#define NO_MALLOC_DECL

void LineBreak();
void Ignore();
void IncrLineNo();
void Echo();
void AddInclude(char *sbFile);
void ErrorExit(const char *sb1);
void UsageExit(void);
void VersionExit(void);
void IncludeFile(char *sbFile);
void InputFile(char *sbFile);
void SetEnvIgnore(const char *sbEnvList);
#ifndef KPATHSEA
void SetInputPaths(void);
#endif
void Warning(const char *sb1, const char *sb2);
int BeginEnv(const char *sbEnv);
int EndEnv(const char *sbEnv);
int InList(char *sbFile);
int SeparateList(char *sbList, char *rgsbList[] ,char chSep, int csbMax);
FILE *TexOpen(char *sbFile);
char *SafeMalloc(int cch, const char *sbMessage);
#ifndef KPATHSEA
char *getenv();
#ifndef NO_MALLOC_DECL
char	*malloc();
#endif
#ifdef OS2
void	yyless(int);
void	OS2UsageExit(void);
#endif
#endif /* KPATHSEA */

char	*rgsbEnvIgnore[MAXENVS];	/* list of environments ignored */
char	*rgsbIncList[MAXINCLIST];	/* list of includeonly files */
char	*rgsbInputPaths[MAXINPUTPATHS];	/* list of input paths in order */
char	sbCurrentIgnoredEnv[CCHMAXENV];	/* current environment being ignored */
char	*sbProgName;			/* name we were invoked with */
#ifndef NOFILE /* might be defined in <sys/param.h> */
#define NOFILE 256
#endif
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
int	fSrcLoc = 0;			/* flag to display source location of original file */
int fShowPictures = 0;	/* flag to show picture names */
int fReplace = 0;		/* flag to replace envirnments with "noun" */

int currBracesLevel = 0;
int footnoteLevel = -100;

#ifdef FLEX_SCANNER
/* flex has contexts for buffers that need to be switched when file changes
 * otherwise output contains imported files in reverse order.  Weird, but
 * true.
 */
YY_BUFFER_STATE rgsb[NOFILE + 1];		/* flex context stack */
char*			fFileNames[NOFILE + 1];	/* names of the buffers in context stack */
int				fFileLines[NOFILE + 1];	/* line number in each of the context files */
int				fIsColumn0 = 1;			/* Are we at the begining of a line? */
int				csb = 0;				/* depth of flex context stack */
#endif /* FLEX_SCANNER */

%}

S	[ \t\n]*
W	[a-zA-Z]+
N	[+-]?(([0-9]+(\.[0-9]+)?)|(\.[0-9]+))
U	pt|pc|in|bp|cm|mm|dd|cc|sp
P   [a-zA-Z!]+
HU	{U}|em
HD	{S}(({N}{S}{HU})|(\\{W})){S}
HG	{HD}(plus{HD})?(minus{HD})?
VU	{U}|ex
VD	{S}(({N}{S}{VU})|(\\{W})){S}
VG	{VD}(plus{VD})?(minus{VD})?
Z	\*?
VERBSYMBOL	=|\\leq|\\geq|\\in|>|<|\\subseteq|\subseteq|\\subset|\\supset|\\sim|\\neq|\\mapsto

%Start Define Display IncludeOnly Input Math Normal Control
%Start LaBegin LaDisplay LaEnd LaEnv LaFormula LaInclude LaSubfile
%Start LaMacro LaOptArg LaMacro2 LaOptArg2 LaVerbatim 
%start LaBreak LaPicture

%%
<Normal>"%".*		/* ignore comments */	{INCRLINENO;}

<Normal>"\\begin"{S}"{"{S}"document"{S}"}""\n"*	{LATEX; IGNORE;}

<Normal>"\\begin"     /* environment start */	{LaBEGIN LaBegin; IGNORE;}

<LaBegin>{S}"{"{S}"verbatim"{S}"}"		{ if (BeginEnv("verbatim"))
							BEGIN LaEnv;
						    else
							BEGIN LaVerbatim;
						    IGNORE;
						}

<LaVerbatim>"\\end"{S}"{"{S}"verbatim"{S}"}" /* verbatim mode */	{BEGIN Normal; IGNORE;}
<LaVerbatim>[^\\]+				ECHO;
<LaVerbatim>.					ECHO;

<LaBegin>{S}"{"{S}"minipage"{S}"}"		{ KILLARGS(1);
						  if (BeginEnv("minipage"))
							BEGIN LaEnv;
						  else
							BEGIN LaMacro; /* Normal; */
						  IGNORE;
						}

<LaBegin>{S}"{"{S}"table"{S}"}"{S}"["{P}"]"		{
						  if (BeginEnv("table"))
							BEGIN LaEnv;
						  else
							BEGIN Normal;
						  IGNORE;
						}

<LaBegin>{S}"{"{S}"figure"{S}"}"{S}"["{P}"]"		{
						  if (BeginEnv("figure"))
							BEGIN LaEnv;
						  else
							BEGIN Normal;
						  IGNORE;
						}
						
<LaBegin>{W}					{ if (BeginEnv(yytext))
							BEGIN LaEnv;
						    else
							BEGIN Normal;
						    IGNORE;
						}
	/*<LaBegin>"\n"					NEWLINE;*/
<LaBegin>.					{INCRLINENO;}

<LaEnv>"\\end"  /* absorb some environments */	{LaBEGIN LaEnd; IGNORE;}
<LaEnv>"\n"+					;/*NEWLINE;*/
<LaEnv>.					{INCRLINENO;}

<LaEnd>{W}		 /* end environment */	{   if (EndEnv(yytext))
							BEGIN Normal;
						    IGNORE;
						}
<LaEnd>"}"					{BEGIN LaEnv; IGNORE;}
	/*<LaEnd>"\n"					NEWLINE;*/
<LaEnd>.					{INCRLINENO;}

<Normal>"\\kern"{HD}				;
<Normal>"\\vskip"{VG}				;
<Normal>"\\vspace"{Z}{S}"{"{VG}"}"		;
<Normal>"\\hskip"{HG}				;
<Normal>"\\hspace"{Z}{S}"{"{HG}"}"		;
<Normal>"\\addvspace"{S}"{"{VG}"}"		;
<Normal>"{"{N}"pt}"						; /* hack to fix \begin{minipage}{300pt} */

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
<LaPicture>[^{}]+				{ if(fShowPictures) { printf("<Picture %s>", yytext); } }
<LaPicture>"\}"{S}"\n"+				{ BEGIN Normal; INCRLINENO; }
<LaPicture>"\}"					BEGIN Normal;

<Normal>"\\definecolor"				{ KILLARGS(3); }
<Normal>"\\color"				{ KILLARGS(1); }
<Normal>"\\textcolor"				{ KILLARGS(2); }
<Normal>"\\colorbox"				{ KILLARGS(2); }
<Normal>"\\fcolorbox"				{ KILLARGS(3); }
<Normal>"\\pagecolor"				{ KILLARGS(1); }
<Normal>"\\foilhead"				{ STRIPARGS(1); }
<Normal>"\\addfontfeature"			{ KILLARGS(1); }
<Normal>"\\thispagestyle"			{ KILLARGS(1); }
<Normal>"\\addcontentsline"			{ KILLARGS(3); }

<Normal>"\\part"{Z}					;/*NEWLINE;*/
<Normal>"\\chapter"{Z}				;/*NEWLINE;*/
<Normal>"\\section"{Z}				;/*NEWLINE;*/
<Normal>"\\subsection"{Z}			;/*NEWLINE;*/
<Normal>"\\subsubsection"{Z}		;/*NEWLINE;*/
<Normal>"\\paragraph"{Z}			;/*NEWLINE;*/
<Normal>"\\subparagraph"{Z}			;/*NEWLINE;*/

<Normal>"\\bibitem"	    /* ignore args  */	{KILLARGS(1); IGNORE;}
<Normal>"\\bibliography"    /* of these \cs */	{KILLARGS(1); IGNORE;}
<Normal>"\\bibstyle"				{KILLARGS(1); IGNORE;}
<Normal>" "?"\\cite"				{KILLARGS(1);} /* kill space before */
<Normal>"\\documentstyle"			{LATEX; KILLARGS(1); IGNORE;}
<Normal>"\\documentclass"			{LATEX; KILLARGS(1); IGNORE;}
<Normal>"\\usepackage"				{KILLARGS(1); IGNORE;}
<Normal>"\\end"					{KILLARGS(1); IGNORE;}
<Normal>"\\hypersetup"			{KILLARGS(1);}
<Normal>"\\index"				{KILLARGS(1);}
	/*<Normal>"\\footnote"				{KILLARGS(1); SPACE;}*/
<Normal>"\\label"				{KILLARGS(1); IGNORE;}
<Normal>"\\nameref"					{CITE(1); IGNORE;}
<Normal>"\\pageref"				{CITE(1); IGNORE;}
<Normal>"\\pagestyle"				{KILLARGS(1); IGNORE;}
<Normal>"\\ref"					{CITE(1); IGNORE;}
<Normal>"\\setcounter"				{KILLARGS(2); IGNORE;}
<Normal>"\\addtocounter"			{KILLARGS(2); IGNORE;}
<Normal>"\\newcounter"				{ KILLARGS(1); }
<Normal>"\\stepcounter"			{ KILLARGS(2); }

<Normal>"\\fontspec"			{KILLARGS(1);}

<Normal>"\\footnote"(\[([^\]])+\])?"{"		{
							putchar('(');
							footnoteLevel = currBracesLevel;
							++currBracesLevel;
							}
<Normal>"\\verb" /* ignore \verb<ch>...<ch> */	{   if (fLatex) {
						 	char verbchar, c;
						 	verbchar = input();
						 	while ((c = input()) != verbchar)
							    /*if (c == '\n')
								NEWLINE;*/
								putchar(c);
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

<Normal>"\\("		/* formula mode */	{LaBEGIN LaFormula; NOUN;}
<LaFormula>"\\)"				BEGIN Normal;
<LaFormula>"\n"					NEWLINE;
<LaFormula>{VERBSYMBOL}			VERBNOUN;
<LaFormula>.					;

<Normal>"\\["		/* display mode */	{LaBEGIN LaDisplay; NOUN;}
<LaDisplay>"\\]"				BEGIN Normal;
<LaDisplay>"\n"					NEWLINE;
<LaDisplay>{VERBSYMBOL}			VERBNOUN;
<LaDisplay>.					;

<Normal>"$$"		/* display mode */	{BEGIN Display; NOUN;}
<Display>"$$"					BEGIN Normal;
<Display>"\n"					NEWLINE;
<Display>{VERBSYMBOL}			VERBNOUN;
<Display>.					;

<Normal>"$"		/* math mode */		{BEGIN Math; NOUN;}
<Math>"$"					BEGIN Normal;
<Math>"\n"					;
<Math>"\\$"					;
<Math>{VERBSYMBOL}			VERBNOUN;
<Math>.						;

<Normal>"\\include"	/* process files */	{LaBEGIN LaInclude; IGNORE;}
<LaInclude>[^{ \t\n}]+				{   IncludeFile(yytext);
						    BEGIN Normal;
						}
<LaInclude>"\n"+					NEWLINE;
<LaInclude>.					;

<Normal>"\\includeonly"				{BEGIN IncludeOnly; IGNORE;}
<IncludeOnly>[^{ \t,\n}]+			AddInclude(yytext);
<IncludeOnly>"}"				{   if (csbIncList == 0)
							rgsbIncList[csbIncList++] = '\0';
						    BEGIN Normal;
						}
<IncludeOnly>"\n"+				NEWLINE;
<IncludeOnly>.					;

<Normal>"\\subfile"	/* process files */	{LaBEGIN LaSubfile; IGNORE;}
<LaSubfile>[^{ \t\n}]+				{   IncludeFile(yytext);
						    BEGIN Normal;
						}
<LaSubfile>"\n"+					NEWLINE;
<LaSubfile>.					;

<Normal>"\\input"				{BEGIN Input; IGNORE;}
<Input>[^{ \t\n}]+				{   InputFile(yytext);
						    BEGIN Normal;
						}
<Input>"\n"+					NEWLINE;
<Input>.					;

	/* escaping commands */
<Normal>"\\slash"			putchar('/');

<Normal>\\(aa|AA|ae|AE|oe|OE|ss)[ \t]*[ \t\n}] /* handle ligatures */	{(void)printf("%.2s", yytext+1);}
<Normal>\\[OoijLl][ \t]*[ \t\n}]		{(void)printf("%.1s", yytext+1);}
<Normal>"\\linebreak"(\[[0-4]\])?			{NEWLINE;}	/*BEGIN LaBreak;*/

<Normal>\\[a-zA-Z@]+	/* ignore other \cs */	{BEGIN Control; IGNORE;}
<Normal>"\\ "					SPACE;
<Normal>"\\\\"{Z}(\[[^\]]*\])?			NEWLINE;
<Normal>\\.					IGNORE;
<LaBreak>[0-4]?					{  if (yytext==NULL || strlen(yytext)==0
							|| atoi(yytext)>=0)
							NEWLINE;
						BEGIN Normal;
						}

<Control>\\[a-zA-Z@]+				IGNORE;
<Control>[a-zA-Z@0-9]*[-'=`][^ \t\n{]*		IGNORE;
<Control>"\n"					{BEGIN Normal; /*NEWLINE;*/}
<Control>[ \t]*[{]+				{++currBracesLevel;BEGIN Normal; IGNORE;}
<Control>[ \t]*				{BEGIN Normal; IGNORE;}
<Control>.					{yyless(0);BEGIN Normal;}

<Normal>[\\|]	/* special characters */	IGNORE;
<Normal>[!?]"`"				IGNORE;
<Normal>~					SPACE;
<Normal>-{2,3}				putchar('-');
<Normal>``					putchar('"');
<Normal>`					putchar('\'');
<Normal>''					putchar('"');
<Normal>,,					putchar('"');

	/* braces */
<Normal>"{"					{ ++currBracesLevel;
							}
<Normal>"}"					{
							--currBracesLevel;
							if (currBracesLevel == footnoteLevel) {
								putchar(')');
								footnoteLevel = -100;
							}
							}
<Normal>{W}[']*{W}				{   if (fWord)
							(void)printf("%s\n", yytext);
						    else
							ECHO;
						}
<Normal>[0-9]+					if (!fWord) ECHO;
<Normal>.					{ INCRLINENO; if (!fWord) ECHO; }
<Normal>"\n"					{ if (!fWord) NEWLINE; }
<Normal>("\t")+					if (!fWord) putchar('\t');

<LaMacro>"\["					{ BEGIN LaOptArg; }
<LaMacro>"{"					{ cOpenBrace++; }
<LaMacro>"}""\n"{0,1}				{   cOpenBrace--; INCRLINENO;
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
**              -1              output some location information
**	Set the list of LaTeX environments to ignore.
**	Process each input file.
**	If no input files are specified on the command line, process stdin.
******/

int
main(int cArgs, char *rgsbArgs[])
{
	char	*pch, sbBadOpt[2];
	const char *sbEnvList = DEFAULTENV;
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
#elif defined(KPATHSEA)
	kpse_set_program_name (rgsbArgs[0], NULL);
	sbProgName = kpse_program_name;
#else
	if ((sbProgName = strrchr(rgsbArgs[0], '/')) != NULL)
	    sbProgName++;
	else
	    sbProgName = rgsbArgs[0];
#endif
	if (strcmp("delatex",sbProgName) == 0)
	    fLatex = 1;

#ifndef KPATHSEA
	/* set rgsbInputPaths for use with TexOpen() */
	SetInputPaths();
#endif

	/* process command line options */
	while (iArgs < cArgs && *(pch = rgsbArgs[iArgs]) == CHOPT) {
		while (*++pch)
		    switch (*pch) {
		    case CHCITEOPT:
			fCite = 1;
			break;
		    case CHENVOPT:
			if (++iArgs >= cArgs) {
				ErrorExit("-e option requires an argument");
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
			case CHREPLACE:
			fReplace = 1;
			break;
			case CHVERSIONOPT:
			VersionExit();
			break;
			case CHSRCLOC:
			fSrcLoc = 1;
			break;
		    default:
			sbBadOpt[0] = *pch;
			sbBadOpt[1] = '\0';
			Warning("unknown option ignored -", sbBadOpt);
			UsageExit();
		    }
		iArgs++;
	}
	SetEnvIgnore(sbEnvList);

#ifdef WIN32
	_setmode(fileno(stdout), _O_BINARY);
#endif

	/* process input files */
	for (; iArgs < cArgs; iArgs++) {
	    fSawFile++;
	    if ((yyin = TexOpen(rgsbArgs[iArgs])) == NULL) {
		Warning("can't open file", rgsbArgs[iArgs]);
		continue;;
	    }
		fFileNames[csb] = rgsbArgs[iArgs];
		fFileLines[csb] = 1;
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

int
yywrap(void)
{
	(void)fclose(yyin);
#ifdef FLEX_SCANNER
        /* Pop context state */
	if (csb > 0) {
		free(fFileNames[csb]);
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

void
yyless(int n)
{
	int 	i = strlen(yytext);

	while (i > n) unput(yytext[--i]);
	yytext[yyleng = n] = '\0';
}
#endif

/******
** PrintPrefix -- In case fSrcLoc is 1 and we are about to
**	print the first column of a line, we want to output the location of
**	that line in the original LaTeX document it came from.
******/

void
PrintPrefix()
{
	if (fSrcLoc && fIsColumn0) {
		printf("%s:%d: ", fFileNames[csb], fFileLines[csb]);
		fIsColumn0 = 0;
	}
}

/******
** LineBreak -- choses the proper way to break a line. If '-1' option is
**	enabled we also want to output some source location information.
******/

void
LineBreak()
{
	if (fWord) return;
	PrintPrefix();
	putchar('\n');
	fFileLines[csb]++; fIsColumn0=1;
}

/******
** Echo -- If we are at column 0 and have specified '-1'; output 
**	the source location information.
******/

void
Echo()
{
	PrintPrefix();
	fprintf(yyout, "%s", yytext);
}

/******
** IncrLineNo -- Increase the correct linenumber counter and
**	reset the the 'fIsColumn0' to true.
******/

void
IncrLineNo()
{
	char *c;
	for (c=yytext; *c != '\0'; c++) {
		if (*c == '\n') {
			fFileLines[csb]++; fIsColumn0=1;
		}
	}
}

/******
** Ignore -- Since we might need to track source location information we
**	cannot just ignore text. We must at least increase the linenumber counter.
******/

void 
Ignore()
{
	IncrLineNo();
	if (fSpace && !fWord) putchar(' ');
}

/******
** SetEnvIgnore -- sets rgsbEnvIgnore to the values indicated by the
**	sbEnvList.
******/

void
SetEnvIgnore(const char *sbEnvList)
{
	char *sb;
	sb = SafeMalloc(strlen(sbEnvList) + 1, "malloc for SetEnvIgnore failed");
	(void) strcpy(sb, sbEnvList);

	
	csbEnvIgnore = SeparateList(sb, rgsbEnvIgnore, CHENVSEP, MAXENVS);
	if (csbEnvIgnore == my_ERROR)
	    ErrorExit("The environment list contains too many environments");
}

/******
** BeginEnv -- checks to see if sbEnv is in the list rgsbEnvIgnore.  If it
**	is, sbCurrentIgnoredEnv is set to sbEnv.
******/

int
BeginEnv(const char *sbEnv)
{
	int	i;

	if (!fLatex) return(0);
	for (i = 0; i < csbEnvIgnore; ++i)
	    if (strcmp(sbEnv, rgsbEnvIgnore[i]) == 0) {
			(void)strcpy(sbCurrentIgnoredEnv, sbEnv);
			return(1);
	    }
	return(0);
}

/******
** EndEnv -- checks to see if sbEnv is the current environment being ignored.
******/

int
EndEnv(const char *sbEnv)
{
	if (!fLatex) return(0);
	if (strcmp(sbEnv, sbCurrentIgnoredEnv) == 0)
	    return(1);
	return(0);
}

/******
** InputFile -- push the current yyin and open sbFile.  If the open fails,
**	the sbFile is ignored.
******/


void
InputFile(char *sbFile)
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
	rgsb[csb++]     = YY_CURRENT_BUFFER;
	fFileLines[csb] = 1;
	fFileNames[csb] = strdup(sbFile);
	yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
#endif /* FLEX_SCANNER */
}

/******
** IncludeFile -- if sbFile is not in the rgsbIncList, push current yyin
**	and open sbFile.  If the open fails, the sbFile is ignored.
******/

void
IncludeFile(char *sbFile)
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
	rgsb[csb++]     = YY_CURRENT_BUFFER;
	fFileLines[csb] = 1;
	fFileNames[csb] = strdup(sbFile);
	yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
#endif /* FLEX_SCANNER */
}

/******
** AddInclude -- adds sbFile to the rgsbIncList and increments csbIncList.
**	If the include list is too long, sbFile is ignored.
******/

void
AddInclude(char *sbFile)
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

int
InList(char *sbFile)
{
	char	*pch, sbBase[PATH_MAX];
	int	i;

	if (csbIncList == 0)	/* no list */
	    return(1);
	(void)strcpy(sbBase, sbFile);
	if ((pch = strrchr(sbBase, '.')) != NULL)
	    *pch = '\0';
	i = 0;
	while ((i < csbIncList) && rgsbIncList[i])
	    if (strcmp(rgsbIncList[i++], sbBase) == 0)
	        return(1);
	return(0);
}

#ifndef KPATHSEA
/******
** SetInputPaths -- sets rgsbInputPaths to the values indicated by the
**	TEXINPUTS environment variable if set or else DEFAULTINPUTS.  If
**	the user's TEXINPUTS has a leading ':' prepend the DEFAULTINPUTS
**	to the path, if there is a trailing ':' append the DEFAULTINPUTS.
**	This is consistent with the most recent TeX.  However, this
**	routine does not honor the '//' construct (expand subdirs).
******/

void
SetInputPaths(void)
{
	const char *sb;
	char *sbPaths;
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
	if (csbInputPaths == my_ERROR)
#ifdef OS2
	    ErrorExit("TEXINPUT(S) environment variable has too many paths");
#else
	    ErrorExit("TEXINPUTS environment variable has too many paths");
#endif
}
#endif

/******
** SeparateList -- takes a chSep separated list sbList, replaces the
**	chSep's with NULLs and sets rgsbList[i] to the beginning of
**	the ith word in sbList.  The number of words is returned.  A
**	my_ERROR is returned if there are more than csbMax words.
******/

int
SeparateList(char *sbList, char *rgsbList[], char chSep, int csbMax)
{
	int	csbList = 0;

	while (sbList && *sbList && csbList < csbMax) {
	    rgsbList[csbList++] = sbList;
	    if ((sbList = strchr(sbList, chSep))) {
			*sbList++ = '\0';
		}
	}
	return(sbList && *sbList ? my_ERROR : csbList);
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
TexOpen(char *sbFile)
{
	char	*sbNew;
#ifndef KPATHSEA
	char	*pch;
	FILE	*fp;
	int	iPath;
	static char	sbFullPath[PATH_MAX];

	for (iPath = 0; iPath < csbInputPaths; iPath++) {
#ifdef OS2
	    if (*sbFile == '/' || *sbFile == '\\' || strchr(sbFile, ':')) {	/* absolute path */
#else
	    if (*sbFile == '/') {	/* absolute path */
#endif
			(void)snprintf(sbFullPath, PATH_MAX-1, "%s", sbFile);
			iPath = csbInputPaths;	/* only check once */
	    } else {
			(void)snprintf(sbFullPath, PATH_MAX-1, "%s/%s", rgsbInputPaths[iPath], sbFile);
		}
#ifdef OS2
	    pch = sbFullPath;
	    while (pch = strchr(pch, '\\')) {
			*pch = '/';
		}
#endif

	    /* If sbFile ends in .tex then it must be there */
	    if ((pch = strrchr(sbFullPath, '.')) != NULL
			&& (strcmp(pch, ".tex") == 0)) {
			if ((fp = fopen(sbFullPath, "r")) != NULL)
				return(fp);
			else
				continue;
	    }

	    /* if .<ext> then try to open it.  the '.' represents   */
	    /* the beginning of an extension if it is not the first */
	    /* character and it does not follow a '.' or a '/'      */
	    if (pch != NULL && pch > &(sbFullPath[0])
                    && *(pch - 1) != '.' && *(pch - 1) != '/'
		    && (fp = fopen(sbFullPath, "r")) != NULL) {
			return(fp);
		}

	    /* just base name, add .tex to the name */
	    sbNew = SafeMalloc(strlen(sbFullPath) + 5, "malloc for TexOpen failed");
	    (void)strcpy(sbNew, sbFullPath);
	    (void)strcat(sbNew, ".tex");
	    if ((fp = fopen(sbNew, "r")) != NULL) {
			free(sbNew);
			return(fp);
		}
		free(sbNew);

	    /* try sbFile regardless */
	    if ((fp = fopen(sbFullPath, "r")) != NULL)
			return(fp);
	}
	return NULL;
#else
	sbNew = kpse_find_file (sbFile, kpse_tex_format, false);

	if (sbNew == NULL)
	    return NULL;

	return fopen (sbNew, "r");
#endif
}

/******
** SafeMalloc -- wrapper around malloc() to check for failure.
******/

char *
SafeMalloc(int cch, const char *sbMessage)
{
	char *sb;

	if ((sb = (char *)malloc((unsigned)cch)) == NULL)
	    ErrorExit(sbMessage);
	return(sb);
}

/******
** Warning -- print a warning message preceded by the program name.
******/

void
Warning(const char *sb1, const char *sb2)
{
	(void)fprintf(stderr, "%s: warning: %s %s\n", sbProgName, sb1, sb2);
}

/******
** ErrorExit -- print an error message preceded by the program name.
**	Stdout is flushed and detex exits.
******/

void
ErrorExit(const char *sb1)
{
	(void)fflush(stdout);
	(void)fprintf(stderr, "%s: error: %s\n", sbProgName, sb1);
	exit(1);
}

/******
** UsageExit -- print usage message and exit.
******/

void
UsageExit(void)
{
	(void)printf("\n%s [ -clnrstw1 ] [ -e environment-list ] [ filename[.tex] ... ]\n",
		sbProgName);
	puts("  -c  echo LaTeX \\cite, \\ref, and \\pageref values\n  \
-e  <env-list> list of LaTeX environments to ignore\n  \
-l  force latex mode\n  \
-n  do not follow \\input, \\include and \\subfile\n  \
-r  replace math with \"noun\" and \"noun verbs noun\" to preserve grammar\n  \
-s  replace control sequences with space\n  \
-t  force tex mode\n  \
-w  word only output\n  \
-1  outputs the original file name and line number in the beginning of each line\n  \
-v  show program version and exit");
	exit(0);
}

/******
** VersionExit -- print version and exit.
******/

void
VersionExit(void)
{
	(void)printf("\nOpenDetex version %s\nhttps://github.com/pkubowicz/opendetex\n",
		VERSION);
	exit(0);
}
