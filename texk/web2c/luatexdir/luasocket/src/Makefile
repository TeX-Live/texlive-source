
SOCKET_SO=socket.a

INSTALL_TOP_SHARE=/usr/local/share/lua/5.1
INSTALL_TOP_LIB=/usr/local/lib/lua/5.1

LUAINC = -I../../lua51

DEF = -DLUASOCKET_DEBUG 
CFLAGS = $(LUAINC) $(DEF) $(XCFLAGS)
AR=ar
ARFLAGS=rc

SOCKET_OBJS:= \
	luasocket.o \
	timeout.o \
	buffer.o \
	io.o \
	auxiliar.o \
	options.o \
	inet.o \
	tcp.o \
	udp.o \
	except.o \
	select.o \
	socket.o \
	mime.o \
    lua_preload.o

all: $(SOCKET_SO)

$(SOCKET_SO): $(SOCKET_OBJS)
	$(AR) $(ARFLAGS) $@ $(SOCKET_OBJS)

#------
# List of dependencies
#
auxiliar.o: auxiliar.c auxiliar.h
buffer.o: buffer.c buffer.h io.h timeout.h
except.o: except.c except.h
inet.o: inet.c inet.h socket.h io.h timeout.h usocket.h
io.o: io.c io.h timeout.h
luasocket.o: luasocket.c luasocket.h auxiliar.h except.h timeout.h \
  buffer.h io.h inet.h socket.h usocket.h tcp.h udp.h select.h
mime.o: mime.c mime.h
options.o: options.c auxiliar.h options.h socket.h io.h timeout.h \
  usocket.h inet.h
select.o: select.c socket.h io.h timeout.h usocket.h select.h
tcp.o: tcp.c auxiliar.h socket.h io.h timeout.h usocket.h inet.h \
  options.h tcp.h buffer.h
timeout.o: timeout.c auxiliar.h timeout.h
udp.o: udp.c auxiliar.h socket.h io.h timeout.h usocket.h inet.h \
  options.h udp.h
unix.o: unix.c auxiliar.h socket.h io.h timeout.h usocket.h options.h \
  unix.h buffer.h
usocket.o: usocket.c socket.h io.h timeout.h usocket.h

clean:
	rm -f $(SOCKET_SO) $(SOCKET_OBJS) 

#------
# End of makefile configuration
#
