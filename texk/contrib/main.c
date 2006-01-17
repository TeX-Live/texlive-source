
#include "mktex.h"
#include "stackenv.h"
#include "variables.h"
#include "fileutils.h"

void __cdecl output_and_cleanup(int code)
{
  FILE *f;
  string line;

  if (code == 0) {
    /* output result if any */
    if ((f = fopen(getval("STDOUT"), "r")) != NULL) {
      if (redirect_stdout >= 0) {
	int fds[3];

	fds[0] = 0;
	fds[1] = redirect_stdout;
	fds[2] = 2;
	push_fd(fds);
      }
      while((line = read_line(f)) != NULL) {
	fputs(line, stdout);
	free (line);
      }
      fclose(f);
      if (redirect_stdout >= 0) {
	pop_fd();
	redirect_stdout = -1;
      }
    }
  }

  if (test_file('d', tmpdir))
    rec_rmdir(tmpdir);

  {
      int count = 0;
      /* Give more than one chance to remove the files */
      while ((count++ < 20) && test_file('d', tmpdir)) {
	Sleep(250);
       rec_rmdir(tmpdir);
      }
  }
}

int main(int argc, char* argv[])
{
  program_description * program = 0;
  int i, errstatus;
#if defined(WIN32)
  /* if _DEBUG is not defined, these macros will result in nothing. */
   SETUP_CRTDBG;
   /* Set the debug-heap flag so that freed blocks are kept on the
    linked list, to catch any inadvertent use of freed memory */
   SET_CRT_DEBUG_FIELD( _CRTDBG_DELAY_FREE_MEM_DF );
#endif

   redirect_stdout = -1;
   output_and_cleanup_function = output_and_cleanup;

#if 0
  extern MKTEXDLL string (* var_lookup)(const_string);
  var_lookup = getval;
#endif

  mktexinit(argc, argv);

  for(i = 0; makedesc[i].name; ++i) {
    if (FILESTRCASEEQ(kpse_program_name, makedesc[i].name)) {
      program_number = i;
      progname = makedesc[i].name;
      program = makedesc+i;
      break;
    }
  }
  if (!makedesc[i].name) {
    fprintf(stderr, "This program was incorrectly copied to the name %s\n", 
	    argv[0]);
    return 1;
  }

  /* mktex_opt may modify argc and shift argv */
  argc = mktex_opt(argc, argv, program);

  errstatus = program->prog(argc, argv);

  mt_exit(errstatus);
  return 0;			/* Not reached !!! */
}
