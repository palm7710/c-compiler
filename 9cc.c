// ターミナルで `arch -x86_64 bash` でスタート

#include <stdio.h>
#include <stdlib.h>

int main(int ac, char **av)
{
    if (ac != 2)
    {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // Intel記法
    printf("global _main\n");
    printf("section .text\n");
    printf("_main:\n");
    printf("    mov rax, %d\n", atoi(av[1]));
    printf("    ret\n");
    return 0;
}