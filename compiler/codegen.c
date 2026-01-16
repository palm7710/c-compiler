#include "compiler.h"

static int depth;
static char *argreg8[] = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};
static char *argreg64[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
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
        if (node->var->is_local) {
            // ローカル変数
            printf("  lea %d(%%rbp), %%rax\n", node->var->offset);
        } else {
            // グローバル関数
            printf("  lea %s(%%rip), %%rax\n", node->var->name);
        }
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
    if (ty->size == 1)
        printf("  movsbq (%%rax), %%rax\n");
    else
        printf("  movq (%%rax), %%rax\n");
}

// %rax をスタック先頭が指すアドレスに格納する。
static void store(Type *ty) {
    pop("%rdi");

    if (ty->size == 1)
        printf("  movb %%al, (%%rdi)\n");
    else
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
        store(node->ty);
        return;
    case ND_FUNCALL: {
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen_expr(arg);
            push();
            nargs++;
        }

        for (int i = nargs - 1; i >= 0; i--)
            pop(argreg64[i]);

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

static void emit_data(Obj *prog) {
    for (Obj *var =prog; var; var = var->next) {
        if (var->is_function)
            continue;

        printf("  .data\n");                    // 以降を .data セクション（初期化済み/静的データ領域）として扱う
        printf("  .globl %s\n", var->name);     // このグローバル変数を他ファイルから参照可能にする
        printf("%s:\n", var->name);             // グローバル変数の先頭アドレスを示すラベルを定義
        printf("  .zero %d\n", var->ty->size);  // 変数サイズ分の領域を確保し、すべて 0 で初期化
    }
}

static void emit_text(Obj *prog) {
    for (Obj *fn = prog; fn; fn = fn->next)
    {
        if (!fn->is_function)
            continue;

        printf("  .globl _%s\n", fn->name); // この関数は外から参照可能とリンカに伝える
        printf("  .text\n");                // これ以降は命令コード（textセクション）
        printf("_%s:\n", fn->name);         // 関数の入口ラベル
        current_fn = fn;

        // 初期化処理
        printf("  pushq %%rbp\n");          // 呼び出し元の rbp をスタックに退避
        printf("  movq %%rsp, %%rbp\n");    // この関数のスタックフレームを確立
        printf("  subq $%d, %%rsp\n", fn->stack_size); // ローカル変数領域をまとめて確保

        // レジスタ経由で渡された引数をスタックに保存する
        int i = 0;
        for (Obj *var = fn->params; var; var = var->next) {
            if (var->ty->size == 1)
                printf("  movb %s, %d(%%rbp)\n", argreg8[i++], var->offset);
            else
                printf("  movq %s, %d(%%rbp)\n", argreg64[i++], var->offset); // レジスタ渡しされた引数を、スタック上のローカル変数として保存
        }
            
        // コードを出力する
        gen_stmt(fn->body);
        assert(depth == 0);

        // 終わり
        printf(".L.return.%s:\n", fn->name); //アセンブリのラベル
        printf("  movq %%rbp, %%rsp\n");     // ローカル変数全部破棄
        printf("  popq %%rbp\n");            // 親のスタックフレームに戻る
        printf("  ret\n");                   // 呼び出し元へ帰る
    }
}

void codegen(Obj *prog) {
    assign_lvar_offsets(prog);
    emit_data(prog);
    emit_text(prog);
}
