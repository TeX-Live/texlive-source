/* Written by K.Asayayam  Sep. 1995 */
#ifdef KPATHSEA

#include <kpathsea/kpathsea.h>
#include <string.h>
#include "kp.h"

int
KP_init(prog)
  char *prog;
{
  kpse_set_progname(prog);
  return 0;
}

#ifdef KPATHSEA3
/* KP_get_value(char *var,char *def_val)
     ARGUMENTS:
       char *var:     name of variable.
       char *def_val: default value.
 */
char *KP_get_value(var,def_val)
  char *var,*def_val;
{
  char *p = kpse_var_value(var);
  return p ? p : def_val;
}

#else

#include <stdlib.h> /* for getenv() */

/* KP_get_value(char *env,char *cnf,char *def_val)
     ARGUMENTS:
       char *env:     name of environment variable.
       char *cnf:     name of variable in `texmf.cnf'.
       char *def_val: default value.
 */
char *KP_get_value(env,cnf,def_val)
  char *env,*cnf,*def_val;
{
  char *val;
  char *cnf_val,*env_val;

  val = def_val ? def_val : "";

  cnf_val = kpse_cnf_get(cnf);
  env_val = getenv(env);
  if (cnf_val) val = kpse_expand_default(cnf_val,val);
  if (env_val) val = kpse_expand_default(env_val,val);

  return val[0] ? val : ((cnf_val || env_val) ? val : (char *)0);
}
#endif

/* KP_get_path(char *var, char *def_val)
     ARGUMENTS:
       char *var:     name of variable.
       char *def_val: default value.
 */
char *KP_get_path(var,def_val)
  char *var,*def_val;
{
  char avar[50];
  char *p;
  strcpy(avar, "${");
  strcat(avar, var);
  strcat(avar, "}");
  p = kpse_path_expand(avar);
  return (p && *p) ? p : def_val;
}

/*
 */
int KP_entry_filetype(info)
  KpathseaSupportInfo *info;
{
#ifdef KPATHSEA3
  info->path = KP_get_path(info->var_name,info->path);
#else
  char *path;
  path = KP_get_value(info->var_name,info->var_name,info->path);
  info->path = kpse_path_expand(path);
#endif
  return 0;
}

/* KP_find_file(KpathseaSupportInfo *info, char *name)
     ARGUMENTS:
       KpathseaSupportInfo *info: Informations about the type of files.
       char *name:                Name of file.
 */
char *KP_find_file(info,name)
  KpathseaSupportInfo *info;
  char *name;
{
  char *ret;
  ret = kpse_path_search(info->path,name,1);
  if (!ret && info->suffix && !find_suffix(name)) {
    char *suff_name;
    suff_name = concat3(name,".",info->suffix);
    ret = kpse_path_search(info->path,suff_name,1);
    free(suff_name);
  }
  return ret ? ret : name;
}

#else
int KP_dummy_variable;
#endif
