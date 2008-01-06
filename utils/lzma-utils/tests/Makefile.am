TESTS_ENVIRONMENT = \
	abs_top_builddir='$(abs_top_builddir)' \
	abs_top_srcdir='$(abs_top_srcdir)' \
	srcdir='$(srcdir)' \
	top_srcdir='$(top_srcdir)' \
	PATH="`pwd`/../src/lzma$(PATH_SEPARATOR)$$PATH"

TESTS = \
	basic \
	perms

EXTRA_DIST = \
	$(TESTS) \
	mkdtemp \
	test-lib.sh

.PHONY: $(TESTS)
