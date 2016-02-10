char version[12] = "2016-01-30";

/*  Copyright (C) 2014-16 R. D. Tennent School of Computing,
 *  Queen's University, rdt@cs.queensu.ca
 *
 *  This program is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A
 *  PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General
 *  Public License along with this program; if not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street,
 *  Fifth Floor, Boston, MA 02110-1301, USA.
 */

/*  msxlint - detects incorrectly formatted notes lines 
 *            in a MusiXTeX source file.
 *
 *  Usage: msxlint [-v | --version | -h | --help] 
 *         msxlint [-d | --debug] { infile[.tex] } ...
 *
 */


# include "utils.h"

PRIVATE int debug = 0;
PRIVATE char infilename[LINE_LEN];
PRIVATE char *infilename_n = infilename;

PRIVATE FILE *infile;

PRIVATE char line[LINE_LEN];            /* line of input   */
PRIVATE int lineno;                     /* line number     */
PRIVATE int ninstr;                     /* number of instruments */
PRIVATE int staffs[12]; /* staffs[i] is the number of staffs
                           for instrument i                      */                             
PRIVATE char terminator[8];   /* "\en" or "\enotes" or "" (either)  */
PRIVATE int nerrors;          /* number of error messages */
# define NERRORSMAX  10

PRIVATE void
checkc (char *s, char c)
{ if (*s != c) 
    printf ("Expected %c but found %c in line %d:\n%s\n", c, *s, lineno, line);
}

PRIVATE void
checkn (char *s)
{ if (strpbrk (s, "0123456789") != s) 
    printf ("Expected digit but found %c in line %d:\n%s\n", *s, lineno, line);
}

PRIVATE void
usage (void)
{
  puts ("Usage: msxlint [-v | --version | -h | --help]");
  puts ("       msxlint [-d | --debug] ... { infile[.tex] } ...");
}

PRIVATE void
analyze_notes ( char **ln) {
  int i,j;
  int newlines = 0; 
  char *s; char *t;
  s = strpbrk (*ln+1, "|&\\\n"); /* skip initial command      */
  while (true)
  { /* look for \en */
    t = strstr(s, "\\en");
    if (t != NULL) break;
    else
    /* replace EOL by a blank and append another line of input */
    { char new_line[LINE_LEN];
      if (debug > 2) printf ("  Reading additional line of input:\n");
      if ( fgets (new_line, LINE_LEN, infile) == NULL)
        error("Unexpected EOF.");
      if ( (t = strchr (s, '\n')) == NULL )
        error("Missing EOL.");
      *t = ' ';
      t++;
      *t = '\0';
      if (append (line, &t, new_line, LINE_LEN) >= LINE_LEN)
        error ("Line too Long.");
      if (debug > 2) printf ("  %s\n", t+1);
      newlines++;
    }
  }
  for (i=1; i <= ninstr; i++)
  { 
    for (j=1; j < staffs[i]; j++)
    { s = strpbrk (s, "|&");
      if (s == NULL || *s == '&' || s > t) 
      {
        printf ("Too few staffs for instrument %d in notes on line %d:\n%s", i, lineno, line);
        nerrors++;
        *ln = t+3;
        return;
      }
      checkc (s, '|'); s++;
    }
    if (i == ninstr) break;
    s = strpbrk (s, "&|");
    if (s == NULL || s > t) 
    {
      printf ("Too few instruments in notes on line %d:\n%s", lineno, line);
      nerrors++;
      *ln = t+3;
      return;
    }
    if (*s == '|') 
    {
      printf ("Too many staffs for instrument %d in notes on line %d:\n%s", i, lineno, line);
      nerrors++;
      *ln = t+3;
      return;
    }
    checkc (s, '&'); s++;
  }
  s = strpbrk (s, "|&");
  if (s != NULL && s < t)
  {
    printf ("Too many fields in notes on line %d:\n%s", lineno, line);
    nerrors++;
    *ln = t+3;
    return;
  }
  s = t; /* \en */
  if (terminator[0] != '\0') 
  { if (!prefix (terminator, s) )
    {
      printf ("Expected %s at end of notes on line %d:\n%s", terminator, lineno, line);
      nerrors++;
      t = strchr(s, '\\'); 
      if (t == NULL) t = *ln + strlen (*ln);
      *ln = t;
      return;
    }
    if ( terminator[3] == '\0' && prefix ("otes", s+3) )
    {
      printf ("Expected %s at end of notes on line %d:\n%s", terminator, lineno, line);
      nerrors++;
      t = strchr(s, '\\'); 
      if (t == NULL) t = *ln + strlen (*ln);
      *ln = t;
      return;
    }
  }
  lineno = lineno + newlines;
  t = strchr(s+1, '\\'); 
  if (t == NULL) t = *ln + strlen (*ln);
  *ln = t;
  return;
}

PRIVATE void
process_command (char **ln)
{ char *s, *t;
  if ( prefix("\\instrumentnumber", *ln) )
  { 
    if (debug > 1) printf ("%s\n", " Processing \\instrumentnumber"); 
    s = strpbrk (*ln, "123456789");
    if ( s == NULL ) error ("\\instrumentnumber command unreadable.");
    ninstr = atoi (s);
    t = strpbrk (*ln+1, "\\");
    if (t == NULL) 
    { *ln = *ln + strlen (*ln); return; }
    *ln = t;
  }

  if ( prefix("\\def\\nbinstrument", *ln) )
  { 
    if (debug > 1) printf ("%s\n", " Processing \\def\\nbinstrument"); 
    s = strpbrk (*ln, "123456789");
    if ( s == NULL ) error ("\\def\\nbinstrument command unreadable.");
    ninstr = atoi (s);
    t = strpbrk (*ln+1, "\\");
    if (t == NULL) 
    { *ln = *ln + strlen (*ln); return; }
    *ln = t;
  }

  else if ( prefix("\\setstaffs", *ln) )
  { 
    int n, p;
    if (debug > 1) printf ("%s\n", " Processing \\setstaffs"); 
    s = strpbrk (*ln, "123456789");
    if ( s == NULL ) error ("\\setstaffs command unreadable.");
    n = (int)(*s) - (int)('0'); /* instrument number  */
    s = strpbrk (s+1, "123456789");
    if ( s == NULL ) error ("\\setstaffs command unreadable.");
    p = (int)(*s) - (int)('0'); /* number of staffs */
    staffs[n] = p;
    t = strpbrk (*ln+1, "\\");
    if (t == NULL) 
    { *ln = *ln + strlen (*ln); return; }
    *ln = t;
  }

  else if ( prefix("\\def\\vnotes#1\\elemskip", *ln) ) 
  { 
    if (debug > 1) printf ("%s\n", " Processing \\def\\vnotes"); 
    s = *ln + 22; /* first parameter */
    ninstr = 0;
    while (true)
    { ninstr++;
      staffs[ninstr] = 1;
      checkc (s, '#'); s++;
      checkn (s); s++; 
      while (*s == '|')
      { staffs[ninstr]++; 
        checkc (s, '|'); s++;
        checkc (s, '#'); s++;
        checkn (s); s++; 
      }
      if (*s != '&') break;
      checkc (s, '&'); s++;
    }
    t = &terminator[0];
    while (*s != '{')
    {  *t = *s; s++; t++; }
    *t = '\0';
    *ln = strchr (*ln, '}') ;
    (*ln)++;
  }

  else if ( prefix("\\TransformNotes", *ln) ) 
  { 
    if (debug > 1) printf ("%s\n", " Processing \\TransformNotes"); 
    s = *ln + 16; /* first parameter */
    ninstr = 0;
    while (true)
    { ninstr++;
      staffs[ninstr] = 1;
      checkc (s, '#'); s++;
      checkn (s); s++; 
      while (*s == '|')
      { staffs[ninstr]++; 
        checkc (s, '|'); s++;
        checkc (s, '#'); s++;
        checkn (s); s++; 
      }
      if (*s != '&') break;
      checkc (s, '&'); s++;
    }
    t = &terminator[0];
    terminator[0] = '\0';
    append ( t, &t, "\\en", sizeof (terminator) );
    do s++; while (*s != '{');
    *ln = strchr (s, '}') ;
    if (*ln == NULL) error ("Can't find '}' after \\TransformNotes.");
    (*ln)++;
  }

  else if ( prefix("\\notes", *ln) || 
            prefix("\\Notes", *ln) ||
            prefix("\\NOtes", *ln) ||
            prefix("\\NOTes", *ln) ||
            prefix("\\NOTEs", *ln) ||
            prefix("\\nnotes", *ln) ||
            prefix("\\vnotes", *ln) ||
            prefix("\\znotes", *ln) 
          )
  { 
    if (debug > 1) printf ("%s\n", " Processing notes"); 
    analyze_notes (ln);
    t = strpbrk (*ln, "\\\n");
    if (t == NULL) 
    { *ln = *ln + strlen (*ln); return; }
    *ln = t;
  }

  else if ( prefix("\\def\\atnextbar", *ln))
  { 
    if (debug > 1) printf ("%s\n", " Processing \\def\\atnextbar"); 
    s = strstr (*ln, "\\znotes");
    if (s != NULL) 
    {
      *ln = s;
      analyze_notes (ln); 
    }
    t = strpbrk (*ln, "\\");
    if (t == NULL) 
    { *ln = *ln + strlen (*ln); return; }
    *ln = t;
  }

  else if ( prefix("\\def", *ln) )
    *ln = *ln + strlen (*ln); 

  else  /* anything else  */
  { 
    if (debug > 1) printf (" Processing "); 
    t = strpbrk (*ln+1, "\\\n");
    if (t == NULL) t = *ln + strlen (*ln);
    if (debug > 1)
    { char *s = *ln; do {putchar (*s); s++;} while (s != t); puts ("");}
    *ln = t;
  }
}

PRIVATE void
process_line (void)
{
  char  *ln; 
  ln = &line[0];
  while ( *ln != '\0') 
  {
    while (*ln == ' ') { ln++;}
    if (*ln == '%' || *ln == '\n') return;
    process_command (&ln);
  }     /* *ln == '\0'  */
}

PRIVATE void 
process_score (void)
/* process .tex file */
{
  int c; int i;
  lineno = 0;
  ninstr = 1;
  nerrors = 0;
  staffs[0] = 0;
  for (i = 1; i<12; i++) staffs[i] = 1;
  terminator[0] = '\0';  /* either \en or \enotes  */
  while ( (c = getc (infile)) != EOF )
  {
    ungetc (c, infile);
    if (fgets(line, LINE_LEN, infile) == NULL) error("IO error");
    if (strlen (line) == LINE_LEN-1) error("Line too long.");
    lineno++;
    if (debug > 0) printf ("Processing line %d.\n", lineno);
    process_line ();
    if (nerrors > NERRORSMAX) {
      puts ("\nToo many errors; processing of this file aborted.");
      return;
    }
  }     /* c == EOF  */
}     


int 
main (int argc, char *argv[])
{
  int c;
  char today[12];
  time_t mytime; 
# define NOPTS 4
  struct option longopts[NOPTS] =
  {  { "help", 0, NULL, 'h'},
     { "version", 0, NULL, 'v'},
     { "debug", 0, NULL, 'd'},
     { NULL, 0, NULL, 0}
  };
  
  time (&mytime);
  strftime (today, 11, "%Y-%m-%d", localtime (&mytime) );
  printf ("This is msxlint, version %s.\n", version);
  c = getopt_long (argc, argv, "hvd", longopts, NULL);
  while (c != -1)
    {
      switch (c)
        {
        case 'h':
          usage ();
          puts ( "Please report bugs to rdt@cs.queensu.ca." );
          exit (0);
        case 'v':
          exit (0);
        case 'd':
          debug++;
          break;
        case '?':
          exit (EXIT_FAILURE);
        default:
          fprintf (stderr,"Function getopt returned character code 0%o.\n",
                  (unsigned int) c);
          exit (EXIT_FAILURE);
        }
      c = getopt_long (argc, argv, "hvd", longopts, NULL);
    }
  puts ( "Copyright (C) 2014-16  R. D. Tennent" );
  puts ( "School of Computing, Queen's University, rdt@cs.queensu.ca" );
  puts ( "License GNU GPL version 2 or later <http://gnu.org/licences/gpl.html>." );
  puts ( "There is NO WARRANTY, to the extent permitted by law." );

  if (optind == argc)
  {
    infile = stdin;
    printf ("\nProcessing stdin.\n\n");
    process_score ();
  }
  else
  do
  {
    infilename[0] = '\0';
    infilename_n = infilename;
    append (infilename, &infilename_n, argv[optind], sizeof (infilename));
    if (!suffix (".tex", infilename))
      append (infilename, &infilename_n, ".tex", sizeof (infilename));
    infile = fopen (infilename, "r");
    if (infile == NULL)
      {
        fprintf (stderr,"Can't open %s\n", infilename);
        optind++;
        continue;
      }
    printf ("\nProcessing %s.\n\n", infilename);
    process_score ();
    optind++;
  }
  while (optind < argc);

  return 0;
}
