/* This code was stolen from dvips for xdvi use.  The file itself did
   not carry any licensing info.  According to other filed the code
   either public domain, or, in the worst case, GPL. 

   - Nicolai Langfeldt, 21/2/1999

*/

#define HAVE_BOOLEAN 1

#include "xdvi-config.h"

#if T1

#include <kpathsea/tex-file.h>

/* Adapted from resident.c */

#define INLINE_SIZE (2000)
static char was_inline[INLINE_SIZE] ;

extern void add_T1_mapentry P4H(char *, char *, char *, char *);

/* Read a dvips psfont map file.  Return 1 if it was read ok.  0
   otherwise. */

int getpsinfo P1C(char *, name)

{
  FILE *deffile ;
  char *p ;
  char *specinfo;
  char specbuf[500] ;
  char *fullname;

  /* Try first for xdvi specific file */
  fullname=kpse_find_file(name, kpse_program_text_format, 0);
  /* Then any fontmap file */
  if (fullname==NULL) 
    fullname=kpse_find_file(name, kpse_fontmap_format, 0);

  if (fullname==NULL)
    return 0;

  deffile=fopen(fullname,"r");
  if (deffile==NULL) return 0;

  fprintf(stderr,"Reading dvips map %s\n",fullname);

  while (fgets(was_inline, INLINE_SIZE, deffile)!=NULL) {
    char *TeXname = NULL ;
    char *PSname = NULL ;
    char *Fontfile = NULL;
    char *Vectfile = NULL;
    char *hdr_name = NULL;
    boolean encoding_p = false;
    specinfo = NULL ;
    specbuf[0] = 0 ;

    /* One line at a time */
    p = was_inline ;

    if (*p < ' ' || *p == '*' || *p == '#' || *p == ';' || *p == '%') 
      continue ;

    while (*p) {
      while (*p && *p <= ' ')
	p++ ;
      if (*p) {
	if (*p == '"') {             /* PostScript instructions? */
	  if (specinfo) {
	    strcat(specbuf, specinfo) ;
	    strcat(specbuf, " ") ;
	  }
	  specinfo = p + 1 ;

	} else if (*p == '<') {    /* Header to download? */
	  if (p[1] == '<') {     /* << means always full download, ignore */
	    p++;
	  } else if (p[1] == '[') { /* <[ means an encoding */
	    p++;
	    encoding_p = true;
	  }
	  p++ ;
	  /* skip whitespace after < */
	  while (*p && *p <= ' ')
	    p++;
                     
	  /* save start of header name */
	  hdr_name = p ;

	} else if (TeXname) /* second regular word on line? */
	  PSname = p ;

	else                /* first regular word? */
	  TeXname = p ;

	if (*p == '"') {
	  p++ ;            /* find end of "..." word */
	  while (*p != '"' && *p)
	    p++ ;
	} else
	  while (*p > ' ') /* find end of anything else */
	    p++ ;

	/* Terminate the substring we just identified */
	if (*p)
	  *p++ = 0 ;

	/* If we had a header we were downloading, figure out what to
	   do; couldn't do this above since we want to check the
	   suffix.  */
	if (hdr_name) {
	  char *suffix = find_suffix (hdr_name);
	  if (encoding_p || (suffix!=NULL && strcmp(suffix, "enc")==0)) {
	    Vectfile = hdr_name;
	  } else {
	    Fontfile = hdr_name;
	  }
	}
      }
    } /* while (*p) */

    /* Line finished.  Lookit what we got */
    if (specinfo)
      strcat(specbuf, specinfo) ;

    if (TeXname)
      add_T1_mapentry(TeXname, Fontfile, Vectfile, specinfo) ;

  }
  (void)fclose(deffile) ;
  return 1;
}

#endif
