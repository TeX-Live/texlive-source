## mpostlib.mk - Makefile fragment for libraries used by mpost
# Public domain.

# Include dependencies to get the built if we don't do make
# from the top-level directory.

Makefile: mpdir/mpostlib.mk

# libmpost 
mpostlib = mpdir/libmpost.a
mpostlib_sources = $(srcdir)/mpdir/*.c $(srcdir)/mpdir/*.h

mpdir/libmpost.a: $(mpostlib_sources)
	cd mpdir && $(MAKE) $(common_makeargs) libmpost.a

# makecpool

makecpool = mpdir/makecpool

mpdir/makecpool: mpdir/makecpool.c
	cd mpdir && $(MAKE) $(common_makeargs) makecpool

# Convenience variables.

mpostlibs = $(mpostlib)
mpostlibsdep = $(mpostlib) $(makecpool)

## end of mpostlib.mk - Makefile fragment for libraries used by mpost

