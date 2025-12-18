#include "9cc.h"

static int depth;

static void push(void) {
    printf("  pushq %%rax\n");
    depth++;
}

static void pop(char *arg) {
    printf("  popq %s\n", arg);
    depth--;
}

// 指定されたノードの絶対アドレスを計算します。
// 指定されたノードがメモリ内に存在しない場合はエラーになります。
static void gen_addr(Node *node) {
    if (node->kind == ND_VAR) {
        int offset = (node->name - 'a' + 1) * 8;
        printf("  lea %d(%%rbp), %%rax\n", -offset);
        return;
    }

    error("not an lvalue");
}


static void gen_expr(Node *node) {
    switch (node->kind) {
    case ND_NUM:
        printf("  movq $%d, %%rax\n", node->val);
        return;
    case ND_NEG:
        gen_expr(node->lhs);
        printf("  negq %%rax\n");
        return;
    case ND_VAR:
        gen_addr(node);
        printf("  movq (%%rax), %%rax\n");
        return;
    case ND_ASSIGN:
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        pop("%rdi");
        printf("  movq %%rax, (%%rdi)\n");
        return;
    default:
        break;
    }

    // 二項演算子
    gen_expr(node->lhs);
    push();
    gen_expr(node->rhs);
    pop("%rdi");

    switch(node->kind) {
    case ND_ADD:
        printf("  addq %%rdi, %%rax\n");
        return;
    case ND_SUB:
        printf("  subq %%rax, %%rdi\n");
        printf("  movq %%rdi, %%rax\n");
        return;
    case ND_MUL:
        printf("  imulq %%rdi, %%rax\n");
        return;
    case ND_DIV:
        printf("  movq %%rax, %%rcx\n");
        printf("  movq %%rdi, %%rax\n");
        printf("  cqo\n");
        printf("  idivq %%rcx\n");
        return;
    case ND_EQ:
        printf("  cmpq %%rax, %%rdi\n");
        printf("  sete %%al\n");
        printf("  movzbq %%al, %%rax\n");
        return;
    case ND_NE:
        printf("  cmpq %%rax, %%rdi\n");
        printf("  setne %%al\n");
        printf("  movzbq %%al, %%rax\n");
        return;
    case ND_LT:
        printf("  cmpq %%rax, %%rdi\n");
        printf("  setl %%al\n");
        printf("  movzbq %%al, %%rax\n");
        return;
    case ND_LE:
        printf("  cmpq %%rax, %%rdi\n");
        printf("  setle %%al\n");
        printf("  movzbq %%al, %%rax\n");
        return;
    default:
        return;
    }

    error("不正な式です");
}

static void gen_stmt(Node *node) {
    if (node->kind == ND_EXPR_STMT) {
        gen_expr(node->lhs);
        return;
    }

    error("無効な文です");
}

void codegen(Node *node) {
    printf(".globl _main\n");
    printf(".text\n");
    printf("_main:\n");

    // 初期化処理
    printf("  pushq %%rbp\n");
    printf("  movq %%rsp, %%rbp\n");
    printf("  subq $208, %%rsp\n");

    for (Node *n = node; n; n = n->next) {
        gen_stmt(n);
        assert(depth == 0);
    }
    printf("  movq %%rbp, %%rsp\n");
    printf("  popq %%rbp\n");
    printf("  ret\n");
}
