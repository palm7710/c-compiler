#!/bin/bash

GREEN='\033[32m'
RED='\033[31m'
CYAN='\033[36m'
WHITE='\033[37m'
RESET='\033[0m'

cat <<EOF | cc -arch x86_64 -xc -c -o tmp2.o -
int ret3() { return 3; }
int ret5() { return 5; }
int add(int x, int y) { return x+y; }
int sub(int x, int y) { return x-y; }

int add6(int a, int b, int c, int d, int e, int f) {
    return a+b+c+d+e+f;
}
EOF

cc -arch x86_64 main.c codegen.c parse.c tokenize.c type.c -o a.out || exit 1

assert() {
    expected="$1"
    input="$2"

    ./a.out "$input" > tmp.s
    cc -arch x86_64 -c tmp.s -o tmp.o
    cc -arch x86_64 tmp.o -o tmp tmp2.o 2>/dev/null
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

    output=$(./a.out "$input" 2>&1 >/dev/null)

    if echo "$output" | grep -q "$expected"; then
        echo -e "${CYAN}${input}${RESET} ${WHITE}=>${RESET} ${GREEN}error OK${RESET}"
    else
        echo -e "${CYAN}${input}${RESET} ${WHITE}=>${RESET} ${RED}expected error \"${expected}\" but got:${RESET}"
        echo -e "${RED}${output}${RESET}"
        exit 1
    fi
}
assert_error "int main() { 1+3++ }" "expected an expression"
assert_error "int main() { 1 + foo + 5 }" "undefined variable"
assert_error "int main() { 42 }" "';' が必要です"
assert_error "int main() { 5" "文の最後に ';' が必要です。プログラム全体をシングルクォートで囲んでください"
assert_error "int main() { @; }" "expected an expression"
assert_error "int main() { return 1 }" "';' が必要です"  # error_tok
assert_error "int main() { A; }" "トークナイズできません"  # error_at
assert 0 'int main() { return 0; }'
assert 42 'int main() { return 42; }'
assert 21 'int main() { return 5+20-4; }'
assert 41 'int main() { return  12 + 34 - 5 ; }'
assert 47 'int main() { return 5+6*7; }'
assert 15 'int main() { return 5*(9-6); }'
assert 4 'int main() { return (3+5)/2; }'
assert 10 'int main() { return -10+20; }'
assert 10 'int main() { return - -10; }'
assert 10 'int main() { return - - +10; }'

assert 0 'int main() { return 0==1; }'
assert 1 'int main() { return 42==42; }'
assert 1 'int main() { return 0!=1; }'
assert 0 'int main() { return 42!=42; }'

assert 1 'int main() { return 0<1; }'
assert 0 'int main() { return 1<1; }'
assert 0 'int main() { return 2<1; }'
assert 1 'int main() { return 0<=1; }'
assert 1 'int main() { return 1<=1; }'
assert 0 'int main() { return 2<=1; }'

assert 1 'int main() { return 1>0; }'
assert 0 'int main() { return 1>1; }'
assert 0 'int main() { return 1>2; }'
assert 1 'int main() { return 1>=0; }'
assert 1 'int main() { return 1>=1; }'
assert 0 'int main() { return 1>=2; }'

assert 3 'int main() { int a; a=3; return a; }'
assert 3 'int main() { int a=3; return a; }'
assert 8 'int main() { int a=3; int z=5; return a+z; }'

assert 1 'int main() { return 1; 2; 3; }'
assert 2 'int main() { 1; return 2; 3; }'
assert 3 'int main() { 1; 2; return 3; }'

assert 3 'int main() { int a=3; return a; }'
assert 8 'int main() { int a=3; int z=5; return a+z; }'
assert 6 'int main() { int a; int b; a=b=3; return a+b; }'
assert 3 'int main() { int foo=3; return foo; }'
assert 8 'int main() { int foo123=3; int bar=5; return foo123+bar; }'

assert 3 'int main() { if (0) return 2; return 3; }'
assert 3 'int main() { if (1-1) return 2; return 3; }'
assert 2 'int main() { if (1) return 2; return 3; }'
assert 2 'int main() { if (2-1) return 2; return 3; }'

assert 55 'int main() { int i=0; int j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }'
assert 3 'int main() { for (;;) return 3; return 5; }'

assert 10 'int main() { int i=0; while(i<10) i=i+1; return i; }'

assert 3 'int main() { {1; {2;} return 3;} }'
assert 5 'int main() { ;;; return 5; }'

assert 10 'int main() { int i=0; while(i<10) i=i+1; return i; }'
assert 55 'int main() { int i=0; int j=0; while(i<=10) {j=i+j; i=i+1;} return j; }'

assert 3 'int main() { int x=3; return *&x; }'
assert 3 'int main() { int x=3; int *y=&x; int **z=&y; return **z; }'
assert 5 'int main() { int x=3; int y=5; return *(&x+1); }'
assert 3 'int main() { int x=3; int y=5; return *(&y-1); }'
assert 5 'int main() { int x=3; int y=5; return *(&x-(-1)); }'
assert 5 'int main() { int x=3; int *y=&x; *y=5; return x; }'
assert 7 'int main() { int x=3; int y=5; *(&x+1)=7; return y; }'
assert 7 'int main() { int x=3; int y=5; *(&y-2+1)=7; return x; }'
assert 5 'int main() { int x=3; return (&x+2)-&x+3; }'
assert 8 'int main() { int x, y; x=3; y=5; return x+y; }'
assert 8 'int main() { int x=3, y=5; return x+y; }'

assert 3 'int main() { return ret3(); }'
assert 5 'int main() { return ret5(); }'
assert 8 'int main() { return add(3, 5); }'
assert 2 'int main() { return sub(5, 3); }'
assert 21 'int main() { return add6(1,2,3,4,5,6); }'
assert 66 'int main() { return add6(1,2,add6(3,4,5,6,7,8),9,10,11); }'
assert 136 'int main() { return add6(1,2,add6(3,add6(4,5,6,7,8,9),10,11,12,13),14,15,16); }'
assert 7 'int main() { return add2(3,4); } int add2(int x, int y) { return x+y; }'
assert 1 'int main() { return sub2(4,3); } int sub2(int x, int y) { return x-y; }'
assert 55 'int main() { return fib(9); } int fib(int x) { if (x<=1) return 1; return fib(x-1) + fib(x-2); }'

assert 32 'int main() { return ret32(); } int ret32() { return 32; }'

assert 3 'int main() { int x[2]; int *y=&x; *y=3; return *x; }'

assert 3 'int main() { int x[3]; *x=3; *(x+1)=4; *(x+2)=5; return *x; }'
assert 4 'int main() { int x[3]; *x=3; *(x+1)=4; *(x+2)=5; return *(x+1); }'
assert 5 'int main() { int x[3]; *x=3; *(x+1)=4; *(x+2)=5; return *(x+2); }'

assert 0 'int main() { int x[2][3]; int *y=x; *y=0; return **x; }'
assert 1 'int main() { int x[2][3]; int *y=x; *(y+1)=1; return *(*x+1); }'
assert 2 'int main() { int x[2][3]; int *y=x; *(y+2)=2; return *(*x+2); }'
assert 3 'int main() { int x[2][3]; int *y=x; *(y+3)=3; return **(x+1); }'
assert 4 'int main() { int x[2][3]; int *y=x; *(y+4)=4; return *(*(x+1)+1); }'
assert 5 'int main() { int x[2][3]; int *y=x; *(y+5)=5; return *(*(x+1)+2); }'

# []演算子を追加する
assert 3 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *x; }'
assert 4 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *(x+1); }'
assert 5 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *(x+2); }'
assert 5 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *(x+2); }'
assert 5 'int main() { int x[3]; *x=3; x[1]=4; 2[x]=5; return *(x+2); }'

assert 0 'int main() { int x[2][3]; int *y=x; y[0]=0; return x[0][0]; }'
assert 1 'int main() { int x[2][3]; int *y=x; y[1]=1; return x[0][1]; }'
assert 2 'int main() { int x[2][3]; int *y=x; y[2]=2; return x[0][2]; }'
assert 3 'int main() { int x[2][3]; int *y=x; y[3]=3; return x[1][0]; }'
assert 4 'int main() { int x[2][3]; int *y=x; y[4]=4; return x[1][1]; }'
assert 5 'int main() { int x[2][3]; int *y=x; y[5]=5; return x[1][2]; }'

# sizeof を追加する
assert 8 'int main() { int x; return sizeof(x); }'
assert 8 'int main() { int x; return sizeof x; }'
assert 8 'int main() { int *x; return sizeof(x); }'
assert 32 'int main() { int x[4]; return sizeof(x); }'
assert 96 'int main() { int x[3][4]; return sizeof(x); }'
assert 32 'int main() { int x[3][4]; return sizeof(*x); }'
assert 8 'int main() { int x[3][4]; return sizeof(**x); }'
assert 9 'int main() { int x[3][4]; return sizeof(**x) + 1; }'
assert 9 'int main() { int x[3][4]; return sizeof **x + 1; }'
assert 8 'int main() { int x[3][4]; return sizeof(**x + 1); }'
assert 8 'int main() { int x=1; return sizeof(x=2); }'
assert 1 'int main() { int x=1; sizeof(x=2); return x; }'

# グローバル変数を追加する
assert 0 'int x; int main() { return x; }'
assert 3 'int x; int main() { x=3; return x; }'
assert 7 'int x; int y; int main() { x=3; y=4; return x+y; }'
assert 7 'int x, y; int main() { x=3; y=4; return x+y; }'
assert 0 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[0]; }'
assert 1 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[1]; }'
assert 2 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[2]; }'
assert 3 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[3]; }'

assert 8 'int x; int main() { return sizeof(x); }'
assert 32 'int x[4]; int main() { return sizeof(x); }'

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
    output=$(./a.out "$long_input" 2>&1 >/dev/null)
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
output=$(./a.out "" 2>&1 >/dev/null)
if echo "$output" | grep -q "空のプログラムです"; then
    echo -e "${CYAN}空入力${RESET} ${WHITE}=>${RESET} ${GREEN}error OK${RESET}"
else
    echo -e "${CYAN}空入力${RESET} ${WHITE}=>${RESET} ${RED}expected empty error but got:${RESET}"
    echo -e "${RED}${output}${RESET}"
    exit 1
fi

# 深い再帰テスト（error() の発火確認）
echo -e "${CYAN}深い再帰テスト${RESET}"
deep_expr="1"
for i in {1..1001}; do
    deep_expr="($deep_expr+1)"
done
deep_expr="$deep_expr;"
deep_expr="{ $deep_expr }"
deep_expr="int main() $deep_expr"
assert_error "$deep_expr" "式が複雑すぎます"

# 変数名長制限テスト
echo -e "${CYAN}変数名長制限テスト${RESET}"
long_var_name=$(printf 'a%.0s' {1..300})
if [ ${#long_var_name} -gt 1 ]; then
    assert_error "int main() { int ${long_var_name}=1; return ${long_var_name}; }" "トークナイズできません"
fi

# 複雑な式でのスタック深度テスト
echo -e "${CYAN}スタック深度テスト${RESET}"
deep_calc="1"
for i in {1..100}; do
    deep_calc="$deep_calc+1"
done
deep_calc="int main() { return $deep_calc; }"
assert 101 "$deep_calc"

# 不正な数値形式テスト
echo -e "${CYAN}不正な数値形式テスト${RESET}"
assert_error "0x123;" "トークナイズできません"  # 16進数は未対応
assert_error "123.45;" "トークナイズできません"  # 浮動小数点は未対応
assert_error "123abc;" "トークナイズできません"  # 数値+文字の混在

echo -e "${GREEN}セキュリティテスト完了${RESET}"
echo -e "${GREEN}OK${RESET}"

# Stop is `Ctrl + Z`
