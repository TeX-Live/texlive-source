/* outocp.c: ASCII output for OCP files, mainly for debugging purposes.

This file is part of Omega,
which is based on the web2c distribution of TeX.

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous
Copyright (C) 2005  Roozbeh Pournader

Omega is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Omega is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Omega; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*/

#include <kpathsea/config.h>
#include <kpathsea/types.h>
#include <kpathsea/c-std.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/tex-file.h>
#include "otp.h"

const char *otp_names[] = {
  "                   ",
  "OTP_RIGHT_OUTPUT   ",
  "OTP_RIGHT_NUM      ",
  "OTP_RIGHT_CHAR     ",
  "OTP_RIGHT_LCHAR    ",
  "OTP_RIGHT_SOME     ",
  "OTP_PBACK_OUTPUT   ",
  "OTP_PBACK_NUM      ",
  "OTP_PBACK_CHAR     ",
  "OTP_PBACK_LCHAR    ",
  "OTP_PBACK_SOME     ",
  "OTP_ADD            ",
  "OTP_SUB            ",
  "OTP_MULT           ",
  "OTP_DIV            ",
  "OTP_MOD            ",
  "OTP_LOOKUP         ",
  "OTP_PUSH_NUM       ",
  "OTP_PUSH_CHAR      ",
  "OTP_PUSH_LCHAR     ",
  "OTP_STATE_CHANGE   ",
  "OTP_STATE_PUSH     ",
  "OTP_STATE_POP      ",
  "OTP_LEFT_START     ",
  "OTP_LEFT_RETURN    ",
  "OTP_LEFT_BACKUP    ",
  "OTP_GOTO           ",
  "OTP_GOTO_NE        ",
  "OTP_GOTO_EQ        ",
  "OTP_GOTO_LT        ",
  "OTP_GOTO_LE        ",
  "OTP_GOTO_GT        ",
  "OTP_GOTO_GE        ",
  "OTP_GOTO_NO_ADVANCE",
  "OTP_GOTO_BEG       ",
  "OTP_GOTO_END       ",
  "OTP_STOP           "
};


int no_words_read = 0;

static int
ctp_get(FILE *input_file)
{
  int ctpchar;
  ctpchar = getc (input_file);
  if (ctpchar == EOF)
    {
      FATAL ("Unexpected end of file");
    }
  return ctpchar;
}

static int
ctp_read(FILE *input_file)
{
  int ctpword;
  no_words_read++;
  ctpword = ctp_get (input_file);
  if (ctpword > 127)
    {
      FATAL ("First octet > 127");
    }
  ctpword = ctpword * 256 + ctp_get (input_file);
  ctpword = ctpword * 256 + ctp_get (input_file);
  ctpword = ctpword * 256 + ctp_get (input_file);
  return ctpword;
}

static void
ctp_explain(FILE *input_file)
{
  int i, j, arg, entry, instruction;
  int ctp_length, real_ctp_length, ctp_input, ctp_output,
    ctp_no_tables, ctp_room_tables, ctp_no_states, ctp_room_states;
  int room_tables[OTP_MAXTABLES], room_states[OTP_MAXSTATES];

  ctp_length = ctp_read (input_file);
  fprintf (stdout, "ctp_length     : %3x(%3d)\n", ctp_length, ctp_length);
  real_ctp_length = ctp_length - 7;
  fprintf (stdout, "real_ctp_length: %3x(%3d)\n",
	   real_ctp_length, real_ctp_length);
  ctp_input = ctp_read (input_file);
  fprintf (stdout, "ctp_input      : %3x(%3d)\n", ctp_input, ctp_input);
  ctp_output = ctp_read (input_file);
  fprintf (stdout, "ctp_output     : %3x(%3d)\n", ctp_output, ctp_output);
  ctp_no_tables = ctp_read (input_file);
  fprintf (stdout, "ctp_no_tables  : %3x(%3d)\n",
	   ctp_no_tables, ctp_no_tables);
  ctp_room_tables = ctp_read (input_file);
  fprintf (stdout, "ctp_room_tables: %3x(%3d)\n",
	   ctp_room_tables, ctp_room_tables);
  ctp_no_states = ctp_read (input_file);
  fprintf (stdout, "ctp_no_states  : %3x(%3d)\n",
	   ctp_no_states, ctp_no_states);
  ctp_room_states = ctp_read (input_file);
  fprintf (stdout, "ctp_room_states: %3x(%3d)\n\n",
	   ctp_room_states, ctp_room_states);
  if (ctp_no_tables >= OTP_MAXTABLES)
    {
      FATAL ("Too many tables");
    }
  if (ctp_no_states >= OTP_MAXSTATES)
    {
      FATAL ("Too many states");
    }
  if (ctp_no_tables != 0)
    {
      for (i = 0; i < ctp_no_tables; i++)
	{
	  room_tables[i] = ctp_read (input_file);
	  fprintf (stdout, "Table %2x(%2d): %3x(%3d)entries\n",
		   i, i, room_tables[i], room_tables[i]);
	}
      fprintf (stdout, "\n");
      for (i = 0; i < ctp_no_tables; i++)
	{
	  for (j = 0; j < room_tables[i]; j++)
	    {
	      entry = ctp_read (input_file);
	      fprintf (stdout, "Table %2x(%2d), entry %3x(%3d): %2x(%3d)\n",
		       i, i, j, j, entry, entry);
	    }
	}
      fprintf (stdout, "\n");
    }
  if (ctp_no_states != 0)
    {
      for (i = 0; i < ctp_no_states; i++)
	{
	  room_states[i] = ctp_read (input_file);
	  fprintf (stdout, "State %2x(%2d): %3x(%3d) entries\n",
		   i, i, room_states[i], room_states[i]);
	}
      fprintf (stdout, "\n");
      for (i = 0; i < ctp_no_states; i++)
	{
	  for (j = 0; j < room_states[i]; j++)
	    {
	      instruction = ctp_read (input_file);
	      arg = instruction & 0xffffff;
	      if ((arg >= 32) && (arg < 127))
		fprintf (stdout,
			 "State %2x(%2d), entry %3x(%3d): %s %2x(%3d,`%c')\n",
			 i, i, j, j, otp_names[instruction >> 24],
			 arg, arg, arg);
	      else
		fprintf (stdout,
			 "State %2x(%2d), entry %3x(%3d): %s %2x(%3d)\n",
			 i, i, j, j, otp_names[instruction >> 24], arg, arg);
	    }
	}
    }
  fprintf (stdout, "\nfile length should be: %3x(%3d)\n",
	   ctp_length, ctp_length);
  fprintf (stdout, "number words read    : %3x(%3d)\n",
	   no_words_read, no_words_read);
}

int
main(int argc, string *argv)
{
  string input_name, full_input_name;
  FILE *input_file;

  kpse_set_program_name (argv[0], "outocp");
  switch (argc)
    {
    case 1:
      FATAL ("No file given");
    case 2:
      input_name = argv[1];
      break;
    default:
      FATAL ("Too many arguments");
    }
  full_input_name = kpse_find_file (input_name, kpse_ocp_format, true);
  if (!full_input_name)
    {
      FATAL1 ("%s not found", input_name);
    }
  input_file = xfopen (full_input_name, FOPEN_RBIN_MODE);
  ctp_explain (input_file);

  return EXIT_SUCCESS;
}
