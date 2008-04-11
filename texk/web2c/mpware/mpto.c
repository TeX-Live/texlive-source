/* $Id: mpto.c,v 1.15 2005/08/28 11:42:55 taco Exp $
 * Public domain.
 *
 * Previous versions of mpto were copyright 1990 - 1995 by AT&T Bell
 * Laboratories.  It has since been put into the public domain.
 *
 * John Hobby wrote the original version, which has since been
 * extensively altered.
 */

/* mpto [-tex|-troff] MPFILE
 * 
 * This program transforms a MetaPost input file into a TeX or troff input
 * file by stripping out btex...etex and verbatimtex...etex sections.
 * Leading and trailing spaces and tabs are removed from the extracted
 * material and it is surrounded by the preceding and following strings
 * defined immediately below.  The input file should be given as argument 1
 * and the resulting TeX or troff file is written on standard output.
 *
 * Changes incorporated from Web2c:
 * - merged TeX and troff mode
 * - support -E<errlog>, for AMIGA
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>


/* MetaPost itself has a configurable max line length, but we can afford to
   use smaller values than that */
#define bufsize  1000

char *tex_predoc = "";
char *tex_postdoc = "\\end{document}\n";
char *tex_pretex1 = "\\gdef\\mpxshipout{\\shipout\\hbox\\bgroup%\n"
    "  \\setbox0=\\hbox\\bgroup}%\n"
    "\\gdef\\stopmpxshipout{\\egroup"
    "  \\dimen0=\\ht0 \\advance\\dimen0\\dp0\n"
    "  \\dimen1=\\ht0 \\dimen2=\\dp0\n"
    "  \\setbox0=\\hbox\\bgroup\n"
    "    \\box0\n"
    "    \\ifnum\\dimen0>0 \\vrule width1sp height\\dimen1 depth\\dimen2 \n"
    "    \\else \\vrule width1sp height1sp depth0sp\\relax\n"
    "    \\fi\\egroup\n"
    "  \\ht0=0pt \\dp0=0pt \\box0 \\egroup}\n"
    "\\mpxshipout%% line %d %s\n";
char *tex_pretex = "\\mpxshipout%% line %d %s\n";
char *tex_posttex = "\n\\stopmpxshipout\n";
char *tex_preverb1 = "";	/* if very first instance */
char *tex_preverb = "%% line %d %s\n";	/* all other instances */
char *tex_postverb = "\n";

/* According to CSTR #54 the ".lf" directive should be ".lf %d %s",
 * not ".lf line %d %s" as used in the original code.  This affects
 * troff_pretex1, troff_pretex, troff_preverb1, troff_preverb.
 */
char *troff_predoc = ".po 0\n";
char *troff_postdoc = "";
char *troff_pretex1 = ".lf %d %s\n";	/* first instance */
char *troff_pretex = ".bp\n.lf %d %s\n";	/* subsequent instances */
char *troff_posttex = "\n";
char *troff_preverb1 = ".lf %d %s\n";
char *troff_preverb = ".lf %d %s\n";
char *troff_postverb = "\n";

char *predoc;
char *postdoc;
char *pretex1;
char *pretex;
char *posttex;
char *preverb1;
char *preverb;
char *postverb;

char *mpname;
FILE *mpfile;
int lnno = 0;			/* current line number */
int texcnt = 0;			/* btex..etex blocks so far */
int verbcnt = 0;		/* verbatimtex..etex blocks so far */
char *bb, *tt, *aa;		/* start of before, token, and after strings */
char buf[bufsize];		/* the input line */

void
err(char *msg)
{
    fprintf(stderr, "mpto: %s:%d: %s\n", mpname, lnno, msg);
    exit(1);
}

void
usage(char *progn)
{
    fprintf(stderr, "Try `%s --help' for more information\n", progn);
    exit(1);
}

char *
getline(void)
{				/* returns NULL on EOF or error, otherwise buf */
    int c;
    unsigned loc = 0;
    while ((c = getc(mpfile)) != EOF && c != '\n' && c != '\r') {
	buf[loc++] = c;
	if (loc == bufsize)
	    err("Line is too long");
    }
    if (c == EOF)
	return NULL;
    buf[loc] = 0;
    if (c == '\r') {
	c = getc(mpfile);
	if (c != '\n')
	    ungetc(c, mpfile);
    }
    lnno++;
    return buf;
}


/* Return nonzero if a prefix of string s matches the null-terminated string t
 * and the next character is not a letter or an underscore.
 */
int
match_str(char *s, char *t)
{
    while (*t != 0) {
	if (*s != *t)
	    return 0;
	s++;
	t++;
    }
    switch (*s) {
    case 'a':
    case 'c':
    case 'd':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case '_':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case 'b':
    case 'e':
    case 'v':
	return 0;
    default:
	return 1;
    }
}


/* This function tries to express s as the concatenation of three strings
 * b, t, a, with the global pointers bb, tt, and aa set to the start of the
 * corresponding strings.  String t is either a quote mark, a percent sign,
 * or an alphabetic token "btex", "etex", or "verbatimtex".  (An alphabetic
 * token is a maximal sequence of letters and underscores.)  If there are
 * several possible substrings t, we choose the leftmost one.  If there is
 * no such t, we set b=s and return 0.
 */
int
getbta(char *s)
{
    int ok = 1;			/* zero if last character was a-z, A-Z, or _ */

    bb = s;
    for (tt = bb; *tt != 0; tt++)
	switch (*tt) {
	case '"':
	case '%':
	    aa = tt + 1;
	    return 1;
	case 'b':
	    if (ok && match_str(tt, "btex")) {
		aa = tt + 4;
		return 1;
	    } else
		ok = 0;
	    break;
	case 'e':
	    if (ok && match_str(tt, "etex")) {
		aa = tt + 4;
		return 1;
	    } else
		ok = 0;
	    break;
	case 'v':
	    if (ok && match_str(tt, "verbatimtex")) {
		aa = tt + 11;
		return 1;
	    } else
		ok = 0;
	    break;
	case 'a':
	case 'c':
	case 'd':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case '_':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	    ok = 0;
	    break;
	default:
	    ok = 1;
	}
    aa = tt;
    return 0;
}


void
copytex(void)
{
    char *s;			/* where a string to print stops */
    char *t;			/* for finding start of last line */
    char c;
    char *res = NULL;
    do {
	if (*aa == 0)
	  if ((aa = getline()) == NULL)
	    err("btex section does not end");

	if (getbta(aa) && *tt == 'e') {
     	    s = tt;
	} else {
	    if (*tt == 'b')
		err("btex in TeX mode");
	    if (*tt == 'v')
		err("verbatimtex in TeX mode");
	    s = aa;
	}
	c = *s;
	*s = 0;
	if (res==NULL) {
	  res = malloc(strlen(bb)+2);
	  if (res==NULL)
	    err("memory allocation failure");
	  res = strncpy(res,bb,(strlen(bb)+1));
	} else {
	  res = realloc(res,strlen(res)+strlen(bb)+2);
	  if (res==NULL)
	    err("memory allocation failure");
	  res = strncat(res,bb, strlen(bb));
	}
	if (c == '\0')
	    res = strncat(res, "\n", 1);
	*s = c;
    } while (*tt != 'e');
    /* whitespace at the end */
    for (s = res + strlen(res) - 1;
	 s >= res && (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n'); s--);
    t = s;
    *(++s) = '\0';
    /* whitespace at the start */
    for (s = res;
	 s < (res + strlen(res)) && (*s == ' ' || *s == '\t' || *s == '\r'
				     || *s == '\n'); s++);
    for (; *t != '\n' && t > s; t--);
    printf("%s", s);
    /* put no '%' at end if it's only 1 line total, starting with '%';
     * this covers the special case "%&format" in a single line. */
    if (t != s || *t != '%')
	printf("%%");
    free(res);
}


void
do_line(void)
{
    aa = buf;
    while (getbta(aa))
	if (*tt == '%')
	    break;
	else if (*tt == '"') {
	    do
		if (!getbta(aa))
		    err("string does not end");
	    while (*tt != '"');
	} else if (*tt == 'b') {
	    if (texcnt++ == 0)
		printf(pretex1, lnno, mpname);
	    else
		printf(pretex, lnno, mpname);
	    copytex();
	    printf("%s", posttex);
	} else if (*tt == 'v') {
	    if (verbcnt++ == 0 && texcnt == 0)
		printf(preverb1, lnno, mpname);
	    else
		printf(preverb, lnno, mpname);
	    copytex();
	    printf("%s", postverb);
	} else
	    err("unmatched etex");
}

int
main(int argc, char **argv)
{
    int mode;
    if (argc == 1) {
	fputs("mpto: Need exactly one file argument.\n", stderr);
	fputs("Try `mpto --help' for more information.\n", stderr);
	exit(1);
    } else if (argc > 1 && (strcmp(argv[1], "--help") == 0
			    || strcmp(argv[1], "-help") == 0)) {
	fputs("Usage: mpto [-tex|-troff] MPFILE", stdout);
#ifdef AMIGA
	fputs(" [-E<errlog-file>]", stdout);
#endif
	fputs("\n  Strip btex..etex and verbatimtex...etex parts from MetaPost input\n\
  file MPFILE, converting to either TeX or troff (TeX by default).\n\
\n\
--help      display this help and exit\n\
--version   output version information and exit\n",
	      stdout);
	fputs("\nEmail bug reports to metapost@tug.org.\n", stdout);
	exit(0);
    } else if (argc > 1 && (strcmp(argv[1], "--version") == 0
			    || strcmp(argv[1], "-version") == 0)) {
	printf("mpto 1.003\n\
This program is in the public domain.\n\
Primary author of mpto: John Hobby.\n\
Current maintainer: Taco Hoekwater.\n");
	exit(0);
    } else if (argc == 2) {
	mpname = argv[1];
	mode = 0;
#ifdef AMIGA
    } else if (argc == 3) {
	if (strcmp(argv[1], "-tex") == 0) {
	    mpname = argv[2];
	    mode = 0;
	} else if (strcmp(argv[1], "-troff") == 0) {
	    mpname = argv[2];
	    mode = 1;
	} else if (strncmp(argv[2], "-E", 2) || (argv[2] + 2 == NULL)) {
	    usage(argv[0]);
	} else {
	    mpname = argv[1];
	    freopen(argv[2] + 2, "w", stderr);
	}
    } else if (argc == 4) {
	if (strcmp(argv[1], "-tex") == 0) {
	    mode = 0;
	} else if (strcmp(argv[1], "-troff") == 0) {
	    mode = 1;
	} else {
	    usage(argv[0]);
	}
	if (strncmp(argv[3], "-E", 2) || (argv[3] + 2 == NULL)) {
	    usage(argv[0]);
	} else {
	    mpname = argv[2];
	    freopen(argv[3] + 2, "w", stderr);
	}
#else				/* not AMIGA */
    } else if (argc == 3) {
	if (strcmp(argv[1], "-tex") == 0) {
	    mode = 0;
	} else if (strcmp(argv[1], "-troff") == 0) {
	    mode = 1;
	} else {
	    usage(argv[0]);
	}
	mpname = argv[2];
#endif				/* not AMIGA */
    } else {
	usage(argv[0]);
    }

    mpfile = fopen(mpname, "r");
    if (mpfile == NULL) {
	fprintf(stderr, "%s: ", argv[0]);
	perror(mpname);
	exit(1);
    }

    /* This is far from elegant, but life is short.  */
    if (mode == 0) {
	predoc = tex_predoc;
	postdoc = tex_postdoc;
	pretex1 = tex_pretex1;
	pretex = tex_pretex;
	posttex = tex_posttex;
	preverb1 = tex_preverb1;
	preverb = tex_preverb;
	postverb = tex_postverb;
    } else {
	predoc = troff_predoc;
	postdoc = troff_postdoc;
	pretex1 = troff_pretex1;
	pretex = troff_pretex;
	posttex = troff_posttex;
	preverb1 = troff_preverb1;
	preverb = troff_preverb;
	postverb = troff_postverb;
    }
    printf("%s", predoc);
    while (getline() != NULL)
	do_line();
    printf("%s", postdoc);
    exit(0);
}
