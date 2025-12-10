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

static void gen_expr(Node *node) {
    switch (node->kind) {
    case ND_NUM:
        printf("  movq $%d, %%rax\n", node->val);
        return;
    case ND_NEG:
        gen_expr(node->lhs);
        printf("  negq %%rax\n");
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

void codegen(Node *node) {
    printf(".globl _main\n");
    printf(".text\n");
    printf("_main:\n");

    gen_expr(node);
    printf("  ret\n");

    assert(depth == 0);
}
