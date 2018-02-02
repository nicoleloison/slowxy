BINDIR =	/usr/local/sbin
MANDIR =	/usr/local/man/man8
CC =		cc
CFLAGS =	-O -ansi -pedantic -U__STRICT_ANSI__ -Wall -Wpointer-arith -Wshadow -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wno-long-long
LDFLAGS =
PORT= 8001

myproxy: 
	gcc $(CFLAGS) $(OPTFLAGS) -o myproxy myproxy.c $(LDFLAGS)
	./myproxy $(PORT)
	
clean:
	rm -f myproxy *.o core core.* *.core


