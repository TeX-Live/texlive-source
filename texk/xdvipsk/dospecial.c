/*
 *   This routine handles special commands;
 *   predospecial() is for the prescan, dospecial() for the real thing.
 */
#ifndef XDVIPSK
#include "dvips.h" /* The copyright notice in that file is included too! */
#else
#include "xdvips.h" /* The copyright notice in that file is included too! */
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "glyphmap.h"
#endif /* XDVIPSK */

#ifdef KPATHSEA
#include <kpathsea/c-ctype.h>
#include <kpathsea/tex-hush.h>
#else /* ! KPATHSEA */
#include <ctype.h>
#include <stdlib.h>
#ifndef WIN32
extern int atoi();
extern int system();
#endif /* WIN32*/
#endif
/*
 *   The external declarations:
 */
#include "protos.h"

static int specialerrors = 20;

struct bangspecial {
   struct bangspecial *next;
   char *actualstuff;
} *bangspecials = NULL;

#ifdef XDVIPSK
#ifndef WIN32
char *strlwr(char *str)
{
    char *ptr = str;
    while(*ptr)
    {
        *ptr = tolower(*ptr);
        ptr++;
    }
    return(str);
}
#endif
#endif /* XDVIPSK */
void
specerror(const char *s)
{
   if (specialerrors > 0
#ifdef KPATHSEA
       && !kpse_tex_hush ("special")
#endif
       ) {
      error(s);
      specialerrors--;
   } else if (specialerrors == 0
#ifdef KPATHSEA
	      && !kpse_tex_hush ("special")
#endif
	      ) {
#ifndef XDVIPSK
      error("more errors in special, being ignored . . .");
      error("(perhaps dvips doesn't support your macro package?)");
#else
      writelogrecord(s);
      fprintf(stderr, "%s\n", "more errors in special, being ignored . . .");
      fprintf(stderr, "%s\n", "(perhaps dvips doesn't support your macro package?)");
#endif /* XDVIPSK */
      specialerrors--;
   }
#ifdef XDVIPSK
   else if (specialerrors < 0
#ifdef KPATHSEA
    && !kpse_tex_hush("special")
#endif
     ) {
     writelogrecord(s);
   }
#endif /* XDVIPSK */
}

static void
outputstring(register char *p)
{
   putc('\n', bitfile);
   while(*p) {
      putc(*p, bitfile);
      p++;
   }
   putc('\n', bitfile);
}

static void
trytobreakout(register char *p)
{
   register int i;
   register int instring = 0;
   int lastc = 0;

   i = 0;
   putc('\n', bitfile);

   if(*p == '%') {
      while(*p) {
         putc(*p, bitfile);
         p++;
      }
      putc('\n', bitfile);
      return;
   }

   while (*p) {
      if (i > 65 && *p == ' ' && instring == 0) {
         putc('\n', bitfile);
         i = 0;
      } else {
         putc(*p, bitfile);
         i++;
      }
      if (*p == '(' && lastc != '\\')
         instring = 1;
      else if (*p == ')' && lastc != '\\')
         instring = 0;
      lastc = *p;
      p++;
   }
   putc('\n', bitfile);
}

static void
dobs(register struct bangspecial *q)
{
   if (q) {
      dobs(q->next);
      trytobreakout(q->actualstuff);
   }
}

/* added for dvi2ps & jdvi2kps format */
static char *mfgets(char *buf, unsigned int bytes, FILE *fp);

static void
fgetboundingbox(char *f, float *llx_p, float *lly_p, float *urx_p, float *ury_p)
{
   FILE *fp;
   char buf[BUFSIZ+1];

   fp = search(figpath, f, READ);
   if (fp == 0)
      fp = search(headerpath, f, READ);
   if (fp) {
      while (mfgets(buf, BUFSIZ, fp) != 0) {
         if (buf[0] == '%' && buf[1] == '%'
             && strncmp(buf+2, "BoundingBox:", 12) == 0) {
             if (sscanf(buf+14, "%f %f %f %f", llx_p, lly_p, urx_p, ury_p) == 4) {
                fclose(fp);
                return;
             }
          }
      }
      fclose(fp);
   }
   sprintf(errbuf, "Couldn't get BoundingBox of %.500s: assuming full A4 size", f);
   specerror(errbuf);
   *llx_p = 0.0;
   *lly_p = 0.0;
   *urx_p = 595.0;
   *ury_p = 842.0;
}

static char *
mfgets(char *buf, unsigned int bytes, FILE *fp)
{
   int i, cc;

   for (i = 0; i < bytes; i++) {
      cc = fgetc(fp);
      if (cc == 0x0a || cc == 0x0d) {
         if (cc == 0x0d) {
            cc = fgetc(fp);
            if (cc != 0x0a) {
               ungetc(cc, fp);
            }
         }
         cc = 0x0a;
         buf[i] = cc;
         buf[i+1] = '\0';
         return buf;
      } else if (cc == EOF) {
         buf[i] = '\0';
         if (i == 0) return NULL;
         else return buf;
      } else {
         buf[i] = cc;
      }
   }
   buf[i] = '\0';
   return buf;
}

static void
floatroundout(float f)
{
   integer i;
   i = (integer)(f<0 ? f-0.5 : f+0.5);
   if (i-f < 0.001 && i-f > -0.001) {
      numout((integer)i);
   } else {
      floatout(f);
   }
}
/* end of addition */

void
outbangspecials(void) {
   if (bangspecials) {
      cmdout("TeXDict");
      cmdout("begin");
      cmdout("@defspecial\n");
      dobs(bangspecials);
      cmdout("\n@fedspecial");
      cmdout("end");
   }
}

/* We recommend that new specials be handled by the following general
 * (and extensible) scheme, in which the user specifies one or more
 * `key=value' pairs separated by spaces.
 * The known keys are given in KeyTab; they take values
 * of one of the following types:
 *
 * None: no value, just a keyword (in which case the = sign is omitted)
 * String: the value should be "<string without double-quotes"
 *                          or '<string without single-quotes'
 * Integer: the value should be a decimal integer (%d format)
 * Number: the value should be a decimal integer or real (%f format)
 * Dimension: like Number, but will be multiplied by the scaledsize
 *       of the current font and converted to default PostScript units
 * (Actually, strings are allowed in all cases; the delimiting quotes
 *  are simply stripped off if present.)
 *
 */

typedef enum {None, String, Integer, Number, Dimension} ValTyp;
typedef struct {
   const char    *Entry;
   ValTyp  Type;
} KeyDesc;

#define NKEYS    (sizeof(KeyTab)/sizeof(KeyTab[0]))

#ifdef XDVIPSK
enum
{
  psfile_key,   /* 0 */
  ifffile_key,  /* 1 */
  tekfile_key,  /* 2 */
  hsize_key,    /* 3 */
  vsize_key,    /* 4 */
  hoffset_key,  /* 5 */
  voffset_key,  /* 6 */
  hscale_key,   /* 7 */
  vscale_key,   /* 8 */
  angle_key,    /* 9 */
  llx_key,      /* 10 */
  lly_key,      /* 11 */
  urx_key,      /* 12 */
  ury_key,      /* 13 */
  rwi_key,      /* 14 */
  rhi_key,      /* 15 */
  clip_key,     /* 16 */
  mapline_key,      /* 17 */
  mapfile_key,      /* 18 */
  g2umapline_key,   /* 19 */
  g2umapfile_key,   /* 20 */
  opentype_key,     /* 21 */
  enc_key,          /* 22 */
  nkeys             /* 23 */
};
#endif /* XDVIPSK */

KeyDesc KeyTab[] = {{"psfile",  String}, /* j==0 in the routine below */
                    {"ifffile", String}, /* j==1 */
                    {"tekfile", String}, /* j==2 */
                    {"hsize",   Number},
                    {"vsize",   Number},
                    {"hoffset", Number},
                    {"voffset", Number},
                    {"hscale",  Number},
                    {"vscale",  Number},
                    {"angle",   Number},
                    {"llx", Number},
                    {"lly", Number},
                    {"urx", Number},
                    {"ury", Number},
                    {"rwi", Number},
                    {"rhi", Number},
#ifndef XDVIPSK
                    {"clip", None}};
#else  /* XDVIPSK */
                    {"clip", None},
                    {"mapline", String},
                    {"mapfile", String},
                    {"g2umapline", String},
                    {"g2umapfile", String},
                    {"opentype", String},
                    {"enc", String},
                    };
#endif /* XDVIPSK */

#ifndef KPATHSEA
#define TOLOWER Tolower
#ifdef VMS
#ifndef __GNUC__	/* GNUC tolower is too simple */
#define Tolower tolower
#endif
#else
#ifdef VMCMS    /* IBM: VM/CMS */
#define Tolower __tolower
#else
#ifdef MVSXA    /* IBM: MVS/XA */
#define Tolower __tolower
#else
/*
 * compare strings, ignore case
 */
char
Tolower(register char c)
{
   if ('A' <= c && c <= 'Z')
      return(c+32);
   else
      return(c);
}
#endif
#endif  /* IBM: VM/CMS */
#endif
#endif /* !KPATHSEA */
static int
IsSame(const char *a, const char *b)
{
   for(; *a != '\0'; ) {
      if( TOLOWER(*a) != TOLOWER(*b) )
         return( 0 );
      a++;
      b++;
   }
   return( *b == '\0' );
}

char *KeyStr;       /* Key and ... */
const char *ValStr; /* ... String values found */
long ValInt; /* Integer value found */
float ValNum; /* Number or Dimension value found */

static char *
GetKeyVal(char *str, int *tno) /* returns NULL if none found, else next scan point */
     /* str : starting point for scan */
     /* tno : table entry number of keyword, or -1 if keyword not found */
{
   unsigned char *s;
   register int i;
   register char t;

#ifdef XDVIPSK
   assert(NKEYS == nkeys);

   for (s = str; ((*s <= ' ') || (*s == ',') || (*s == ';')) && *s; s++); /* skip over blanks and key-value delimiters*/
#else
   for (s=str; *s <= ' ' && *s; s++); /* skip over blanks */
#endif /* XDVIPSK */
   if (*s == '\0')
      return (NULL);
   KeyStr = s;
   while (*s>' ' && *s!='=') s++;
   if (0 != (t = *s))
      *s++ = 0;

   for(i=0; i<NKEYS; i++)
      if( IsSame(KeyStr, KeyTab[i].Entry) )
         goto found;
   *tno = -1;
   return (s);

found: *tno = i;
   if (KeyTab[i].Type == None)
      return (s);

   if (t && t <= ' ') {
      for (; *s <= ' ' && *s; s++); /* now look for the value part */
      if ((t = *s)=='=')
         s++;
   }
   ValStr = "";
   if ( t == '=' ) {
      while (*s <= ' ' && *s)
         s++;
      if (*s=='\'' || *s=='\"')
         t = *s++;               /* get string delimiter */
#ifdef XDVIPSK
      else if (*s == '{')
      {
          t = '}';
          while (*(++s) <= ' ');
      }
#endif /* XDVIPSK */
      else t = ' ';
      ValStr = s;
#ifdef XDVIPSK
      while (*s != t && ((t != ' ') || ((*s != ',') && (*s != ';'))) && *s)
#else
      while (*s!=t && *s)
#endif /* XDVIPSK */
         s++;
      if (*s)
         *s++ = 0;
   }
   switch (KeyTab[i].Type) {
 case Integer:
      if(sscanf(ValStr,"%ld",&ValInt)!=1) {
          sprintf(errbuf,"Non-integer value (%.500s) given for keyword %.500s",
              ValStr, KeyStr);
          specerror(errbuf);
          ValInt = 0;
      }
      break;
 case Number:
 case Dimension:
      if(sscanf(ValStr,"%f",&ValNum)!=1) {
          sprintf(errbuf,"Non-numeric value (%.500s) given for keyword %.500s",
              ValStr, KeyStr);
          specerror(errbuf);
          ValNum = 0.0;
      }
      if (KeyTab[i].Type==Dimension) {
         if (curfnt==NULL)
            error("! No font selected");
         ValNum = ValNum * ((double)curfnt->scaledsize) * conv * 72 / DPI;
      }
      break;
 default: break;
   }
   return (s);
}

/*
 *   Now our routines.  We get the number of bytes specified and place them
 *   into the string buffer, and then parse it. Numerous conventions are
 *   supported here for historical reasons.
 *
 *   To support GNUplot's horribly long specials, we go ahead and malloc a
 *   new string buffer if necessary.
 */

void
#ifdef XDVIPSK
predospecial(integer numbytes, Boolean scanning, boolean parse_maps)
#else
predospecial(integer numbytes, Boolean scanning)
#endif /* XDVIPSK */
{
   register char *p = nextstring;
   register int i = 0;
   int j;
   static int omega_specials = 0;
#ifdef XDVIPSK
   size_t ll;
   char *spec_buf, *spec_buf_end, *spec_ptr, *key_buf, *key_ptr, *pp;
   const char *spec_name;
   int ii, jj;
#endif /* XDVIPSK */

   if (numbytes < 0 || numbytes > maxstring - nextstring) {
      if (numbytes < 0 || numbytes > (INT_MAX - 1000) / 2 ) {
         error("! Integer overflow in predospecial");
#ifndef XDVIPSK
         exit(1);
#else
         dvips_exit(1);
#endif /* XDVIPSK */
      }
      p = nextstring = mymalloc(1000 + 2 * numbytes);
      maxstring = nextstring + 2 * numbytes + 700;
   }
   for (i=numbytes; i>0; i--)
#ifdef VMCMS /* IBM: VM/CMS */
      *p++ = ascii2ebcdic[(char)dvibyte()];
#else
#ifdef MVSXA /* IBM: MVS/XA */
      *p++ = ascii2ebcdic[(char)dvibyte()];
#else
      *p++ = (char)dvibyte();
#endif /* IBM: VM/CMS */
#endif
#ifdef XDVIPSK
   if ((!parse_maps) && pprescan)
#else
   if (pprescan)
#endif /* XDVIPSK */
      return;
   while (p[-1] <= ' ' && p > nextstring)
      p--; /* trim trailing blanks */
   if (p==nextstring) return; /* all blank is no-op */
   *p = 0;
   p = nextstring;
   while (*p <= ' ')
      p++;
#ifdef DEBUG
   if (dd(D_SPECIAL))
      fprintf_str(stderr, "Preprocessing special: %s\n", p);
#endif

#ifdef XDVIPSK
   if (!parse_maps)
   {
      ll = run_lua_specials(L, "prescan_specials_callback", p, lua_prescan_specials);
      if (ll == 0) return;
   }
#endif /* XDVIPSK */

/*
 *   We use strncmp() here to also pass things like landscape()
 *   or landscape: or such.
 */

#ifdef XDVIPSK
   if (!parse_maps)
   {
#endif /* XDVIPSK */
   switch (*p) {
case 'o':
   if (strncmp(p, "om:", 3)==0) {       /* Omega specials ignored */
        if (omega_specials==0) {
                fprintf(stderr, "Omega specials are currently ignored\n");
                omega_specials++;
        }
        return;
   }
   break;
case 'l':
   if (strncmp(p, "landscape", 9)==0) {
      if (hpapersize || vpapersize)
         error(
             "both landscape and papersize specified:  ignoring landscape");
      else
         landscape = 1;
      return;
   }
   break;
case 'p':
   if (strncmp(p, "pos:", 4)==0) return; /* positional specials */
   if (strncmp(p, "papersize", 9)==0) {
      p += 9;
      while (*p == '=' || *p == ' ')
         p++;
      if (lastpsizwins || hpapersize == 0 || vpapersize == 0) {
         if (landscape) {
            error(
             "both landscape and papersize specified:  ignoring landscape");
            landscape = 0;
         }
         handlepapersize(p, &hpapersize, &vpapersize);
      }
      return;
   }
   break;
case 'x':
   if (strncmp(p, "xtex:", 5)==0) return;
   break;
case 's':
   if (strncmp(p, "src:", 4)==0) return; /* source specials */
   break;

case 'h':
   if (strncmp(p, "header", 6)==0) {
      char *q, *r, *pre = NULL, *post = NULL;
      p += 6;
      while ((*p <= ' ' || *p == '=' || *p == '(') && *p != 0)
         p++;
      if(*p == '{') {
	 p++;
	 while ((*p <= ' ' || *p == '=' || *p == '(') && *p != 0)
	    p++;
	 q = p;
	 while (*p != '}' && *p != 0)
	    p++;
	 r = p-1;
	 while ((*r <= ' ' || *r == ')') && r >= q)
	    r--;
	 if (*p != 0) p++;
	 r[1] = 0; /* q is the file name */
	 while ((*p <= ' ' || *p == '=' || *p == '(') && *p != 0)
	    p++;
	 if(strncmp(p, "pre", 3) == 0) {
	    int bracecount = 1, num_bytes = 0;
	    while(*p != '{' && *p != 0)
	       p++;
	    if (*p != 0) p++;
	    for(r = p; *r != 0; r++) {
	       if (*r == '{') bracecount++;
	       else if (*r == '}') bracecount--;
	       if (bracecount == 0) break;
	       num_bytes++;
	    }
	    pre = (char *)malloc(num_bytes+1);
	    r = pre;
	    for (j=0; j < num_bytes; j++)
	       *r++ = *p++;
	    *r = 0;
	    if (*p != 0) p++;
	 }
	 while ((*p <= ' ' || *p == '=' || *p == '(') && *p != 0)
	    p++;
	 if(strncmp(p, "post", 4) == 0) {
	    int bracecount = 1, num_bytes = 0;
	    while(*p != '{' && *p != 0)
	       p++;
	    if (*p != 0) p++;
	    for(r = p; *r != 0; r++) {
	       if (*r == '{') bracecount++;
	       else if (*r == '}') bracecount--;
	       if (bracecount == 0) break;
	       num_bytes++;
	    }
	    post = (char *)malloc(num_bytes+1);
	    r = post;
	    for (j=0; j < num_bytes; j++)
	       *r++ = *p++;
	    *r = 0;
	 }
	 if (strlen(q) > 0)
	    add_header_general(q, pre, post);
      } else {
	 q = p;  /* we will remove enclosing parentheses */
	 p = p + strlen(p) - 1;
	 while ((*p <= ' ' || *p == ')') && p >= q)
	    p--;
	 p[1] = 0;
	 if (p >= q)
	    add_header(q);
      }
   }
   break;
/* IBM: color - added section here for color header and color history */
/* using strncmp in this fashion isn't perfect; if later someone wants
   to introduce a verb like colorspace, well, just checking
   strcmp(p, "color", 5) will not do.  But we leave it alone for the moment.
   --tgr */
case 'b':
   if (strncmp(p, "background", 10) == 0) {
      usescolor = 1;
      p += 10;
      while ( *p && *p <= ' ' ) p++;
      background(p);
      return;
   }
   break;
case 'c':
   if (strncmp(p, "color", 5) == 0) {
      usescolor = 1;
      p += 5;
      while ( *p && *p <= ' ' ) p++;
      if (strncmp(p, "push", 4) == 0 ) {
         p += 4;
         while ( *p && *p <= ' ' ) p++;
         pushcolor(p, 0);
      } else if (strncmp(p, "pop", 3) == 0 ) {
         popcolor(0);
      } else {
         resetcolorstack(p,0);
      }
   }   /* IBM: color - end changes */
   break;
case '!':
   {
      register struct bangspecial *q;
      p++;
      q = (struct bangspecial *)mymalloc((integer)(sizeof(struct bangspecial)));
      q->actualstuff = mymalloc(strlen(p) + 1);
      strcpy(q->actualstuff, p);
      q->next = bangspecials;
      bangspecials = q;
      usesspecial = 1;
      return;
   }
   break;
default:
   break;
   }
#ifdef XDVIPSK
   }
   else /* if (!parse_maps) */
   {
   switch (*p) {
case 'm':
   /* "mapline=MTMI MinionPro-It \" scottm-mn-oml ReEncodeFont \" <[scottm-mn-oml.enc <MinionPro-It.pfb " */
   /* "mapline: [lmroman10-regular]:+tlig;  LMRoman10-Regular \"LMRoman10-Regular\" >$SELFAUTOPARENT/texmf-dist/fonts/opentype/public/lm/lmroman10-regular.otf" */
   /* "mapline: [De-Gruyter-Sans-Regular-v105.otf]:mode=node;script=latn;language=dflt;+tlig;  De-Gruyter-Sans-Regular-v105 \"De Gruyter Sans Regular v105\" >$SELFAUTOGRANDPARENT/vtex-dist/fonts/opentype/google/noto-dg-v105/De-Gruyter-Sans-Regular-v105.otf" */
    spec_name = KeyTab[mapline_key].Entry;
    ll = strlen(spec_name);
    if (strncmp(p, spec_name, ll) == 0)
    {
         char *specinfo, *downloadinfo;
         char downbuf[500];
         char specbuf[500];
         char *specval, *spec_ptr;
         int slen;
         char *TeXname = NULL;
         char *PSname = NULL;
         char *Fontfile = NULL;
         char *Vectfile = NULL;
         char *hdr_name = NULL;
         boolean nopartial_p = false;
         boolean encoding_p = false;
         boolean repl = 1;
         boolean inside_quotes = FALSE;

         p += ll;
         if ((*p == ':') || (*p == '='))
            p++;
         if ((*p == ' ') || (*p == '\t'))
            p++;
         if (*p == '+') {
            repl = 0;
            p++;
         }
         specval = strdup(p);
         assert(specval);
         strlwr(specval);
         if (strstr(specval, ".ttf") ||
             strstr(specval, ".ttc") ||
             strstr(specval, ".otf") ||
             strstr(specval, ".dfont"))
         {
             spec_ptr = specval;
             inside_quotes = FALSE;
             while (*p)
             {
                 if (*p == '\"')
                     inside_quotes = !inside_quotes;
                 else
                 {
                     if ((!inside_quotes) && (*p == ' '))
                         *spec_ptr++ = '\t';
                     else
                         *spec_ptr++ = *p;
                 }
                 p++;
             }
             *spec_ptr = '\0';
             parse_otf_map_line(specval);
         }
         else
         {
            specinfo = NULL;
            downloadinfo = NULL;
            downbuf[0] = 0;
            specbuf[0] = 0;
            while (*p) {
               encoding_p = false;
               while (*p && *p <= ' ')
                  p++;
               if (*p) {
                  if (*p == '"') {             /* PostScript instructions? */
                     if (specinfo) {
                        strcat(specbuf, specinfo);
                        strcat(specbuf, " ");
                     }
                     specinfo = p + 1;

                  } else if (*p == '<') {    /* Header to download? */
                     /* If had previous downloadinfo, save it.  */
                     if (downloadinfo) {
                        strcat(downbuf, downloadinfo);
                        strcat(downbuf, " ");
                        downloadinfo = NULL;
                     }
                     if (p[1] == '<') {     /* << means always full download */
                       p++;
                       nopartial_p = true;
                     } else if (p[1] == '[') { /* <[ means an encoding */
                       p++;
                       encoding_p = true;
                     }
                     p++;
                     /* skip whitespace after < */
                     while (*p && *p <= ' ')
                       p++;
                        /* save start of header name */
                     hdr_name = p;
                   } else if (TeXname) /* second regular word on line? */
                     PSname = p;

                   else                /* first regular word? */
                     TeXname = p;

                   if (*p == '"') {
                        p++;            /* find end of "..." word */
                        while (*p != '"' && *p)
                           p++;
                     } else
                        while (*p > ' ') /* find end of anything else */
                           p++;
                     if (*p)
                        *p++ = 0;

                     /* If we had a header we were downloading, figure
                        out what to do; couldn't do this above since we
                        want to check the suffix.  */
                     if (hdr_name) {
                        const char *suffix = find_suffix (hdr_name);
                        if (encoding_p || STREQ (suffix, "enc")) {
                           /* (SPQR) if it is a reencoding, pass on to
                              FontPart, and download as usual */
                           Vectfile = downloadinfo = hdr_name;
                        } else if (nopartial_p) {
                           downloadinfo = hdr_name;
                        } else if (FILESTRCASEEQ (suffix, "pfa")
                                || FILESTRCASEEQ (suffix, "pfb")
                                || STREQ (suffix, "PFA")
                                || STREQ (suffix, "PFB")) {
                                Fontfile = hdr_name;
                        } else {
                           downloadinfo = hdr_name;
                        }
                    hdr_name = NULL;
                    }
                }
            }
            if (specinfo)
               strcat(specbuf, specinfo);
            if (downloadinfo)
               strcat(downbuf, downloadinfo);
            slen = strlen(downbuf) - 1;
            if (slen > 0 && downbuf[slen] == ' ') {
               downbuf[slen] = 0;
            }
            nextstring = p + 1;
            if (TeXname) {
               TeXname = newstring(TeXname);
               PSname = newstring(PSname);
               Vectfile = newstring(Vectfile);
               Fontfile = newstring(Fontfile);
               specinfo = newstring(specbuf);
               downloadinfo = newstring(downbuf);
               add_entry_spec(TeXname, PSname, Fontfile, Vectfile,
                  specinfo, downloadinfo, !nopartial_p, 0, 0, repl);
            }
         }
         free(specval);
   }
   else
   {
       spec_name = KeyTab[mapfile_key].Entry;
       ll = strlen(spec_name);
       if (strncmp(p, spec_name, ll) == 0)
       {
           boolean repl = 1;
           p += ll;
           if ((*p == ':') || (*p == '='))
               p++;
           if ((*p == ' ') || (*p == '\t'))
               p++;
           if (*p == '+') {
               repl = 0;
               p++;
           }
           getpsinfo_spec(p, repl);
       }
   }
   break;
case 'g':
   /* "g2umapline=Acircle,24B6" */
   /* "g2umapline=FFIsmall,0066 0066 0069" */
   spec_name = KeyTab[g2umapline_key].Entry;
   ll = strlen(spec_name);
   if (strncmp(p, spec_name, ll) == 0)
   {
       static char map_line[ENC_BUF_SIZE + 1];

       p += ll;
       if ((*p == ':') || (*p == '='))
           p++;
       if ((*p == ' ') || (*p == '\t'))
           p++;

       strcpy(map_line, "load_touni({['");

       spec_ptr = spec_buf = strdup(p);
       assert(spec_buf);
       spec_buf_end = spec_buf + strlen(spec_buf);
       spec_ptr = strtok(spec_ptr, ",");
       if (spec_ptr)
       {
           strncat(map_line, spec_ptr, ENC_BUF_SIZE - strlen(map_line) - 20);
           map_line[ENC_BUF_SIZE] = '\0';
           pp = map_line + strlen(map_line);
           while (*(--pp) == ' ')
               *pp = '\0'; /* strip trailing spaces */
           strcat(map_line, "'] = {");

           spec_ptr += strlen(spec_ptr) + 1;
           while (spec_ptr && (spec_ptr <= spec_buf_end))
           {
               spec_ptr = strtok(spec_ptr, " ");
               if (spec_ptr)
               {
                   strcat(map_line, "0x");
                   strncat(map_line, spec_ptr, ENC_BUF_SIZE - strlen(map_line) - 20);
                   map_line[ENC_BUF_SIZE] = '\0';
                   strcat(map_line, ", ");
                   spec_ptr += strlen(spec_ptr) + 1;
                   if (spec_ptr >= spec_buf_end)
                       break;
               }
           }
           strcat(map_line, "}}, true, \'\')");
           if (luaL_dostring(L, map_line))
           {
               PRINTF_PR("Error: Lua script %.500s execution error: %.500s.\n", map_line, lua_tostring(L, -1));
               lua_pop(L, 1);
           }
       }
       else
       {
           PRINTF_PR("Error: special %s format not recognized: %.500s\n", spec_name, p);
       }
       free(spec_buf);
   }
   else
   {
       /* "g2umapfile=texglyphlist.lua" */
       /* "g2umapfile=pfb:lmmi8/lmmi8.lua" */
       spec_name = KeyTab[g2umapfile_key].Entry;
       ll = strlen(spec_name);
       if (strncmp(p, spec_name, ll) == 0)
       {
           char* pfb_name;
           p += ll;
           if ((*p == ':') || (*p == '='))
               p++;
           if ((*p == ' ') || (*p == '\t'))
               p++;
           pfb_name = strchr(p, ':');
           if (pfb_name)
           {
               if (strncmp(p, "pfb:", 4))
               {
                   PRINTF_PR("Error: special %s prefix format not recognized: %.500s\n", spec_name, p);
               }
               pfb_name++;
               p = pfb_name;
               while (*p && (*p != '/'))
                   p++;
               if (*p)
                   *p++ = '\0';
           }
           load_touni_file(p, TRUE, pfb_name, 2);
       }
   }
   break;
case 'v':
   spec_ptr = spec_buf = strdup(p);
   assert(spec_buf);
   spec_ptr = strtok(spec_ptr, ":");
   if (spec_ptr && (strcmp(spec_ptr, "vtex") == 0))
   {
       spec_ptr += strlen(spec_ptr) + 1;
       spec_ptr = strtok(spec_ptr, ".");
       if (spec_ptr && (strcmp(spec_ptr, "settings") == 0))
       {
           spec_ptr += strlen(spec_ptr) + 1;
           spec_ptr = strtok(spec_ptr, ".");
           if (spec_ptr && (strcmp(spec_ptr, "xdvipsk") == 0))
           {
               spec_ptr += strlen(spec_ptr) + 1;

               key_ptr = GetKeyVal(spec_ptr, &ii);
               if (key_ptr)
               {
                   key_ptr = key_buf = strdup((ii >= 0) ? ValStr : "");
                   assert(key_buf);
                   switch (ii)
                   {
                   /* "vtex:settings.xdvipsk.opentype={enc=gid,...}" */
                   case opentype_key:
                       spec_name = KeyTab[ii].Entry;
                       while ((key_ptr = GetKeyVal(key_ptr, &jj)) != NULL)
                           switch (jj)
                           {
                           case enc_key:
                               if (strcmp(ValStr, "gid") == 0)
                               {
                                   Otf_Enc_Type = enc_gid;
                                   noToUnicode = TRUE;
                               }
                               else
                               {
                                   PRINTF_PR("Unknown keyword `%s' value `%.500s' in \\special{vtex:settings.xdvipsk.%s={...}} will be ignored\n", KeyStr, ValStr, spec_name);
                               }
                               break;
                           default:
                               PRINTF_PR("Unknown keyword `%.500s' in \\special{vtex:settings.xdvipsk.%s={...}} will be ignored\n", KeyStr, spec_name);
                               break;
                           }
                       break;
                   default:
                       PRINTF_PR("Unknown keyword `%.500s' in \\special{vtex:settings.xdvipsk...} will be ignored\n", KeyStr);
                       break;
                   }
                   free(key_buf);
               }
               else
               {
                   PRINTF_PR("%s", "No keyword in \\special{vtex:settings.xdvipsk...}\n");
               }
           }
           else
               spec_ptr = NULL;
       }
       else
           spec_ptr = NULL;
   }
   else
       spec_ptr = NULL;
   free(spec_buf);
   break;
default:
   break;
   }
   }

   p = spec_buf = strdup(p); /* GetKey() alters the string */
   assert(spec_buf);
#endif /* XDVIPSK */
   usesspecial = 1;  /* now the special prolog will be sent */
#ifndef XDVIPSK
   if (scanning && *p != '"' && (p=GetKeyVal(p, &j)) != NULL && j==0
#else  /* XDVIPSK */
   if (scanning && *p != '"' && (p=GetKeyVal(p, &j)) != NULL && j == psfile_key
#endif /* XDVIPSK */
       && *ValStr != '`') /* Don't bother to scan compressed files.  */
      scanfontcomments(ValStr);
#ifdef XDVIPSK
   free(spec_buf);
#endif /* XDVIPSK */
}

/* Return 1 if S is readable along figpath, 0 if not. */
static int
maccess(char *s)
{
   FILE *f = search(figpath, s, "r");
   int found = (f != 0);
   if (found)
      (*close_file) (f);
   return found;
}

const char *tasks[] = { 0, "iff2ps", "tek2ps" };

#define PSFILESIZ 511
static char psfile[PSFILESIZ];

void
dospecial(integer numbytes)
{
   register char *p = nextstring;
   register int i = 0;
   int j, systemtype = 0;
   register const char *q;
   Boolean psfilewanted = 1;
   const char *task = 0;
   char cmdbuf[111];
#ifdef XDVIPSK
   size_t l;
#endif /* XDVIPSK */
#ifdef HPS
if (HPS_FLAG && PAGEUS_INTERUPPTUS) {
     HREF_COUNT--;
     start_new_box();
     PAGEUS_INTERUPPTUS = 0;
     }
if (HPS_FLAG && NEED_NEW_BOX) {
       vertical_in_hps();
       NEED_NEW_BOX = 0;
       }
#endif
   if (nextstring + numbytes > maxstring)
      error("! out of string space in dospecial");
   for (i=numbytes; i>0; i--)
#ifdef VMCMS /* IBM: VM/CMS */
      *p++ = ascii2ebcdic[(char)dvibyte()];
#else
#ifdef MVSXA /* IBM: MVS/XA */
      *p++ = ascii2ebcdic[(char)dvibyte()];
#else
      *p++ = (char)dvibyte();
#endif  /* IBM: VM/CMS */
#endif
   while (p[-1] <= ' ' && p > nextstring)
      p--; /* trim trailing blanks */
   if (p==nextstring) return; /* all blank is no-op */
   *p = 0;
   p = nextstring;
   while (*p <= ' ')
      p++;
#ifdef DEBUG
   if (dd(D_SPECIAL))
      fprintf_str(stderr, "Processing special: %s\n", p);
#endif
#ifdef XDVIPSK
   l = run_lua_specials(L, "scan_specials_callback", p, lua_scan_specials);
   if (l == 0) return;

   if (VTEX_SPEC_MODE) {
	   if ((strncmp(p, "mt:", 3) == 0) || (strncmp(p, "vtex:", 5) == 0) ||
		   (strncmp(p, "MC:", 3) == 0) || (strncmp(p, "BMC:", 4) == 0) ||
		   (strncmp(p, "EMC:", 4) == 0)) {								/* VTeX private specials ignored */
		   return;
	   }
   }
#endif /* XDVIPSK */
   switch (*p) {
case 'o':
   if (strncmp(p, "om:", 3)==0) {       /* Omega specials ignored */
        return;
   }
   break;
case 'e':
   if (strncmp(p, "em:", 3)==0) {	/* emTeX specials in emspecial.c */
	emspecial(p);
	return;
   }

/* added for dvi2ps special */
   if (strncmp(p, "epsfile=", 8)==0) {  /* epsf.sty for dvi2ps-j */
      float llx, lly, urx, ury;
      unsigned psfilelen = 0;

      p += 8;
      while (*p && !isspace((unsigned char)*p)) {
        if (psfilelen < PSFILESIZ - 1) {
          psfile[psfilelen] = *p;
          psfilelen++;
          p++;
        } else {
          psfile[psfilelen] = 0; /* should not strictly be necessary */
          sprintf(errbuf,
                  "! epsfile=%.20s... argument longer than %d characters",
                  psfile, PSFILESIZ);
          error(errbuf);
        }
      }
      if (psfilelen == 0) {
        error ("! epsfile= argument empty");
      }
      psfile[psfilelen] = 0;
      p += strlen(psfile);
      fgetboundingbox(psfile, &llx, &lly, &urx, &ury);
      hvpos();
      cmdout("@beginspecial");
      floatroundout(llx);
      cmdout("@llx");
      floatroundout(lly);
      cmdout("@lly");
      floatroundout(urx);
      cmdout("@urx");
      floatroundout(ury);
      cmdout("@ury");

      while ((p = GetKeyVal(p, &j))) {
         switch (j) {
#ifndef XDVIPSK
            case 3: /* hsize */
#else  /* XDVIPSK */
            case hsize_key:
#endif /* XDVIPSK */
               floatroundout(ValNum*10);
               cmdout("@rwi");
               break;
#ifndef XDVIPSK
            case 4: /* vsize */
#else  /* XDVIPSK */
            case vsize_key:
#endif /* XDVIPSK */
               floatroundout(ValNum*10);
               cmdout("@rhi");
               break;
#ifndef XDVIPSK
            case 7: /* hscale */
#else  /* XDVIPSK */
            case hscale_key:
#endif /* XDVIPSK */
               floatroundout((urx-llx)*ValNum*10);
               cmdout("@rwi");
               break;
#ifndef XDVIPSK
            case 8: /* vscale */
#else  /* XDVIPSK */
            case vscale_key:
#endif /* XDVIPSK */
               floatroundout((ury-lly)*ValNum*10);
               cmdout("@rhi");
               break;
            default:
               sprintf(errbuf, "Unknown keyword `%.500s' in \\special{epsfile=%.500s...} will be ignored\n",
               KeyStr, psfile);
               specerror(errbuf);
               break;
         }
      }
      cmdout("@setspecial");
      numout((integer)0);
      cmdout("lly");
      cmdout("ury");
      cmdout("sub");
      cmdout("TR");
      figcopyfile(psfile, 0);
      cmdout("\n@endspecial");
      return;
   }
/* end addition */
   break;
case 'p':
   if (strncmp(p, "pos:", 4)==0) return; /* positional specials */
   if (strncmp(p, "ps:", 3)==0) {
        psflush(); /* now anything can happen. */
        if (p[3]==':') {
           if (strncmp(p+4, "[nobreak]", 9) == 0) {
              hvpos();
              outputstring(&p[13]);
           } else if (strncmp(p+4, "[begin]", 7) == 0) {
              hvpos();
              trytobreakout(&p[11]);
           } else if (strncmp(p+4, "[end]", 5) == 0)
              trytobreakout(&p[9]);
           else trytobreakout(&p[4]);
        } else if (strncmp(p+3, " plotfile ", 10) == 0) {
             char *sfp;
             hvpos();
             p += 13;
           /*
            *  Fixed to allow popen input for plotfile
            *  TJD 10/20/91
            */
           while (*p == ' ') p++;
           if (*p == '"') {
             p++;
             for (sfp = p; *sfp && *sfp != '"'; sfp++);
           } else {
             for (sfp = p; *sfp && *sfp != ' '; sfp++);
           }
           *sfp = '\0';
           if (*p == '`')
             figcopyfile(p+1, 1);
           else
             figcopyfile (p, 0);
           /* End TJD changes */
        } else {
           hvpos();
           trytobreakout(&p[3]);
           psflush();
           hvpos();
        }
        return;
   }
   if (strncmp(p, "papersize", 9) == 0)
      return;
#ifdef TPIC
   if (strncmp(p, "pn ", 3) == 0) {setPenSize(p+2); return;}
   if (strncmp(p, "pa ", 3) == 0) {addPath(p+2); return;}
#endif

/* added for jdvi2kps special */
   if (strncmp(p, "postscriptbox", 13)==0) { /* epsbox.sty for jdvi2kps */
      float w, h;
      float llx, lly, urx, ury;
      if (strlen(p)-13-6 >= PSFILESIZ) { /* -6 for the braces */
         /* We're not allowing as long a name as we could, since however
            many characters the two {%fpt} arguments consume is not
            taken into account. But parsing it all so we know the
            character length is too much work for this obscure special. */
         sprintf(errbuf,
                 "! postscriptbox{} arguments longer than %d characters",
                 PSFILESIZ);
         error(errbuf);
      }
      if (sscanf(p+13, "{%fpt}{%fpt}{%[^}]}", &w, &h, psfile) != 3)
         break;
      fgetboundingbox(psfile, &llx, &lly, &urx, &ury);
      hvpos();
      cmdout("@beginspecial");
      floatroundout(llx);
      cmdout("@llx");
      floatroundout(lly);
      cmdout("@lly");
      floatroundout(urx);
      cmdout("@urx");
      floatroundout(ury);
      cmdout("@ury");
      floatroundout(w*10*72/72.27);
      cmdout("@rwi");
      floatroundout(h*10*72/72.27);
      cmdout("@rhi");
      cmdout("@setspecial");
      figcopyfile(psfile, 0);
      cmdout("\n@endspecial");
      return;
   }
/* end addition */
   break;
case 'l':
   if (strncmp(p, "landscape", 9)==0) return;
   break;
case '!':
   return;
case 'h':
   if (strncmp(p, "header", 6)==0) return;
#ifdef HPS
   if (strncmp(p, "html:", 5)==0) {
      if (! HPS_FLAG) return;
      p += 5;
      while (isspace((unsigned char)*p))
         p++;
      if (*p == '<') {
         char *sp = p;
         char *str;
         int  ii=0, len, lower_len;

         while ((*p) && (*p != '>')) {
            ii++;
            p++;
         }
         str = (char *)mymalloc(ii+2);
         strncpy(str,sp+1,ii-1);
         str[ii-1] = 0;len=strlen(str);
         if(len>6) lower_len=6; else lower_len=len;
         for(ii=0;ii<lower_len;ii++) str[ii]=tolower((unsigned char)str[ii]);
         do_html(str);
         free(str);
      } else
#ifdef KPATHSEA
         if (!kpse_tex_hush ("special"))
#endif
      {

         sprintf(errbuf,"Error in html special\n");
         error(errbuf);
         return;
      }
      return;
   }
#else
   if (strncmp(p, "html:", 5)==0) return;
#endif
case 'w':
case 'W':
   if (strncmp(p+1, "arning", 6) == 0) {
      static int maxwarns = 50;
      if (maxwarns > 0) {
         error(p);
         maxwarns--;
      } else if (maxwarns == 0) {
         error(". . . rest of warnings suppressed");
         maxwarns--;
      }
      return;
   }
#ifdef TPIC
   if (strcmp(p, "wh") == 0) {whitenLast(); return;}
#endif
   break;
case 'b':
   if ( strncmp(p, "background", 10) == 0 )
      return; /* already handled in prescan */
#ifdef TPIC
   if (strcmp(p, "bk") == 0) {blackenLast(); return;}
#endif
   break;
case 'c':
   if (strncmp(p, "color", 5) == 0) {
      p += 5;
      while ( *p && *p <= ' ' ) p++;
      if (strncmp(p, "push", 4) == 0 ) {
         p += 4;
         while ( *p && *p <= ' ' ) p++;
         pushcolor(p,1);
      } else if (strncmp(p, "pop", 3) == 0 ) {
         popcolor(1);
      } else {
         resetcolorstack(p,1);
      }
      return;
   } /* IBM: color - end changes*/
   break;
case 'f':
#ifdef TPIC
   if (strcmp(p, "fp") == 0) {flushPath(0); return;}
#endif
   break;
case 'i':
#ifdef TPIC
   if (strcmp(p, "ip") == 0) {flushPath(1); return;} /* tpic 2.0 */
   if (strncmp(p, "ia ", 3) == 0) {arc(p+2, 1); return;} /* tpic 2.0 */
#endif
   break;
case 'd':
#ifdef TPIC
   if (strncmp(p, "da ", 3) == 0) {flushDashed(p+2, 0); return;}
   if (strncmp(p, "dt ", 3) == 0) {flushDashed(p+2, 1); return;}
#endif
   break;
case 's':
   if (strncmp(p, "src:", 4) == 0) return; /* source specials */
#ifdef TPIC
   if (strcmp(p, "sp") == 0) {flushSpline(p+2); return;} /* tpic 2.0 */
   if (strncmp(p, "sp ", 3) == 0) {flushSpline(p+3); return;} /* tpic 2.0 */
   if (strcmp(p, "sh") == 0) {shadeLast(p+2); return;} /* tpic 2.0 */
   if (strncmp(p, "sh ", 3) == 0) {shadeLast(p+3); return;} /* tpic 2.0 */
#endif
   break;
case 'a':
#ifdef TPIC
   if (strncmp(p, "ar ", 3) == 0) {arc(p+2, 0); return;} /* tpic 2.0 */
#endif
   break;
case 't':
#ifdef TPIC
   if (strncmp(p, "tx ", 3) == 0) {SetShade(p+3); return;}
#endif
   break;
case '"':
   hvpos();
   cmdout("@beginspecial");
   cmdout("@setspecial");
   trytobreakout(p+1);
   cmdout("\n@endspecial");
   return;
   break;
#ifdef XDVIPSK
case 'm':
   if (strncmp(p, "mapline", 7)==0) return;
   else if (strncmp(p, "mapfile", 7)==0) return;
   break;
#endif /* XDVIPSK */
default:
   break;
   }
/* At last we get to the key/value conventions */
   psfile[0] = '\0';
   hvpos();
   cmdout("@beginspecial");

   while( (p=GetKeyVal(p,&j)) != NULL )
      switch (j) {
 case -1: /* for compatibility with old conventions, we allow a file name
           * to be given without the 'psfile=' keyword */
         if (!psfile[0] && maccess(KeyStr)==1) { /* yes we can read it */
             if (strlen(KeyStr) >= PSFILESIZ) {
               sprintf(errbuf, 
           "! Bare filename (%.20s...) in \\special longer than %d characters",
                       KeyStr, PSFILESIZ);
             }
             strcpy(psfile,KeyStr);
         } else {
           if (strlen(KeyStr) < 40) {
              sprintf(errbuf,
                      "Unknown keyword (%s) in \\special will be ignored",
                              KeyStr);
           } else {
              sprintf(errbuf,
                     "Unknown keyword (%.40s...) in \\special will be ignored",
                              KeyStr);
           }
           specerror(errbuf);
#ifdef XDVIPSK
         cmdout("@endspecial");
         return;
#endif /* XDVIPSK */
         }
         break;
#ifndef XDVIPSK
 case 0: case 1: case 2: /* psfile */
#else  /* XDVIPSK */
 case psfile_key: case ifffile_key: case tekfile_key:
#endif /* XDVIPSK */
         if (psfile[0]) {
           sprintf(errbuf, "More than one \\special %s given; %.40s ignored",
                    "psfile", ValStr);
           specerror(errbuf);
#ifdef XDVIPSK
           cmdout("@endspecial");
           return;
#endif /* XDVIPSK */
         } else {
           if (strlen(ValStr) >= PSFILESIZ) {
               sprintf(errbuf, 
           "! PS filename (%.20s...) in \\special longer than %d characters",
                       ValStr, PSFILESIZ);
	       error(errbuf);
           }
           strcpy(psfile, ValStr);
         }
         task = tasks[j];
         break;
#ifdef XDVIPSK
 case mapline_key:
 case mapfile_key:
 case g2umapline_key:
 case g2umapfile_key:
 case opentype_key:
 case enc_key:
         cmdout("@endspecial");
         return;
         break;
#endif /* XDVIPSK */
 default: /* most keywords are output as PostScript procedure calls */
         if (KeyTab[j].Type == Integer)
            numout((integer)ValInt);
         else if (KeyTab[j].Type == String)
            for (q=ValStr; *q; q++)
               scout(*q);
         else if (KeyTab[j].Type == None);
         else { /* Number or Dimension */
            ValInt = (integer)(ValNum<0? ValNum-0.5 : ValNum+0.5);
            if (ValInt-ValNum < 0.001 && ValInt-ValNum > -0.001)
                numout((integer)ValInt);
            else {
               snprintf(cmdbuf, sizeof(cmdbuf), "%f", ValNum);
               cmdout(cmdbuf);
            }
         }
      snprintf(cmdbuf, sizeof(cmdbuf), "@%s", KeyStr);
      cmdout(cmdbuf);
      }

   cmdout("@setspecial");

   if(psfile[0]) {
      if (task == 0) {
         systemtype = (psfile[0]=='`');
         figcopyfile(psfile+systemtype, systemtype);
      } else {
#ifndef XDVIPSK
         fprintf (stderr, "dvips: iff2ps and tek2ps are not supported.\n");
#else
         error("dvips: iff2ps and tek2ps are not supported.");
#endif /* XDVIPSK */
      }
   } else if (psfilewanted
#ifdef KPATHSEA
	      && !kpse_tex_hush ("special")
#endif
	      )
      specerror("No \\special psfile was given; figure will be blank");

   cmdout("@endspecial");
}

/*
 *   Handles specials encountered during bounding box calculations.
 *   Currently we only deal with psfile's or PSfiles and only those
 *   that do not use rotations.
 */
static float rbbox[4];
float *
bbdospecial(int nbytes)
{
   char *p = nextstring;
   int i, j;
   char seen[NKEYS];
   float valseen[NKEYS];

   if (nbytes < 0 || nbytes > maxstring - nextstring) {
      if (nbytes < 0 || nbytes > (INT_MAX - 1000) / 2 ) {
         error("! Integer overflow in bbdospecial");
#ifndef XDVIPSK
         exit(1);
#else
         dvips_exit(1);
#endif /* XDVIPSK */
      }
      p = nextstring = mymalloc(1000 + 2 * nbytes);
      maxstring = nextstring + 2 * nbytes + 700;
   }
   if (nextstring + nbytes > maxstring)
      error("! out of string space in bbdospecial");
   for (i=nbytes; i>0; i--)
#ifdef VMCMS /* IBM: VM/CMS */
      *p++ = ascii2ebcdic[(char)dvibyte()];
#else
#ifdef MVSXA /* IBM: MVS/XA */
      *p++ = ascii2ebcdic[(char)dvibyte()];
#else
      *p++ = (char)dvibyte();
#endif  /* IBM: VM/CMS */
#endif
   while (p[-1] <= ' ' && p > nextstring)
      p--; /* trim trailing blanks */
   if (p==nextstring) return NULL; /* all blank is no-op */
   *p = 0;
   p = nextstring;
   while (*p && *p <= ' ')
      p++;
   if (strncmp(p, "psfile", 6)==0 || strncmp(p, "PSfile", 6)==0) {
      float originx = 0, originy = 0, hscale = 1, vscale = 1,
            hsize = 0, vsize = 0;
      for (j=0; j<NKEYS; j++)
         seen[j] = 0;
      j = 0;
      while ((p=GetKeyVal(p, &j))) {
         if (j >= 0 && j < NKEYS && KeyTab[j].Type == Number) {
	    seen[j]++;
	    valseen[j] = ValNum;
         }
      }
      /*
       *   This code mimics what happens in @setspecial.
       */
#ifndef XDVIPSK
      if (seen[3])
         hsize = valseen[3];
      if (seen[4])
         vsize = valseen[4];
      if (seen[5])
         originx = valseen[5];
      if (seen[6])
         originy = valseen[6];
      if (seen[7])
         hscale = valseen[7] / 100.0;
      if (seen[8])
         vscale = valseen[8] / 100.0;
      if (seen[10] && seen[12])
         hsize = valseen[12] - valseen[10];
      if (seen[11] && seen[13])
         vsize = valseen[13] - valseen[11];
      if (seen[14] || seen[15]) {
         if (seen[14] && seen[15] == 0) {
	    hscale = vscale = valseen[14] / (10.0 * hsize);
         } else if (seen[15] && seen[14] == 0) {
	    hscale = vscale = valseen[15] / (10.0 * vsize);
         } else {
            hscale = valseen[14] / (10.0 * hsize);
            vscale = valseen[15] / (10.0 * vsize);
         }
#else  /* XDVIPSK */
      if (seen[hsize_key])
         hsize = valseen[hsize_key];
      if (seen[vsize_key])
         vsize = valseen[vsize_key];
      if (seen[hoffset_key])
         originx = valseen[hoffset_key];
      if (seen[voffset_key])
         originy = valseen[voffset_key];
      if (seen[hscale_key])
         hscale = valseen[hscale_key] / 100.0;
      if (seen[vscale_key])
         vscale = valseen[vscale_key] / 100.0;
      if (seen[llx_key] && seen[urx_key])
         hsize = valseen[urx_key] - valseen[llx_key];
      if (seen[lly_key] && seen[ury_key])
         vsize = valseen[ury_key] - valseen[lly_key];
      if (seen[rwi_key] || seen[rhi_key]) {
         if (seen[rwi_key] && seen[rhi_key] == 0) {
	    hscale = vscale = valseen[rwi_key] / (10.0 * hsize);
         } else if (seen[rhi_key] && seen[rwi_key] == 0) {
	    hscale = vscale = valseen[rhi_key] / (10.0 * vsize);
         } else {
            hscale = valseen[rwi_key] / (10.0 * hsize);
            vscale = valseen[rhi_key] / (10.0 * vsize);
         }
#endif /* XDVIPSK */
      }
      /* at this point, the bounding box in PostScript units relative to
         the current dvi point is
           originx originy originx+hsize*hscale originy+vsize*vscale
         We'll let the bbox routine handle the remaining math.
       */
      rbbox[0] = originx;
      rbbox[1] = originy;
      rbbox[2] = originx+hsize*hscale;
      rbbox[3] = originy+vsize*vscale;
      return rbbox;
   }
   return 0;
}
