#include "compiler.h"

static int depth;
static char *argreg[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
static Obj *current_fn;
#define MAX_STACK_DEPTH 10000

static void gen_expr(Node *node);

static int count(void) {
    static int i = 1;
    return i++;
}

static void push(void) {
    if (depth >= MAX_STACK_DEPTH) {
        error("スタックが深すぎます");
    }
    printf("  pushq %%rax\n");
    depth++;
}

static void pop(char *arg) {
    if (depth <= 0) {
        error("スタックが空です");
    }
    printf("  popq %s\n", arg);
    depth--;
}

// 数値 n を align の最小の倍数に切り上げます。
// 例えば、align_to(5, 8) は 8 を、align_to(11, 8) は 16 を返します。
static int align_to(int n, int align) {
    return (n + align - 1) / align * align;
}

// 指定されたノードの絶対アドレスを計算します。
// 指定されたノードがメモリ内に存在しない場合はエラーになります。
static void gen_addr(Node *node) {
    switch (node->kind) {
    case ND_VAR:
        printf("  lea %d(%%rbp), %%rax\n", node->var->offset);
        return;
    case ND_DEREF:
        gen_expr(node->lhs);
        return;
    default:
        break;
    }

    error_tok(node->tok, "not an lvalue");
}

// %rax が指している場所から値を読み込む。
static void load(Type *ty) {
    if (ty->kind == TY_ARRAY) {
        return;
    }
    printf("  movq (%%rax), %%rax\n");
}

// %rax をスタック先頭が指すアドレスに格納する。
static void store(void) {
    pop("%rdi");
    printf("  movq %%rax, (%%rdi)\n");
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
        load(node->ty);
        return;
    case ND_DEREF:
        gen_expr(node->lhs);
        load(node->ty);
        return;
    case ND_ADDR:
        gen_addr(node->lhs);
        return;
    case ND_ASSIGN:
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        store();
        return;
    case ND_FUNCALL: {
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen_expr(arg);
            push();
            nargs++;
        }

        for (int i = nargs - 1; i >= 0; i--)
            pop(argreg[i]);

        printf("  movq $0, %%rax\n");
        printf("  call _%s\n", node->funcname);
        return;
    }
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

    error_tok(node->tok, "invalid expression");
}

static void gen_stmt(Node *node) {
    switch (node->kind) {
    case ND_IF: {
        int c = count();
        gen_expr(node->cond);
        printf("  cmp $0, %%rax\n");
        printf("  je  .L.else.%d\n", c);
        gen_stmt(node->then);
        printf("  jmp .L.end.%d\n", c);
        printf(".L.else.%d:\n", c);
        if (node->els)
            gen_stmt(node->els);
        printf(".L.end.%d:\n", c);
        return;
    }
    case ND_FOR: {
        int c = count();
        if (node->init)
            gen_stmt(node->init);
        printf(".L.begin.%d:\n", c);
        if (node->cond) {
            gen_expr(node->cond);
            printf("  cmp $0, %%rax\n");
            printf("  je  .L.end.%d\n", c);
        }
        gen_stmt(node->then);
        if (node->inc)
            gen_expr(node->inc);
        printf("  jmp .L.begin.%d\n", c);
        printf(".L.end.%d:\n", c);
        return;
    }
    case ND_BLOCK:
        for (Node *n = node->body; n; n = n->next)
            gen_stmt(n);
        return;
    case ND_RETURN:
        gen_expr(node->lhs);
        printf("  jmp .L.return.%s\n", current_fn->name);
        return;
    case ND_EXPR_STMT:
        gen_expr(node->lhs);
        return;
    default:
        error_tok(node->tok, "invalid statement");
    }
}

static void assign_lvar_offsets(Obj *prog) {
    for (Obj *fn = prog; fn; fn = fn->next)
    {
        if (!fn->is_function)
            continue;

        int offset = 0;
        for (Obj *var = fn->locals; var; var = var->next) {
            offset += var->ty->size;
            var->offset = -offset;
        }
        fn->stack_size = align_to(offset, 16);
    } 
}

void codegen(Obj *prog) {
    assign_lvar_offsets(prog);

    for (Obj *fn = prog; fn; fn = fn->next)
    {
        if (!fn->is_function)
            continue;

        printf("  .globl _%s\n", fn->name);
        printf("  .text\n");
        printf("_%s:\n", fn->name);
        current_fn = fn;

        // 初期化処理
        printf("  pushq %%rbp\n");
        printf("  movq %%rsp, %%rbp\n");
        printf("  subq $%d, %%rsp\n", fn->stack_size);

        // レジスタ経由で渡された引数をスタックに保存する
        int i = 0;
        for (Obj *var = fn->params; var; var = var->next)
            printf("  movq %s, %d(%%rbp)\n", argreg[i++], var->offset);

        // コードを出力する
        gen_stmt(fn->body);
        assert(depth == 0);

        // 終わり
        printf(".L.return.%s:\n", fn->name);
        printf("  movq %%rbp, %%rsp\n");
        printf("  popq %%rbp\n");
        printf("  ret\n");
    }
}
