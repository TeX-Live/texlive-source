#include "run_tests.h"
#include "util.h"
#include "string-utils.h"

static Boolean
test_src_format_arguments(int verbose)
{
    static struct stringtest {
	char *str;
	char *res[16]; /* big enuff */
    } tests[] = {
	{ "emacsclient --no-wait +%l %f",
	  { "emacsclient", "--no-wait", "+10", "/this/is/a/filename.tex", NULL }
	},
	{ "vim +%l:%c %f",
	  { "vim", "+10:0", "/this/is/a/filename.tex", NULL }
	},
	{ "bazoo %f:%l:%c",
	  { "bazoo", "/this/is/a/filename.tex:10:0", NULL }
	}
    };

    const char *filename = "/this/is/a/filename.tex";
    int lineno = 10;
    int colno = 0;
    size_t i;
    Boolean res = True;
    
    for (i = 0; i < (sizeof tests / sizeof tests[0]); i++) {
	char **argv = get_separated_list(tests[i].str, " \t", True);
	argv = src_format_arguments(argv, filename, lineno, colno);
	if (!test_str_list_equality(verbose, argv, tests[i].res)) {
	    res = False;
	}
    }

    return res;
}


void
register_all_from_test_util(void)
{
    register_test(test_src_format_arguments, "src_format_arguments");
}
