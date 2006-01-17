# cross.mk -- used by Makefiles for build-host executables.

BUILDCC = @BUILDCC@
BUILDCFLAGS = @BUILDCFLAGS@
BUILDCPPFLAGS = @BUILDCPPFLAGS@

ALL_BUILDCPPFLAGS = $(DEFS) -I. -I$(srcdir) -I$(kpathsea_parent) \
  -I$(kpathsea_srcdir_parent) $(prog_cflags) $(BUILDCPPFLAGS)
ALL_BUILDCFLAGS = $(ALL_BUILDCPPFLAGS) $(BUILDCFLAGS)
build_compile = $(BUILDCC) $(ALL_BUILDCFLAGS)

# Don't include $(BUILDCFLAGS), since ld -g under Linux forces
# static libraries, e.g., libc.a and libX*.a.
BUILDLDFLAGS = @BUILDLDFLAGS@

# May as well separate linking from compiling, just in case.
BUILDCCLD = @BUILDCCLD@
build_link_command = $(BUILDCCLD) -o $@ $(BUILDLDFLAGS) 

# End of cross.mk.
