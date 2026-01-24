// ターミナルで `arch -x86_64 bash` でスタート

#include "compiler.h"

static char *opt_o;

static char *input_path;

static void usage(int status) {
    fprintf(stderr, "a.out [-o <path> ] <file>\n");
    exit(status);
}

static void parse_args(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help"))
            usage(0);

        if (!strcmp(argv[i], "-o")) {
            if (!argv[i++])
                usage(1);
            opt_o = argv[i];
            continue;
        }

        if (!strncmp(argv[i], "-o", 2)) {
            opt_o = argv[i] + 2;
            continue;
        }

        if (argv[i][0] == '-' && argv[i][1] != '\0')
            error("unknown argument: %s", argv[i]);

        input_path = argv[i];
    }
    if (!input_path)
        error("no input files");
}

static FILE *open_file(char *path) {
    if (!path || strcmp(path, "-") == 0)
        return stdout;

    FILE *out = fopen(path, "w");
    if (!out)
        error("cannot open output file: %s: %s", path, strerror(errno));
    return out;
}

char *user_input; 

int main(int ac, char **av) {
    parse_args(ac, av);
    
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
    
    // トークン化して解析する。
    user_input = av[1];
    Token *tok = tokenize_file(input_path);
    Obj *prog = parse(tok);

    // ASTをトラバース（走査）し、アセンブリを出力します。
    FILE *out = open_file(opt_o);
    codegen(prog, out);
    return 0;
}