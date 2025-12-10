#include "9cc.h"

// 入力プログラム
static char *current_input;

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_tok(Token *tok, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    error_at(tok->loc, fmt, ap);
}

bool equal(Token * tok, char *op) {
    return memcmp(tok->loc, op, tok->len) == 0 &&
    op[tok->len] == '\0';
}

Token *skip(Token *tok, char *op) {
    if (!equal(tok, op))
        error_tok(tok, "'%s' が必要です", op);
    return tok->next;
}

// 新しいトークンを作成してcurに繋げる
static Token *new_token(TokenKind kind, char *start, char *end) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->loc = start;
    tok->len = end - start;
    return tok;
}

static bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

static int read_punct(char *p) {
    if (startswith(p, "==") || startswith(p, "!=") ||
        startswith(p, "<=") || startswith(p, ">=")) {
        return 2;
    }

    return ispunct(*p) ? 1 : 0;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
    current_input = p;
    Token head = {};
    Token *cur = &head;

    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (isdigit(*p)) {
            cur = cur->next = new_token(TK_NUM, p, p);

            char *q = p;
            cur->val = strtoul(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        int punct_len = read_punct(p);
        if (punct_len) {
            cur = cur->next = new_token(TK_PUNCT, p, p + punct_len);
            p += punct_len;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    cur = cur->next = new_token(TK_EOF, p, p);
    return head.next;
}
