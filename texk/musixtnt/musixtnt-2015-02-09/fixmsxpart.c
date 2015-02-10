char version[12] = "2015-02-09";

/*  Copyright (C) 2014-15 R. D. Tennent School of Computing,
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

/*  fixmsxpart - corrects note spacing in a single-staff 
 *               musixtex part
 */


# include "utils.h"

# define NNOTES "\\vnotes1.6\\elemskip"
# define NNNOTES "\\vnotes1.28\\elemskip"

PRIVATE char infilename[LINE_LEN];
PRIVATE char *infilename_n = infilename;
PRIVATE char outfilename[LINE_LEN];
PRIVATE char *outfilename_n = outfilename;

PRIVATE FILE *infile, *outfile;

PRIVATE int debug = 0;
PRIVATE char line[LINE_LEN];    /* line of input                                        */
PRIVATE int staff = 1;          /* staff number                                         */
PRIVATE char *notes;            /* notes or other material for staff part               */
PRIVATE int spacing = 0;        /* 0 = not determined; 1 = whole or breve, 2 = half, 
                                   4 = quarter, 8 = eigth, etc.                         */
PRIVATE int beaming = 0;        /* beam-note spacing                                    */
PRIVATE int new_beaming = 0;    /* spacing to be used *after* the next beam note        */
PRIVATE int restbars = 0;       /* number of whole-bar rests not yet output             */

PRIVATE void
usage (FILE *f)
{
  fprintf (f, "Usage: fixmsxpart [-v | --version | -h | --help]\n");
  fprintf (f, "       fixmsxpart infile[.tex] [outfile[.tex]] \n");
}


PRIVATE char 
*analyze_notes (char **ln) {
/*
   skip initial TeX command, which may be unreliable; 
   then skip staff-1 ampersands or vertical bars; 
   then truncate tail and return a pointer to the 
   beginning of the staff commands.
   ln is moved to the token after \en
*/
  int i; char *s; char *t; char *tt;
  s = strpbrk (*ln+1, "|&\\\n"); /* skip initial command      */
  while (true)
  { /* look for \en */
    t = strstr(s, "\\en");
    if (t != NULL) break;
    else
    /* replace EOL by a blank and append another line of input */
    { char new_line[LINE_LEN];
      if (fgets (new_line, LINE_LEN, infile) == NULL)
        error("Unexpected EOF.");
      if ((t = strchr (s, '\n')) == NULL )
        error("Missing EOL.");
      *t = ' ';
      t++;
      *t = '\0';
      if (append (line, &t, new_line, LINE_LEN) >= LINE_LEN)
        error ("Line too Long.");
    }
  }
  for (i=1; i<staff; i++)      /* skip staff-1 & or |       */
  { 
     s = strpbrk (s, "|&");
     if (s == NULL) error ("Can't find & or |.");
     s++;
  }
  t = strstr(s, "\\en"); 
  if (t == NULL) error ("Can't find \\en.");
  *ln = t+3;
  tt = strpbrk (s, "|&");
  if ( tt != NULL && tt < t) t = tt;
  *t = '\0';
  if (debug > 0)
    fprintf (stderr, "After analyze_notes notes=%s\n", s);
  return s;
}


PRIVATE void
output_notes (char *t) 
{ /* outputs text from *notes to *t (except for 
     commands \sk \hsk \noteskip= \multnoteskip);
     then updates notes.
  */
  char *s = notes;
  if (debug > 0)
    fprintf (stderr, "\nBefore output_notes: notes=%s\n", s);
  while (s < t) 
  {
    /* discard  commands \sk, \hsk, \noteskip= ..., \multnoteskip  */ 
    if ( prefix("\\sk", s)  ||
         prefix("\\hsk", s)  ||
         prefix("\\noteskip", s) ||
         prefix("\\multnoteskip", s ) )
    { 
      s = strchr (s+1, '\\');
      if (s == NULL) break;
    }
    else
    {
      char *tt;
      tt = strchr (s+1, '\\');
      if (tt == NULL) tt = t;
      while (s < tt)
      {  if (*s != '*')  /* equivalent to \sk */
           putc (*s, outfile); 
         s++; 
      }
    }
  }
  notes = t;
  if (debug > 0) 
   fprintf (stderr, "\nAfter output_notes, notes=%s\n", notes);
}

PRIVATE void
output_rests (bool output_bar)
{ /* outputs multi-bar rest and, possibly, a deferred \bar command */
  switch (restbars)
  { case 0: return; 
    case 1: 
      fprintf ( outfile, "\\NOTEs\\sk\\en\n" );
      fprintf ( outfile, "\\def\\atnextbar{\\znotes\\centerpause\\en}%%\n" );
      break;
    case 2: 
      fprintf ( outfile, "\\NOTesp\\sk\\en\n" );
      fprintf ( outfile, "\\def\\atnextbar{\\znotes\\centerbar{\\zcharnote{9}{\\meterfont\\lrlap2}}\\centerbar{\\PAuse}\\en}%%\n" );
      break;
    case 3:
      fprintf ( outfile, "\\NOTes\\hqsk\\sk\\en\n" );
      fprintf ( outfile, "\\def\\atnextbar{\\znotes\\centerbar{\\zcharnote{9}{\\meterfont\\lrlap3}}\\centerbar{\\PAuse\\off{2.5\\elemskip}\\pause}\\en}%%\n" );
      break;
    case 4:
      fprintf ( outfile, "\\NOtesp\\sk\\en\n" );
      fprintf ( outfile, "\\def\\atnextbar{\\znotes\\centerbar{\\zcharnote{9}{\\meterfont\\lrlap4}}\\centerbar{\\PAUSe}\\en}%%\n" );
      break;
    case 5:
      fprintf ( outfile, "\\NOTes\\hqsk\\sk\\en\n" );
      fprintf ( outfile, "\\def\\atnextbar{\\znotes\\centerbar{\\zcharnote{9}{\\meterfont\\lrlap5}}\\centerbar{\\PAUSe\\off{2.5\\elemskip}\\pause}\\en}%%\n" );
      break;
    case 6:
      fprintf ( outfile, "\\NOTes\\hqsk\\sk\\en\n" );
      fprintf ( outfile, "\\def\\atnextbar{\\znotes\\centerbar{\\zcharnote{9}{\\meterfont\\lrlap6}}\\centerbar{\\PAUSe\\off{2.5\\elemskip}\\PAuse}\\en}%%\n" );
      break;
    case 7:
      fprintf ( outfile, "\\NOTEs\\hqsk\\sk\\en\n" );
      fprintf ( outfile, "\\def\\atnextbar{\\znotes\\centerbar{\\zcharnote{9}{\\meterfont\\lrlap7}}\\centerbar{\\PAUSe\\off{2.5\\elemskip}\\PAuse\\off{2.5\\elemskip}\\pause}\\en}%%\n" );
      break;
    case 8:
      fprintf ( outfile, "\\NOTes\\hqsk\\sk\\en\n" );
      fprintf ( outfile, "\\def\\atnextbar{\\znotes\\centerbar{\\zcharnote{9}{\\meterfont\\lrlap8}}\\centerbar{\\PAUSe\\off{2.5\\elemskip}\\PAUSe}\\en}%%\n" );
      break;
    case 9:
      fprintf ( outfile, "\\NOTEs\\hqsk\\sk\\en\n" );
      fprintf ( outfile, "\\def\\atnextbar{\\znotes\\centerbar{\\zcharnote{9}{\\meterfont\\lrlap9}}\\centerbar{\\PAUSe\\off{2.5\\elemskip}\\PAUSe\\off{2.5\\elemskip}\\pause}\\en}%%\n" );
      break;
    default:  
      fprintf ( outfile, "\\NOTEs\\hqsk\\Hpause4{0.83}\\en\n" );
      fprintf ( outfile, "\\def\\atnextbar{\\znotes\\centerbar{\\zcharnote{9}{\\meterfont\\lrlap{%d}}}\\en}%%\n", restbars );
  }
  if (restbars > 1) 
    fprintf ( outfile, "\\advance\\barno%d%%\n", restbars-1 );
  restbars = 0; 
  if (output_bar) 
    fprintf ( outfile, "\\bar\n"); 
}

PRIVATE char *spacing_note (char *s, int new_spacing)
{ /* outputs from notes* to just after note at *s,
     if necessary, preceded by a new notes command;
     then updates notes.
  */
  char *t;
  if (debug > 0)
    fprintf (stderr, "\nspacing_note: spacing=%d new_spacing=%d s=%s\n", spacing, new_spacing, s);
  if (spacing != new_spacing)
  {
    if (spacing != 0) fprintf (outfile,"\\en\n");
    output_rests (true);
    switch (new_spacing)
    { /* start a new notes command  */
      case 128:fprintf (outfile,"%s", NNNOTES); break;
      case 64: fprintf (outfile,"%s", NNNOTES); break;
      case 32: fprintf (outfile,"%s", NNOTES); break;
      case 16: fprintf (outfile,"\\notes"); break;
      case 8:  fprintf (outfile,"\\Notes"); break;
      case 4:  fprintf (outfile,"\\NOtes"); break;
      case 2:  fprintf (outfile,"\\NOTes"); break;
      case 1:  fprintf (outfile,"\\NOTEs"); break;
      default: error ("Spacing value not recognized");
    }
    spacing = new_spacing;
  }
  t = strchr (s+1, '\\');
  if (t == NULL) t = s + strlen(s);
  output_notes (t);
  if (debug > 0)
    fprintf (stderr, "\nAfter spacing_note: spacing=%d notes=%s\n", spacing, notes);
  return notes;
}

PRIVATE char *semiautomatic_beam (char *s)
{
  char *t;
  if ( prefix ("\\Dqbb", s) 
    || prefix ("\\Tqbb", s)
    || prefix ("\\Qqbb", s) )
    beaming = 16; 
  else 
  if ( prefix ("\\Dqb", s)
    || prefix ("\\Tqb", s)
    || prefix ("\\Qqb", s) )
    beaming = 8; 
  if (beaming > spacing)
  {
    if (spacing != 0) fprintf (outfile, "\\en\n");
    output_rests (true);
    switch (beaming)
    {
      case 16: fprintf (outfile,"\\notes"); break;
      case 8:  fprintf (outfile,"\\Notes"); break;
      default: error ("Beaming value not recognized");
    }
    spacing = beaming;  
  }
  t = strchr (s+1, '\\');
  if (t == NULL) t = s + strlen(s);
  output_notes (t);
  return t;
}

PRIVATE char *beam_initiation (char *s) 
{ /* sets beaming, but doesn't output anything yet */
  char *t;
  if ( prefix ("\\ibbbbbb", s)
    || prefix ("\\Ibbbbbb", s)
    || prefix ("\\nbbbbbb", s) )
    beaming = 256; 
  else if ( prefix ("\\ibbbbb", s)
    || prefix ("\\Ibbbbb", s)
    || prefix ("\\nbbbbb", s) )
    beaming = 128; 
  else if ( prefix ("\\ibbbb", s)
    || prefix ("\\Ibbbb", s)
    || prefix ("\\nbbbb", s) )
    beaming = 64; 
  else if ( prefix ("\\ibbb", s) 
    || prefix ("\\Ibbb", s)
    || prefix ("\\nbbb", s) ) 
    beaming = 32; 
  else if ( prefix ("\\ibb", s) 
    || prefix ("\\Ibb", s)
    || prefix ("\\nbb", s) )
    beaming = 16; 
  else if ( prefix ("\\ib", s) 
    || prefix ("\\Ib", s) )
    beaming = 8; 
  t = strchr (s+1, '\\');
  if (t == NULL) t = s + strlen(s);
  return t;
}

PRIVATE char *beam_note (char *s)  /* \qb... */
{ char *t;
  if (debug > 0) 
    fprintf (stderr, "\nIn beam_note: spacing=%d beaming=%d new_beaming=%d\n",  
                             spacing,   beaming,   new_beaming);
  if (beaming > spacing)
  {
    if (spacing != 0) fprintf (outfile, "\\en\n");
    output_rests (true);
    switch (beaming)
    {
      case 256: case 128: case 64: 
               fprintf (outfile,"%s", NNNOTES); break;
      case 32: fprintf (outfile,"%s", NNOTES); break;
      case 16: fprintf (outfile,"\\notes"); break;
      case 8:  fprintf (outfile,"\\Notes"); break;
      default: error ("Beaming value not recognized");
    }
    spacing = beaming;  
  }
  t = strchr (s+1, '\\');
  if (t == NULL) t = s + strlen (s);
  output_notes (t);
  if (new_beaming != 0)  /* set by preceding \tb... */
  {
    beaming = new_beaming; new_beaming = 0;
    if (*notes != '\0' && beaming != spacing)
    { 
      if (spacing != 0) fprintf (outfile,"\\en\n");
      switch (beaming)
      {
        case 256: case 128: case 64: 
                 fprintf (outfile,"%s", NNNOTES); break;
        case 32: fprintf (outfile,"%s", NNOTES); break;
        case 16: fprintf (outfile,"\\notes"); break;
        case 8:  fprintf (outfile,"\\Notes"); break;
        default: error ("Beaming value not recognized");
      }
      spacing = beaming;
    }
  }
  return t;
}

PRIVATE char *beam_termination (char *s)  /* \tb... */
{ char *t;
  if (debug > 0)
    fprintf (stderr, "\nIn beam_termination: spacing=%d beaming=%d new_beaming=%d\n",  
                                    spacing,   beaming,   new_beaming);
  if (spacing == 0)
  {
    output_rests (true);
    switch (beaming)
    {
      case 256: case 128: case 64: 
               fprintf (outfile,"%s", NNNOTES); break;
      case 32: fprintf (outfile,"%s", NNOTES); break;
      case 16: fprintf (outfile,"\\notes"); break;
      case 8:  fprintf (outfile,"\\Notes"); break;
      default: error ("Beaming value not recognized");
    }
    spacing = beaming;  
  }
  if ( prefix ("\\tbbbbbb", s) )
    new_beaming = 128;
  else if ( prefix ("\\tbbbbb", s) )
    new_beaming = 64;
  else if ( prefix ("\\tbbbb", s) )
    new_beaming = 32;
  else if ( prefix ("\\tbbb", s) )
    new_beaming = 16;
  else if ( prefix ("\\tbb", s) ) 
    new_beaming = 8;
  else if ( prefix ("\\tb", s) )
  { new_beaming = 0; }

  t = strchr (s+1, '\\');
  if (t == NULL) t = s + strlen(s);
  return t;
}

PRIVATE char *beam_completion (char *s)  /* \tq... */
{ char *t;
  if (debug > 0)
    fprintf (stderr, "\nIn beam_completion: spacing=%d beaming=%d new_beaming=%d\n",  
                                   spacing,   beaming,   new_beaming);
  if (beaming > spacing)
  {
    if (spacing != 0) fprintf (outfile, "\\en\n");
    output_rests (true);
    switch (beaming)
    {
      case 256: case 128: case 64:
               fprintf (outfile,"%s", NNNOTES); break;
      case 32: fprintf (outfile,"%s", NNOTES); break;
      case 16: fprintf (outfile,"\\notes"); break;
      case 8:  fprintf (outfile,"\\Notes"); break;
      default: error ("Beaming value not recognized");
    }
    spacing = beaming;  
  }
  t = strchr (s+1, '\\');
  if (t == NULL) t = s + strlen(s);
  output_notes (t);
  new_beaming = 0; spacing = 8;
  return t;
}

PRIVATE char *nonspacing_beam_termination (char *s)   /*  \ztq...  */
{ char *t;
  if (spacing == 0)
  {
    output_rests (true);
    fprintf (outfile,"\\znotes"); 
    spacing = beaming;
  }
  t = strchr (s+1, '\\');
  if (t == NULL) t = s + strlen(s);
  output_notes (t);
  new_beaming = 0;
  return t;
}

PRIVATE char  *tiny (char *s) 
/* \tinynotesize  */
{
  char *t;
  if (spacing > 32) 
  {
    if (spacing != 0) fprintf (outfile,"\\en\n");
    /*  for appoggiatura  */
    fprintf (outfile, NNNOTES);
  }
  spacing = 32;
  t = s + strlen(s);
  output_notes (t);
  return t;
}

PRIVATE char *simple (char *s)
/* detect and correct simple slurs and ties over > 1 note  */
{ char *t;
  t = strchr (s+1, '\\');
  if (t == NULL) t = s + strlen(s);
  if (*(t-1) != '1')
  {
    char msg[LINE_LEN];
    sprintf (msg, "simple slur or tie with n == %c changed to 1.", *(t-1));
    warning (msg);
    *(t-1) = '1';
  }
  return t;
}

PRIVATE void
synthesize_notes (void)
/* generate well-spaced notes */
{ char *s; 
  s = notes;
  while (true) 
  { 
    if (debug > 0)
      fprintf (stderr, "\nIn synthesize s=%s\n", s);
    if (*s == '\0') 
    {      
      if (spacing == 0) 
      {  if (*notes == '\0') return; /* to avoid \znotes\en */
         fprintf (outfile,"\\znotes");
      }
      output_notes (s);
      fprintf (outfile,"\\en"); 
      spacing = 0; 
      return; 
    }

    if ( prefix ("\\h", s) 
       && !prefix ("\\hs", s) 
       && !prefix ("\\hqsk", s) 
       && !prefix ("\\hroff", s) 
       && !prefix ("\\hloff", s) )
      s = spacing_note (s, 2); 

    else if ( prefix ("\\hs", s) 
       && !prefix ("\\hsk", s) ) 
      s = spacing_note (s, 32);

    else if ( prefix ("\\q", s)
       && !prefix ("\\qb", s) 
       && !prefix ("\\qs", s) 
       && !prefix ("\\qqsk", s) ) 
      s = spacing_note (s, 4);

    else if ( prefix ("\\qs", s)
       && !prefix ("\\qsk", s) ) 
      s = spacing_note (s, 16); 

    else if ( prefix("\\qb", s) ) 
      s = beam_note (s); 

    else if ( prefix ("\\ccccc", s) )
      s = spacing_note (s, 128);

    else if ( prefix ("\\cccc", s) )
      s = spacing_note (s, 64);

    else if ( prefix ("\\ccc", s) )
      s = spacing_note (s, 32);

    else if ( prefix ("\\cc", s) 
       && !prefix ("\\ccharnote", s) ) 
      s = spacing_note (s, 16); 

    else if ( prefix ("\\c", s) 
       && !prefix ("\\cna", s)  
       && !prefix ("\\csh", s)  
       && !prefix ("\\cfl", s) 
       && !prefix ("\\curve", s) 
       && !prefix ("\\ccharnote", s) ) 
      s = spacing_note (s, 8);

    else if ( prefix ("\\ds", s ) 
       && !prefix ("\\dsh", s) ) 
      s = spacing_note (s, 8);

    else if ( prefix ("\\wh", s)
       || prefix ("\\pa", s)  
       || prefix ("\\breve", s) )  
      s = spacing_note (s, 1);

    else if ( prefix ("\\Hpause", s) ) 
      s = spacing_note (s, 2);

    else if ( prefix("\\ib",s) 
       || prefix("\\Ib",s) 
       || prefix("\\nb",s) ) 
      s = beam_initiation (s); 

    else if ( prefix("\\Dq",s) 
       || prefix("\\Tq",s) 
       || prefix("\\Qq",s) )
      s = semiautomatic_beam (s); 

    else if ( prefix("\\tb", s) ) 
      s = beam_termination (s);

    else if ( prefix("\\tq", s) ) 
      s = beam_completion (s);

    else if ( prefix("\\ztq", s) ) 
      s = nonspacing_beam_termination (s); 

    else if ( prefix("\\tinynotesize", s)) 
      s = tiny (s);

    else if ( prefix("\\slur", s)
       || prefix("\\tie", s)
       || prefix("\\sslur", s)
       || prefix("\\bslur", s) )
      s = simple (s);
    
    else if ( prefix("\\zchar", s) 
       || prefix("\\lchar", s)
       || prefix("\\cchar", s) )
    { char *t; /* need to skip two arguments */
      s = strchr (s+1, '}'); /* first }  */
      s = strchr (s+1, '}'); /* second } */
      t = strchr(s+1,'\\');
      if (t == NULL) t = s + strlen(s); 
      s = t;
    }
      
    else /* non-spacing; skip to next command */
    { char *t; 
      t = strchr(s+1,'\\'); 
      if (t == NULL) t = s + strlen(s); 
      s = t; 
    }
  }
}

PRIVATE void
process_command (char **ln)
{ char *s, *t;

  if ( prefix("\\def\\vnotes#1\\elemskip", *ln) ) 
  { /* determine staff */
    s = strstr (*ln, "@vnotes") + 7;
    t = strchr(s, '#'); /* may have commands before # */
    if (sscanf (t, "#%d", &staff) != 1) error("sscanf for staff fails");
    staff--; 
    fprintf (outfile,"\\def\\vnotes#1\\elemskip#2\\en{\\noteskip#1\\@l@mskip\\@vnotes");
    while (s != t)  
    { putc(*s, outfile); s++; }
    fprintf (outfile,"#2\\enotes}");
    *ln = strchr (*ln, '}') ;
    (*ln)++;
  }

  else if ( prefix("\\TransformNotes", *ln) ) 
  { /* determine staff */
    s = strchr (*ln, '}');
    if (s == NULL) error ("Can't parse \\TransformNotes.");
    s = strchr (s+1, '{');
    if (s == NULL) error ("Can't parse \\TransformNotes.");
    s++;
    t = strchr(s, '#'); /* may have \transpose... before # */
    if (sscanf (t, "#%d", &staff) != 1) error("sscanf for staff fails");
    staff--; 
    fprintf (outfile,"\\TransformNotes{#2}{");
    while (s != t)
    { putc(*s, outfile); s++; }
    fprintf (outfile,"#2}");
    *ln = strchr (t, '}') ;
    if (*ln == NULL) error ("Can't find '}' after \\TransformNotes.\n");
    (*ln)++;
  }

  else if ( prefix("\\nnotes", *ln) ||  /* non-standard */
            prefix("\\notes", *ln) || 
            prefix("\\Notes", *ln) ||
            prefix("\\NOtes", *ln) ||
            prefix("\\NOTes", *ln) ||
            prefix("\\NOTEs", *ln) ||
            prefix("\\NOTES", *ln) )
  { /*  spaced notes */
    char *save_notes;
    notes = analyze_notes (ln);
    save_notes = notes;
    synthesize_notes ();
    t = strpbrk (*ln, "%\\\n");
    *ln = t;
    if (notes == save_notes) 
      (*ln)++;  /* avoids blank line if no notes  */
  }

  else if ( prefix("\\vnotes", *ln) )
  { 
    char *s; char *t;
    s = *ln+7;
    t = strchr ( s, '\\');
    *ln = t; /* skip to \elemskip */
    notes = analyze_notes (ln);
    if ( strstr (notes, "\\tinynotesize") != NULL )
    { /* appoggiatura note  */
      output_rests (true);
      fprintf (outfile,"\\vnotes");
      while (s!=t) 
      { putc(*s, outfile); s++; } 
      fprintf (outfile,"\\elemskip%s\\en", notes);
      t = strpbrk (*ln, "%\\\n");
      if (t == NULL) t = *ln + strlen (*ln);
      *ln = t;
    }
    else
    { char *save_notes;
      save_notes = notes;
      synthesize_notes ();
      t = strpbrk (*ln, "%\\\n");
      *ln = t;
      if (notes == save_notes) 
        (*ln)++;  /* avoids blank line if no notes  */
    }
  }

  else if ( prefix("\\znotes", *ln) )
  { /*  non-spacing material */
    notes = analyze_notes (ln);
    fprintf (outfile,"\\znotes%s\\en", notes);
    t = strpbrk (*ln, "%\\\n");
    if (t == NULL) t = *ln + strlen (*ln);
    *ln = t;
  }

  else if ( prefix("\\def\\atnextbar{\\znotes", *ln))
  { /*  whole-bar or multi-bar rest? */
    *ln = *ln + 15;
    notes = analyze_notes(ln);
    if (strstr (notes, "\\centerpause") != NULL )
    { restbars++;   /* defer output until after a   */
                    /* possible multi-bar rest      */
    }
    else
    {
      if (*notes != '\0') 
      { fprintf (outfile,"\\def\\atnextbar{\\znotes%s\\en}", notes);}
    }
    t = strchr (*ln, '}');
    if (t == NULL) error ("Can't find }.");
    *ln = t+1;
    t = strchr (*ln, '\\');
    if (t == NULL ) t = *ln + strlen(*ln);
    *ln = t;
  }

  else if ( (prefix("\\bar", *ln) && !prefix("\\barno", *ln) ) 
         || prefix("\\doublebar", *ln)
         || prefix("\\ala", *ln) )
       { 
         if ( restbars > 0 ) 
         { /* defer output till next \Notes bar */ 
           t = strpbrk (*ln+1, "%\\");
           if (t == NULL) t = *ln + strlen (*ln); 
         }
         else 
         {
           if ( prefix("\\doublebar", *ln) )
             fprintf (outfile, "\\doublebar");
           else
             fprintf (outfile, "\\bar");
           t = strpbrk (*ln+1, "%\\\n");
           if (t == NULL) t = *ln + strlen (*ln); 
         }
         *ln = t;
       }

  else if ( prefix("\\Endpiece", *ln) 
        ||  prefix("\\endpiece", *ln) )
  {  
    output_rests (false);
    fprintf (outfile,"%s", *ln);
    *ln = *ln + strlen (*ln);
  }

  else if ( prefix("\\instrumentnumber", *ln) )
  { fprintf (outfile, "\\instrumentnumber1\n");
    t = strpbrk (*ln+1, "%\\");
    if (t == NULL) 
    { *ln = *ln + strlen (*ln); fprintf (outfile, "\n"); return; }
    *ln = t;
  }

  else if ( prefix("\\startpiece", *ln) )
  { fprintf (outfile, "%s", "\\setstaffs11\n\\nostartrule\n\\startpiece");
    t = strpbrk (*ln+1, "%\\\n");
    if (t == NULL) t = *ln + strlen (*ln);
    *ln = t;
  }

  else if ( prefix("\\mulooseness", *ln) 
         || prefix("\\eject", *ln)
         || prefix("\\linegoal", *ln)  
         || prefix("\\song", *ln)
         || prefix("\\group", *ln) 
         || prefix("\\akkoladen", *ln) )
  { fputc ('%', outfile);
    t = strpbrk (*ln+1, "%\\\n");
    if (t == NULL) 
    { fprintf (outfile, "%s", *ln); return; }
    while (*ln < t) { fputc (**ln, outfile); (*ln)++; } 
    fputc ('\n', outfile);
  }

  else if ( prefix("\\def", *ln) )
  { char msg[LINE_LEN];
    sprintf (msg, "Macro definition ignored: %s", *ln);
    fprintf (stderr, "Warning: %s", msg);
    t = *ln + strlen(*ln);
    while (*ln < t)
    { fputc (**ln, outfile); 
      (*ln)++;
    }
  }

  else  /* everything else */
  { t = strpbrk (*ln+1, "%\\");
    if (t == NULL || *t == '%') t = *ln + strlen (*ln);
    while (*ln < t) 
    { fputc (**ln, outfile); 
      (*ln)++;
    }
  }
}

PRIVATE void
process_line (void)
{
  char  *ln; 
  ln = &line[0];
  while ( *ln != '\0') 
  {
    while (*ln == ' ') { ln++; fputc (' ', outfile); }
    if (*ln == '%') {
      fprintf (outfile, "%s", ln);
      return;
    }
    process_command (&ln);
  }     /* *ln == '\0'  */
}

PRIVATE void
process_score (void)
/* process .tex file */
{
  int c;
  while ( (c = getc (infile)) != EOF )
  {
    ungetc (c, infile);
    if ( fgets(line, LINE_LEN, infile) == NULL) error("IO error");
    if (strlen (line) == LINE_LEN-1) error("Line too long.");
    process_line ();
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
  c = getopt_long (argc, argv, "hvd", longopts, NULL);
  while (c != -1)
    {
      switch (c)
        {
        case 'h':
          fprintf (stdout, "This is fixmsxpart, version %s.\n", version);
          usage (stdout);
          fprintf (stdout, "Please report bugs to rdt@cs.queensu.ca.\n" );
          exit (0);
        case 'v':
          fprintf (stdout, "This is fixmsxpart, version %s.\n", version);
          exit (0);
        case 'd':
          debug++; break;
        case '?':
          exit (EXIT_FAILURE);
        default:
          fprintf (stderr,"Function getopt returned character code 0%o.\n",
                  (unsigned int) c);
          exit (EXIT_FAILURE);
        }
      c = getopt_long (argc, argv, "hvd", longopts, NULL);
    }
  fprintf (stderr, "This is fixmsxpart, version %s.\n", version);
  fprintf (stderr, "Copyright (C) 2014-15  R. D. Tennent\n" );
  fprintf (stderr, "School of Computing, Queen's University, rdt@cs.queensu.ca\n" );
  fprintf (stderr, "License GNU GPL version 2 or later <http://gnu.org/licences/gpl.html>.\n" );
  fprintf (stderr, "There is NO WARRANTY, to the extent permitted by law.\n\n" );

  infilename[0] = '\0';
  infilename_n = infilename;
  if ( (optind < argc) && (optind+2 >= argc))
    {
      append (infilename, &infilename_n, argv[optind], sizeof (infilename));
      if (!suffix (".tex", infilename))
      append (infilename, &infilename_n, ".tex", sizeof (infilename));
    }
  else 
  {  usage (stderr);
     exit (EXIT_FAILURE);
  }
  optind++;
  outfilename[0] = '\0';
  outfilename_n = outfilename;
  if (optind < argc)  /* user-provided outfilename */
    {
      append (outfilename, &outfilename_n, argv[optind], sizeof (outfilename));
      if (!suffix (".tex", outfilename))
        append (outfilename, &outfilename_n, ".tex", sizeof (outfilename));
    }
  infile = fopen (infilename, "r");
  if (infile == NULL)
    {
      fprintf (stderr,"Can't open %s\n", infilename);
      exit (EXIT_FAILURE);
    }
  fprintf (stderr, "Reading from %s.", infilename);
  if (*outfilename == '\0')
  {
    outfile = stdout;
    fprintf (stderr, " Writing to stdout.\n");
  }
  else
  {
    outfile = fopen (outfilename, "w");
    if (outfile == NULL)
    {
      fprintf (stderr,"Can't open %s\n", outfilename);
      exit (EXIT_FAILURE);
    }
    fprintf (stderr, " Writing to %s.\n", outfilename);
  }
  fprintf (stderr,"\n");

  fprintf (outfile, "%% Generated on %s by fixmsxpart (%s).\n", today, version);
  process_score ();

  return 0;
}
