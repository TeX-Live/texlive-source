# This file is part of the FreeType project.
#
# It builds the library and test programs for emx-gcc under OS/2 or under
# Unix with extra debugging capabilities.
#
# You will need GNU make.
#
# Use this file while in the lib directory with the following statement:
#
#   make -f arch/debugger/Makefile

ARCH = arch/debugger
FT_MAKEFILE = $(ARCH)/Makefile

CC = gcc

#CFLAGS = -W -Wall -O0 -g -ansi -pedantic -I$(ARCH) -I. -Iextend

CFLAGS = -Wall -O0 -g -ansi -I$(ARCH) -I. -Iextend

#CFLAGS = -Wall -ansi -pedantic -O2 -s -I$(ARCH) -I. -Iextend

# Detect OS/2 to add the flag -DOS2 when compiling ttinterp.c
#
ifdef OS2_SHELL
OS := OS2
RM := del
else
OS := UNIX    # Unix build otherwise
RM := rm -f
endif

TTFILE   = ./ttfile.c
TTMEMORY = ./ttmemory.c
TTMUTEX  = ./ttmutex.c

PORT = $(TTFILE) $(TTMEMORY) $(TTMUTEX)

SRC_X = extend/ftxgasp.c extend/ftxkern.c  extend/ftxpost.c \
        extend/ftxcmap.c extend/ftxwidth.c extend/ftxsbit.c \
        extend/ftxgsub.c extend/ftxgpos.c  extend/ftxopen.c \
        extend/ftxgdef.c
OBJS_X = $(SRC_X:.c=.o)

SRC_M = ttapi.c     ttcache.c   ttcalc.c   ttcmap.c  ttdebug.c \
        ttgload.c   ttinterp.c  ttload.c   ttobjs.c \
        ttraster.c  ttextend.c  $(PORT)
OBJS_M = $(SRC_M:.c=.o) $(OBJS_X)

SRC_S = $(ARCH)/freetype.c
OBJ_S = $(SRC_S:.c=.o)
OBJS_S = $(OBJ_S) $(OBJS_X)

# We place the library file in the ARCH directory, so that it doesn't
# interfere with the normal build.
#
LIBTTF_A = $(ARCH)/libttf.a

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< -D$(OS)

.PHONY: all debug clean distclean depend


all:
	$(MAKE) -f $(FT_MAKEFILE) LIB_FILES=OBJS_S $(LIBTTF_A)

debug:
	$(MAKE) -f $(FT_MAKEFILE) LIB_FILES=OBJS_M $(LIBTTF_A)

$(OBJ_S): $(SRC_S) $(SRC_M)

$(LIBTTF_A): $($(LIB_FILES))
	-$(RM) $@
	ar src $@ $^

clean:
ifdef OS2_SHELL
	-$(RM) $(subst /,\,$(LIBTTF_A))
	-$(RM) $(subst /,\,$(OBJS_S))
else
	-$(RM) $(LIBTTF_A)
	-$(RM) $(OBJS_S)
endif

distclean: clean
	-$(RM) dep.end

depend: $(SRC_S) $(SRC_M) $(SRC_X)
	$(CC) $(CFLAGS) -E -M $^ > dep.end

ifeq (dep.end,$(wildcard dep.end))
  include dep.end
endif

# end of Makefile
