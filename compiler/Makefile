CFLAGS=-std=c11 -g -static -arch x86_64

SRCS =$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

TEST_SRCS=$(wildcard ../test/*.c)
TESTS=$(TEST_SRCS:.c=.exe)

a.out: $(OBJS)
		$(CC) -o a.out $(OBJS) $(LDFLAGS)

$(OBJS): compiler.h

../test/%.exe: a.out ../test/%.c
		$(CC) -o- -E -P -C ../test/$*.c | ./a.out -o ../test/$*.s -
		$(CC) -o $@ ../test/$*.s -xc ../test/common

test: $(TESTS)
		for i in $^; do echo $$i; ./$$i || exit 1; echo; done

clean:
		rm -rf a.out tmp* $(TESTS) ../test/*.s ../test/*.exe
		find * -type f '(' -name '*~' -o -name '*.o' ')' -exec rm {} ';'

.PHONY: test clean
