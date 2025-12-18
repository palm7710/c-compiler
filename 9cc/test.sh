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
    cc -arch x86_64 -c tmp.s -o tmp.o
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
assert_error "1+3++" "数ではありません"
assert_error "1 + foo + 5" "トークナイズできません"
assert_error "42" "文の最後に ';' が必要です"
assert_error "5" "文の最後に ';' が必要です。プログラム全体をシングルクォートで囲んでください"
assert 0 '0;'
assert 42 '42;'
assert 21 '5+20-4;'
assert 41 ' 12 + 34 - 5 ;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 10 '- -10;'
assert 10 '- - +10;'

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

assert 3 'a=3; a;'
assert 8 'a=3; z=5; a+z;'
assert 6 'a=b=3; a+b;'

# セキュリティテスト
echo -e "${CYAN}=== セキュリティテスト ===${RESET}"

# 整数オーバーフローテスト
echo -e "${CYAN}整数オーバーフローテスト${RESET}"
assert_error "2147483648;" "数値が大きすぎます"
assert_error "99999999999999999999;" "数値が大きすぎます"

# 入力長制限テスト（DoS攻撃防止）
echo -e "${CYAN}入力長制限テスト${RESET}"
long_input=$(printf 'a+%.0s' {1..5000})'1;'
if [ ${#long_input} -gt 10000 ]; then
    output=$(./9cc "$long_input" 2>&1 >/dev/null)
    if echo "$output" | grep -q "プログラムが長すぎます"; then
        echo -e "${CYAN}長すぎる入力${RESET} ${WHITE}=>${RESET} ${GREEN}error OK${RESET}"
    else
        echo -e "${CYAN}長すぎる入力${RESET} ${WHITE}=>${RESET} ${RED}expected length error but got:${RESET}"
        echo -e "${RED}${output}${RESET}"
        exit 1
    fi
fi

# 空入力テスト
echo -e "${CYAN}空入力テスト${RESET}"
output=$(./9cc "" 2>&1 >/dev/null)
if echo "$output" | grep -q "空のプログラムです"; then
    echo -e "${CYAN}空入力${RESET} ${WHITE}=>${RESET} ${GREEN}error OK${RESET}"
else
    echo -e "${CYAN}空入力${RESET} ${WHITE}=>${RESET} ${RED}expected empty error but got:${RESET}"
    echo -e "${RED}${output}${RESET}"
    exit 1
fi

# 深い再帰テスト（スタックオーバーフロー防止）
echo -e "${CYAN}深い再帰テスト${RESET}"
deep_expr="1"
for i in {1..1001}; do
    deep_expr="($deep_expr+1)"
done
deep_expr="$deep_expr;"
assert_error "$deep_expr" "式が複雑すぎます"

# 変数名長制限テスト
echo -e "${CYAN}変数名長制限テスト${RESET}"
long_var_name=$(printf 'a%.0s' {1..300})
if [ ${#long_var_name} -gt 1 ]; then
    assert_error "${long_var_name}=1;" "トークナイズできません"
fi

# 複雑な式でのスタック深度テスト
echo -e "${CYAN}スタック深度テスト${RESET}"
deep_calc="1"
for i in {1..100}; do
    deep_calc="$deep_calc+1"
done
deep_calc="$deep_calc;"
assert 101 "$deep_calc"

# 不正な数値形式テスト
echo -e "${CYAN}不正な数値形式テスト${RESET}"
assert_error "0x123;" "トークナイズできません"  # 16進数は未対応
assert_error "123.45;" "トークナイズできません"  # 浮動小数点は未対応
assert_error "123abc;" "トークナイズできません"  # 数値+文字の混在

echo -e "${GREEN}セキュリティテスト完了${RESET}"
echo -e "${GREEN}OK${RESET}"

# Stop is `Ctrl + Z`