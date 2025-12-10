#!/bin/bash

GREEN='\033[32m'
RED='\033[31m'
CYAN='\033[36m'
WHITE='\033[37m'
RESET='\033[0m'

cc -arch x86_64 9cc.c codegen.c parse.c tokenize.c -o 9cc || exit 1

assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    nasm -f macho64 tmp.s -o tmp.o
    cc -arch x86_64 tmp.o -o tmp 2>/dev/null
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo -e "${CYAN}${input}${RESET} ${WHITE}=>${RESET} ${GREEN}${actual}${RESET}"
    else
        echo -e "${CYAN}${input}${RESET} ${WHITE}=>${RESET} ${RED}${expected} expected, but got ${actual}${RESET}"
        exit 1
    fi
}

assert_error() {
    input="$1"
    expected="$2"

    output=$(./9cc "$input" 2>&1 >/dev/null)

    if echo "$output" | grep -q "$expected"; then
        echo -e "${CYAN}${input}${RESET} ${WHITE}=>${RESET} ${GREEN}error OK${RESET}"
    else
        echo -e "${CYAN}${input}${RESET} ${WHITE}=>${RESET} ${RED}expected error \"${expected}\" but got:${RESET}"
        echo -e "${RED}${output}${RESET}"
        exit 1
    fi
}

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 41 " 12 + 34 - 5 "
assert_error "1+3++" "数ではありません"
assert_error "1 + foo + 5" "トークナイズできません"
assert 47 '5+6*7'
assert 15 '5*(9-6)'
assert 4 '(3+5)/2'
assert 10 '-10+20'
assert 10 '- -10'
assert 10 '- - +10'

assert 0 '0==1'
assert 1 '42==42'
assert 1 '0!=1'
assert 0 '42!=42'

assert 1 '0<1'
assert 0 '1<1'
assert 0 '2<1'
assert 1 '0<=1'
assert 1 '1<=1'
assert 0 '2<=1'

assert 1 '1>0'
assert 0 '1>1'
assert 0 '1>2'
assert 1 '1>=0'
assert 1 '1>=1'
assert 0 '1>=2'

echo -e "${GREEN}OK${RESET}"

# Stop is `Ctrl + Z`