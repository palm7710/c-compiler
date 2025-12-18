#include "9cc.h"
#include <limits.h>

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
    if (!equal(tok, op)) {
        if (strlen(op) == 0) {
            error_tok(tok, "予期しないトークンです");
        } else {
            error_tok(tok, "'%s' が必要です", op);
        }
    }
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

// 文字cが識別子の先頭文字として有効な場合にtrueを返します。
static bool is_ident1(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '-';
}

// 文字 c が、識別子の2文字目以降（先頭以外）として有効な場合は true を返します。
static bool is_ident2(char c) {
    return is_ident1(c) || ('0' <= c && c <= '9');
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
            char *endptr;
            unsigned long val = strtoul(p, &endptr, 10);
            
            // 整数オーバーフローチェック
            if (val > INT_MAX) {
                error_at(p, "数値が大きすぎます");
            }
            
            // 変換エラーチェック
            if (endptr == p) {
                error_at(p, "数値の解析に失敗しました");
            }
            
            cur->val = (int)val;
            cur->len = endptr - q;
            p = endptr;
            continue;
        }

        // 識別子 (単一文字のみ)
        if ('a' <= *p && *p <= 'z') {
            char *start = p;
            p++;
            // 複数文字の識別子はエラー
            if ('a' <= *p && *p <= 'z') {
                error_at(start, "トークナイズできません");
            }
            cur = cur->next = new_token(TK_IDENT, start, p);
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
