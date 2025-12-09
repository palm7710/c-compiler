#!/bin/bash

cc -arch x86_64 9cc.c -o 9cc || exit 1

assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    nasm -f macho64 tmp.s -o tmp.o
    cc -arch x86_64 tmp.o -o tmp
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert_error() {
    input="$1"
    expected="$2"

    output=$(./9cc "$input" 2>&1 >/dev/null)

    if echo "$output" | grep -q "$expected"; then
        echo "$input => error OK"
    else
        echo "$input => expected error \"$expected\" but got:"
        echo "$output"
        exit 1
    fi
}

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 41 " 12 + 34 - 5 "
assert_error "1+3++" "数ではありません"
assert_error "1 + foo + 5" "トークナイズできません"

echo OK

# Stop is `Ctrl + Z`