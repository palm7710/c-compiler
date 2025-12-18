// ターミナルで `arch -x86_64 bash` でスタート

#include "9cc.h"

char *user_input; 

int main(int ac, char **av) {
    if (ac != 2)
    {
        fprintf(stderr, "使用方法: %s '<プログラム>'\n", av[0]);
        fprintf(stderr, "例: %s '5+3;'\n", av[0]);
        fprintf(stderr, "注意: プログラムはシングルクォートで囲んでください（セミコロンを含むため）\n");
        return 1;
    }
    
    user_input = av[1];
    Token *tok = tokenize(av[1]);
    Function *prog = parse(tok);

    // ASTをトラバース（走査）し、アセンブリを出力します。
    codegen(prog);
    return 0;
}