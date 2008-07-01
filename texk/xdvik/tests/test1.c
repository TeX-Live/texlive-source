#include "run_tests.h"

static Boolean mytest1(void) {
    return 1 == 1;
}

static Boolean mytest2(void) {
    return 0 == 0;
}

void register_all_from_test1(void) {
    register_test(mytest1, "Trivial equality");
    register_test(mytest2, "ld_list");
}
