#include "compiler.h"
#include <limits.h>

// 入力ファイル名
static char *current_filename;

// 入力プログラム
static char *current_input;

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 以下の形式でエラーメッセージを報告し終了する。
//
// foo.c:10: x = y + 1;
//               ^ <ここにエラーメッセージ>
static void verror_at(char *loc, char *fmt, va_list ap) {
    // `loc` を含む行を探す
    char *line = loc;
    while (current_input < line && line[-1] != '\n')
        line--;
    
    char *end = loc;
    while (*end != '\n')
        end++;

    // 行番号を取得する
    int line_no = 1;
    for (char *p = current_input; p < line; p++)
        if (*p == '\n')
            line_no++;
    // 行を出力する
    int indent = fprintf(stderr, "%s:%d: ", current_filename, line_no);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);
    // エラーメッセージを表示する
    int pos = loc - line + indent;

    fprintf(stderr, "%*s", pos, ""); // pos 個のスペースを表示する
    fprintf(stderr, "^ ");
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
    int pos = tok->loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool equal(Token *tok, char *op) {
    size_t op_len = strlen(op);
    if (tok->len != (int)op_len)
        return false;
    return memcmp(tok->loc, op, op_len) == 0;
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

bool consume(Token **rest, Token *tok, char *str) {
    if (equal(tok, str)) {
        *rest = tok->next;
        return true;
    }
    *rest = tok;
    return false;
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

static int from_hex(char c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    return c - 'A' + 10;
}

static int read_punct(char *p) {
    if (startswith(p, "==") || startswith(p, "!=") ||
        startswith(p, "<=") || startswith(p, ">=")) {
        return 2;
    }

    return ispunct(*p) ? 1 : 0;
}

static bool is_keyword(Token *tok) {
    static char *kw[] = {"return", "if", "else", "for", "while", "int", "sizeof", "char"};

    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
        if (equal(tok, kw[i]))
            return true;
    return false;
}

static int read_escaped_char(char **new_pos, char *p) {
    if ('0' <= *p && *p <= '7') {
        // 8進数の数値を読み取る
        int c = *p++ - '0';
        if ('0' <= *p && *p <= '7') {
            c = (c << 3) + (*p++ - '0');
            if ('0' <= *p && *p <= '7')
                c = (c << 3) + (*p++ - '0');
        }
        *new_pos = p;
        return c;
    }

    if (*p == 'x') {
        // 16進数を読み取る。
        p++;
        if (!isxdigit(*p))
            error_at(p, "invalid hex escape sequence");

        int c = 0;
        for (; isxdigit(*p); p++)
            c = (c <<4) + from_hex(*p);
        *new_pos = p;
        return c;
    }

    *new_pos = p + 1;

    switch (*p) {
        case 'a': return '\a'; // ベル文字（端末でビープ音）
        case 'b': return '\b'; // バックスペース（1文字戻る）
        case 't': return '\t'; // 水平タブ（Tabキー）
        case 'n': return '\n'; // 改行（newline）
        case 'v': return '\v'; // 垂直タブ（ほぼ使われない）
        case 'f': return '\f'; // 改ページ（ページ送り）
        case 'r': return '\r'; // 復帰（行頭に戻る）
        // [GNU] ASCIIエスケープ文字を表す \e は、GNU C の拡張機能。
        case 'e': return 27;   // ESCキー（ASCIIコード27）
        default: return *p;   // それ以外はそのままの文字
    }
}

// 閉じダブルクォーテーションを探す
static char *string_literal_end(char *p) {
    char *start = p;
    for (; *p != '"'; p++) {
        if (*p== '\n' || *p == '\0')
            error_at(start, "unclosed string literal");
        if (*p == '\\')
            p++;
    }
    return p;
}

static Token *read_string_literal(char *start) {
    char *end = string_literal_end(start + 1);
    char *buf = calloc(1, end - start);
    int len = 0;

    for (char *p = start + 1; p < end;) {
        if (*p == '\\') {
            buf[len++] = read_escaped_char(&p, p + 1);
        } else {
            buf[len++] = *p++;
        }
    }
    Token *tok = new_token(TK_STR, start, end + 1);
    tok->ty = array_of(ty_char, len + 1);
    tok->str = buf;
    return tok;
}

static void convert_keywords(Token *tok) {
    for (Token *t = tok; t->kind != TK_EOF; t = t->next)
        if (is_keyword(t))
            t->kind = TK_KEYWORD;
}

// 入力文字列pをトークナイズしてそれを返す
static Token *tokenize(char *filename, char *p) {
    current_filename = filename;
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

        // 文字列リテラル
        if (*p == '"') {
            cur = cur->next = read_string_literal(p);
            p += cur->len;
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
            if (len > 255) {
                error_at(start, "トークナイズできません");
            }
            
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
            } else if (len == 3 && memcmp(start, "int", 3) == 0) {
                cur = cur->next = new_token(TK_PUNCT, start, p);
            } else if (len == 4 && memcmp(start, "char", 4) == 0) {
                cur = cur->next = new_token(TK_PUNCT, start, p);
            } else {
                cur = cur->next = new_token(TK_IDENT, start, p);
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

// 指定されたファイルの内容を読み取ります。
static char *read_file(char *path) {
    FILE *fp;
    
    if (strcmp(path, "-") == 0) {
        // 指定されたファイル名が 「-」 の場合は標準入力から読み取る
        fp = stdin;
    } else {
        fp = fopen(path, "r");
        if (!fp)
            error("cannot open %s: %s", path, strerror(errno));
    }

    char *buf;
    size_t buflen;
    FILE *out = open_memstream(&buf, &buflen);

    // ファイル全体を読み込む。
    for (;;) {
        char buf2[4096];
        int n = fread(buf2, 1, sizeof(buf2), fp);
        if (n == 0)
            break;
        fwrite(buf2, 1, n, out);
    }

    if (fp != stdin)
        fclose(fp);

    // 最後の行が適切に `\n`で終了していることを確認する。
    fflush(out);
    if (buflen == 0 || buf[buflen - 1] != '\n')
        fputc('\n', out);
    fclose(out);
    return buf;
}

Token *tokenize_file(char *path) {
    char *input = read_file(path);
    user_input = input;
    return tokenize(path, input);
}
