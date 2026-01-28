  .data
  .globl .L..26
.L..26:
  .byte 79
  .byte 75
  .byte 10
  .byte 0
  .data
  .globl .L..25
.L..25:
  .byte 49
  .byte 62
  .byte 61
  .byte 50
  .byte 0
  .data
  .globl .L..24
.L..24:
  .byte 49
  .byte 62
  .byte 61
  .byte 49
  .byte 0
  .data
  .globl .L..23
.L..23:
  .byte 49
  .byte 62
  .byte 61
  .byte 48
  .byte 0
  .data
  .globl .L..22
.L..22:
  .byte 49
  .byte 62
  .byte 50
  .byte 0
  .data
  .globl .L..21
.L..21:
  .byte 49
  .byte 62
  .byte 49
  .byte 0
  .data
  .globl .L..20
.L..20:
  .byte 49
  .byte 62
  .byte 48
  .byte 0
  .data
  .globl .L..19
.L..19:
  .byte 50
  .byte 60
  .byte 61
  .byte 49
  .byte 0
  .data
  .globl .L..18
.L..18:
  .byte 49
  .byte 60
  .byte 61
  .byte 49
  .byte 0
  .data
  .globl .L..17
.L..17:
  .byte 48
  .byte 60
  .byte 61
  .byte 49
  .byte 0
  .data
  .globl .L..16
.L..16:
  .byte 50
  .byte 60
  .byte 49
  .byte 0
  .data
  .globl .L..15
.L..15:
  .byte 49
  .byte 60
  .byte 49
  .byte 0
  .data
  .globl .L..14
.L..14:
  .byte 48
  .byte 60
  .byte 49
  .byte 0
  .data
  .globl .L..13
.L..13:
  .byte 52
  .byte 50
  .byte 33
  .byte 61
  .byte 52
  .byte 50
  .byte 0
  .data
  .globl .L..12
.L..12:
  .byte 48
  .byte 33
  .byte 61
  .byte 49
  .byte 0
  .data
  .globl .L..11
.L..11:
  .byte 52
  .byte 50
  .byte 61
  .byte 61
  .byte 52
  .byte 50
  .byte 0
  .data
  .globl .L..10
.L..10:
  .byte 48
  .byte 61
  .byte 61
  .byte 49
  .byte 0
  .data
  .globl .L..9
.L..9:
  .byte 45
  .byte 32
  .byte 45
  .byte 32
  .byte 43
  .byte 49
  .byte 48
  .byte 0
  .data
  .globl .L..8
.L..8:
  .byte 45
  .byte 32
  .byte 45
  .byte 49
  .byte 48
  .byte 0
  .data
  .globl .L..7
.L..7:
  .byte 45
  .byte 49
  .byte 48
  .byte 43
  .byte 50
  .byte 48
  .byte 0
  .data
  .globl .L..6
.L..6:
  .byte 40
  .byte 51
  .byte 43
  .byte 53
  .byte 41
  .byte 47
  .byte 50
  .byte 0
  .data
  .globl .L..5
.L..5:
  .byte 53
  .byte 42
  .byte 40
  .byte 57
  .byte 45
  .byte 54
  .byte 41
  .byte 0
  .data
  .globl .L..4
.L..4:
  .byte 53
  .byte 43
  .byte 54
  .byte 42
  .byte 55
  .byte 0
  .data
  .globl .L..3
.L..3:
  .byte 49
  .byte 50
  .byte 32
  .byte 43
  .byte 32
  .byte 51
  .byte 52
  .byte 32
  .byte 45
  .byte 32
  .byte 53
  .byte 0
  .data
  .globl .L..2
.L..2:
  .byte 53
  .byte 43
  .byte 50
  .byte 48
  .byte 45
  .byte 52
  .byte 0
  .data
  .globl .L..1
.L..1:
  .byte 52
  .byte 50
  .byte 0
  .data
  .globl .L..0
.L..0:
  .byte 48
  .byte 0
  .globl _main
  .text
_main:
  pushq %rbp
  movq %rsp, %rbp
  subq $0, %rsp
  movq $0, %rax
  pushq %rax
  movq $0, %rax
  pushq %rax
  lea .L..0(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $42, %rax
  pushq %rax
  movq $42, %rax
  pushq %rax
  lea .L..1(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $21, %rax
  pushq %rax
  movq $5, %rax
  pushq %rax
  movq $20, %rax
  popq %rdi
  addq %rdi, %rax
  pushq %rax
  movq $4, %rax
  popq %rdi
  subq %rax, %rdi
  movq %rdi, %rax
  pushq %rax
  lea .L..2(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $41, %rax
  pushq %rax
  movq $12, %rax
  pushq %rax
  movq $34, %rax
  popq %rdi
  addq %rdi, %rax
  pushq %rax
  movq $5, %rax
  popq %rdi
  subq %rax, %rdi
  movq %rdi, %rax
  pushq %rax
  lea .L..3(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $47, %rax
  pushq %rax
  movq $5, %rax
  pushq %rax
  movq $6, %rax
  pushq %rax
  movq $7, %rax
  popq %rdi
  imulq %rdi, %rax
  popq %rdi
  addq %rdi, %rax
  pushq %rax
  lea .L..4(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $15, %rax
  pushq %rax
  movq $5, %rax
  pushq %rax
  movq $9, %rax
  pushq %rax
  movq $6, %rax
  popq %rdi
  subq %rax, %rdi
  movq %rdi, %rax
  popq %rdi
  imulq %rdi, %rax
  pushq %rax
  lea .L..5(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $4, %rax
  pushq %rax
  movq $3, %rax
  pushq %rax
  movq $5, %rax
  popq %rdi
  addq %rdi, %rax
  pushq %rax
  movq $2, %rax
  popq %rdi
  movq %rax, %rcx
  movq %rdi, %rax
  cqo
  idivq %rcx
  pushq %rax
  lea .L..6(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $10, %rax
  pushq %rax
  movq $10, %rax
  negq %rax
  pushq %rax
  movq $20, %rax
  popq %rdi
  addq %rdi, %rax
  pushq %rax
  lea .L..7(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $10, %rax
  pushq %rax
  movq $10, %rax
  negq %rax
  negq %rax
  pushq %rax
  lea .L..8(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $10, %rax
  pushq %rax
  movq $10, %rax
  negq %rax
  negq %rax
  pushq %rax
  lea .L..9(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $0, %rax
  pushq %rax
  movq $0, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  sete %al
  movzbq %al, %rax
  pushq %rax
  lea .L..10(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $1, %rax
  pushq %rax
  movq $42, %rax
  pushq %rax
  movq $42, %rax
  popq %rdi
  cmpq %rax, %rdi
  sete %al
  movzbq %al, %rax
  pushq %rax
  lea .L..11(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $1, %rax
  pushq %rax
  movq $0, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setne %al
  movzbq %al, %rax
  pushq %rax
  lea .L..12(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $0, %rax
  pushq %rax
  movq $42, %rax
  pushq %rax
  movq $42, %rax
  popq %rdi
  cmpq %rax, %rdi
  setne %al
  movzbq %al, %rax
  pushq %rax
  lea .L..13(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $1, %rax
  pushq %rax
  movq $0, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setl %al
  movzbq %al, %rax
  pushq %rax
  lea .L..14(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $0, %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setl %al
  movzbq %al, %rax
  pushq %rax
  lea .L..15(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $0, %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setl %al
  movzbq %al, %rax
  pushq %rax
  lea .L..16(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $1, %rax
  pushq %rax
  movq $0, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setle %al
  movzbq %al, %rax
  pushq %rax
  lea .L..17(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $1, %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setle %al
  movzbq %al, %rax
  pushq %rax
  lea .L..18(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $0, %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setle %al
  movzbq %al, %rax
  pushq %rax
  lea .L..19(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $1, %rax
  pushq %rax
  movq $0, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setl %al
  movzbq %al, %rax
  pushq %rax
  lea .L..20(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $0, %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setl %al
  movzbq %al, %rax
  pushq %rax
  lea .L..21(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $0, %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setl %al
  movzbq %al, %rax
  pushq %rax
  lea .L..22(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $1, %rax
  pushq %rax
  movq $0, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setle %al
  movzbq %al, %rax
  pushq %rax
  lea .L..23(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $1, %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setle %al
  movzbq %al, %rax
  pushq %rax
  lea .L..24(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  movq $0, %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $1, %rax
  popq %rdi
  cmpq %rax, %rdi
  setle %al
  movzbq %al, %rax
  pushq %rax
  lea .L..25(%rip), %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  movq $0, %rax
  call _assert
  lea .L..26(%rip), %rax
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
