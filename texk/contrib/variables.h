#ifndef _MKTEX_VARIABLES_H_

#define _MKTEX_VARIABLES_H_

#include <kpathsea/cnf.h>
#include <kpathsea/db.h>
#include <kpathsea/fn.h>
#include <kpathsea/hash.h>

extern MKTEXDLL void init_vars(void);
extern MKTEXDLL char *getval(char *name);
extern MKTEXDLL char *setval(char *name,char *value);
extern MKTEXDLL char *setval_default(char *name,char *value);
extern MKTEXDLL char **grep(char *regexp,char *line,int num_vars) ;

extern MKTEXDLL boolean parse_variable(char *line);

extern char *strcasestr(char *s1,char *s2);

extern char *subst(char *line, char *from, char *to);
extern MKTEXDLL string expand_var(const_string);
extern MKTEXDLL string mktex_var_expand (const_string src);
#endif /* _MKTEX_VARIABLES_H_ */
