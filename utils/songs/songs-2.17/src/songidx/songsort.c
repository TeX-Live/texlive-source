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


#if HAVE_CONFIG_H
#  include "config.h"
#else
#  include "vsconfig.h"
#endif

#include "chars.h"
#include "songidx.h"
#include "fileio.h"

/* skipesc(<ptr>,<sm_flag>,<group_flag>)
 *   Walk <ptr> past any LaTeX macros or braces until we reach the next "real"
 *   character.  If <sm_flag> is 1, then space macros "\ " cause p to stop on
 *   the space; otherwise they are skipped as macros.  If <group_flag> is 1
 *   then content between (non-escaped) braces is skipped; otherwise braces
 *   are ignored. */
void
skipesc(p, stop_on_space, skipover_groups)
  const WCHAR **p;
  int stop_on_space;
  int skipover_groups;
{
  unsigned int group_depth = 0;

  for (;;)
  {
    if (**p == wc_backslash)
    {
      ++(*p);
      if (stop_on_space && !group_depth && wc_isspace(**p)) return;
      if (wc_isalpha(**p)) {
        while (wc_isalpha(**p)) ++(*p);
        while (wc_isspace(**p)) ++(*p);
      } else if (**p != wc_null) ++(*p);
    }
    else if (**p == wc_lbrace)
    {
      ++(*p);
      if (skipover_groups) ++group_depth;
      while (wc_isspace(**p)) ++(*p);
    }
    else if (**p == wc_rbrace)
    {
      ++(*p);
      if (group_depth) --group_depth;
    }
    else if (group_depth)
      ++(*p);
    else
      break;
  }
}

/* inword(<char>)
 *   Return 0 if <char> is a word-delimiter (for sorting purposes) and 1
 *   otherwise.  Other than TeX macros (which are handled by skipesc above),
 *   words only have alphabetics and apostrophes. */
static int
inword(c)
  WCHAR c;
{
  return (wc_isalpha(c) || (c==wc_apostrophe) || (c==wc_backquote));
}

/* songcmp(<song1>,<song2>)
 *   Return a negative number if <song1> is less than <song2>, a positive
 *   number if <song1> is greater than <song2>, and 0 if <song1> and <song2>
 *   are equal.  The ordering is first by title, then by index.  This function
 *   is suitable for use with qsort(). */
int
songcmp(s1,s2)
  const void *s1;
  const void *s2;
{
  static WCHAR buf1[MAXLINELEN+1], *bp1;
  static WCHAR buf2[MAXLINELEN+1], *bp2;
  const WCHAR *t1 = (*((SONGENTRY * const *) s1))->title;
  const WCHAR *t2 = (*((SONGENTRY * const *) s2))->title;
  int diff;

  for (;;)
  {
    /* Find the next word or number in each string. */
    skipesc(&t1,0,0);
    while(*t1 && !wc_isalpha(*t1) && !wc_isdigit(*t1))
    {
      ++t1;
      skipesc(&t1,0,0);
    }
    skipesc(&t2,0,0);
    while(*t2 && !wc_isalpha(*t2) && !wc_isdigit(*t2))
    {
      ++t2;
      skipesc(&t2,0,0);
    }

    /* If there is no next word/number in one or both, sort the shorter
     * string before the longer one. */
    if ((*t1==wc_null) || (*t2==wc_null))
    {
      if ((*t1==wc_null) && (*t2==wc_null)) break;
      return (t1!=wc_null) ? 1 : -1;
    }

    /* If one is a number, sort the number before the word.  If both are
     * numbers, sort in numerical order. */
    if (wc_isdigit(*t1) || wc_isdigit(*t2))
    {
      long n1, n2;
      WCHAR *p1, *p2;
      if (!wc_isdigit(*t1)) return 1;
      if (!wc_isdigit(*t2)) return -1;
      n1 = ws_strtol(t1, &p1, 10);
      t1 = p1;
      n2 = ws_strtol(t2, &p2, 10);
      t2 = p2;
      if (n1 == n2) continue;
      return n1-n2;
    }

    /* Otherwise, both are words.  Copy the words into scratch buffers,
     * omitting any macros or braces.  Then lexographically compare the buffer
     * contents to determine how the original strings should be sorted. */
    for (bp1=buf1; inword(*t1); skipesc(&t1,1,0)) *bp1++=wc_tolower(*t1++);
    for (bp2=buf2; inword(*t2); skipesc(&t2,1,0)) *bp2++=wc_tolower(*t2++);
    *bp1 = *bp2 = wc_null;
    if ((diff = ws_coll(buf1,buf2)) != 0) return diff;
  }

  /* If each corresponding word/number is identical, then sort alternate-
   * form entries (e.g., lyrics) after normal entries (e.g., titles). */
  if (((*((SONGENTRY * const *) s1))->title[0] == wc_asterisk)
      && ((*((SONGENTRY * const *) s2))->title[0] != wc_asterisk))
    return 1;
  if (((*((SONGENTRY * const *) s1))->title[0] != wc_asterisk)
      && ((*((SONGENTRY * const *) s2))->title[0] == wc_asterisk))
    return -1;

  /* If everything is the same, sort by the right-hand sides of the index
   * entries (e.g., the song or page numbers). */
  return (*((SONGENTRY * const *) s1))->idx - (*((SONGENTRY * const *) s2))->idx;
}
