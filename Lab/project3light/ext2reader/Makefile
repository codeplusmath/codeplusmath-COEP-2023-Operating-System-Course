CC= gcc
CFLAGS= -Wall -O2

all: ext2reader

ext2reader: ext2reader.o ext2access.o testcode.o
	${CC} -o $@ ${CFLAGS} $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f ext2reader *.o *~ #*
