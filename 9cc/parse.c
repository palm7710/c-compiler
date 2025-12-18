#include "9cc.h"

// 解析中に作成されたすべてのローカル変数のインスタンスは、
// このリストに蓄積されます。
Obj *locals;

// 再帰深度制限（スタックオーバーフロー防止）
static int recursion_depth = 0;
#define MAX_RECURSION_DEPTH 1000

static Node *expr(Token **rest, Token *tok);
static Node *expr_stmt(Token **rest, Token *tok);
static Node *assign(Token **rest, Token *tok);
static Node *equality(Token **rest, Token *tok);
static Node *relational(Token **rest, Token *tok);
static Node *add(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *unary(Token **rest, Token *tok);
static Node *primary(Token **rest, Token *tok);

// 名前からローカル変数を検索します。
static Obj *find_var(Token *tok) {
    for (Obj *var = locals; var; var = var->next)
        if (strlen(var->name) == tok->len && !strncmp(tok->loc, var->name, tok->len))
            return var;
    return NULL;
}

// 左辺と右辺を受け取る2項演算子
static Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

// 2項演算子ノード
static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// 単項演算子ノード
static Node *new_unary(NodeKind kind, Node *expr) {
    Node *node = new_node(kind);
    node->lhs = expr;
    return node;
}

// 数値を受け取るノード
static Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

// 新規変数を受け取るノード
static Node *new_var_node(Obj *var) {
    Node *node = new_node(ND_VAR);
    node->var = var;
    return node;
}

// 新しいローカル変数作成
static Obj *new_lvar(char *name) {
    Obj *var = calloc(1, sizeof(Obj));
    if (!var) {
        error("メモリ不足です");
    }
    
    // 変数名の長さ制限（バッファオーバーフロー防止）
    size_t name_len = strlen(name);
    if (name_len > 255) {
        error("変数名が長すぎます");
    }
    
    var->name = malloc(name_len + 1);
    if (!var->name) {
        free(var);
        error("メモリ不足です");
    }
    strcpy(var->name, name);
    
    var->next = locals;
    locals = var;
    return var;
}

// stmt = expr - stmt
static Node *stmt(Token **rest, Token *tok) {
    return expr_stmt(rest, tok);
}

// ";"
static Node *expr_stmt(Token **rest, Token *tok) {
    Node *node = new_unary(ND_EXPR_STMT, expr(&tok, tok));
    if (!equal(tok, ";")) {
        if (tok->kind == TK_EOF) {
            error_tok(tok, "文の最後に ';' が必要です。プログラム全体をシングルクォートで囲んでください");
        } else {
            error_tok(tok, "';' が必要です");
        }
    }
    *rest = tok->next;
    return node;
}

// パーサー
// expr = assign
static Node *expr(Token **rest, Token *tok) {
    return assign(rest, tok);
}

// assign = equality ("=" assign)?
static Node *assign(Token **rest, Token *tok) {
    Node *node = equality(&tok, tok);
    if (equal(tok, "="))
        node = new_binary(ND_ASSIGN, node, assign(&tok, tok->next));
    *rest = tok;
    return node;
}

static Node *equality(Token **rest, Token *tok) {
    Node *node = relational(&tok, tok);

    for (;;) {
        if (equal(tok, "==")) {
            node = new_binary(ND_EQ, node, relational(&tok, tok->next));
            continue;
        }
        if (equal(tok, "!=")) {
            node = new_binary(ND_NE, node, relational(&tok, tok->next));
            continue;
        }

        *rest = tok;
        return node;
    }
}

static Node *relational(Token **rest, Token *tok) {
    Node *node = add(&tok, tok);

    for (;;) {
    if (equal(tok, "<")) {
        node = new_binary(ND_LT, node, add(&tok, tok->next));
        continue;
    }

    if (equal(tok, "<=")) {
        node = new_binary(ND_LE, node, add(&tok, tok->next));
        continue;
    }

    if (equal(tok, ">")) {
        node = new_binary(ND_LT, add(&tok, tok->next), node);
        continue;
    }

    if (equal(tok, ">=")) {
        node = new_binary(ND_LE, add(&tok, tok->next), node);
        continue;
    }

    *rest = tok;
    return node;
    }
}

static Node *add(Token **rest, Token *tok) {
    Node *node = mul(&tok, tok);

    for (;;) {
    if (equal(tok, "+")) {
        node = new_binary(ND_ADD, node, mul(&tok, tok->next));
        continue;
    }

    if (equal(tok, "-")) {
        node = new_binary(ND_SUB, node, mul(&tok, tok->next));
        continue;
    }

    *rest = tok;
    return node;
    }
}

static Node *mul(Token **rest, Token *tok) {
    Node *node = unary(&tok, tok);

    for (;;) {
    if (equal(tok, "*")) {
        node = new_binary(ND_MUL, node, unary(&tok, tok->next));
        continue;
    }

    if (equal(tok, "/")) {
        node = new_binary(ND_DIV, node, unary(&tok, tok->next));
        continue;
    }

    *rest = tok;
    return node;
    }
}

static Node *unary(Token **rest, Token *tok) {
    if (equal(tok, "+"))
        return unary(rest, tok->next);

    if (equal(tok, "-"))
        return new_unary(ND_NEG, unary(rest, tok->next));

    return primary(rest, tok);
}

// primary = "(" expr ")" | ident | num
static Node *primary(Token **rest, Token *tok) {
    // 次のトークンが "(" なら、"(" expr ")"
    if (equal(tok, "(")) {
        if (++recursion_depth > MAX_RECURSION_DEPTH) {
            error("式が複雑すぎます");
        }
        
        Node *node = expr(&tok, tok->next);
        recursion_depth--;
        *rest = skip(tok, ")");
        return node;
    }

    if (tok->kind == TK_IDENT) {
        Obj *var = find_var(tok);
        if (!var) {
            // 安全な文字列複製
            if (tok->len <= 0 || tok->len > 255) {
                error_tok(tok, "変数名の長さが不正です");
            }
            
            char *name = malloc(tok->len + 1);
            if (!name) {
                error("メモリ不足です");
            }
            memcpy(name, tok->loc, tok->len);
            name[tok->len] = '\0';
            
            var = new_lvar(name);
        }
        *rest = tok->next;
        return new_var_node(var);
    }

    // それ以外は数値
    if (tok->kind == TK_NUM) {
        Node *node = new_num(tok->val);
        *rest = tok->next;
        return node;
    }

    error_tok(tok, "数ではありません");
    return NULL;
}

// program = stmt*
Function *parse(Token *tok) {
    Node head = {};
    Node *cur = &head;

    while (tok->kind != TK_EOF)
        cur = cur->next = stmt(&tok, tok);
    
    Function *prog = calloc(1, sizeof(Function));
    prog->body = head.next;
    prog->locals = locals;
    return prog;
}
