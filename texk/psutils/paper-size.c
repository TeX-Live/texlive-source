/* paper-size.c: TeX Live specific definition of paper_size()

   Copyright 2014 Peter Breitenlohner.
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>
#include <psutil.h>
#include <paper.h>

#define CONFIG_NAME "paper.cfg"
#define MAX_PAPER 16
static char default_paper[MAX_PAPER];

static void
do_init (void)
{
  static int inited = 0;
  int ch, i = 0;
  char *fqpn;
  FILE *fp = NULL;

  if (inited)
    return;

  paperinit ();

  fqpn = kpse_find_file (CONFIG_NAME, kpse_program_text_format, false);
  if (fqpn)
    fp = fopen (fqpn, "r");
  if (!fp)
    die ("can't open config file %s", CONFIG_NAME);

  while (i < MAX_PAPER - 1 &&
         (ch = fgetc (fp)) > 0 && ch != ' ' && ch != '\n' && ch != '\r')
    default_paper[i++] = ch;
  default_paper[i] = 0;
  if (i == 0)
    die ("can't read default papersize from file %s", CONFIG_NAME);

  inited = 1;
}

int
paper_size (const char *paper_name, double *width, double *height)
{
  const struct paper *pi;

  do_init ();

  if (!(pi = paperinfo (paper_name ? paper_name : default_paper)))
    return 0;

  *width = paperpswidth (pi);
  *height = paperpsheight (pi);
  return 1;
}
