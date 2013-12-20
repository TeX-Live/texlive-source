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

#if HAVE_STRING_H
#  include <string.h>
#endif

/* fileopen(<fstate>,<filename>)
 *   Open <filename> for reading, storing the file handle in <fstate>. Return
 *   1 on success or 0 on failure.
 */
int
fileopen(fs,fnam)
  FSTATE *fs;
  const char *fnam;
{
  if (strcmp(fnam,"-"))
  {
    fs->f = fopen(fnam, "r");
    if (!fs->f)
    {
      fprintf(stderr, "songidx: Unable to open %s for reading.\n", fnam);
      return 0;
    }
  }
  else
  {
    fs->f = stdin;
    fnam = "stdin";
  }
  fs->filename = (char *) calloc(strlen(fnam)+1, sizeof(char));
  if (!fs->filename)
  {
    fprintf(stderr, "songidx: Out of memory!\n");
    fclose(fs->f);
    return 0;
  }
  strcpy(fs->filename, fnam);
  fs->lineno = 0;
  return 1;
}

/* fileclose(<fstate>)
 *   Close file handle <fstate>.
 */
void
fileclose(fs)
  FSTATE *fs;
{
  if (fs->f != stdin) fclose(fs->f);
  free(fs->filename);
  fs->f = NULL;
  fs->filename = NULL;
  fs->lineno = 0;
}

/* filereadln(<fstate>,<buffer>,<eof flag>)
 *   Read a line of output into <buffer>, which should be at least MAXLINELEN
 *   wide-characters in size.  If the line is too long to fit into the buffer,
 *   close the file and report an error.  Eliminate the trailing \n from the
 *   returned line.  Return 1 on success or end-of-line, or 0 on failure.  If
 *   end-of-line, set <eof flag> to 1. */
int
filereadln(fs,buf,eof)
  FSTATE *fs;
  WCHAR *buf;
  int *eof;
{
  size_t n;

  ++fs->lineno;
  if (!ws_fgets(buf, MAXLINELEN, fs->f))
  {
    if (!ferror(fs->f))
    {
      *eof=1;
      return 1;
    }
    fprintf(stderr, "songidx:%s:%d: read error\n", fs->filename, fs->lineno);
    fileclose(fs);
    return 0;
  }
  n = ws_strlen(buf);
  if ((n==0) || ((buf[n-1] != wc_newline) && (buf[n-1] != wc_cr)))
  {
    fprintf(stderr, "songidx:%s:%d: line too long\n", fs->filename, fs->lineno);
    fileclose(fs);
    return 0;
  }
  buf[n-1] = wc_null;
  if ((n>=2) && ((buf[n-2] == wc_newline) || (buf[n-2] == wc_cr)))
    buf[n-2] = wc_null;
  return 1;
}
