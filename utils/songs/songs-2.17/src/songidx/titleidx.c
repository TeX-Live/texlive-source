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


/* Create a LaTeX title index file from a title .sxd file. */

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

/* By convention, some word prefixes always get moved to the end of a title
 * before it is indexed.  For example, in English "The Song Title" becomes
 * "Song Title, The".  The following structure stores the list of these prefix
 * words.  The default is "The" and "A". */
typedef struct prefix
{
  struct prefix *next;
  WCHAR *s;
}
PREFIX;

WCHAR ws_A[] = ws_lit("A");
WCHAR ws_The[] = ws_lit("The");
PREFIX pre_A = { NULL, ws_A };
PREFIX pre_defaults = { &pre_A, ws_The };

/* freeprefixes(<list>)
 *   Free prefix list <list>. */
static void
freeprefixes(pl)
  PREFIX *pl;
{
  if (pl == &pre_defaults) return;
  while (pl)
  {
    PREFIX *nxt = pl->next;
    free(pl->s);
    free(pl);
    pl = nxt;
  }
}

/* rotate(<title>, <prelist>)
 *   If the first word of <title> is any word in list <prelist>, then <title>
 *   is modified in-place so that the word is shifted to the end of the string
 *   and preceded by a comma and a space.  So for example, rotate("The Title",
 *   <prelist>) where <prelist> is the default results in "Title, The".  Words
 *   in <prelist> are matched case-insensitively.  If <title> begins with the
 *   marker character '*', that character is ignored and left unchanged.  Note
 *   that <title> MUST be allocated with ONE EXTRA WCHAR OF STORAGE in order
 *   to leave room for the extra comma.
 *   Return Value: 1 if <title> was modified and 0 otherwise. */
static int
rotate(s,pl)
  WCHAR *s;
  PREFIX *pl;
{
  const WCHAR *s2, *w2;

  w2 = NULL;
  if (*s==wc_asterisk) ++s;
  for (; pl; pl=pl->next)
  {
    for (s2=s, w2=pl->s; *w2; ++s2, ++w2)
      if (wc_tolower(*s2)!=wc_tolower(*w2)) break;
    if ((*w2 == wc_null) && (*s2 != wc_null) && !wc_isalpha(*s2)) break;
  }
  if (pl)
  {
    WCHAR buf[MAXLINELEN];
    size_t slen = ws_strlen(s);
    size_t wlen = w2 - pl->s;
    ws_strncpy(buf, s, wlen);
    buf[wlen] = wc_null;
    ws_memmove(s, s+wlen+1, slen-wlen-1);
    *s = wc_toupper(*s);
    *(s+slen-wlen-1) = wc_comma;
    *(s+slen-wlen) = wc_tilda;
    ws_strcpy(s+slen-wlen+1, buf);
    return 1;
  }
  return 0;
}

/* getstartchar(<string>)
 *   Find the first non-escaped, alphanumeric character of <string> if any.
 *   If alphabetic, return it in upper case.  If numeric, return '#'.  If
 *   none, return space. */
static WCHAR
getstartchar(s)
  const WCHAR *s;
{
  if (!s) return wc_space;
  skipesc(&s,0,0);
  while (*s && !wc_isalpha(*s) && !wc_isdigit(*s))
  {
    ++s;
    skipesc(&s,0,0);
  }
  return wc_isalpha(*s) ? wc_toupper(*s) : (*s ? wc_hash : wc_space);
}

/* freesongarray(<array>,<len>)
 *   Free song array <array>, which should have length <len>. */
static void
freesongarray(songs,len)
  SONGENTRY **songs;
  int len;
{
  int i;

  if (!songs) return;
  for (i=0; i<len; ++i)
  {
    if (songs[i])
    {
      if (songs[i]->title) free(songs[i]->title);
      if (songs[i]->num) free(songs[i]->num);
      if (songs[i]->linkname) free(songs[i]->linkname);
      free(songs[i]);
    }
  }
  free(songs);
}

/* gentitleindex(<file>,<outname>)
 *   Reads a title index data file from file handle <file> and generates a new
 *   file named <outfile> containing a LaTeX title index.
 *   Return Value: 0 on success, 1 on warnings, or 2 on failure */
int
gentitleindex(fs,outname)
  FSTATE *fs;
  const char *outname;
{
  FILE *f;
  int eof = 0;
  int numsongs, arraysize, i;
  PREFIX *prelist = &pre_defaults;
  SONGENTRY **songs;
  WCHAR buf[MAXLINELEN], *bp;
  WCHAR startchar;

  fprintf(stderr, "songidx: Parsing title index data file %s...\n",
          fs->filename);

  songs = NULL;
  eof = 0;
  for (arraysize=numsongs=0; !eof; ++numsongs)
  {
    if (!filereadln(fs,buf,&eof))
    {
      freesongarray(songs,numsongs);
      return 2;
    }
    if (eof) break;
    if (buf[0] == wc_percent)
    {
      if (!ws_strncmp(buf, wc_lit("%prefix "), 8))
      {
        PREFIX *temp = (prelist==&pre_defaults) ? NULL : prelist;
        prelist = (PREFIX *) malloc(sizeof(PREFIX));
        prelist->next = temp;
        prelist->s = (WCHAR *) calloc(ws_strlen(buf)-7, sizeof(WCHAR));
        ws_strcpy(prelist->s, buf+8);
      }
      --numsongs;
      continue;
    }
    if (numsongs >= arraysize)
    {
      SONGENTRY **temp;
      arraysize *= 2;
      if (arraysize==0) arraysize=64;
      temp = (SONGENTRY **) realloc(songs,arraysize*sizeof(SONGENTRY *));
      if (!temp)
      {
        fprintf(stderr, "songidx:%s:%d: too many songs (out of memory)\n",
                fs->filename, fs->lineno);
        freeprefixes(prelist);
        freesongarray(songs,numsongs);
        return 2;
      }
      songs = temp;
    }
    if ((songs[numsongs] = (SONGENTRY *) malloc(sizeof(SONGENTRY))) == NULL)
    {
      fprintf(stderr, "songidx:%s:%d: too many songs (out of memory)\n",
              fs->filename, fs->lineno);
      freeprefixes(prelist);
      freesongarray(songs,numsongs);
      return 2;
    }
    songs[numsongs]->title = songs[numsongs]->num =
      songs[numsongs]->linkname = NULL;
    for (bp=buf+ws_strlen(buf)-1; (bp > buf) && wc_isspace(*bp); --bp) ;
    if (wc_isspace(*bp)) *bp = wc_null;
    for (bp=buf; wc_isspace(*bp); ++bp) ;
    /* The following allocates one extra char of storage because we might add
     * a comma later. */
    if ((songs[numsongs]->title =
         (WCHAR *) calloc(ws_strlen(bp)+2, sizeof(WCHAR))) == NULL)
    {
      fprintf(stderr, "songidx:%s:%d: too many songs (out of memory)\n",
              fs->filename, fs->lineno);
      freeprefixes(prelist);
      freesongarray(songs,numsongs+1);
      return 2;
    }
    ws_strcpy(songs[numsongs]->title, bp);
    rotate(songs[numsongs]->title, prelist);
    if (!filereadln(fs,buf,&eof))
    {
      freeprefixes(prelist);
      freesongarray(songs,numsongs+1);
      return 2;
    }
    if (eof)
    {
      fprintf(stderr, "songidx:%s:%d: incomplete song entry (orphan title)\n",
              fs->filename, fs->lineno);
      freeprefixes(prelist);
      freesongarray(songs,numsongs+1);
      return 2;
    }
    if ((songs[numsongs]->num =
         (WCHAR *) calloc(ws_strlen(buf)+1, sizeof(WCHAR))) == NULL)
    {
      fprintf(stderr, "songidx:%s:%d: too many songs (out of memory)\n",
              fs->filename, fs->lineno);
      freeprefixes(prelist);
      freesongarray(songs,numsongs+1);
      return 2;
    }
    ws_strcpy(songs[numsongs]->num, buf);
    if (!filereadln(fs,buf,&eof))
    {
      freeprefixes(prelist);
      freesongarray(songs,numsongs+1);
      return 2;
    }
    if (eof)
    {
      fprintf(stderr, "songidx:%s:%d: incomplete song entry"
                      " (missing hyperlink)\n", fs->filename, fs->lineno);
      freeprefixes(prelist);
      freesongarray(songs,numsongs+1);
      return 2;
    }
    if ((songs[numsongs]->linkname =
         (WCHAR *) calloc(ws_strlen(buf)+1, sizeof(WCHAR))) == NULL)
    {
      fprintf(stderr, "songidx:%s:%d: too many songs (out of memory)\n",
              fs->filename, fs->lineno);
      freeprefixes(prelist);
      freesongarray(songs,numsongs);
      return 2;
    }
    ws_strcpy(songs[numsongs]->linkname, buf);
    songs[numsongs]->idx = numsongs;
  }
  fileclose(fs);
  freeprefixes(prelist);

  /* Sort the song array */
  qsort(songs, numsongs, sizeof(*songs), songcmp);

  /* Write the sorted data out to the output file. */
  fprintf(stderr, "songidx: Generating title index TeX file %s...\n", outname);
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
    freesongarray(songs,numsongs); \
    return 2; \
  }

  startchar = 0;
  for (i=0; i<numsongs; ++i)
  {
    if ((i>0) && !ws_coll(songs[i]->title, songs[i-1]->title))
    {
      TRYWRITE((ws_fputs(ws_lit("\\\\\\songlink{"), f) >= 0) &&
               (ws_fputs(songs[i]->linkname, f) >= 0) &&
               (ws_fputs(ws_lit("}{"), f) >= 0) &&
               (ws_fputs(songs[i]->num, f) >= 0) &&
               (ws_fputc(wc_rbrace, f) == wc_rbrace))
    }
    else
    {
      WCHAR c = getstartchar(songs[i]->title);
      if (startchar != c)
      {
        startchar = c;
        TRYWRITE((ws_fputs((i<=0) ? ws_lit("\\begin{idxblock}{")
                    : ws_lit("}\n\\end{idxblock}\n\\begin{idxblock}{"), f) >= 0)
                 && ((startchar!=wc_hash) ||
                     (ws_fputc(wc_backslash, f)==wc_backslash))
                 && (ws_fputc(startchar, f)==startchar))
      }
      if (songs[i]->title[0] == wc_asterisk)
      {
        TRYWRITE((ws_fputs(ws_lit("}\n\\idxaltentry{"), f) >= 0) &&
                 (ws_fputs(songs[i]->title+1, f) >= 0) &&
                 (ws_fputs(ws_lit("}{\\songlink{"), f) >= 0) &&
                 (ws_fputs(songs[i]->linkname, f) >= 0) &&
                 (ws_fputs(ws_lit("}{"), f) >= 0) &&
                 (ws_fputs(songs[i]->num, f) >= 0) &&
                 (ws_fputc(wc_rbrace, f) == wc_rbrace))
      }
      else
      {
        TRYWRITE((ws_fputs(ws_lit("}\n\\idxentry{"), f) >= 0) &&
                 (ws_fputs(songs[i]->title, f) >= 0) &&
                 (ws_fputs(ws_lit("}{\\songlink{"), f) >= 0) &&
                 (ws_fputs(songs[i]->linkname, f) >= 0) &&
                 (ws_fputs(ws_lit("}{"), f) >= 0) &&
                 (ws_fputs(songs[i]->num, f) >= 0) &&
                 (ws_fputc(wc_rbrace, f) == wc_rbrace))
      }
    }
  }
  if (numsongs>0)
  {
    TRYWRITE(ws_fputs(ws_lit("}\n\\end{idxblock}\n"), f) >= 0)
  }

#undef TRYWRITE

  if (f == stdout) fflush(f); else fclose(f);
  freesongarray(songs,numsongs);
  return 0;
}
