#include "run_tests.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* helper routines */
Boolean test_str_equality(int verbosity, const char *str1, const char *str2)
{
    int res = strcmp(str1, str2);
    if (res != 0) {
	if (verbosity) {
	    ERROR((stderr, "|%s| != |%s|\n", str1, str2));
	}
	return False;
    }
    if (verbosity) {
	INFO((stderr, "|%s| == |%s|\n", str1, str2));
    }
    return True;
}

Boolean
test_str_list_equality(int verbosity, char **str_list1, char **str_list2)
{
    size_t i;
    for (i = 0; str_list1[i] != NULL || str_list2[i] != NULL; i++)
    {
	int res;

	if (str_list1[i] == NULL || str_list2[i] == NULL) {
	    if (verbosity) {
		ERROR((stderr, "Items %ld differ: |%s| != |%s|\n", (long) i, str_list1[i], str_list2[i]));
	    }
	    return False;
	}
	
	res = strcmp(str_list1[i], str_list2[i]);
	if (res != 0) {
	    if (verbosity) {
		ERROR((stderr, "Items %ld differ: |%s| != |%s|\n", (long) i, str_list1[i], str_list2[i]));
	    }
	    return False;
	}
	else if (verbosity) {
	    INFO((stderr, "Items %ld equal: |%s| == |%s|\n", (long) i, str_list1[i], str_list2[i]));
	}
    }
    return True;
}



struct test_proc {
    const char *name;
    testProcT proc;
};

static size_t m_test_proc_size = 0;
static struct test_proc *m_test_proc_list = NULL;

void register_test(testProcT proc, const char *name)
{
    size_t old_idx = m_test_proc_size;
    m_test_proc_size++;
    m_test_proc_list = xrealloc(m_test_proc_list, m_test_proc_size * sizeof *m_test_proc_list);
    m_test_proc_list[old_idx].name = xstrdup(name);
    m_test_proc_list[old_idx].proc = proc;
}


static Boolean
run_all_tests(int verbosity)
{
    size_t i;

    int tests_failed = 0;
    int tests_ok = 0;

    fprintf(stdout, "\nGoing to run %ld tests ...\n", (long) m_test_proc_size);
    
    for (i = 0; i < m_test_proc_size; i++) {
	Boolean retval = m_test_proc_list[i].proc(verbosity);
	if (!retval) {
	    tests_failed++;
	    m_test_proc_list[i].proc(True);
	}
	else {
	    tests_ok++;
	}
	fprintf(stdout, "%sTest %ld: %s (%s)\n",
		retval ? "" : "*****",
		(long) i + 1,
		retval ? "OK" : "FAILURE",
		m_test_proc_list[i].name);
    }

/*     fprintf(stdout, "Ran %d tests: %d OK, %d failures.\n", m_test_proc_size, tests_ok, tests_failed); */
    if (tests_failed == 0) {
	fprintf(stdout, "Great! All tests succeeded.\n\n");
	return True;
    }
    else {
	fprintf(stdout, "Darn, %d of %ld tests failed!\n\n", tests_failed, (long) m_test_proc_size);
	return False;
    }
}

int main(int argc, char *argv[])
{
    int verbosity = 0;
    /* fprintf(stderr, "argc: %d, argv1: %s\n", argc, argv[1]); */
    if (argc > 1 && strcmp(argv[1], "-v") == 0) {
	verbosity = 1;
    }

    register_all_from_test_dl_list();
    register_all_from_test_string_utils();
    register_all_from_test_string_list();
    register_all_from_test_util();

    if (!run_all_tests(verbosity))
	return 1;
    else
	return 0;
}
