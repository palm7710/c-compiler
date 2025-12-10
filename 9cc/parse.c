#include "9cc.h"

static Node *expr(Token **rest, Token *tok);
static Node *equality(Token **rest, Token *tok);
static Node *relational(Token **rest, Token *tok);
static Node *add(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *unary(Token **rest, Token *tok);
static Node *primary(Token **rest, Token *tok);

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

// パーサー
static Node *expr(Token **rest, Token *tok) {
    return equality(rest, tok);
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

static Node *primary(Token **rest, Token *tok) {
    // 次のトークンが "(" なら、"(" expr ")"
    if (equal(tok, "(")) {
        Node *node = expr(&tok, tok->next);
        *rest = skip(tok, ")");
        return node;
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

Node *parse(Token *tok) {
    Token *rest;
    Node *node = expr(&rest, tok);
    if (rest->kind != TK_EOF)
        error_tok(rest, "余分なトークンです");
    return node;
}
