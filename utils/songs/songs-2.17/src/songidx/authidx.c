/* Copyright (C) 2013 Kevin W. Hamlen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 *
 * The latest version of this program can be obtained from
 * http://songs.sourceforge.net.
 */


/* Create a LaTeX author index file from an author .dat file. */

#if HAVE_CONFIG_H
#  include "config.h"
#else
#  include "vsconfig.h"
#endif

#include "chars.h"
#include "songidx.h"
#include "fileio.h"

#if HAVE_STRING_H
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <strings.h>
#endif

/* Word-separators are spaces. */
#define auth_wsep(x) wc_isspace((x))

/* Entry-separators are commas or semicolons. */
#define auth_esep(x) (((x)==wc_comma) || ((x)==wc_semicolon))

/* Everything else is considered part of a word---it is not dropped when
 * migrating author text to the author index. */
#define auth_sep(x) (auth_wsep(x) || auth_esep(x))

typedef struct wordlist
{
  struct wordlist *next;
  WCHAR *w;
}
WORDLIST;

WORDLIST wl_and = { NULL, ws_lit("and") };
WORDLIST wl_by = { NULL, ws_lit("by") };
WORDLIST wl_unknown = { NULL, ws_lit("unknown") };

/* wl_insert(<wordlist>,<word>)
 *  Insert a word onto the head of a wordlist. */
static void
wl_insert(wl,w)
  WORDLIST **wl;
  const WCHAR *w;
{
  WORDLIST *head = (WORDLIST *) malloc(sizeof(WORDLIST));
  head->next = ((*wl==&wl_and) || (*wl==&wl_by) || (*wl==&wl_unknown)) ?
               NULL : *wl;
  head->w = (WCHAR *) calloc(ws_strlen(w)+1, sizeof(WCHAR));
  ws_strcpy(head->w, w);
  *wl = head;
}

/* matchany(<string>,<wordlist>)
 *  If a word in <wordlist> case-insensitively matches to a prefix of <string>,
 *  and if the match concludes with whitespace, then return the length of the
 *  match plus 1.  Otherwise return 0.
 */
static size_t
matchany(s,wl)
  const WCHAR *s;
  WORDLIST *wl;
{
  const WCHAR *s2, *w2;

  for (; wl; wl=wl->next)
  {
    for (s2=s, w2=wl->w; *w2; ++s2, ++w2)
      if (wc_tolower(*s2)!=wc_tolower(*w2)) break;
    if ((*w2 == wc_null) && auth_wsep(*s2)) return s2-s+1;
  }
  return 0;
}

/* matchwithin(<string>,<wordlist>)
 *  Return 1 if any word in <wordlist> appears in <string> (case insensitive
 *  match). */
static char
matchwithin(s,wl)
  const WCHAR *s;
  WORDLIST *wl;
{
  const WCHAR *s2, *w2;
  WORDLIST *wl2;
  int spc;

  spc = 1;
  while (*s!=wc_null)
  {
    if (spc)
    {
      for (wl2=wl; wl2; wl2=wl2->next)
      {
        for (s2=s, w2=wl->w; *w2!=wc_null; ++s2, ++w2)
          if (wc_tolower(*s2)!=wc_tolower(*w2)) break;
        if ((*w2==wc_null) && ((*s2==wc_null) || auth_sep(*s2))) return 1;
      }
    }
    spc = auth_wsep(*s);
    s2 = s;
    skipesc(&s,1,0);
    if (s==s2) ++s;
  }
  return 0;
}

/* issuffix(<string>)
 *   Return 1 if <string> points to the name suffix "Jr" or a roman numeral
 *   consisting solely of I's, V's, and/or X's.  Return 0 otherwise. */
static int
issuffix(s)
  const WCHAR *s;
{
  WCHAR buf[3];
  size_t n;

  n = ws_strspn(s, ws_lit("IVX"));
  if (n>0)
    s += n;
  else if ((*s==wc_null) || (*(s+1)==wc_null))
    return 0;
  else
  {
    buf[0] = *s++;
    buf[1] = *s++;
    buf[2] = wc_null;
    if (ws_coll(buf, ws_lit("Jr"))) return 0;
  }
  return (auth_sep(*s) || (*s==wc_period) || (*s==wc_null));
}

/* grabauthor(<string>,<seplist>,<afterlist>,<ignorelist>)
 *   Return a string of the form "Sirname, Restofname" denoting the full name
 *   of the first author found in <string>; or return NULL if no author name
 *   can be found.  Set <string> to point to the suffix of <string> that was
 *   not parsed.  The string returned (if any) is static, so successive calls
 *   will overwrite it.
 *
 *   Heuristics:
 *     * Names are separated by punctuation (other than hyphens, periods,
 *       apostrophes, or backslashes) or by the word "and" (or whatever words
 *       are in seplist).
 *       Special case: If a comma is followed by the abbreviation "Jr" or by a
 *       roman numeral, then the comma does NOT end the author's name.
 *     * If a name contains the word "by" (or anything in afterlist), then
 *       everything before it is not considered part of the name.  (Let's hope
 *       nobody is named "By".)
 *     * The author's last name is always the last capitalized word in the
 *       name unless the last capitalized word is "Jr." or a roman numeral.
 *       In that case the author's last name is the second-last capitalized
 *       word.
 *     * If an author appears to have only a first name, or if the last name
 *       found according to the above heuristics is an abbreviation (ending in
 *       a period), look ahead in <string> until we find someone with a last
 *       name and use that one.  This allows us to identify the first author in
 *       a string like "Joe, Billy E., and Bob Smith" to be "Joe Smith".
 *     * If the resultant name contains the word "unknown" (or any word in
 *       unknownlist), it's probably not a real name.  Recursively attempt to
 *       parse the next author. */
static WCHAR *
grabauthor(authline,seplist,afterlist,unknownlist)
  const WCHAR **authline;
  WORDLIST *seplist;
  WORDLIST *afterlist;
  WORDLIST *unknownlist;
{
  static WCHAR buf[MAXLINELEN+2], *bp;
  const WCHAR *first, *last, *suffix, *next, *scanahead, *endp;
  size_t len;

  /* Point "first" to the first character of the name, "last" to the first
   * character of the sirname, "suffix" to any suffix like "Jr." or "III"
   * (or NULL if there is none), and "next" to the first character beyond the
   * end of the name. */
  if (!authline) return NULL;
  for (first=*authline; (*first!=wc_null) && auth_sep(*first); ++first) ;
  if (((len = matchany(first,seplist)) > 0) ||
      ((len = matchany(first,afterlist)) > 0))
    first += len;
  if (*first==wc_null) return NULL;
  last=suffix=NULL;
  for (next=first; *next; ++next)
  {
    skipesc(&next,0,1);
    if (*next==wc_comma)
    {
      for (scanahead = next+1; auth_wsep(*scanahead); ++scanahead) ;
      if (issuffix(scanahead)) continue;
      *authline = next+1;
      break;
    }
    else if (*next==wc_semicolon)
    {
      *authline = next+1;
      break;
    }
    if (auth_wsep(*next))
    {
      for (++next; auth_wsep(*next); ++next) ;
      if ((len = matchany(next,seplist)) > 0)
      {
        *authline=next+len;
        break;
      }
      if ((len = matchany(next,afterlist)) > 0)
      {
        first = next+len;
        next += len-1;
        last = suffix = NULL;
        continue;
      }
      if (issuffix(next))
        suffix=next;
      else
      {
        scanahead = next;
        skipesc(&scanahead,0,0);
        if (wc_isupper(*scanahead))
        {
          last = next;
          suffix = NULL;
        }
      }
      --next;
    }
  }
  if (*next==wc_null) *authline = next;

  /* Put the sirname into the buffer first. */
  endp = NULL;
  if (last)
  {
    endp = (suffix ? suffix : next)-1;
    if (endp<last) abort();
    for (; (endp>last) && auth_sep(*endp); --endp) ;
  }
  if (!endp || (*endp==wc_period))
  {
    /* Here's where it gets tough.  We either have a single-word name, or the
     * last name ends in a "." which means maybe it's just a middle initial or
     * other abbreviation.  We could be dealing with a line like, "Billy,
     * Joe E., and Bob Smith", in which case we have to go searching for the
     * real last name. To handle this case, we will try a recursive call. */
    scanahead = *authline;
    if (grabauthor(&scanahead,seplist,afterlist,unknownlist) &&
        ((bp = ws_strchr(buf, wc_comma)) != NULL))
       /* got it! Make our old last name part of the first name. */
      last = NULL;
    else if (last)
    {
      /* Couldn't find a last name. Just use this one and hope it's okay. */
      ws_strncpy(buf, last, endp-last+1);
      bp = buf+(endp-last+1);
    }
    else
      bp = buf;
  }
  else
  {
    ws_strncpy(buf, last, endp-last+1);
    bp = buf+(endp-last+1);
  }

  /* Next, put the first name into the buffer. */
  endp = (last ? last : (suffix ? suffix : next))-1;
  if (endp<first) abort();
  for (; (endp>=first) && auth_sep(*endp); --endp) ;
  ++endp;
  if (endp>first)
  {
    if (bp > buf)
    {
      *bp++ = wc_comma;
      *bp++ = wc_space;
    }
    ws_strncpy(bp, first, endp-first);
    bp += endp-first;
  }

  /* Finally, put the suffix (if any) into the buffer. */
  if (suffix)
  {
    for (endp=next-1; (endp>=suffix) && auth_sep(*endp); --endp) ;
    ++endp;
    if (endp>suffix)
    {
      if (bp > buf) *bp++ = wc_space;
      ws_strncpy(bp, suffix, endp-suffix);
      bp += endp-suffix;
    }
  }

  *bp = wc_null;
  if (matchwithin(buf, unknownlist))
    return grabauthor(authline,seplist,afterlist,unknownlist);
  return buf;
}

/* genauthorindex(<file>,<inname>,<outname>)
 *   Read author data from file handle <file> and generate from it a LaTeX
 *   author index file named <outname>.
 *   Return 0 on success, 1 on warnings, or 2 on error. */
int
genauthorindex(fs,outname)
  FSTATE *fs;
  const char *outname;
{
  FILE *f;
  int eof = 0;
  int arraysize, numauthors, i;
  WORDLIST *seplist=&wl_and, *afterlist=&wl_by, *ignorelist=&wl_unknown;
  SONGENTRY **authors;
  WCHAR authorbuf[MAXLINELEN];
  WCHAR songnumbuf[MAXLINELEN], linknamebuf[MAXLINELEN];
  WCHAR *thisnum, *thislink;
  const WCHAR *auth, *bp;

  fprintf(stderr, "songidx: Parsing author index data file %s...\n",
          fs->filename);

  eof = 0;
  authors = NULL;
  for (arraysize=numauthors=i=0; !eof; ++i)
  {
    if (!filereadln(fs,authorbuf,&eof)) return 2;
    if (eof) break;
    if (authorbuf[0] == wc_percent)
    {
      if (!ws_strncmp(authorbuf, ws_lit("%sep "), 5))
        wl_insert(&seplist, authorbuf + 5);
      else if (!ws_strncmp(authorbuf, ws_lit("%after "), 7))
        wl_insert(&afterlist, authorbuf + 7);
      else if (!ws_strncmp(authorbuf, ws_lit("%ignore "), 8))
        wl_insert(&ignorelist, authorbuf + 8);
      --i;
      continue;
    }
    if (!filereadln(fs,songnumbuf,&eof))
    {
      fileclose(fs);
      return 2;
    }
    if (eof)
    {
      fprintf(stderr, "songidx:%s:%d: incomplete author entry"
                      " (orphan byline)\n", fs->filename, fs->lineno);
      fileclose(fs);
      return 2;
    }
    if (!filereadln(fs,linknamebuf,&eof))
    {
      fileclose(fs);
      return 2;
    }
    if (eof)
    {
      fprintf(stderr, "songidx:%s:%d: incomplete author entry"
                      " (missing hyperlink)\n", fs->filename, fs->lineno);
      fileclose(fs);
      return 2;
    }
    if (((thisnum = (WCHAR *) calloc(ws_strlen(songnumbuf)+1,
                                     sizeof(WCHAR))) == NULL) ||
        ((thislink = (WCHAR *) calloc(ws_strlen(linknamebuf)+1,
                                      sizeof(WCHAR))) == NULL))
    {
      fprintf(stderr, "songidx:%s:%d: song number/link too long"
                      " (out of memory)\n", fs->filename, fs->lineno);
      return 2;
    }
    ws_strcpy(thisnum, songnumbuf);
    ws_strcpy(thislink, linknamebuf);

    for (bp=authorbuf;
         (auth=grabauthor(&bp, seplist, afterlist,
                          ignorelist)) != NULL;
         ++numauthors)
    {
      if (numauthors >= arraysize)
      {
        SONGENTRY **temp;
        arraysize *= 2;
        if (arraysize==0) arraysize=64;
        temp = (SONGENTRY **) realloc(authors,arraysize*sizeof(SONGENTRY *));
        if (!temp)
        {
          fprintf(stderr, "songidx:%s:%d: too many song authors"
                          " (out of memory)\n", fs->filename, fs->lineno);
          return 2;
        }
        authors = temp;
      }
      if ((authors[numauthors] = (SONGENTRY *)
                                 malloc(sizeof(SONGENTRY))) == NULL)
      {
        fprintf(stderr, "songidx:%s:%d: too many song authors"
                        " (out of memory)\n", fs->filename, fs->lineno);
        return 2;
      }
      if ((authors[numauthors]->title =
           (WCHAR *) calloc(ws_strlen(auth)+1, sizeof(WCHAR))) == NULL)
      {
        fprintf(stderr, "songidx:%s:%d: author name too long (out of memory)\n",
                fs->filename, fs->lineno);
        return 2;
      }
      ws_strcpy(authors[numauthors]->title, auth);
      authors[numauthors]->num = thisnum;
      authors[numauthors]->linkname = thislink;
      authors[numauthors]->idx = i;
    }
  }
  fileclose(fs);

  /* Sort the array by author. */
  qsort(authors, numauthors, sizeof(*authors), songcmp);

  /* Generate an author index LaTeX file from the sorted data.
   * Combine any entries with the same author name into a single index entry. */
  fprintf(stderr, "songidx: Generating author index TeX file %s...\n", outname);
  if (strcmp(outname,"-"))
  {
    if ((f = fopen(outname, "w")) == NULL)
    {
      fprintf(stderr, "songidx: Unable to open %s for writing.\n", outname);
      return 2;
    }
  }
  else
  {
    f = stdout;
    outname = "stdout";
  }

#define TRYWRITE(x) \
  if (!(x)) \
  { \
    fprintf(stderr, "songidx:%s: write error\n", outname); \
    if (f == stdout) fflush(f); else fclose(f); \
    return 2; \
  }

  TRYWRITE(ws_fputs(ws_lit("\\begin{idxblock}{"), f) >= 0)
  for (i=0; i<numauthors; ++i)
  {
    if ((i>0) && !ws_coll(authors[i]->title, authors[i-1]->title))
    {
      TRYWRITE(ws_fputs(ws_lit("\\\\"), f) >= 0)
    }
    else
    {
      TRYWRITE((ws_fputs(ws_lit("}\n\\idxentry{"), f) >= 0) &&
               (ws_fputs(authors[i]->title, f) >= 0) &&
               (ws_fputs(ws_lit("}{"), f) >= 0))
    }
    TRYWRITE((ws_fputs(ws_lit("\\songlink{"), f) >= 0) &&
             (ws_fputs(authors[i]->linkname, f) >= 0) &&
             (ws_fputs(ws_lit("}{"), f) >= 0) &&
             (ws_fputs(authors[i]->num, f) >= 0) &&
             (ws_fputs(ws_lit("}"), f) >= 0))
  }
  TRYWRITE(ws_fputs(ws_lit("}\n\\end{idxblock}\n"), f) >= 0)

#undef TRYWRITE

  if (f == stdout)
    fflush(f);
  else
    fclose(f);
  return 0;
}
