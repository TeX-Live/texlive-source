#ifndef _STACKENV_H_

#define _STACKENV_H_

#include "mktexlib.h"

/*
  We are keeping trace of the environment (ie: cwd, file redirections)
  with the help of these ops and structures. There is a global stack
  inidcating wich actions have been taken.
  */
typedef enum { CHDIR = 1, REDIRECT } op_env;
typedef struct mod_env {
  op_env op;
  union {
  char *path;
  int oldfd[3];
  } data;
} mod_env;

/* from stackenv.c */
extern void oops(const char *, ...);
#ifdef __GNUC__
extern void mt_exit(int) __attribute__((noreturn));
#else
extern MKTEXDLL void mt_exit(int);
#endif
extern MKTEXDLL void pushd(char *);
extern MKTEXDLL void popd(void);
extern MKTEXDLL void popenv(void);
extern char *peek_dir(int);
extern MKTEXDLL void push_fd(int [3]);
extern MKTEXDLL void pop_fd(void);
extern MKTEXDLL void start_redirection(int);
#ifdef _WIN32
/* extern void mt_exit(int); */
extern MKTEXDLL int sigint_handler(DWORD);
#else
extern void sigint_handler(int);
#endif

typedef void (__cdecl * pfnOutputAndCleanup)(int);
extern MKTEXDLL pfnOutputAndCleanup output_and_cleanup_function;

extern MKTEXDLL int redirect_stdout;
#endif /* _STACKENV_H_ */
