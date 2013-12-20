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


/* Create a LaTeX scripture index file from a scripture index .dat file. */

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
#else
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif
#endif

/* The following constant should be a large integer (but not greater than
 * INT_MAX) limiting the maximum number of verses in a single chapter of the
 * bible. */
#define MAXVERSES 999

/* A BBOOK struct describes a book of the Bible, including:
 *   name: a \0-separated string of names (the first one is the name used when
 *         printing the scripture index)
 *   verses: an array of numbers giving the number of verses in each chapter
 *   aliases: the number of names in the name field
 *   chapters: the number of chapters in the book */
typedef struct
{
  WCHAR *name;
  int *verses;
  int aliases;
  int chapters;
}
BBOOK;

/* A VERSE struct refers to a verse in a book of the Bible.
 *   chapter: the integral chapter number of the verse (1 if the book has only
 *            one chapter)
 *   verse: the integral verse number */
typedef struct
{
  int chapter, verse;
}
VERSE;

/* A SONGLIST is a list of songs
 *   num: a string denoting the song number (e.g. "3" or "H10")
 *   link: a string denoting the hyperlink label for the song
 *   int: an integer key used to order songs when sorting
 *   next: next pointer */
typedef struct songliststruct
{
  WCHAR *num;
  WCHAR *link;
  int key;
  struct songliststruct *next;
}
SONGLIST;

/* A VERSELIST is the list of verses in a particular book of the Bible at which
 * the set of songs referencing that verse differs from the set of songs
 * referencing the previous verse in the book.  Such a list allows us to
 * sparsely represent the set of songs referencing each verse in the book
 * without having to create a distinct list entry for each verse in the entire
 * book.  For example, if song "H1" references verses 3:1-10, that would
 * represented by a two-element list:
 *      [ {v={chapter=3, verse=1}, adds=[H1], drops=[]};
 *        {v={chapter=3, verse=10}, adds=[], drops=[H1]} ]
 * This is more reasonable than creating ten list items to refer to such a
 * range.
 *   v: the verse at which the set of song references is changing
 *   adds: a list of songs that refer to this verse but not the previous one
 *   drops: a list of songs that refer to this verse but not the next one
 *   next: pointer to next verse at which the set of references changes */
typedef struct verseliststruct
{
  VERSE v;
  SONGLIST *adds, *drops;
  struct verseliststruct *next;
}
VERSELIST;

/* The bible global variable stores the array of bible books being used to
 * generate the scripture index.  The numbooks variable stores the number of
 * elements in the bible array. */
BBOOK *bible = NULL;
int numbooks = 0;

/* free_bible()
 *   Free the bible array, set it to NULL, and set numbooks to 0. */
static void
free_bible()
{
  int i;

  if (!bible) return;
  for (i=0; i<numbooks; ++i)
  {
    if (bible[i].name) free(bible[i].name);
    if (bible[i].verses) free(bible[i].verses);
  }
  free(bible);
  bible = NULL;
  numbooks = 0;
}

/* free_songlist(<slist>)
 *   Free song list <slist>. */
static void
free_songlist(sl)
  SONGLIST *sl;
{
  SONGLIST *next;

  for (; sl; sl=next)
  {
    if (sl->num) free(sl->num);
    if (sl->link) free(sl->link);
    next = sl->next;
    free(sl);
  }
}

/* free_verselist(<vlist>)
 *    Free verse list <vlist>. */
static void
free_verselist(vl)
  VERSELIST *vl;
{
  VERSELIST *next;

  for (; vl; vl=next)
  {
    free_songlist(vl->adds);
    free_songlist(vl->drops);
    next = vl->next;
    free(vl);
  }
}

/* freeidxarray(<array>,<len>)
 *   Free the array of verselists given by <array>.  The array should have
 *   length <len>. */
static void
freeidxarray(idx,len)
  VERSELIST **idx;
  int len;
{
  int i;

  if (!idx) return;
  for (i=0; i<len; ++i) free_verselist(idx[i]);
  free(idx);
}

/* readbible(<filename>)
 *   Read bible data file <filename> into the bible array and set numbooks to
 *   the number of books read.  Return 0 on error or 1 on success. */
static int
readbible(filename)
  const char *filename;
{
  FSTATE fs;
  int eof = 0;
  WCHAR buf[MAXLINELEN];
  int verses[MAXLINELEN/2+1];
  int arraysize, j;
  WCHAR *p1, *p2;

  if (!fileopen(&fs,filename)) return 0;

  bible = NULL;
  eof = 0;
  for (arraysize=numbooks=0; !eof; ++numbooks)
  {
    if (!filereadln(&fs,buf,&eof))
    {
      free_bible();
      return 0;
    }
    if (eof) break;
    if (*buf == wc_hash)
    {
      --numbooks;
      continue;
    }
    if (numbooks >= arraysize)
    {
      BBOOK *temp;
      arraysize *= 2;
      if (arraysize==0) arraysize=64;
      temp = (BBOOK *) realloc(bible,arraysize*sizeof(BBOOK));
      if (!temp)
      {
        fprintf(stderr, "songidx:%s:%d: too many bible books (out of memory)\n",
                fs.filename, fs.lineno);
        free_bible();
        return 0;
      }
      bible = temp;
    }
    if ((bible[numbooks].name =
         (WCHAR *) calloc(ws_strlen(buf)+1,sizeof(WCHAR))) == NULL)
    {
      fprintf(stderr, "songidx:%s:%d: bible book name too large"
                      " (out of memory)\n", fs.filename, fs.lineno);
      fileclose(&fs);
      free_bible();
      return 0;
    }
    for(j=1, p1=buf, p2=bible[numbooks].name; *p1!=wc_null; ++p1, ++p2)
    {
      if (*p1 == wc_pipe)
      {
        ++j;
        *p2=wc_null;
      }
      else *p2=*p1;
    }
    *p2 = wc_null;
    if (j == 0)
    {
      fprintf(stderr, "songidx:%s:%d: bible book has no name\n",
              fs.filename, fs.lineno);
      free(bible[numbooks].name);
      fileclose(&fs);
      free_bible();
      return 0;
    }
    bible[numbooks].aliases = j;
    do
    {
      if (!filereadln(&fs,buf,&eof))
      {
        free_bible();
        return 0;
      }
      if (eof)
      {
        fprintf(stderr, "songidx:%s:%d: incomplete bible book entry (book"
                        " title with no verses)\n", fs.filename, fs.lineno);
        free_bible();
        return 0;
      }
    }
    while (*buf == wc_hash);
    for (j=0, p1=buf; *p1!=wc_null; ++j)
    {
      long v = ws_strtol(p1, &p1, 10);
      if ((v<=0) || (v>MAXVERSES))
      {
        fprintf(stderr, "songidx:%s:%d: illegal verse count for chapter %d\n",
                fs.filename, fs.lineno, j+1);
        fileclose(&fs);
        free_bible();
        return 0;
      }
      verses[j] = (int) v;
    }
    if (j == 0)
    {
      fprintf(stderr, "songidx:%s:%d: book ", fs.filename, fs.lineno);
      ws_fputs(bible[numbooks].name, stderr);
      fprintf(stderr, " has no chapters\n");
      fileclose(&fs);
      free_bible();
      return 0;
    }
    bible[numbooks].chapters = j;
    if ((bible[numbooks].verses = (int *) calloc(j, sizeof(int))) == NULL)
    {
      fprintf(stderr, "songidx:%s:%d: too many verses (out of memory)\n",
              fs.filename, fs.lineno);
      fileclose(&fs);
      free_bible();
      return 0;
    }
    for (j=0; j<bible[numbooks].chapters; ++j)
      bible[numbooks].verses[j] = verses[j];
  }

  fileclose(&fs);
  return 1;
}

/* ws_stricmp(<string1>,<string2>)
 *   Case-insensitive, wide-string comparison function.  (Some standard C libs
 *   already have one of these, but not all do; for compatibility I have to
 *   write my own.) */
static int
ws_stricmp(p1,p2)
  const WCHAR *p1;
  const WCHAR *p2;
{
  for (;;)
  {
    int diff = wc_tolower(*p1) - wc_tolower(*p2);
    if (diff != 0) return diff;
    if (*p1 == wc_null) return 0;
    ++p1; ++p2;
  }
}

/* parseref(<string>,<book>,<verse>)
 *   Interpret <string> as a scripture reference, and store the NUMBER of the
 *   book of the Bible it refers to in <book>, and the chapter and verse that
 *   it refers to in <verse>.  Return a pointer into <string> to the suffix of
 *   <string> that was not parsed.  The <book> and <verse> should hold the book
 *   and verse of the PREVIOUS scripture reference (if any) on entry to the
 *   function.  If book or chapter information is missing, they will be drawn
 *   from this info.  That way, successive calls can correctly parse a run-on
 *   string like "Philippians 3:1,5; 4:3", inferring that "5" refers to
 *   "Philippians 3" and "4:3" refers to "Philippians".  If the parser
 *   encounters an error in processing the book name (e.g., a book name was
 *   specified but not recognized), <book> is set to numbooks.  If no chapter
 *   or no verse is provided (e.g., the reference is just "Philippians" or
 *   "Philippians 3") then the chapter and/or verse fields of <verse> are set
 *   to -1. */
static WCHAR *
parseref(r,book,v)
  WCHAR *r;
  int *book;
  VERSE *v;
{
  WCHAR *p;
  WCHAR c;
  int i;
  const WCHAR *a;

  while (*r==wc_space) ++r;
  p = ws_strpbrk(r, ws_lit(",;-"));
  if (!p) p=r+ws_strlen(r);
  if (p>r) --p;
  while ((p>r) && (*p==wc_space)) --p;
  while ((p>r) && (wc_isdigit(*p) || (*p==wc_colon))) --p;
  if (p>r)
  {
    if (*p==wc_space)
    {
      *p++ = wc_null;
      for (*book=0; *book<numbooks; ++(*book))
      {
        for (i=0, a=bible[*book].name;
             i<bible[*book].aliases;
             ++i, a+=ws_strlen(a)+1)
          if (!ws_stricmp(r,a)) break;
        if (i<bible[*book].aliases) break;
      }
    }
    else
    {
      *book=numbooks;
    }
    r=p;
    v->chapter = v->verse = -1;
  }
  while (wc_isdigit(*p)) ++p;
  if (*p==wc_colon)
  {
    *p++ = wc_null;
    v->chapter = ws_strtol(r,NULL,10);
    r=p;
    while (wc_isdigit(*p)) ++p;
  }
  if (r==p) return NULL;
  c = *p;
  *p = wc_null;
  if ((v->chapter < 0) && (*book>=0) && (*book<numbooks) &&
      (bible[*book].chapters == 1))
  {
    /* Special case: This book only has one chapter. */
    v->chapter = 1;
  }
  if (v->chapter < 0)
    v->chapter = ws_strtol(r,NULL,10);
  else
    v->verse = ws_strtol(r,NULL,10);
  *p = c;
  while (*p==wc_space) ++p;
  if (*p && (*p!=wc_comma) && (*p!=wc_semicolon) && (*p!=wc_hyphen))
    return NULL;
  return p;
}

/* vlt(<v1>,<v2>)
 *   Return 1 if verse <v1> precedes <v2> and 0 otherwise. */
static int
vlt(v1,v2)
  const VERSE v1;
  const VERSE v2;
{
  return ((v1.chapter == v2.chapter) ? (v1.verse < v2.verse)
                                     : (v1.chapter < v2.chapter));
}

/* vcpy(<vdest>,<vsrc>)
 *   Copy the data from verse <vsrc> into verse <vdest>. */
static void
vcpy(vdest,vsrc)
  VERSE *vdest;
  const VERSE vsrc;
{
  vdest->chapter = vsrc.chapter;
  vdest->verse = vsrc.verse;
}

/* getvcount(<book>,<chapter>)
 *   Return the number of verses in chapter <chapter> of book number <book> of
 *   the Bible. */
static int
getvcount(book,chapter)
  int book;
  int chapter;
{
  if ((book<0) || (book>=numbooks))
  {
    /* This should never happen. */
    fprintf(stderr, "songidx: Internal error (getvcount):"
                    " Book %d out of range.\n", book);
    return 0;
  }
  if ((chapter<1) || (chapter > bible[book].chapters))
    return 0;
  return bible[book].verses[chapter-1];
}

/* vinc(<book>,<verse>)
 *   Return a reference to the verse immediately following <verse> in book
 *   number <book> of the Bible.  The verse returned is static, so successive
 *   calls will overwrite it.  If <verse> is the last verse in <book>, the
 *   return value will reference verse 1 of a non-existant chapter. */
static VERSE
vinc(book,v)
  int book;
  const VERSE v;
{
  static VERSE vret;
  int cnt;

  vret.chapter = v.chapter;
  vret.verse = v.verse + 1;
  cnt = getvcount(book, vret.chapter);
  if (cnt <= 0)
  {
    /* This should never happen. */
    fprintf(stderr, "songidx: Internal Error (vinc): Book ");
    ws_fputs(bible[book].name, stderr);
    fprintf(stderr, " has no chapter %d.\n", vret.chapter);
  }
  else if (vret.verse > cnt)
  {
    ++vret.chapter;
    vret.verse = 1;
  }
  return vret;
}

/* vdec(<book>,<verse>)
 *   Return a reference to the verse immediately preceding <verse> in book
 *   number <book> of the Bible.  The verse returned is static, so successive
 *   calls will overwrite it.  If <verse> is the first verse in <book>, the
 *   return value will reference chapter -1 and an error message will be
 *   printed to stderr. */
static VERSE
vdec(book,v)
  int book;
  const VERSE v;
{
  static VERSE vret;
  vret.chapter = v.chapter;
  vret.verse = v.verse - 1;
  if (vret.verse < 1)
  {
    --vret.chapter;
    vret.verse = getvcount(book, vret.chapter);
    if (vret.verse <= 0)
    {
      /* This should never happen. */
      fprintf(stderr, "songidx: Internal Error (vdec): Book ");
      ws_fputs(bible[book].name, stderr);
      fprintf(stderr, " has no chapter %d.\n", vret.chapter);
      vret.verse = -1;
    }
  }
  return vret;
}

/* addref(<reflist>,<song>,<link>,<key>)
 *   Add a reference to song (<song>,<link>,<key>) to REFLIST <reflist>.  The
 *   <reflist> is assumed to be sorted by <key>.  If a reference with the same
 *   <key> already exists, <reflist> is left unchanged.  Both <song> and
 *   <link> are COPIED into the list.  Return 1 on success or 0 on failure. */
static int
addref(sl,n,l,key)
  SONGLIST **sl;
  const WCHAR *n;
  const WCHAR *l;
  int key;
{
  SONGLIST *newsong;

  for (; *sl; sl=&((*sl)->next))
    if ((*sl)->key >= key) break;
  if (*sl && ((*sl)->key == key)) return 1;

  if ((newsong = (SONGLIST *) malloc(sizeof(SONGLIST))) == NULL)
  {
    fprintf(stderr, "songidx: too many scripture references (out of memory)\n");
    return 0;
  }
  newsong->num = newsong->link = NULL;
  if (((newsong->num = (WCHAR *) calloc(ws_strlen(n)+1,
                                        sizeof(WCHAR))) == NULL) ||
      ((newsong->link = (WCHAR *) calloc(ws_strlen(l)+1,
                                         sizeof(WCHAR))) == NULL))
  {
    fprintf(stderr, "songidx: too many scripture references (out of memory)\n");
    if (newsong->num) free(newsong->num);
    if (newsong->link) free(newsong->link);
    free(newsong);
    return 0;
  }
  ws_strcpy(newsong->num, n);
  ws_strcpy(newsong->link, l);
  newsong->key = key;
  newsong->next = *sl;
  *sl = newsong;
  return 1;
}

/* insertref(<type>,<verselist>,<verse>,<song>,<link>,<key>)
 *   If <type> is INSERT_ADD, then insert song (<song>,<link>,<key>) into the
 *   set of "adds" for verse <verse> in <verselist>.  If <type> is INSERT_DROP,
 *   then insert the song into the set of "drops" for verse <verse>.  In either
 *   case, create a new entry for <verse> in <verselist> if it doesn't already
 *   exist.  Return 1 on success or 0 on failure. */
#define INSERT_ADD 1
#define INSERT_DROP 0

static int
insertref(type,vpp,v,n,l,key)
  char type;
  VERSELIST **vpp;
  const VERSE v;
  const WCHAR *n;
  const WCHAR *l;
  int key;
{
  for (;;)
  {
    if (!*vpp || vlt(v, (*vpp)->v))
    {
      VERSELIST *vnew = NULL;
      SONGLIST *sl = NULL;
      if (((vnew = (VERSELIST *) malloc(sizeof(VERSELIST))) == NULL) ||
          ((sl = (SONGLIST *) malloc(sizeof(SONGLIST))) == NULL))
      {
        fprintf(stderr, "songidx: too many scripture references"
                        " (out of memory)\n");
        if (vnew) free(vnew);
        if (sl) free(sl);
        return 0;
      }
      sl->num = sl->link = NULL;
      if (((sl->num = (WCHAR *) calloc(ws_strlen(n)+1,
                                       sizeof(WCHAR))) == NULL) ||
          ((sl->link = (WCHAR *) calloc(ws_strlen(l)+1,
                                        sizeof(WCHAR))) == NULL))
      {
        fprintf(stderr, "songidx: too many scripture references"
                        " (out of memory)\n");
        if (sl->num) free(sl->num);
        if (sl->link) free(sl->link);
        free(vnew);
        free(sl);
        return 0;
      }
      ws_strcpy(sl->num, n);
      ws_strcpy(sl->link, l);
      sl->key = key;
      sl->next = NULL;
      vcpy(&(vnew->v), v);
      if (type)
      {
        vnew->adds = sl;
        vnew->drops = NULL;
      }
      else
      {
        vnew->adds = NULL;
        vnew->drops = sl;
      }
      vnew->next = *vpp;
      *vpp = vnew;
      return 1;
      }
    else if (!vlt((*vpp)->v, v))
    {
      return addref(type ? &((*vpp)->adds) : &((*vpp)->drops), n, l, key);
    }
    vpp=&((*vpp)->next);
  }
}

/* addrefs(<dest>,<src>)
 *   Take the union of SONGLIST <dest> and SONGLIST <src>, storing the result
 *   in <dest>.  Return 1 on success, or 0 on failure. */
static int
addrefs(dest,src)
  SONGLIST **dest;
  const SONGLIST *src;
{
  for (; src; src=src->next)
    if (!addref(dest, src->num, src->link, src->key)) return 0;
  return 1;
}

/* removerefs(<dest>,<src>)
 *   Take the set difference between SONGLIST <dest> and SONGLIST <src>,
 *   storing the result in <dest>. */
static void
removerefs(dest,src)
  SONGLIST **dest;
  const SONGLIST *src;
{
  SONGLIST **sl;

  for (; src; src=src->next)
  {
    for (sl=dest; *sl; sl=&((*sl)->next))
      if ((*sl)->key >= src->key) break;
    if (*sl && ((*sl)->key == src->key))
    {
      SONGLIST *temp = (*sl)->next;
      free((*sl)->num);
      free((*sl)->link);
      free(*sl);
      *sl = temp;
    }
  }
}

/* slcmp(<slist1>,<slist2>)
 *   Compare SONGLIST's <slist1> and <slist2>, returning 1 if they are
 *   different and 0 if they are the same. */
static int
slcmp(s1,s2)
  const SONGLIST *s1;
  const SONGLIST *s2;
{
  for (; s1 && s2; s1=s1->next, s2=s2->next)
    if (s1->key != s2->key) return 1;
  return (s1 || s2);
}

/* print_vrange(<file>,<book>,<verse1>,<verse2>,<lastchapter>)
 *   Output LaTeX material to file handle <file> for verse range
 *   <verse1>-<verse2> of book number <book>.  Depending on <lastchapter>,
 *   the outputted material might be the start of a new index entry or the
 *   continuation of a previous entry.  If <lastchapter> is 0, start a new
 *   index entry.  If <lastchapter> is >0, continue the previous entry and
 *   print the chapter of <verse1> only if it differs from <lastchapter>.
 *   If <lastchapter> is <0, continue the previous entry and always print the
 *   chapter number of <verse1>.  Return 0 on success or -1 on failure. */
static int
print_vrange(f,book,v1,v2,lastchapter)
  FILE *f;
  int book;
  const VERSE v1;
  const VERSE v2;
  int lastchapter;
{
  if (ws_fputs(lastchapter ? ws_lit(",") : ws_lit("\\idxentry{"), f) < 0)
    return -1;
  if (v1.verse <= 0)
  {
    if (ws_fprintf1(f, lastchapter ? ws_lit("\\thinspace %d") : ws_lit("%d"),
                    v1.chapter) < 0)
      return -1;
  }
  else if ((book>=0) && (book<numbooks) && (bible[book].chapters == 1))
  {
    /* This book has only one chapter. */
    if (ws_fprintf1(f, lastchapter ? ws_lit("\\thinspace %d") : ws_lit("%d"),
                    v1.verse) < 0)
      return -1;
  }
  else if ((lastchapter<=0) || (lastchapter!=v1.chapter) ||
           (v1.chapter!=v2.chapter))
  {
    if (ws_fprintf2(f, lastchapter ? ws_lit(" %d:%d") : ws_lit("%d:%d"),
                    v1.chapter, v1.verse) < 0)
      return -1;
  }
  else
  {
    if (ws_fprintf1(f, lastchapter ? ws_lit("\\thinspace %d") : ws_lit("%d"),
                    v1.verse) < 0)
      return -1;
  }

  if (vlt(v1,v2))
  {
    if (v2.verse <= 0)
    {
      if (ws_fprintf1(f, ws_lit("--%d"), v2.chapter) < 0) return -1;
    }
    else if (v1.chapter!=v2.chapter)
    {
      if (ws_fprintf2(f, ws_lit("--%d:%d"), v2.chapter, v2.verse) < 0)
        return -1;
    }
    else
    {
      if (ws_fprintf1(f, ws_lit("--%d"), v2.verse) < 0) return -1;
    }
  }
  return 0;
}

/* print_reflist(<file>,<slist>)
 *   Output the list of song references given by <slist> to file handle <file>.
 *   Return 0 on success or -1 on failure. */
static int
print_reflist(f,sp)
  FILE *f;
  const SONGLIST *sp;
{
  char first;
  for (first=1; sp; sp=sp->next)
  {
    if (!first)
    {
      if (ws_fputs(ws_lit("\\\\"), f) < 0) return -1;
    }
    first = 0;
    if ((ws_fputs(ws_lit("\\songlink{"), f) < 0) ||
        (ws_fputs(sp->link, f) < 0) ||
        (ws_fputs(ws_lit("}{"), f) < 0) ||
        (ws_fputs(sp->num, f) < 0) ||
        (ws_fputs(ws_lit("}"), f) < 0))
      return -1;
  }
  return 0;
}

/* genscriptureindex(<fstate>,<outname>,<biblename>)
 *   Generate a LaTeX file named <outname> containing material suitable to
 *   typeset the scripture index data found in input file handle <file>.
 *   Input Bible data from an ascii file named <biblename>.  Return 0 on
 *   success, 1 if there were warnings, and 2 if there was a fatal error. */
int
genscriptureindex(fs,outname,biblename)
  FSTATE *fs;
  const char *outname;
  const char *biblename;
{
  FILE *f;
  int eof = 0;
  VERSELIST **idx;
  int book, book2;
  WCHAR ref[MAXLINELEN], songnum[MAXLINELEN], linkname[MAXLINELEN];
  int songcount;
  WCHAR *p;
  VERSE v1, v2;
  char hadwarnings = 0;

  fprintf(stderr, "songidx: Parsing scripture index data file %s...\n",
          fs->filename);

  /* Read the bible data file into the bible array. */
  if (!readbible(biblename)) return 2;
  if ((idx = (VERSELIST **) calloc(numbooks, sizeof(VERSELIST *))) == NULL)
  {
    fprintf(stderr, "songidx: too many books of the bible (out of memory)\n");
    free_bible();
    return 2;
  }
  for (book=0; book<numbooks; ++book) idx[book] = NULL;

  /* Walk through the input file and construct a VERSELIST for each book
   * of the Bible. Each VERSELIST represents the set of verses in that book
   * referred to by songs in the song book. */
  songcount = 0;
  book = -1;
  v1.chapter = -1;
  v1.verse = -1;
  for (;;)
  {
    /* Get the next song data record. */
    if (!filereadln(fs,ref,&eof))
    {
      freeidxarray(idx,numbooks);
      free_bible();
      return 2;
    }
    if (eof) break;
    if (!filereadln(fs,songnum,&eof) || eof ||
        !filereadln(fs,linkname,&eof) || eof)
    {
      if (eof)
      {
        fprintf(stderr, "songidx:%s:%d: incomplete entry\n",
                fs->filename, fs->lineno);
        fileclose(fs);
      }
      freeidxarray(idx,numbooks);
      free_bible();
      return 2;
    }

    /* Add all scripture references for this song to the VERSELIST array */
    p = ref;
    while (*p != wc_null)
    {
      /* Parse the next reference. */
      p = parseref(p, &book, &v1);
      if (!p)
      {
        fprintf(stderr, "songidx:%s:%d: WARNING: Malformed scripture reference"
                        " for song ", fs->filename, fs->lineno);
        ws_fputs(songnum, stderr);
        fprintf(stderr, ". Ignoring it.\n");
        hadwarnings = 1;
        break;
      }
      if (book<0)
      {
        fprintf(stderr, "songidx:%s:%d: WARNING: Scripture reference for song ",
                fs->filename, fs->lineno);
        ws_fputs(songnum, stderr);
        fprintf(stderr, " doesn't include a book name. Ignoring it.\n");
        hadwarnings = 1;
        break;
      }
      if (book>=numbooks)
      {
        fprintf(stderr, "songidx:%s:%d: WARNING: Scripture reference for song ",
                fs->filename, fs->lineno);
        ws_fputs(songnum, stderr);
        fprintf(stderr, " references unknown book. Ignoring it.\n");
        hadwarnings = 1;
        break;
      }
      if (v1.chapter < 1)
      {
        fprintf(stderr, "songidx:%s:%d: WARNING: Scripture reference for song ",
                fs->filename, fs->lineno);
        ws_fputs(songnum, stderr);
        fprintf(stderr, " has no chapter or verse. Ignoring it.\n");
        hadwarnings = 1;
        break;
      }
      /* Unless the reference ended in a "-", it consists of just one verse. */
      v2.chapter = v1.chapter;
      v2.verse = v1.verse;
      if (*p==wc_hyphen)
      {
         /* This reference ended in a "-", which means it starts a range.
         * Parse the next reference to find the range's end. */
        book2 = book;
        p = parseref(++p, &book2, &v2);
        if (!p)
        {
          fprintf(stderr, "songidx:%s:%d: WARNING: Malformed scripture"
                          " reference for song ", fs->filename, fs->lineno);
          ws_fputs(songnum, stderr);
          fprintf(stderr, ". Ignoring it.\n");
          hadwarnings = 1;
          break;
        }
        if (book2!=book)
        {
          fprintf(stderr, "songidx:%s:%d: WARNING: Scripture reference for"
                          " song ", fs->filename, fs->lineno);
          ws_fputs(songnum, stderr);
          fprintf(stderr, " appears to span books! Ignoring it.\n");
          hadwarnings = 1;
          break;
        }
      }
      if (*p!=wc_null) ++p;
      /* If either the start or end references of the range were incomplete
       * (e.g., the range was something like "Philippians 3", where the verse
       * numbers are not explicit), try to fill in the missing information
       * based on how many verses are in the chapter and how many chapters are
       * in the book. */
      if (v1.verse <= 0) v1.verse = 1;
      if (v2.verse <= 0)
      {
        v2.verse = getvcount(book,v2.chapter);
        if (v2.verse <= 0)
        {
          fprintf(stderr, "songidx:%s:%d: WARNING: Scripture reference for "
                          " song ", fs->filename, fs->lineno);
          ws_fputs(songnum, stderr);
          fprintf(stderr, " refers implicitly to chapter %d of ", v2.chapter);
          ws_fputs(bible[book].name, stderr);
          fprintf(stderr, ", which doesn't exist. Ignoring it.\n");
          hadwarnings = 1;
          break;
        }
      }
      if (v1.verse > getvcount(book,v1.chapter))
      {
        fprintf(stderr, "songidx:%s:%d: WARNING: Song ",
                fs->filename, fs->lineno);
        ws_fputs(songnum, stderr);
        fprintf(stderr, " references ");
        ws_fputs(bible[book].name, stderr);
        fprintf(stderr, " %d:%d, which doesn't exist! Ignoring it.\n",
                v1.chapter, v1.verse);
        hadwarnings = 1;
        break;
      }
      if (v2.verse > getvcount(book,v2.chapter))
      {
        fprintf(stderr, "songidx:%s:%d: WARNING: Song ",
                fs->filename, fs->lineno);
        ws_fputs(songnum, stderr);
        fprintf(stderr, " references ");
        ws_fputs(bible[book].name, stderr);
        fprintf(stderr, " %d:%d, which doesn't exist! Ignoring it.\n",
                v2.chapter, v2.verse);
        hadwarnings = 1;
        break;
      }

      /* Add the range to the SONGLIST array. */
      if (!insertref(INSERT_ADD, &(idx[book]), v1, songnum, linkname,
                     songcount) ||
          !insertref(INSERT_DROP, &(idx[book]), v2, songnum, linkname,
                     songcount))
      {
        fileclose(fs);
        freeidxarray(idx,numbooks);
        free_bible();
        return 2;
      }
    }
    /* finished adding all refs for this song */
    ++songcount;
  }
  /* finished adding all refs for all songs */
  fileclose(fs);

  /* Now create the index .sbx file */
  fprintf(stderr, "songidx: Generating scripture index TeX file %s...\n",
          outname);
  if (strcmp(outname,"-"))
  {
    if ((f = fopen(outname, "w")) == NULL)
    {
      fprintf(stderr, "songidx: Unable to open %s for writing.\n", outname);
      freeidxarray(idx,numbooks);
      free_bible();
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
    freeidxarray(idx,numbooks); \
    free_bible(); \
    return 2; \
  }

  /* For each book of the Bible that has songs that reference it, go through
   * its VERSELIST and generate a sequence of index entries.  Wherever
   * possible, compact adjacent entries that have identical SONGLISTS so that
   * we never have two consecutive index entries with identical right-hand
   * sides. */
  for (book=0; book<numbooks; ++book)
    if (idx[book])
    {
      VERSE vrstart;
      VERSELIST *vp;
      SONGLIST *sl = NULL;
      int lastchapter; /* 0=none, -1=force printing of chapter */

      TRYWRITE((ws_fputs(ws_lit("\\begin{idxblock}{"), f) >=0)
               && (ws_fputs(bible[book].name, f) >= 0)
               && (ws_fputs(ws_lit("}\n"), f) >=0))
      lastchapter = 0;
      vcpy(&vrstart, idx[book]->v);
      for (vp=idx[book]; vp; vp=vp->next)
      {
        if (!addrefs(&sl, vp->adds))
        {
          fclose(f);
          freeidxarray(idx,numbooks);
          free_bible();
          return 2;
        }
        if (!slcmp(vp->drops, vp->next ? vp->next->adds : NULL))
        {
          /* Set of drops here equals set of adds next time.  There's at least
	   * a chance that we can combine this item and the next one into a
	   * single index entry. */
          if (vp->next && !vlt(vinc(book,vp->v), vp->next->v))
          {
            /* If the next item is adjacent to this one, do nothing.  Just let
	     * the range in progress be extended.  We'll output a single entry
	     * for all of these adjacent verses when we reach the end. */
            continue;
          }
          else if (vp->next && !slcmp(sl, vp->drops))
          {
            /* Otherwise, if the next item is not adjacent but all refs are
	     * dropped here, then print a partial entry to be continued with a
	     * comma next time. */
            TRYWRITE(print_vrange(f, book, vrstart, vp->v, lastchapter) >= 0)
            lastchapter = (vrstart.chapter != vp->v.chapter) ?
                          -1 : vp->v.chapter;
            vcpy(&vrstart, vp->next->v);
            continue;
          }
        }
        if (vp->drops)
        {
          /* Some songs get dropped here, and either the next item is not
	   * adjacent to this one, or it's adjacent and the set of adds is not
	   * the same.  In either case, that means the set of refs changes at
	   * this point, so we need to output a full entry (or finish the one
	   * in progress). */
          TRYWRITE((print_vrange(f, book, vrstart, vp->v, lastchapter) >= 0) &&
                   (ws_fputs(ws_lit("}{"), f) >= 0) &&
                   (print_reflist(f, sl) >= 0) &&
                   (ws_fputs(ws_lit("}\n"), f) >= 0))
          removerefs(&sl, vp->drops);
          lastchapter = 0;
          vcpy(&vrstart, (!sl && vp->next) ? vp->next->v : vinc(book,vp->v));
        }
        if (sl && vp->next && vp->next->adds && vlt(vrstart, vp->next->v))
        {
          /* There are verses between this item and the next which have refs,
	   * but the refs change at the beginning of the next item.  Make an
	   * entry for the intermediate block of verses. */
          VERSE vrend;
          vcpy(&vrend, vdec(book,vp->next->v));
          TRYWRITE((print_vrange(f, book, vrstart, vrend, lastchapter) >= 0) &&
                   (ws_fputs(ws_lit("}{"), f) >= 0) &&
                   (print_reflist(f, sl) >= 0) &&
                   (ws_fputs(ws_lit("}\n"), f) >= 0))
          lastchapter = 0;
          vcpy(&vrstart, vp->next->v);
        }
      }
      if (sl)
      {
        fprintf(stderr, "songidx: scripture references in ");
        ws_fputs(bible[book].name, stderr);
        fprintf(stderr, " appear to extend past the end of the book!"
                        " Aborting.\n");
        fclose(f);
        freeidxarray(idx,numbooks);
        free_bible();
        return 2;
      }
      TRYWRITE(ws_fputs(ws_lit("\\end{idxblock}\n"), f) >= 0)
    }

#undef TRYWRITE

  fclose(f);
  freeidxarray(idx,numbooks);
  free_bible();

  return hadwarnings;
}
