#include "config.h"

/* The entry point: set up for reading the command line, which will
   happen in `topenin', then call the main body.  Public domain. */

extern TEXDLL void mainbody (void);
extern TEXDLL void maininit (int, string *);

int
main (int ac, string *av)
{
#ifdef __EMX__
  _wildcard (&ac, &av);
  _response (&ac, &av);
#endif

#if defined(WIN32)
  /* if _DEBUG is not defined, these macros will result in nothing. */
   SETUP_CRTDBG;
   /* Set the debug-heap flag so that freed blocks are kept on the
    linked list, to catch any inadvertent use of freed memory */
   SET_CRT_DEBUG_FIELD( _CRTDBG_DELAY_FREE_MEM_DF );
#endif

  maininit(ac, av);

  /* Call the real main program.  */
  mainbody ();
  return EXIT_SUCCESS;
} 
