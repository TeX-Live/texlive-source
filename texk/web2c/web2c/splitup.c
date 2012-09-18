/* splitup -- take TeX or MF in C as a single stream on stdin,
   and it produces several .c and .h files in the current directory
   as its output.

   $Id$

   Tim Morgan  September 19, 1987.  */

#include <w2c/config.h>
#include <kpathsea/getopt.h>

#if defined (FATAL)
#undef FATAL
#endif

#define FATAL(str) do {                                                 \
        fprintf (stderr, "%s: fatal: ", argv[0]);                       \
        fputs (str, stderr);                                            \
        fputs (".\n", stderr); exit (1); } while (0)


#if defined (FATAL1)
#undef FATAL1
#endif

#define FATAL1(str, e1) do {                                            \
        fprintf (stderr, "%s: fatal: ", argv[0]);                       \
        fprintf (stderr, str, e1);                                      \
        fputs (".\n", stderr); exit (1); } while (0)


#ifdef VMS
#define unlink delete
#endif

int filenumber = 0, ifdef_nesting = 0, lines_in_file = 0;
char *output_name = NULL;
boolean has_ini;

/* This used to be a fixed 2000, but since bibtex.c is almost 10000 lines
   (200+K), we may as well decrease the number of split files we create.
   Probably faster for the compiler, definitely faster for the linker,
   simpler for the Makefiles, and generally better.  Now we specify this
   in 'convert'. */
long int max_lines;

/* Do we split out a separate *ini.c file? */
boolean do_ini;

/* Don't need long filenames, since we generate them all.  */
char buffer[1024], tempfile[100], filename[100], ini_name[100];

FILE *out, *ini, *temp;

/*
 * Read a line of input into the buffer, returning `false' on EOF.
 * If the line is of the form "#ifdef INI...", we set "has_ini"
 * `true' else `false'.  We also keep up with the #ifdef/#endif nesting
 * so we know when it's safe to finish writing the current file.
 */
static int
read_line (void)
{
  if (fgets (buffer, sizeof (buffer), stdin) == NULL)
    return false;
  if (strncmp (buffer, "#ifdef", 6) == 0
      || strncmp (buffer, "#ifndef", 7) == 0)
    {
      ++ifdef_nesting;
      if (strncmp (&buffer[7], "INI", 3) == 0)
	has_ini = true;
    }
  else if (strncmp (buffer, "#endif", 6) == 0)
    --ifdef_nesting;
  return true;
}

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

int
main (int argc, string *argv)
{
  const_string coerce;
  unsigned coerce_len;
  int option;

#ifdef WIN32
  setmode(fileno(stdout), _O_BINARY);
#endif

  while ((option = getopt(argc, argv, "il:")) != -1) {
    switch (option) {
    case 'i':
      do_ini = true;
      break;
    case 'l':
      max_lines = atoi(optarg);
      if (max_lines <= 0)
        FATAL("[-i] [-l lines] name");
      break;
    default:
      FATAL("[-i] [-l lines] name");
      break;
    }
  }
  if (optind + 1 != argc)
    FATAL("[-i] [-l lines] name");
  output_name = argv[optind];

  sprintf (filename, "%sd.h", output_name);
  sprintf (tempfile, "%s.tmp", output_name);
  out = xfopen (filename, FOPEN_W_MODE);
  fputs ("#undef TRIP\n#undef TRAP\n", out);
  /* We have only one binary that can do both ini stuff and vir stuff.  */
  fputs ("#define STAT\n#define INI\n", out);
  
  if (STREQ (output_name, "mf")) {
    fputs ("#define INIMF\n#define MF\n", out);
    coerce = "mfcoerce.h";
  } else if (STREQ (output_name, "tex")) {
    fputs ("#define INITEX\n#define TeX\n#define onlyTeX\n", out);
    coerce = "texcoerce.h";
  } else if (STREQ (output_name, "aleph")) {
    fputs ("#define INITEX\n#define TeX\n#define Aleph\n", out);
    coerce = "alephcoerce.h";
  } else if (STREQ (output_name, "etex")) {
    fputs ("#define INITEX\n#define TeX\n#define eTeX\n", out);
    coerce = "etexcoerce.h";
  } else if (STREQ (output_name, "pdftex")) {
    fputs ("#define INITEX\n#define TeX\n#define pdfTeX\n", out);
    coerce = "pdftexcoerce.h";
  } else if (STREQ (output_name, "ptex")) {
    fputs ("#define INITEX\n#define TeX\n#define pTeX\n", out);
    coerce = "ptexcoerce.h";
  } else if (STREQ (output_name, "eptex")) {
    fputs ("#define INITEX\n#define TeX\n#define epTeX\n", out);
    coerce = "eptexcoerce.h";
  } else if (STREQ (output_name, "euptex")) {
    fputs ("#define INITEX\n#define TeX\n#define eupTeX\n", out);
    coerce = "euptexcoerce.h";
  } else if (STREQ (output_name, "uptex")) {
    fputs ("#define INITEX\n#define TeX\n#define upTeX\n", out);
    coerce = "uptexcoerce.h";
  } else if (STREQ (output_name, "xetex")) {
    fputs ("#define INITEX\n#define TeX\n#define XeTeX\n", out);
    coerce = "xetexcoerce.h";
  } else
    FATAL1 ("Can only split mf, tex, aleph, eptex, euptex, etex, pdftex, ptex, uptex, or xetex,\n not %s", output_name);
  
  coerce_len = strlen (coerce);
  
  /* Read everything up to coerce.h.  */
  while (fgets (buffer, sizeof (buffer), stdin))
    {
      if (strncmp (&buffer[10], coerce, coerce_len) == 0)
	break;

      if (buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == '}'
	  || buffer[0] == '/' || buffer[0] == ' '
	  || strncmp (buffer, "typedef", 7) == 0)
	/*nothing */ ;
      else
	fputs ("EXTERN ", out);

      fputs (buffer, out);
    }

  if (strncmp (&buffer[10], coerce, coerce_len) != 0)
    FATAL1 ("No #include %s line", coerce);

  fputs (buffer, out);
  xfclose (out, filename);

  if (do_ini) {
    sprintf (ini_name, "%sini.c", output_name);
    ini = xfopen (ini_name, FOPEN_W_MODE);
    fputs ("#define EXTERN extern\n", ini);
    fprintf (ini, "#include \"%sd.h\"\n\n", output_name);
  }

  sprintf (filename, "%s0.c", output_name);
  out = xfopen (filename, FOPEN_W_MODE);
  fputs ("#define EXTERN extern\n", out);
  fprintf (out, "#include \"%sd.h\"\n\n", output_name);

  do
    {
      /* Read one routine into a temp file */
      has_ini = false;
      temp = xfopen (tempfile, "wb+");

      while (read_line ())
	{
	  fputs (buffer, temp);
	  if (buffer[0] == '}')
	    break;		/* End of procedure */
	}
      while (ifdef_nesting > 0 && read_line ())
	fputs (buffer, temp);
      rewind (temp);

      if (do_ini && has_ini)
	{			/* Contained "#ifdef INI..." */
	  while (fgets (buffer, sizeof (buffer), temp))
	    fputs (buffer, ini);
	}
      else
	{			/* Doesn't contain "#ifdef INI..." */
	  while (fgets (buffer, sizeof (buffer), temp))
	    {
	      fputs (buffer, out);
	      lines_in_file++;
	    }
	}
      xfclose (temp, tempfile);

      /* Switch to new output file.  */
      if (max_lines && lines_in_file > max_lines)
	{
	  xfclose (out, filename);
	  sprintf (filename, "%s%d.c", output_name, ++filenumber);
	  out = xfopen (filename, FOPEN_W_MODE);
	  fputs ("#define EXTERN extern\n", out);
	  fprintf (out, "#include \"%sd.h\"\n\n", output_name);
	  lines_in_file = 0;
	}
    }
  while (!feof (stdin));

  xfclose (out, filename);
  if (lines_in_file == 0)
    unlink (filename);

  if (do_ini)
    xfclose (ini, ini_name);

  if (unlink (tempfile)) {
      perror (tempfile);
      exit (EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
