#include "run_tests.h"
#include "dl_list.h"

static struct elem {
    int i;
    const char *name;
} elems[] = {
    { 1, "Hello" },
    { 2, " " },
    { 3, "World" },
};

static struct elem test_elems[] = {
    { 1, "Hello" },
    { 3, "Another" },
    { 1, "Hello" },
    { 2, " " },
    { 3, "World" },
};

static void printer_func(const void *e_in) {
    const struct elem *e = (const struct elem *)e_in;
    INFO((stderr, "ELEM: %d, `%s'\n", e->i, e->name));
}

static Boolean test_equality(int verbose,
			     struct dl_list *list,
			     const struct elem *elems,
			     size_t list_len,
			     size_t elem_cnt)
{
    size_t i;
    struct dl_list *ptr = NULL;

    if (list_len != elem_cnt) {
	if (verbose) {
	    ERROR((stderr, "List len %ld != %ld\n", (long) list_len, (long) elem_cnt));
	}
	return False;
    }
    
    for (i = 0, ptr = list; i < list_len; i++, ptr = ptr->next) {
	const struct elem *curr = (const struct elem *)ptr->item;

	if (verbose) {
	    INFO((stderr, "Checking: `%s' - `%s'\n", elems[i].name, curr->name));
	}
	if (strcmp(test_elems[i].name, curr->name) != 0) {
	    return False;
	}
	if (elems[i].i != curr->i) {
	    return False;
	}
    }
    return True;
}

static Boolean dl_list_test1(int verbosity) {
    struct dl_list *testlist = NULL;
    size_t list_len;
    static struct elem e1 = { 3, "Another" };
    static struct elem e2 = { 1, "Hello" };

    testlist = dl_list_insert(testlist, &elems[0]);
    testlist = dl_list_insert(testlist, &elems[1]);
    testlist = dl_list_insert(testlist, &elems[2]);
    testlist = dl_list_head(testlist);

    if (verbosity) {
	testlist = dl_list_head(testlist);
	dl_list_apply(testlist, printer_func);
    }
    
    list_len = dl_list_len(testlist);

    test_equality(verbosity, testlist, elems, list_len, 3);

    /* test pushing */
    testlist = dl_list_push_front(testlist, &e1);
    testlist = dl_list_push_front(testlist, &e2);
    testlist = dl_list_head(testlist);
    list_len = dl_list_len(testlist);

    test_equality(verbosity, testlist, test_elems, list_len, 5);

    /* test truncating */
    testlist = dl_list_truncate(testlist);
    testlist = dl_list_truncate(testlist);
    

    return True;
}

void register_all_from_test_dl_list(void) {
    register_test(dl_list_test1, "dl_list construction");
}
