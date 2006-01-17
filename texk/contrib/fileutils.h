#ifndef _FILEUTILS_H_

#define _FILEUTILS_H_

#include "mktexlib.h"
#include "stackenv.h"


/* Type of function to execute */
typedef void (*execfn)(string, struct stat*);

extern MKTEXDLL void recurse_dir(string, execfn, execfn);
extern MKTEXDLL void do_rmdir(string);
extern MKTEXDLL void rec_rmdir(string);
extern MKTEXDLL int  do_makedir(string);
extern MKTEXDLL void remove_path(string, struct stat*);
extern MKTEXDLL int  mvfile(const char *, const char *);
extern MKTEXDLL int  catfile(const char *, const char *, int);
extern MKTEXDLL boolean test_file(int, string);
extern MKTEXDLL boolean is_writable(string);
extern MKTEXDLL int execute_command(string);
extern MKTEXDLL char *normalize(char *path);
extern MKTEXDLL char *my_basename(char *name,char *suffix);
extern MKTEXDLL char *concat_pathes(const char *p1,const char *p2);

#endif /* _FILEUTILS_H_ */
