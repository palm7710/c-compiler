#include "compiler.h"
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

static bool is_keyword(Token *tok) {
    static char *kw[] = {"return", "if", "else", "for", "while"};

    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
        if (equal(tok, kw[i]))
            return true;
    return false;
}

static void convert_keywords(Token *tok) {
    for (Token *t = tok; t->kind != TK_EOF; t = t->next)
        if (is_keyword(t))
            t->kind = TK_KEYWORD;
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
            
            // 数値の後に不正な文字が続いていないかチェック
            if (endptr < p + strlen(p) && (isalpha(*endptr) || *endptr == 'x' || *endptr == '.')) {
                error_at(p, "トークナイズできません");
            }
            
            cur->val = (int)val;
            cur->len = endptr - q;
            p = endptr;
            continue;
        }

        // キーワードと識別子
        if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z')) {
            char *start = p;
            
            // 大文字は許可しない
            if ('A' <= *p && *p <= 'Z') {
                error_at(start, "トークナイズできません");
            }
            
            // 識別子の文字を読み取り
            while (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || ('0' <= *p && *p <= '9') || *p == '_') {
                p++;
            }
            
            int len = p - start;
            
            // キーワードチェック
            if (len == 6 && memcmp(start, "return", 6) == 0) {
                cur = cur->next = new_token(TK_PUNCT, start, p);
            } else if (len == 2 && memcmp(start, "if", 2) == 0) {
                cur = cur->next = new_token(TK_PUNCT, start, p);
            } else if (len == 4 && memcmp(start, "else", 4) == 0) {
                cur = cur->next = new_token(TK_PUNCT, start, p);
            } else if (len == 3 && memcmp(start, "for", 3) == 0) {
                cur = cur->next = new_token(TK_PUNCT, start, p);
            } else if (len == 5 && memcmp(start, "while", 5) == 0) {
                cur = cur->next = new_token(TK_PUNCT, start, p);
            } else if (len == 1) {
                // 単一文字の識別子のみ許可
                cur = cur->next = new_token(TK_IDENT, start, p);
            } else {
                // 複数文字の識別子（キーワード以外）はエラー
                error_at(start, "トークナイズできません");
            }
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
    convert_keywords(head.next);
    return head.next;
}
