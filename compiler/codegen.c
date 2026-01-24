#include "compiler.h"

static int depth;
static char *argreg8[] = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};
static char *argreg64[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
static Obj *current_fn;
#define MAX_STACK_DEPTH 10000

static void gen_expr(Node *node);
static void gen_stmt(Node *node);

static void println(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

static int count(void) {
    static int i = 1;
    return i++;
}

static void push(void) {
    if (depth >= MAX_STACK_DEPTH) {
        error("スタックが深すぎます");
    }
    println("  pushq %%rax");
    depth++;
}

static void pop(char *arg) {
    if (depth <= 0) {
        error("スタックが空です");
    }
    println("  popq %s", arg);
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
            println("  lea %d(%%rbp), %%rax", node->var->offset);
        } else {
            // グローバル関数
            println("  lea %s(%%rip), %%rax", node->var->name);
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
        println("  movsbq (%%rax), %%rax");
    else
        println("  movq (%%rax), %%rax");
}

// %rax をスタック先頭が指すアドレスに格納する。
static void store(Type *ty) {
    pop("%rdi");

    if (ty->size == 1)
        println("  movb %%al, (%%rdi)");
    else
        println("  movq %%rax, (%%rdi)");
}

static void gen_expr(Node *node) {
    switch (node->kind) {
    case ND_NUM:
        println("  movq $%d, %%rax", node->val);
        return;
    case ND_NEG:
        gen_expr(node->lhs);
        println("  negq %%rax");
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
    case ND_STMT_EXPR:
        for (Node *n = node->body; n; n = n->next)
            gen_stmt(n);
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

        println("  movq $0, %%rax");
        println("  call _%s", node->funcname);
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
        println("  addq %%rdi, %%rax");
        return;
    case ND_SUB:
        println("  subq %%rax, %%rdi");
        println("  movq %%rdi, %%rax");
        return;
    case ND_MUL:
        println("  imulq %%rdi, %%rax");
        return;
    case ND_DIV:
        println("  movq %%rax, %%rcx");
        println("  movq %%rdi, %%rax");
        println("  cqo");
        println("  idivq %%rcx");
        return;
    case ND_EQ:
        println("  cmpq %%rax, %%rdi");
        println("  sete %%al");
        println("  movzbq %%al, %%rax");
        return;
    case ND_NE:
        println("  cmpq %%rax, %%rdi");
        println("  setne %%al");
        println("  movzbq %%al, %%rax");
        return;
    case ND_LT:
        println("  cmpq %%rax, %%rdi");
        println("  setl %%al");
        println("  movzbq %%al, %%rax");
        return;
    case ND_LE:
        println("  cmpq %%rax, %%rdi");
        println("  setle %%al");
        println("  movzbq %%al, %%rax");
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
        println("  cmp $0, %%rax");
        println("  je  .L.else.%d", c);
        gen_stmt(node->then);
        println("  jmp .L.end.%d", c);
        println(".L.else.%d:", c);
        if (node->els)
            gen_stmt(node->els);
        println(".L.end.%d:", c);
        return;
    }
    case ND_FOR: {
        int c = count();
        if (node->init)
            gen_stmt(node->init);
        println(".L.begin.%d:", c);
        if (node->cond) {
            gen_expr(node->cond);
            println("  cmp $0, %%rax");
            println("  je  .L.end.%d", c);
        }
        gen_stmt(node->then);
        if (node->inc)
            gen_expr(node->inc);
        println("  jmp .L.begin.%d", c);
        println(".L.end.%d:", c);
        return;
    }
    case ND_BLOCK:
        for (Node *n = node->body; n; n = n->next)
            gen_stmt(n);
        return;
    case ND_RETURN:
        gen_expr(node->lhs);
        println("  jmp .L.return.%s", current_fn->name);
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

        println("  .data");                    // 以降を .data セクション（初期化済み/静的データ領域）として扱う
        println("  .globl %s", var->name);     // このグローバル変数を他ファイルから参照可能にする
        println("%s:", var->name);             // グローバル変数の先頭アドレスを示すラベルを定義
        
        if (var->init_data) {
            for (int i = 0; i < var->ty->size; i++)
                println("  .byte %d", var->init_data[i]);
        } else {
            println("  .zero %d", var->ty->size);  // 変数サイズ分の領域を確保し、すべて 0 で初期化
        }
    }
}

static void emit_text(Obj *prog) {
    for (Obj *fn = prog; fn; fn = fn->next)
    {
        if (!fn->is_function)
            continue;

        println("  .globl _%s", fn->name); // この関数は外から参照可能とリンカに伝える
        println("  .text");                // これ以降は命令コード（textセクション）
        println("_%s:", fn->name);         // 関数の入口ラベル
        current_fn = fn;

        // 初期化処理
        println("  pushq %%rbp");          // 呼び出し元の rbp をスタックに退避
        println("  movq %%rsp, %%rbp");    // この関数のスタックフレームを確立
        println("  subq $%d, %%rsp", fn->stack_size); // ローカル変数領域をまとめて確保

        // レジスタ経由で渡された引数をスタックに保存する
        int i = 0;
        for (Obj *var = fn->params; var; var = var->next) {
            if (var->ty->size == 1)
                println("  movb %s, %d(%%rbp)", argreg8[i++], var->offset);
            else
                println("  movq %s, %d(%%rbp)", argreg64[i++], var->offset); // レジスタ渡しされた引数を、スタック上のローカル変数として保存
        }
            
        // コードを出力する
        gen_stmt(fn->body);
        assert(depth == 0);

        // 終わり
        println(".L.return.%s:", fn->name); //アセンブリのラベル
        println("  movq %%rbp, %%rsp");     // ローカル変数全部破棄
        println("  popq %%rbp");            // 親のスタックフレームに戻る
        println("  ret");                   // 呼び出し元へ帰る
    }
}

void codegen(Obj *prog) {
    assign_lvar_offsets(prog);
    emit_data(prog);
    emit_text(prog);
}
