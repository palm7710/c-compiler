#define _POSIX_C_SOURCE 200809L // POSIX標準（2008年版）の機能を有効
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct Type Type;
typedef struct Node Node;

// string.c
char *format(char *fmt, ...);

// tokenize.c

// トークンの種類
typedef enum {
    TK_IDENT,    // 識別子
    TK_PUNCT,    // 記号
    TK_KEYWORD,  // キーワード
    TK_STR,      // 文字列リテラル
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 整数トークン
    Type *ty;       // 型、例えば int や int へのポインタ（TK_STR の場合にも使用）
    char *loc;      // kind が TK_NUM の場合、その数値
    int len;        // トークンの長さ
    char *str;      // 終端の '\0' を含む文字列リテラルの内容
    int val;        // 数値の場合の値
};

extern char *user_input;

// parse.c

// 変数または関数
typedef struct Obj Obj;
struct Obj {
    Obj *next;
    char *name;    // 変数名
    Type *ty;      // 型
    bool is_local; // ローカル、またはグローバル/関数

    // 関数
    // ローカル変数
    int offset;

    // グローバル変数または関数
    bool is_function;

    // グローバル変数
    char *init_data;

    // 関数
    Obj *params;
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
    ND_FUNCALL,   // 関数呼び出し
    ND_EXPR_STMT, // セミコロン
    ND_STMT_EXPR, // ステートメント式（複数の文）
    ND_VAR,       // 変数
    ND_NUM,       // 整数
} NodeKind;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの型
    Node *next;    // 次のノード
    Token *tok;    // 代表トークン
    Type *ty;      // 式の型
    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
    // "if"
    Node *cond;
    Node *then;
    Node *els;
    // "for"
    Node *init;
    Node *inc;
    //block か　ステートメント式（複数の文）
    Node *body;
    // Function call
    char *funcname;
    Node *args;
    
    int val;       // kindがND_NUMの場合のみ使う
    Obj *var;      // kindがND_VARの場合のみ使う
    int offset;    // kindがND_LVARの場合のみ使う
};

Obj *parse(Token *tok);

// type.c

typedef enum {
    TY_CHAR,
    TY_INT,
    TY_PTR,
    TY_FUNC,
    TY_ARRAY,
} TypeKind;

struct Type {
    TypeKind kind;

    // ポインター
    int size;   // sizeof()
    Type *base;

    // 宣言
    Token *name;

    // 配列
    int array_len;

    // 関数の型
    Type *return_ty;
    Type *params;
    Type *next;
};

extern Type *ty_char;
extern Type *ty_int;

bool is_integer(Type *ty);
Type *copy_type(Type *ty);
Type *pointer_to(Type *base);
Type *func_type(Type *return_ty);
Type *array_of(Type *base, int len);
void add_type(Node *node);

// codegen.c

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
bool consume(Token **rest, Token *tok, char *str);
Token *tokenize_file(char *filename);
Obj *parse(Token *tok);
void codegen(Obj *code);
