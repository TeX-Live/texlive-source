#include "xdvi-config.h"

#include <sys/types.h> /* ZLB: must be before sys/socket.h for IRIX 5.3 */
#include <sys/socket.h>
#include <sys/file.h>	/* this defines FASYNC */
#include <sys/ioctl.h>	/* this defines SIOCSPGRP and FIOASYNC */
#include <sys/wait.h>	/* this defines WIFEXITED and WEXITSTATUS */

#include "xdvi.h"
#include "hypertex.h"
#include "dvi-init.h"
#include "special.h"
#include "string-utils.h"

#include "kpathsea/tex-file.h"

#include "events.h"
#include "util.h"

#define ERROR(X) do { fprintf(stderr, "%s:%d: Error: ", __FILE__, __LINE__); fprintf X; } while(0)
#define INFO(X) do { fprintf(stderr, "%s:%d: Info: ", __FILE__, __LINE__); fprintf X; } while(0)

typedef Boolean (*testProcT)(int verbosity);

extern void register_test(testProcT testproc, const char *name);

/* test helpers */
extern Boolean test_str_equality(int verbosity, const char *str1, const char *str2);
extern Boolean test_str_list_equality(int verbosity, char **str_list1, char **str_list2);

extern void register_all_from_test_dl_list(void);
extern void register_all_from_test_string_utils(void);
extern void register_all_from_test_string_list(void);
extern void register_all_from_test_util(void);
