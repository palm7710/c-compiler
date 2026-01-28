  .data
  .globl .L..10
.L..10:
  .byte 79
  .byte 75
  .byte 10
  .byte 0
  .data
  .globl .L..9
.L..9:
  .byte 40
  .byte 123
  .byte 32
  .byte 115
  .byte 117
  .byte 98
  .byte 95
  .byte 99
  .byte 104
  .byte 97
  .byte 114
  .byte 40
  .byte 55
  .byte 44
  .byte 32
  .byte 51
  .byte 44
  .byte 32
  .byte 51
  .byte 41
  .byte 59
  .byte 32
  .byte 125
  .byte 41
  .byte 0
  .data
  .globl .L..8
.L..8:
  .byte 102
  .byte 105
  .byte 98
  .byte 40
  .byte 57
  .byte 41
  .byte 0
  .data
  .globl .L..7
.L..7:
  .byte 115
  .byte 117
  .byte 98
  .byte 50
  .byte 40
  .byte 52
  .byte 44
  .byte 51
  .byte 41
  .byte 0
  .data
  .globl .L..6
.L..6:
  .byte 97
  .byte 100
  .byte 100
  .byte 50
  .byte 40
  .byte 51
  .byte 44
  .byte 52
  .byte 41
  .byte 0
  .data
  .globl .L..5
.L..5:
  .byte 97
  .byte 100
  .byte 100
  .byte 54
  .byte 40
  .byte 49
  .byte 44
  .byte 50
  .byte 44
  .byte 97
  .byte 100
  .byte 100
  .byte 54
  .byte 40
  .byte 51
  .byte 44
  .byte 97
  .byte 100
  .byte 100
  .byte 54
  .byte 40
  .byte 52
  .byte 44
  .byte 53
  .byte 44
  .byte 54
  .byte 44
  .byte 55
  .byte 44
  .byte 56
  .byte 44
  .byte 57
  .byte 41
  .byte 44
  .byte 49
  .byte 48
  .byte 44
  .byte 49
  .byte 49
  .byte 44
  .byte 49
  .byte 50
  .byte 44
  .byte 49
  .byte 51
  .byte 41
  .byte 44
  .byte 49
  .byte 52
  .byte 44
  .byte 49
  .byte 53
  .byte 44
  .byte 49
  .byte 54
  .byte 41
  .byte 0
  .data
  .globl .L..4
.L..4:
  .byte 97
  .byte 100
  .byte 100
  .byte 54
  .byte 40
  .byte 49
  .byte 44
  .byte 50
  .byte 44
  .byte 97
  .byte 100
  .byte 100
  .byte 54
  .byte 40
  .byte 51
  .byte 44
  .byte 52
  .byte 44
  .byte 53
  .byte 44
  .byte 54
  .byte 44
  .byte 55
  .byte 44
  .byte 56
  .byte 41
  .byte 44
  .byte 57
  .byte 44
  .byte 49
  .byte 48
  .byte 44
  .byte 49
  .byte 49
  .byte 41
  .byte 0
  .data
  .globl .L..3
.L..3:
  .byte 97
  .byte 100
  .byte 100
  .byte 54
  .byte 40
  .byte 49
  .byte 44
  .byte 50
  .byte 44
  .byte 51
  .byte 44
  .byte 52
  .byte 44
  .byte 53
  .byte 44
  .byte 54
  .byte 41
  .byte 0
  .data
  .globl .L..2
.L..2:
  .byte 115
  .byte 117
  .byte 98
  .byte 50
  .byte 40
  .byte 53
  .byte 44
  .byte 32
  .byte 51
  .byte 41
  .byte 0
  .data
  .globl .L..1
.L..1:
  .byte 97
  .byte 100
  .byte 100
  .byte 50
  .byte 40
  .byte 51
  .byte 44
  .byte 32
  .byte 53
  .byte 41
  .byte 0
  .data
  .globl .L..0
.L..0:
  .byte 114
  .byte 101
  .byte 116
  .byte 51
  .byte 40
  .byte 41
  .byte 0
  .globl _main
  .text
_main:
  pushq %rbp
  movq %rsp, %rbp
  subq $0, %rsp
  movq $3, %rax
  pushq %rax
  movq $0, %rax
  call _ret3
  pushq %rax
  lea .L..0(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $8, %rax
  pushq %rax
  movq $3, %rax
  pushq %rax
  movq $5, %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _add2
  pushq %rax
  lea .L..1(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $2, %rax
  pushq %rax
  movq $5, %rax
  pushq %rax
  movq $3, %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _sub2
  pushq %rax
  lea .L..2(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $21, %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $3, %rax
  pushq %rax
  movq $4, %rax
  pushq %rax
  movq $5, %rax
  pushq %rax
  movq $6, %rax
  pushq %rax
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _add6
  pushq %rax
  lea .L..3(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $66, %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $3, %rax
  pushq %rax
  movq $4, %rax
  pushq %rax
  movq $5, %rax
  pushq %rax
  movq $6, %rax
  pushq %rax
  movq $7, %rax
  pushq %rax
  movq $8, %rax
  pushq %rax
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _add6
  pushq %rax
  movq $9, %rax
  pushq %rax
  movq $10, %rax
  pushq %rax
  movq $11, %rax
  pushq %rax
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _add6
  pushq %rax
  lea .L..4(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $136, %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $3, %rax
  pushq %rax
  movq $4, %rax
  pushq %rax
  movq $5, %rax
  pushq %rax
  movq $6, %rax
  pushq %rax
  movq $7, %rax
  pushq %rax
  movq $8, %rax
  pushq %rax
  movq $9, %rax
  pushq %rax
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _add6
  pushq %rax
  movq $10, %rax
  pushq %rax
  movq $11, %rax
  pushq %rax
  movq $12, %rax
  pushq %rax
  movq $13, %rax
  pushq %rax
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _add6
  pushq %rax
  movq $14, %rax
  pushq %rax
  movq $15, %rax
  pushq %rax
  movq $16, %rax
  pushq %rax
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _add6
  pushq %rax
  lea .L..5(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $7, %rax
  pushq %rax
  movq $3, %rax
  pushq %rax
  movq $4, %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _add2
  pushq %rax
  lea .L..6(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $1, %rax
  pushq %rax
  movq $4, %rax
  pushq %rax
  movq $3, %rax
  pushq %rax
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _sub2
  pushq %rax
  lea .L..7(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $55, %rax
  pushq %rax
  movq $9, %rax
  pushq %rax
  popq %rdi
  movq $0, %rax
  call _fib
  pushq %rax
  lea .L..8(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $1, %rax
  pushq %rax
  movq $7, %rax
  pushq %rax
  movq $3, %rax
  pushq %rax
  movq $3, %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _sub_char
  pushq %rax
  lea .L..9(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  lea .L..10(%rip), %rax
  pushq %rax
  popq %rdi
  movq $0, %rax
  call _printf
  movq $0, %rax
  jmp .L.return.main
.L.return.main:
  movq %rbp, %rsp
  popq %rbp
  ret
  .globl _fib
  .text
_fib:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rdi, -8(%rbp)
  lea -8(%rbp), %rax
  movq (%rax), %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setle %al
  movzbq %al, %rax
  cmp $0, %rax
  je  .L.else.1
  movq $1, %rax
  jmp .L.return.fib
  jmp .L.end.1
.L.else.1:
.L.end.1:
  lea -8(%rbp), %rax
  movq (%rax), %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  subq %rax, %rdi
  movq %rdi, %rax
  pushq %rax
  popq %rdi
  movq $0, %rax
  call _fib
  pushq %rax
  lea -8(%rbp), %rax
  movq (%rax), %rax
  pushq %rax
  movq $2, %rax
  popq %rdi
  subq %rax, %rdi
  movq %rdi, %rax
  pushq %rax
  popq %rdi
  movq $0, %rax
  call _fib
  popq %rdi
  addq %rdi, %rax
  jmp .L.return.fib
.L.return.fib:
  movq %rbp, %rsp
  popq %rbp
  ret
  .globl _sub_char
  .text
_sub_char:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movb %dil, -1(%rbp)
  movb %sil, -2(%rbp)
  movb %dl, -3(%rbp)
  lea -1(%rbp), %rax
  movsbq (%rax), %rax
  pushq %rax
  lea -2(%rbp), %rax
  movsbq (%rax), %rax
  popq %rdi
  subq %rax, %rdi
  movq %rdi, %rax
  pushq %rax
  lea -3(%rbp), %rax
  movsbq (%rax), %rax
  popq %rdi
  subq %rax, %rdi
  movq %rdi, %rax
  jmp .L.return.sub_char
.L.return.sub_char:
  movq %rbp, %rsp
  popq %rbp
  ret
  .globl _addx
  .text
_addx:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  lea -8(%rbp), %rax
  movq (%rax), %rax
  movq (%rax), %rax
  pushq %rax
  lea -16(%rbp), %rax
  movq (%rax), %rax
  popq %rdi
  addq %rdi, %rax
  jmp .L.return.addx
.L.return.addx:
  movq %rbp, %rsp
  popq %rbp
  ret
  .globl _add6
  .text
_add6:
  pushq %rbp
  movq %rsp, %rbp
  subq $48, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rdx, -24(%rbp)
  movq %rcx, -32(%rbp)
  movq %r8, -40(%rbp)
  movq %r9, -48(%rbp)
  lea -8(%rbp), %rax
  movq (%rax), %rax
  pushq %rax
  lea -16(%rbp), %rax
  movq (%rax), %rax
  popq %rdi
  addq %rdi, %rax
  pushq %rax
  lea -24(%rbp), %rax
  movq (%rax), %rax
  popq %rdi
  addq %rdi, %rax
  pushq %rax
  lea -32(%rbp), %rax
  movq (%rax), %rax
  popq %rdi
  addq %rdi, %rax
  pushq %rax
  lea -40(%rbp), %rax
  movq (%rax), %rax
  popq %rdi
  addq %rdi, %rax
  pushq %rax
  lea -48(%rbp), %rax
  movq (%rax), %rax
  popq %rdi
  addq %rdi, %rax
  jmp .L.return.add6
.L.return.add6:
  movq %rbp, %rsp
  popq %rbp
  ret
  .globl _sub2
  .text
_sub2:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  lea -8(%rbp), %rax
  movq (%rax), %rax
  pushq %rax
  lea -16(%rbp), %rax
  movq (%rax), %rax
  popq %rdi
  subq %rax, %rdi
  movq %rdi, %rax
  jmp .L.return.sub2
.L.return.sub2:
  movq %rbp, %rsp
  popq %rbp
  ret
  .globl _add2
  .text
_add2:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  lea -8(%rbp), %rax
  movq (%rax), %rax
  pushq %rax
  lea -16(%rbp), %rax
  movq (%rax), %rax
  popq %rdi
  addq %rdi, %rax
  jmp .L.return.add2
.L.return.add2:
  movq %rbp, %rsp
  popq %rbp
  ret
  .globl _ret3
  .text
_ret3:
  pushq %rbp
  movq %rsp, %rbp
  subq $0, %rsp
  movq $3, %rax
  jmp .L.return.ret3
  movq $5, %rax
  jmp .L.return.ret3
.L.return.ret3:
  movq %rbp, %rsp
  popq %rbp
  ret
