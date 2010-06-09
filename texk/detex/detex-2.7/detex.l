%{
#ifndef lint
static char	rcsid[] = "$Header: /p/src/local/bin/detex/RCS/detex.l,v 2.19 1997/09/10 18:12:37 trinkle Exp $";
#endif

/*
 * detex [-e environment-list] [-c] [-l] [-n] [-s] [-t] [-w] [file[.tex] ]
 *
 *	This program is used to remove TeX or LaTeX constructs from a text
 *	file.
 *
 * Written by:
 *	Daniel Trinkle
 *	Department of Computer Science
 *	Purdue University
 *
 */

#ifdef FPTEX
#include <win32lib.h>
#undef ERROR
#endif

#include "detex.h"
#ifndef KPATHSEA

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
#define PATH_MAX MAXPATHLEN
#ifdef OS2
#include <stdlib.h>
#endif
#ifndef NO_MALLOC_DECL
char	*malloc();
#endif

#else

#include "c-auto.h"
#include "kpathsea/c-auto.h"
#include "kpathsea/config.h"
#include "kpathsea/c-memstr.h"
#include "kpathsea/c-pathmx.h"
#include "kpathsea/c-std.h"
#include "kpathsea/tex-file.h"

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#endif

#undef IGNORE

void SetEnvIgnore(const char *sbEnvList);
int BeginEnv(const char *sbEnv);
int EndEnv(const char *sbEnv);
void InputFile(char *sbFile);
void IncludeFile(char *sbFile);
void AddInclude(char *sbFile);
int InList(char *sbFile);
void SetInputPaths(void);
int SeparateList(char *sbList, char **rgsbList ,char chSep,int csbMax);
FILE * TexOpen(char *sbFile);
char * SafeMalloc(int cch, const char *sbMessage);
void Warning(const char *sb1, const char *sb2);
void ErrorExit(const char *sb1);

#define	LaBEGIN		if (fLatex) BEGIN
#define	CITEBEGIN	if (fLatex && !fCite) BEGIN
#define	IGNORE		if (fSpace && !fWord) putchar(' ')
#define	SPACE		if (!fWord) putchar(' ')
#define	NEWLINE		if (!fWord) putchar('\n')

#ifdef OS2
void	yyless(int);
void OS2UsageExit(void);
#endif

char	*rgsbEnvIgnore[MAXENVS];	/* list of environments ignored */
char	*rgsbIncList[MAXINCLIST];	/* list of includeonly files */
char	*rgsbInputPaths[MAXINPUTPATHS];	/* list of input paths in order */
char	sbCurrentEnv[CCHMAXENV];	/* current environment being ignored */
char	*sbProgName;			/* name we were invoked with */
#ifndef NOFILE /* might be defined in <sys/param.h> */
#define NOFILE 256
#endif
FILE	*rgfp[NOFILE+1];		/* stack of input/include files */
int	cfp = 0;			/* count of files in stack */
int	cOpenBrace = 0;			/* count of `{' in <LaMacro2> */
int	csbEnvIgnore;			/* count of environments ignored */
int	csbIncList = 0;			/* count of includeonly files */
int	csbInputPaths;			/* count of input paths */
int	fLatex = 0;			/* flag to indicated delatex */
int	fWord = 0;			/* flag for -w option */
int	fFollow = 1;			/* flag to follow input/include */
int	fCite = 0;			/* flag to echo \cite and \ref args */
int	fSpace = 0;			/* flag to replace \cs with space */
int	fForcetex = 0;			/* flag to inhibit latex mode */
%}

S	[ \t\n]*
W	[a-zA-Z]+

%Start Define Display IncludeOnly Input Math Normal Control
%Start LaBegin LaDisplay LaEnd LaEnv LaFormula LaInclude
%Start LaMacro LaMacro2 LaVerbatim

%%
<Normal>"%".*		/* ignore comments */	;

<Normal>"\\begin"{S}"{"{S}"document"{S}"}"	{fLatex = !fForcetex; IGNORE;}

<Normal>"\\begin"     /* environment start */	{LaBEGIN LaBegin; IGNORE;}

<LaBegin>{S}"{"{S}"verbatim"{S}"}"		{   if (BeginEnv("verbatim"))
							BEGIN LaEnv;
						    else
							BEGIN LaVerbatim;
						    IGNORE;
						}

<LaVerbatim>"\\end"{S}"{"{S}"verbatim"{S}"}" /* verbatim mode */	{BEGIN Normal; IGNORE;}
<LaVerbatim>.					ECHO;

<LaBegin>{W}					{   if (BeginEnv(yytext))
							BEGIN LaEnv;
						    else
							BEGIN LaMacro;
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

<Normal>"\\bibitem"	    /* ignore args  */	{LaBEGIN LaMacro2; IGNORE;}
<Normal>"\\bibliography"    /* of these \cs */	{LaBEGIN LaMacro; IGNORE;}
<Normal>"\\bibstyle"				{LaBEGIN LaMacro; IGNORE;}
<Normal>"\\cite"				{CITEBEGIN LaMacro2; IGNORE;}
<Normal>"\\documentstyle"			{LaBEGIN LaMacro; IGNORE;}
<Normal>"\\end"					{LaBEGIN LaMacro; IGNORE;}
<Normal>"\\footnote"				{SPACE;}
<Normal>"\\index"				{LaBEGIN LaMacro2; SPACE;}
<Normal>"\\label"				{LaBEGIN LaMacro; IGNORE;}
<Normal>"\\pageref"				{CITEBEGIN LaMacro; IGNORE;}
<Normal>"\\pagestyle"				{LaBEGIN LaMacro; IGNORE;}
<Normal>"\\ref"					{CITEBEGIN LaMacro; IGNORE;}
<Normal>"\\setcounter"				{LaBEGIN LaMacro; IGNORE;}
<Normal>"\\verb" /* ignore \verb<ch>...<ch> */	{   if (fLatex) {
						 	char verbchar, c;
						 	verbchar = input();
						 	while ((c = input()) != verbchar)
							    if (c == '\n')
								NEWLINE;
						    }
						    IGNORE;
						}
<LaMacro>"}"					BEGIN Normal;
<LaMacro>"\n"					NEWLINE;
<LaMacro>.					;
<LaMacro2>"{"					{   cOpenBrace++; }
<LaMacro2>"}"					{   cOpenBrace--;
						    if (cOpenBrace == 0)
							BEGIN Normal;
						}
<LaMacro2>"\n"					NEWLINE;
<LaMacro2>.					;

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

<Normal>\\[a-zA-Z@]+	/* ignore other \cs */	{BEGIN Control; IGNORE;}
<Normal>"\\ "					SPACE;
<Normal>\\.					IGNORE;
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

int
main(int cArgs, char *rgsbArgs[])
{
	char	*pch, sbBadOpt[2];
	const char	*sbEnvList = DEFAULTENV;
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
#ifdef KPATHSEA
	kpse_set_program_name (rgsbArgs[0], NULL);
#endif
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
			sbEnvList = rgsbArgs[++iArgs];
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

int
yywrap(void)
{
	(void)fclose(yyin);
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
	if (csbEnvIgnore == ERROR)
	    ErrorExit("The environtment list contains too many environments");
}

/******
** BeginEnv -- checks to see if sbEnv is in the list rgsbEnvIgnore.  If it
**	is, sbCurrentEnv is set to sbEnv.
******/

int
BeginEnv(const char *sbEnv)
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

int
EndEnv(const char *sbEnv)
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

void
InputFile(char *sbFile)
{
	if (!fFollow)
	    return;
	rgfp[cfp++] = yyin;
	if ((yyin = TexOpen(sbFile)) == NULL) {
	    Warning("can't open \\input file", sbFile);
	    yyin = rgfp[--cfp];
	}
}

/******
** IncludeFile -- if sbFile is not in the rgsbIncList, push current yyin
**	and open sbFile.  If the open fails, the sbFile is ignored.
******/

void
IncludeFile(char *sbFile)
{
	if (!fFollow)
	    return;
	if (!InList(sbFile))
	    return;
	rgfp[cfp++] = yyin;
	if ((yyin = TexOpen(sbFile)) == NULL) {
	    Warning("can't open \\include file", sbFile);
	    yyin = rgfp[--cfp];
	}
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
#ifndef WIN32
	char *getenv();
#endif
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

int
SeparateList(char *sbList, char **rgsbList, char chSep, int csbMax)
{
	int	csbList = 0;

	while (sbList && *sbList && csbList < csbMax) {
	    rgsbList[csbList++] = sbList;
	    if ((sbList = strchr(sbList, chSep)))
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
	    if (*sbFile == '/' || *sbFile == '\\' || strchr(sbFile, ':'))	/* absolute path */
#else
	    if (*sbFile == '/')	/* absolute path */
#endif
            {
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
	    if ((pch = strrchr(sbFullPath, '.')) != NULL
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
#else
	sbNew = kpse_find_file (sbFile, kpse_tex_format, false);

	if (sbNew == NULL)
	    return (FILE *)NULL;

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
#ifdef OS2

/******
** OS2UsageExit -- print OS/2 usage message and exit.
******/

void
OS2UsageExit(void)
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
