#define _POSIX_C_SOURCE 200809L // POSIX標準（2008年版）の機能を有効
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct Node Node;

// tokenize.c

// トークンの種類
typedef enum {
    TK_IDENT,    // 識別子
    TK_PUNCT,    // 記号
    TK_KEYWORD,  // キーワード
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 整数トークン
    char *loc;      // kind が TK_NUM の場合、その数値
    int len;        // トークンの長さ
    int val;        // 数値の場合の値
};

extern char *user_input;

// parse.c

// ローカル変数
typedef struct Obj Obj;
struct Obj {
    Obj *next;
    char *name;  // 変数名
    int offset;  // RBPからのオフセット
};

// 関数
typedef struct Function Function;
struct Function {
    Node *body;
    Obj *locals;
    int stack_size;
};

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD,       // +
    ND_SUB,       // -
    ND_MUL,       // *
    ND_DIV,       // /
    ND_NEG,       // 単項 -
    ND_EQ,        // ==
    ND_NE,        // !=
    ND_LT,        // <
    ND_LE,        // <=
    ND_ASSIGN,    // =
    ND_ADDR,      // 単項 &
    ND_DEREF,     // 単行 *
    ND_RETURN,    // "return"
    ND_IF,        // "if"
    ND_FOR,       // "for" or "while"
    ND_BLOCK,     // { ... }
    ND_EXPR_STMT, // セミコロン
    ND_VAR,       //変数
    ND_NUM,       // 整数
} NodeKind;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの型
    Node *next;    // 次のノード
    Token *tok;    // 代表トークン
    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
    // "if"
    Node *cond;
    Node *then;
    Node *els;
    // "for"
    Node *init;
    Node *inc;
    //block
    Node *body;
    int val;       // kindがND_NUMの場合のみ使う
    Obj *var;      // kindがND_VARの場合のみ使う
    int offset;    // kindがND_LVARの場合のみ使う
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
Token *tokenize(char *p);
Function *parse(Token *tok);
void codegen(Function *code);
