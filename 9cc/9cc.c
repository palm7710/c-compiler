// ターミナルで `arch -x86_64 bash` でスタート

#include "9cc.h"

char *user_input; 

int main(int ac, char **av) {
    if (ac != 2)
    {
        error("引数の個数が正しくありません");
        return 1;
    }
    
    user_input = av[1];
    Token *tok = tokenize(av[1]);
    Node *node = parse(tok);
    codegen(node);
    return 0;
}