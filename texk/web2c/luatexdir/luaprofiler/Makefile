
srcdir=../../../src/libs/luaprofiler

OBJS= clocks.o         \
      core_profiler.o  \
      function_meter.o \
      stack.o          \
      lua50_profiler.o


profiler: $(OBJS)
	ar cru libprofiler.a  $(OBJS)
	ranlib libprofiler.a	

clean:
	rm -f $(OBJS) libprofiler.a

%.o: $(srcdir)/%.c
	$(CC) -c -I../../../src/libs/lua51 $< $(CFLAGS) -o $@
