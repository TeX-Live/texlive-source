#define DUMMY \
set -ex; \
gcc -O3 -s -DNDEBUG=1 -DNO_CONFIG=1 -ansi -pedantic -Wunused \
  -Wall -W -Wstrict-prototypes -Wtraditional -Wnested-externs -Winline \
  -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wmissing-prototypes \
  -Wmissing-declarations ps_tiny.c -o ps_tiny; \
exit
/* ps_tiny.c -- read .pin files, write short PostScript code
 * by pts@fazekas.hu at Sat Aug 24 12:46:14 CEST 2002
 * -- Sat Aug 24 18:23:08 CEST 2002
 * -- Sat Aug 24 23:47:02 CEST 2002
 * -- Mon Sep  2 11:28:19 CEST 2002
 * %<B added -- Thu Sep 19 09:23:00 CEST 2002
 * v0.02 WORKS for flatedecode.psm -- Sun Sep 22 00:39:54 CEST 2002
 * DEVEL
 */

/*
 * Imp: extensive documentation
 * Imp: verify acount + xcount
 * Imp: uncompressed méretellenõrzés
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* exit() */

#if NO_CONFIG
  #include <assert.h>
  #if SIZEOF_INT>=4
    typedef unsigned slen_t;
    typedef int slendiff_t;
  #  define SLEN_P ""
  #else
    typedef unsigned long slen_t;
    typedef long slendiff_t;
  #  define SLEN_P "l"
  #endif
  typedef char sbool;
  #if ';'!=59 || 'a'!=97
  #  error ASCII system is required to compile this program
  #endif
#else
  #include "config2.h" /* by sam2p... */
  typedef bool sbool;
#endif
#if OBJDEP
#  warning PROVIDES: ps_tiny_main
#endif

#define ULE(a,b) (((a)+0U)<=((b)+0U))
#define ISWSPACE(i,j) ((i j)==32 || ULE(i-9,13-9) || i==0)

#define PROGNAME "ps_tiny"
#define VERSION "0.02"

/* --- Data */

#define SBUFSIZE 4096

/** Data area holding strings; */
char sbuf[SBUFSIZE], *sbufb;

#define WORDSSIZE 127

/** Pointers inside sbuf, indexed by chars, [0..31] are special */
char const* words[WORDSSIZE];

static void sbuff(void) {
  unsigned i;
  sbuf[0]='\0';
  sbufb=sbuf+1;
  for (i=0;i<WORDSSIZE;i++) words[i]=sbuf;
}

/** @return -1 or the word found */
static int findword(char const*word) {
  unsigned i=32;
  while (i<WORDSSIZE && 0!=strcmp(word,words[i])) i++;
  return i==WORDSSIZE ? -1 : (int)i;
}

/** @return -1 or the word found */
static int findword_stripslash(char const*word) {
  unsigned i=32;
  assert(word[0]!='/');
  while (i<WORDSSIZE && 0!=strcmp(word,words[i]+(words[i][0]=='/'))) i++;
  return i==WORDSSIZE ? -1 : (int)i;
}

/* --- Reading */

/** Current byte offset in input. (from 0) */
slen_t curofs;

/** Current line in input. (from 1) */
slen_t curline;

/** Offset of the leftmost char of current line in input. (from 0) */
slen_t leftofs;

/** Current position in input override format string */
char const* ifmt;

/** Input override string list */
char const* const* ilist;

/** Current position in current item of ilist */
char const* iitem;

/** Char unget */
int ilast;

static void rewindd(void) { ifmt=0; curofs=leftofs=0; curline=1; }

static void setifmt(char const *ifmt_, char const* const* ilist_) {
  ifmt=ifmt_;
  ilist=ilist_;
  iitem="";
  ilast=-1;
}

static void noifmt(void) { ifmt=0; }

static int getcc(void) {
  int c;
  if (ifmt!=0) {
   nextitem:
    if (ilast>=0) { c=ilast; ilast=-1; }
    else if (iitem[0]!='\0') c=*iitem++;
    else if (ifmt[0]=='$') { iitem=*ilist++; ifmt++; goto nextitem; }
    else if (ifmt[0]=='\0') c=-1; /* EOF forever */
    else c=*ifmt++;
    /* fprintf(stderr, "igetcc()='%c'\n", c); */
  } else {
    if ((c=getchar())=='\n') { leftofs=curofs; curline++; }
    curofs++;
  }
  return c;
}

static void ungetcc(int c) {
  if (ifmt!=0) {
    assert(ilast<0);
    ilast=c;
  } else {
    if (c=='\n') curline--;
    if (c>=0) { curofs--; ungetc(c,stdin); }
  }
}

static void erri(char const*msg1, char const*msg2) {
  fprintf(stderr, "%s: error at %"SLEN_P"u.%"SLEN_P"u.%"SLEN_P"u: %s%s\n",
    PROGNAME, curline, curofs-leftofs+1, curofs, msg1, msg2?msg2:"");
  exit(3);
}

static void setword(int c, char const*word, int slash) {
  slen_t len;
  assert(slash==0 || slash==1);
  if (words[c][0]!='\0') erri("abbr letter already used",0);
  if (sbuf+SBUFSIZE-sbufb+0U<(len=strlen(word))+slash+1U) erri("words overflow",0);
  /* fprintf(stderr,"setword(%d,\"%s\");\n", c, word); */
  words[c]=sbufb;
  if (slash) *sbufb++='/';
  strcpy(sbufb, word);
  sbufb+=len+1;
  /* assert(c<3 || findword(word)==c); */
}

/** So a long string from ps_tiny --copy <bts2.ttt will fit */
/* #define IBUFSIZE 512 */
#define IBUFSIZE 32000

/** Input buffer for several operations. */
char ibuf[IBUFSIZE];
/** Position after last valid char in ibuf */
char *ibufb;

typedef slendiff_t psint_t;

#if 0
static psint_t getint(void) {
  int c, sgn=1;
  psint_t v, w;
  while (ISWSPACE(c,=getcc())) ;
  if (c=='-') { c=getcc(); sgn=-1; }
  if (!ULE(c-'0','9'-'0')) erri("integer expected",0); /* Dat: EOF err */
  v=sgn*(c-'0');
  while (ULE((c=getcc())-'0','9'-'0')) {
    if ((w=10*v+sgn*(c-'0'))/10!=v) erri("integer overflow",0);
    /* ^^^ Dat: overflow check is ok between -MAX-1 .. MAX */
    v=w;
  }
  ungetcc(c);
  return v;
}

static psint_t getuint(void) {
  psint_t v=getint();
  if (v<0) erri("nonnegative integer expected",0);
  return v;
}
#endif

static /*inline*/ sbool is_ps_white(int/*char*/ c) {
  return c=='\n' || c=='\r' || c=='\t' || c==' ' || c=='\f' || c=='\0';
}

static /*inline*/ sbool is_ps_name(int/*char*/ c) {
  /* Dat: we differ from PDF since we do not treat the hashmark (`#') special
   *      in names.
   * Dat: we differ from PostScript since we accept names =~ /[!-~]/
   */
  return c>='!' && c<='~'
      && c!='/' && c!='%' && c!='{' && c!='}' && c!='<' && c!='>'
      && c!='[' && c!=']' && c!='(' && c!=')';
  /* Dat: PS avoids: /{}<>()[]% \n\r\t\000\f\040 */
}

/** Definition chosen rather arbitrarily by pts */
static sbool is_wordx(char const *s) {
  if (!ULE(*s-'A','Z'-'A') && !ULE(*s-'a','z'-'a') && *s!='.') return 0;
   /* && !ULE(*s-'0','9'-'0') && *s!='-') return 0; */
  while (*++s!='\0') if (!is_ps_name(*s)) return 0;
  return 1;
}

/** @param b: assume null-terminated @return true on error */
static /*inline*/ sbool toInteger(char *s, psint_t *ret) {
  int n=0; /* BUGFIX?? found by __CHECKER__ */
  return sscanf(s, "%"SLEN_P"i%n", ret, &n)<1 || s[n]!='\0';
}

/** @param b: assume null-terminated @return true on error */
static /*inline*/ sbool toReal(char *s, double *ret) {
  int n;
  char c;
  /* Dat: glibc accepts "12e", "12E", "12e+" and "12E-" */
  return sscanf(s, "%lf%n", ret, &n)<1
      || (c=s[n-1])=='e' || c=='E' || c=='+' || c=='-' || s[n]!='\0';
}

/** Returns a PostScript token ID, puts token into buf */
static char gettok(void) {
  /* Derived from MiniPS::Tokenizer::yylex() of sam2p-0.37 */
  int c=0; /* dummy initialization */
  sbool hi;
  unsigned hv=0; /* =0: pacify G++ 2.91 */
  slen_t nest;
  char *ibufend=ibuf+IBUFSIZE;
  ibufb=ibuf;

#if 0  
  if (ungot==EOFF) return EOFF;
  if (ungot!=NO_UNGOT) { c=ungot; ungot=NO_UNGOT; goto again; }
#endif
 again_getcc:
  c=getcc();
 /* again: */
  switch (c) {
   case -1: eof:
    return 0; /*ungot=EOFF */;
   case '\n': case '\r': case '\t': case ' ': case '\f': case '\0':
    goto again_getcc;
   case '%': /* one-line comment */
    if ((c=getcc())=='<') { /* XMLish tag */
      char ret='<';
      if ((c=getcc())=='/') { ret='>'; c=getcc(); } /* close tag */
      if (!ULE(c-'A','Z'-'A')) erri("invalid tag",0); /* catch EOF */
      (ibufb=ibuf)[0]=c; ibufb++;
      while (ULE((c=getcc())-'A','Z'-'A') || ULE(c-'a','z'-'a')) {
        if (ibufb==ibufend-1) erri("tag too long",0);
        *ibufb++=c;
      }
      if (c<0) erri("unfinished tag",0);
      *ibufb='\0';
      ungetcc(c);
      return ret;
    }
    while (c!='\n' && c!='\r' && c!=-1) c=getcc();
    if (c==-1) goto eof;
    goto again_getcc;
   case '{': case '[':
    *ibufb++=c;
    return '[';
   case '}': case ']':
    *ibufb++=c;
    return ']';
   case '>':
    if (getcc()!='>') goto err;
    *ibufb++='>'; *ibufb++='>';
    return ']';
   case '<':
    if ((c=getcc())==-1) { uf_hex: erri("unfinished hexstr",0); }
    if (c=='<') {
      *ibufb++='<'; *ibufb++='<';
      return '[';
    }
    if (c=='~') erri("a85str unsupported",0);
    hi=1;
    while (c!='>') {
           if (ULE(c-'0','9'-'0')) hv=c-'0';
      else if (ULE(c-'a','f'-'a')) hv=c-'a'+10;
      else if (ULE(c-'A','F'-'A')) hv=c-'A'+10;
      else if (is_ps_white(c)) hv=16;
      else erri("syntax error in hexstr",0);
      if (hv==16) ;
      else if (!hi) { ibufb[-1]|=hv; hi=1; }
      else if (ibufb==ibufend) erri("hexstr literal too long",0);
      else { *ibufb++=(char)(hv<<4); hi=0; }
      if ((c=getcc())==-1) goto uf_hex;
    }
    /* This is correct even if an odd number of hex digits have arrived */
    return '(';
   case '(':
    nest=1;
    while ((c=getcc())!=-1) {
      if (c==')' && --nest==0) return '(';
      if (c!='\\') { if (c=='(') nest++; }
      else switch (c=getcc()) { /* read a backslash escape */
       case -1: goto uf_str;
       case 'n': c='\n'; break;
       case 'r': c='\r'; break;
       case 't': c='\t'; break;
       case 'b': c='\010'; break; /* \b and \a conflict between -ansi and -traditional */
       case 'f': c='\f'; break;
       default:
        if (!ULE(c-'0','7'-'0')) break;
        hv=c-'0'; /* read at most 3 octal chars */
        if ((c=getcc())==-1) goto uf_str;
        if (c<'0' || c>'7') { ungetcc(c); c=hv; }
        hv=8*hv+(c-'0');
        if ((c=getcc())==-1) goto uf_str;
        if (c<'0' || c>'7') { ungetcc(c); c=hv; }
        c=(char)(8*hv+(c-'0'));
      } /* SWITCH */
      if (ibufb==ibufend) erri("str literal too long",0);
      /* putchar(c); */
      *ibufb++=c;
    } /* WHILE */    
    /* if (c==')') return '('; */
    uf_str: erri("unfinished str",0);
   case ')': goto err;
   case '/':
    *ibufb++='/';
    while (ISWSPACE(c,=getcc())) ;
    /* ^^^ `/ x' are two token in PostScript, but here we overcome the C
     *     preprocessor's feature of including whitespace.
     */
    /* fall-through, b will begin with '/' */
   default: /* /nametype, /integertype or /realtype */
    *ibufb++=c;
    while ((c=getcc())!=-1 && is_ps_name(c)) {
      if (ibufb==ibufend) erri("token too long",0);
      *ibufb++=c;
    }
    ungetcc(c);
    if (ibuf[0]=='/') return '/';
    /* Imp: optimise numbers?? */
    if (ibufb!=ibufend) {
      psint_t l;
      double d;
      *ibufb='\0';
      /* Dat: we don't support base-n number such as `16#100' == 256 in real PostScript */
      if (!toInteger(ibufb, &l)) return '1'; /* throw away integer value */
      /* Dat: call toInteger _before_ toReal */
      if (!toReal(ibufb, &d)) return '.';
    }
    return 'E'; /* executable /nametype */
  }
 err:
  erri("syntax error, token expected",0);
  goto again_getcc; /* notreached */
}

static void getotag(char const*tag) {
#if 0 /* This code segment cannot ignore comments */
  char const *p=tag;
  int c;
  while (ISWSPACE(c,=getcc())) ;
  if (c!='%' || (c=getcc())!='<') erri("tag expected: ", tag);
  while (ISWSPACE(c,=getcc())) ;
  while (p[0]!='\0') {
    if (c!=*p++) erri("this tag expected: ", tag);
    c=getcc();
  }
  ungetcc(c);
#else
  if (gettok()!='<' || 0!=strcmp(ibuf,tag)) erri("tag expected: ", tag);
#endif
}

static void gettagbeg(void) {
  int c;
  while (ISWSPACE(c,=getcc())) ;
  if (c!='>') erri("`>' expected",0);
}

static void gettagend(void) {
  int c;
  while (ISWSPACE(c,=getcc())) ;
  if (c!='/') erri("`/>' expected",0);
  while (ISWSPACE(c,=getcc())) ;
  if (c!='>') erri("`/>' expected",0);
}

static void getkey(char const *key) {
  char const *p=key;
  int c;
  while (ISWSPACE(c,=getcc())) ;
  while (p[0]!='\0') {
    if (c!=*p++) erri("this key expected: ", key);
    c=getcc();
  }
  while (ISWSPACE(c,)) c=getcc();
  if (c!='=') erri("key `=' expected", 0);
}


/** Loads a value into ibuf, '\0'-terminated. */
static void getval(void) {
  sbool g=1;
  char *ibufend1=ibuf+IBUFSIZE-1, c;
  ibufb=ibuf;
  while (ISWSPACE(c,=getcc())) ;
  while (1) {
    if (c=='"') g=!g;
    else if (g && (ISWSPACE(c,) || c=='/' || c=='>')) { ungetcc(c); break; }
    else if (c<0) erri("unfinished tag val",0);
    else if (c==0) erri("\\0 disallowed in tag val",0);
    else if (ibufb==ibufend1) erri("tag val too long",0);
    else *ibufb++=c;
    c=getcc();
  } /* WHILE */
  *ibufb='\0';
}

static psint_t getuintval(void) {
  psint_t ret;
  getval();
  /* fprintf(stderr, "[%s]\n", ibuf); */
  if (toInteger(ibuf, &ret) || ret<0) erri("tag val must be nonnegative integer",0);
  return ret;
}

/* --- Writing */

/** Maximum number of characters in a line. */
#define MAXLINE 78

/** Number of characters already written into this line. (from 0) */
slen_t wcolc;

/** Last token was a self-closing one */
sbool wlastclosed;

static void prepare(void) { wcolc=0; wlastclosed=1; }

static void newline(void) {
  if (wcolc!=0) {
    putchar('\n');
    wcolc=0; wlastclosed=1;
  } else assert(wlastclosed);
}

/** 2: "\\n"; 1: "\n" */
static int pstrq_litn_pp=2;

/** @return the byte length of a string as a quoted PostScript ASCII string
 * literal
 */
static slen_t pstrqlen(register char const* p, char const* pend) {
  slen_t olen=2; /* '(' and ')' */
  char c;
  p=ibuf; pend=ibufb;  while (p!=pend) {
    if ((c=*(unsigned char const*)p++)=='\r' || c=='\t' || c=='\010' || c=='\f' || c=='(' || c==')') olen+=2;
    else if (c=='\n') olen+=pstrq_litn_pp;
    else if (c>=32 && c<=126) olen++;
    else if (c>=64 || p==pend || ULE(*p-'0','7'-'0')) olen+=4;
    else if (c>=8) olen+=3;
    else olen+=2;
  }
  return olen;
}

/** Prints the specified string as a quoted PostScript ASCII string literal.
 * Does not modify wcolc etc.
 */
static void pstrqput(register char const* p, char const* pend) {
  char c;
  putchar('('); 
  p=ibuf; pend=ibufb;  while (p!=pend) {
    if ((ULE((c=*(unsigned char const*)p++)-32, 126-32) && c!='(' && c!=')')
     || (c=='\n' && pstrq_litn_pp==1)
       ) { putchar(c); continue; }
    putchar('\\');
         if (c=='\n')   putchar('n');
    else if (c=='\r')   putchar('r');
    else if (c=='\t')   putchar('t');
    else if (c=='\010') putchar('b');
    else if (c=='\f')   putchar('f');
    else if (c=='(')    putchar('('); /* BUGFIX at Tue Feb 22 00:22:19 CET 2005 */
    else if (c==')')    putchar(')');
    else if (c>=64 || p==pend || ULE(*p-'0','7'-'0')) {
      putchar((c>>6&7)+'0');
      putchar((c>>3&7)+'0');
      putchar((   c&7)+'0');
    } else if (c>=8) {
      putchar((c>>3)  +'0');
      putchar((   c&7)+'0');
    } else putchar(c+'0');
  }
  putchar(')');
}

static int copy_longstr_warn_p=1;

/** Copies PostScript code from input to output. Strips comments, superfluous
 * whitespace etc., substitutes words etc.
 * @param tag may be NULL, this signals: copy till EOF, doesn't substitute
 *   words
 */
static void copy(char const*tag) {
  int c;
  slen_t len, olen;
  while (1) switch (gettok()) {
   case 0:
    if (tag) erri("eof before close tag: ", tag);
    return;
   case '[': case ']':
    if (wcolc+(len=ibufb-ibuf)>MAXLINE) newline();
    wlastclosed=1;
   write:
    if (len>MAXLINE) fprintf(stderr, "%s: warning: output line too long\n", PROGNAME);
    fwrite(ibuf, 1, len, stdout); wcolc+=len;
    break;
   case '/':
    if ((len=ibufb-ibuf)<IBUFSIZE && tag && !(*ibufb='\0') && (c=findword(ibuf))>=0) {
      ibuf[0]='/';
      ibuf[1]=c;
      ibufb=ibuf+2;
      len=2;
    }
    if (wcolc+len>MAXLINE) newline();
    wlastclosed=0;
    goto write;
   case '<': erri("tag unexpected",0);
   case '>':
    if (!tag) erri("close tag unexpected",0);
    if (strlen(tag)!=(len=ibufb-ibuf) || 0!=memcmp(ibuf,tag,len)) erri("this close tag expected: ", tag);
    /* wlastclosed is left unmodified */
    gettagbeg();
    return;
   case '(':
    olen=pstrqlen(ibuf,ibufb);
    if (wcolc+olen>MAXLINE) newline();
    if (olen>MAXLINE && copy_longstr_warn_p) fprintf(stderr, "%s: warning: output string too long\n", PROGNAME);
    /* putchar(ibuf[1]); */
    wcolc+=olen; pstrqput(ibuf,ibufb);
    wlastclosed=1;
    break;
   default: /* case '1': case '.': case 'E': */
    /* fprintf(stderr,"fw(%s) %c\n", ibuf, findword("32768")); */
    if ((len=ibufb-ibuf)<IBUFSIZE && tag && !(*ibufb='\0') && (c=findword_stripslash(ibuf))>=0) {
      ibuf[0]=c;
      ibufb=ibuf+1;
      len=1;
    }
    if (wcolc+len+!wlastclosed>MAXLINE) newline();
    else if (!wlastclosed) { putchar(' '); wcolc++; }
    wlastclosed=0;
    goto write;
  }
}

/** Prefix chars: 32..41 ' '..')' */
static void cprefix(slen_t olen) {
  slen_t plen;
  if (0==olen) erri("empty token","");
  if (olen>11) erri("compressed token too long (>11)","");
  plen=olen+(olen>1)+(olen==7);
  if (wcolc+(wcolc==0)+plen>MAXLINE) newline();
  if (wcolc==0) { putchar('%'); wcolc=1; }
  if (plen>=MAXLINE) fprintf(stderr, "%s: warning: output line too long\n", PROGNAME);
  if (olen==7) { putchar(8+30); putchar(' '); wcolc+=2; } /* Dat: ASCII 37=='%' */
  else if (olen>1) { putchar(olen+30); wcolc++; }
}

/** Copies PostScript code from input to output. Strips comments, superfluous
 * whitespace etc., substitutes words etc. Output is not valid PostScript code,
 * but it needs decompression (i.e adding spaces to words). Ignores var
 * `wlastclosed' (sets it to 1 by calling newline()).
 */
static void compress0(char const*tag) {
  int c;
  slen_t len, olen;
  newline();
  while (1) switch (gettok()) {
   case 0: erri("eof before close tag: ", tag);
   case '[': case ']':
    len=ibufb-ibuf;
   write:
    cprefix(len);
    fwrite(ibuf, 1, len, stdout); wcolc+=len;
    break;
   case '/':
    if ((len=ibufb-ibuf)<IBUFSIZE && !(*ibufb='\0') && (c=findword(ibuf))>=0) {
      /* erri("possibly stupid redef: ", ibuf); */
      ibuf[0]='/';
      ibuf[1]=c;
      ibufb=ibuf+2;
      len=2;
    }
    goto write;
   case '<': erri("tag unexpected",0);
   case '>':
    if (strlen(tag)!=(len=ibufb-ibuf) || 0!=memcmp(ibuf,tag,len)) erri("this close tag expected: ", tag);
    /* wlastclosed is left unmodified */
    fputs((wcolc+2>MAXLINE) ? "\n%   %%" : "%%", stdout);
    newline();
    gettagbeg();
    return;
   case '(':
    cprefix(olen=pstrqlen(ibuf,ibufb));
    wcolc+=olen; pstrqput(ibuf,ibufb);
    break;
   default: /* case '1': case '.': case 'E': */
    if ((len=ibufb-ibuf)<IBUFSIZE && !(*ibufb='\0') && (c=findword_stripslash(ibuf))>=0) {
      ibuf[0]=c;
      ibufb=ibuf+1;
      len=1;
    }
    goto write;
  }
}

/** by pts@fazekas.hu at Fri Sep  6 11:34:57 CEST 2002. Copies data from stdin
 * to stdout till close tag %</tag>. (Note that it isn't safe to copy data of
 * the ASCII85Encode filter this way
 */
static void copydata(char const*tag) {
  int c;
  while (ISWSPACE(c,=getcc())) ;
  /* ungetcc(c); */
  while (1) {
    if (c<0) erri("missing close tag: ", tag);
    if (c!='%') { putchar(c); c=getcc(); continue; }
    if ((c=getcc())=='<') {
      if ((c=getcc())=='/') {
        while (tag[0]!='\0') {
          if ((c=getcc())!=*tag++) erri("this close tag expected: ", tag);
        }
        while (ISWSPACE(c,=getcc())) ;
        if (c!='>') erri("`>' expected", 0);
        break;
      } else {
        putchar('%');
        putchar('<');
      }
    } else {
      putchar('%');
    }
  }
}

/* --- Main */

int main(int argc, char**argv) {
  slen_t acount, xcount, inlining, slen;
  char tmp[40];
  (void)argc;

  /* freopen("t.pin","rb",stdin); */

  sbuff();
  rewindd();
  prepare();
  if (argv[0] && argv[1] && 0==strcmp(argv[1],"--copy")) {
    copy_longstr_warn_p=0;
    pstrq_litn_pp=1;
    copy((char const*)0);
    return 0;
  }

  { char tok=gettok();
    slen_t len;
    len=ibufb-ibuf;
    if (tok=='<' && len==4 && 0==memcmp(ibuf, "Head", 4)) {
      gettagbeg();
      copydata("Head");
      getotag("Open");
    } else if (tok=='<' && len==4 && 0==memcmp(ibuf, "Open", 4)) {
    } else erri("tag %<Head or %<Open expected",0);
  }
  gettagbeg();
  copy("Open");

  getotag("Abbr");
  getkey("acount");  acount=getuintval();
  getkey("xcount");  xcount=getuintval();
  sprintf(tmp,"%"SLEN_P"u dict%%</I>", acount+xcount);
  setifmt(tmp,0); copy("I"); noifmt();
  gettagbeg();

#define E_RANGE 3
#define E_SYNTAX 4
#define E_COMPRESS 5
  copy("Abbr");
  setword(E_RANGE, ")range", 0);
  setword(E_SYNTAX, ")syntax", 0);
  setword(E_COMPRESS, ")compress", 0);
  { int c;
    for (c=7;c<32;c++) words[c]=words[E_RANGE];
    for (c=32;c<42;c++) words[c]=words[E_COMPRESS];
    for (c=47;c<58;c++) words[c]=words[E_SYNTAX]; /* Name:'/', Number:'0'..'9' */
    words[0U+'<']=words[0U+'>']= /* Hex */
      words[0U+'[']=words[0U+']']= /* Bracket */
      words[0U+'{']=words[0U+'}']= /* Brace */
      words[E_SYNTAX];
    for (c=127;c<WORDSSIZE;c++) words[c]=words[E_RANGE];
  }
  
  { char tok;
    slen_t len;
    int c;
    char ct[2];
    char const* ilist_[3];
    psint_t i;
    sbool g;
    while (1) {
      tok=gettok(); len=ibufb-ibuf;
      if (tok=='<' && len==1 && 0==memcmp(ibuf, "A", 1)) {
        while (ISWSPACE(c,=getcc())) ;
        if (c<32 || c>=WORDSSIZE) erri("invalid %<A letter",0);
        ct[0]=c; ct[1]='\0'; ilist_[0]=ct;
        getval();
        if (is_wordx(ibuf)) {
          setword(c, ibuf, 0);
          ilist_[1]=words[c]; words[c]=sbuf; /* temp hide */
          ilist_[2]=words[0][0]=='\0' ? "load def" : words[0];
          setifmt("/$ /$ $%</I>", ilist_);
          if (words[1][0]=='\0' && 0==strcmp(ibuf,"def")) setword(1, ct, 0);
        } else {
          g=toInteger(ibuf, &i); /* need braces? */
          if (g) { words[2]=sbuf; c=2; }
          setword(c, ibuf, 0); /* temp save */
          ilist_[1]=words[c]; words[c]=sbuf; /* temp hide */
          ilist_[2]=words[1][0]=='\0' ? "def" : words[1];
          setifmt(g ? "/$ { $\n} $%</I>" : "/$ $\n $%</I>", ilist_);
          if (words[0][0]=='\0' && 0==strcmp(ibuf,"load def")) setword(0, ct, 0);
        }
        copy("I");
        noifmt();
        words[c]=ilist_[1];
      } else if (tok=='<' && len==1 && 0==memcmp(ibuf, "D", 1)) {
        /* define both /x and x */
        while (ISWSPACE(c,=getcc())) ;
        if (c<32 || c>=WORDSSIZE) erri("invalid %<D letter",0);
        ct[0]=c; ct[1]='\0'; ilist_[0]=ct;
        getval();
        if (!is_wordx(ibuf)) erri("body for %<D must be a single word",0);
        setword(c, ibuf, 1);
      } else if (tok=='<' && len==7 && 0==memcmp(ibuf, "TokSubs", 7)) {
        break;
      } else erri("tag %<TokSubs or %<A or %<D expected",0);
      gettagend();
    }
    words[2]=sbuf; /* Imp: free chars */
  }

  getkey("name"); getval(); setword(2, ibuf, 0);
  if (ibufb-ibuf==1 && words[(unsigned char)ibuf[0]]!=sbuf) erri("abbr used as %<TokSubs name= : ",ibuf);
  getkey("inlining"); inlining=getuintval();
  gettagend();
  { char const* ilist_[2]; ilist_[0]=ilist_[1]=words[2];
    setifmt(inlining!=0 ? "/${mark exch{dup xcheck{dup type/arraytype eq{$}{dup type/nametype eq{dup currentdict exch known{load dup dup type/arraytype eq exch xcheck and{$ aload pop}if}if}if}ifelse}if}forall counttomark array astore exch pop cvx}bind def%</I>"
    :"/$ {\n"
       "mark exch{\n"
         "dup xcheck{\n"
           "dup type/arraytype eq{$}{\n"
             "dup type/nametype eq{\n"
               "dup currentdict exch known {load}if\n"
             "}if\n"
           "}ifelse\n"
         "}if\n"
       "}forall\n"
       "counttomark array astore exch pop cvx\n"
     "} bind def%</I>", ilist_);
    copy("I"); noifmt();
  }

  getotag("Test");
  gettagbeg();
  copy("Test");

  getotag("S");
  getkey("len");  slen=getuintval(); /* Imp: verify slen for overruns */ 
  gettagbeg();
  setifmt("{%</I>",0); copy("I"); noifmt(); /*}*/
  copy("S");

  { char const* ilist_[2]; ilist_[0]=ilist_[1]=words[2];
    setifmt(
      "0\n"
      "{ % Stack: <dst-str> <i>\n"
      "  currentfile read pop\n"
      "  dup 37 eq{% '%'\n"
      "    pop currentfile read pop\n"
      "    dup 37 eq{pop pop exit}if % exit when '%%'\n"
      "  }if\n"
      "  % Stack: <dst-str> <i> <char-read>\n"
      "  dup 42 lt{\n"
      "    30 sub\n"
      "    % Stack: <dst-str> <i> <repeat-count>\n"
      "    dup 0 gt{\n"
      "      3 copy getinterval\n"
      "      % Stack: <dst-str> <i> <repeat-count> <sub-dst-str>\n"
      "      currentfile exch readstring pop pop\n"
      "      % Stack: <dst-str> <i> <repeat-count>\n"
      "      add 1 add\n"
      "    }{pop}ifelse % skip newlines, tabs etc.\n"
      "  }{\n"
      "    3 copy put\n"
      "    pop 2 add % \\0 as space\n"
      "  }ifelse\n"
      "  % Stack: <dst-str> <new-i>\n"
      "}$ bind loop\n"
      "% Stack: <dst-str>\n"
      "cvx exec $ bind exec\n" /* CONST, VAR és CODE futtatása */
      "currentfile token pop pop\n" /* A %%BeginData utáni exec beolvasása */
      "%</I>\n", ilist_);
    copy("I"); noifmt();
  }

  getotag("True");
  gettagbeg();
  copy("True");
  
  setifmt("}{currentfile token pop pop%</I>",0);
  copy("I"); noifmt();

  getotag("False");
  gettagbeg();
  copy("False");

  setifmt("}ifelse%</I>",0);
  copy("I"); noifmt();

  getotag("Defs");
  gettagbeg();
  compress0("Defs");

  { char tok=gettok();
    slen_t len;
    len=ibufb-ibuf;
    if (tok=='<' && len==4 && 0==memcmp(ibuf, "Data", 4)) {
      gettagbeg();
      copydata("Data");
    } else if (tok=='<' && len==4 && 0==memcmp(ibuf, "Fini", 4)) {
      fputs("%%BeginData:\nexec\n`S\n%%EndData\n",stdout);
      getotag("Fini");
      gettagbeg();
      copy("Fini");
      newline();
      fputs("%%Trailer\n%%EOF\n", stdout);
    } else erri("tag %<Data or %<Fini expected",0);
  }
  
  if (gettok()!=0) erri("premature EOF",0);
  
  return 0;
}
