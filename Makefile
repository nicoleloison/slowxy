BINDIR =	/usr/local/sbin
MANDIR =	/usr/local/man/man8
CC =		cc
CFLAGS =	-O -ansi -pedantic -U__STRICT_ANSI__ -Wall -Wpointer-arith -Wshadow -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wno-long-long
LDFLAGS =
PORT= 8001

mp:	micro_proxy.o
	$(CC) micro_proxy.o $(LDFLAGS) -o micro_proxy

micro_proxy.o:	micro_proxy.c
	$(CC) $(CFLAGS) -c micro_proxy.c

install:	mp
	rm -f $(BINDIR)/micro_proxy
	cp micro_proxy $(BINDIR)
	rm -f $(MANDIR)/micro_proxy.8
	cp micro_proxy.8 $(MANDIR)

ts:	gcc $(CFLAGS) $(OPTFLAGS) -o tcpserver tcpserver.c $(LDFLAGS)
	./tcpserver $(PORT)

tc:	gcc $(CFLAGS) $(OPTFLAGS) -o tcpclient tcpclient.c $(LDFLAGS)
	./tcpclient $(PORT)
	
p:
	gcc $(CFLAGS) $(OPTFLAGS) -o proxy proxy1.c $(LDFLAGS)
	./proxy $(PORT)

clean:
	rm -f server proxy server.exe client.exe micro_proxy *.o core core.* *.core


