/* Written by K.Asayayam  Sep. 1995 */
#ifndef KSUPPORT_H_INCLUDED
#define KSUPPORT_H_INCLUDED
#ifdef KPATHSEA
typedef struct {
  char *var_name;
  char *path;
  char *suffix;
} KpathseaSupportInfo;

extern int KP_init();
extern int KP_entry_filetype();
extern char *KP_find_file();
extern char *KP_get_value();
extern char *KP_get_path();
#endif /* KPATHSEA */
#endif /* ! KSUPPORT_H_INCLUDED */
