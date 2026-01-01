CFLAGS=-std=c11 -g -static
SRCS =$(wildcard *.c)
OBJS=$(SRCS: .c=.o)

a.out: $(OBJS)
		$(CC) -o a.out $(OBJS) $(LDFLAGS)

$(OBJS): compiler.h

test: a.out
		./test.sh

clean:
		rm -f a.out *.o *~ tmp*

.PHONY: test clean
