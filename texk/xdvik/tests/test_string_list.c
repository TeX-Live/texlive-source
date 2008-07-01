#include "run_tests.h"
#include "string_list.h"

static Boolean
string_list_test1(int verbosity)
{
    static char *test_str = "This\nis\na\ntest\n";
    static char *test_str_rotated = "is\na\ntest\nThis\n";
    static char *test_list[] = {
	"This",
	"is",
	"a",
	"test",
	NULL
    };

    char *res_str;
    char **test_list_orig, **test_list_rotated;

    res_str = string_list_to_str(test_list, "\n");
    if (!test_str_equality(verbosity, res_str, test_str)) {
	return False;
    }
    
    test_list_rotated = string_list_rotate_down(test_list);
    if (!test_str_equality(verbosity,
			   string_list_to_str(test_list_rotated, "\n"),
			   test_str_rotated)) {
	return False;
    }

    test_list_orig = string_list_rotate_up(test_list_rotated);
    if (!test_str_list_equality(verbosity,
				test_list_orig,
				test_list)) {
	return False;
    }
    return True;
}

static Boolean
string_list_test2(int verbosity)
{
    static char *test_str = "";
    static char *test_list[] = { NULL };
    char **res_list;
    char *res_str = string_list_to_str(test_list, "#");

    if (!test_str_equality(verbosity, res_str, test_str)) {
	return False;
    }

    /*     string_list_print(test_list); */
    
    res_list = string_list_rotate_down(test_list);

    /*     string_list_print(res_list); */

    if (!test_str_equality(verbosity,
			   test_str,
			   string_list_to_str(res_list, "\n")))
    {
	return False;
    }
    return True;
}

/* todo: append, prepend, reorder */

void
register_all_from_test_string_list(void)
{
    register_test(string_list_test1, "string lists");
    register_test(string_list_test2, "empty string lists");
}
