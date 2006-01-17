/*  $Header
    
    dvipos-20030225

    Copyright (C) 2003 by Jin-Hwan <chofchof@ktug.or.kr>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#include "dvicore.h"

#ifdef KPATHSEA
string progname;
unsigned dpi = 600;
string mode = NULL;
extern KPSEDLL char *kpathsea_version_string;
#endif

/* To be called from dvicore.c */
extern void dvi_init    (char *filename);
extern void dvi_close   (void);
extern void dvi_do_page (int n);

/* Global variables */
FILE *tuo_file = NULL;
extern int dvi_pages;

/* Internal variables */
static char *dvi_filename = NULL, *tuo_filename = NULL;

static void set_default_tuo_filename(void)
{
  if (strlen(dvi_filename) < 5 || strncmp(".dvi", dvi_filename+strlen(dvi_filename)-4, 4)) {
    tuo_filename = (char *)calloc(strlen(dvi_filename)+5, sizeof(char));
    strcpy(tuo_filename, dvi_filename);
    strcat(tuo_filename, ".tuo");
  } else {
    tuo_filename = (char *)calloc(strlen(dvi_filename)+1, sizeof(char));
    strncpy(tuo_filename, dvi_filename, strlen(dvi_filename)-4);
    strcpy(tuo_filename+strlen(dvi_filename)-4, ".tuo");
  }
}

#ifdef MIKTEX
#define PLATFORM "(MiKTeX 2.2)"
#else
#define PLATFORM kpathsea_version_string
#endif
#define VERSION "20030225"

static void title()
{
  fprintf(stderr, "This is DVIpos, Version %s %s\nby Jin-Hwan Cho <chofchof@ktug.or.kr>\n\n", VERSION, PLATFORM);
}

static void usage()
{
  fprintf(stderr, "Usage: dvipos infile[.dvi] [outfile]\n\nDVIpos appends output to infile.tuo if no outfile is specified.\n\n");
}

int main (int argc, char *argv[]) 
{
  register int i;

  if (argc < 2) {
    title();
    usage();
    fprintf(stderr, "No DVI filename specified.\n");
    exit(1);
  }

  if (strncmp(".dvi", argv[1]+strlen(argv[1])-4, 4)) {
    dvi_filename = (char *)calloc(strlen(argv[1])+5, sizeof(char));
    strcpy(dvi_filename, argv[1]);
    strcat(dvi_filename, ".dvi");
  } else
    dvi_filename = argv[1];

  if (argc > 2)
    tuo_filename = argv[2];
  else
    set_default_tuo_filename();

  /* Open the output TUO file */
  if ((tuo_file = fopen(tuo_filename, "at")) == NULL) {
    fprintf(stderr, "Could not open %s.\n", tuo_filename);
    exit(1);
  }
  
#ifdef MIKTEX
  miktex_initialize();
#endif

#ifdef KPATHSEA
  progname = argv[0];
  kpse_set_program_name (argv[0], progname);
  /* NULL for no fallback font.  */
  kpse_init_prog (uppercasify (kpse_program_name), dpi, mode, NULL);
#endif

  dvi_init(dvi_filename);

  for (i = 0; i < dvi_pages; i++)
    dvi_do_page(i);

  dvi_close();

  fclose(tuo_file);

#ifdef MIKTEX
  miktex_uninitialize ();
#endif

  exit(0);
}

