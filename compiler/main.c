// ターミナルで `arch -x86_64 bash` でスタート

#include "compiler.h"

char *user_input; 

int main(int ac, char **av) {
    if (ac != 2)
    {
        fprintf(stderr, "使用方法: %s '<プログラム>'\n", av[0]);
        fprintf(stderr, "例: %s '5+3;'\n", av[0]);
        fprintf(stderr, "注意: プログラムはシングルクォートで囲んでください（セミコロンを含むため）\n");
        return 1;
    }
    
    // 入力長制限（DoS攻撃防止）
    size_t input_len = strlen(av[1]);
    if (input_len > 10000) {
        fprintf(stderr, "エラー: プログラムが長すぎます（最大10000文字）\n");
        return 1;
    }
    
    if (input_len == 0) {
        fprintf(stderr, "エラー: 空のプログラムです\n");
        return 1;
    }
    
    user_input = av[1];
    Token *tok = tokenize(av[1]);
    Function *prog = parse(tok);

    // ASTをトラバース（走査）し、アセンブリを出力します。
    codegen(prog);
    return 0;
}