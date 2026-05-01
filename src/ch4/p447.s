
.globl main
main:
	movq	$3, -32(%rsp)
	movq	$1, -24(%rsp)
	movq	$4, -16(%rsp)
	movq	$2, -8(%rsp)
	movq	$5, (%rsp)
  subq 40, %rsp

  movq %rsp, %rdi 
	movl	$5, %esi
	call	bubble_b
  
  # Test
  movq -32(%rsp), %rdx
  movq -24(%rsp), %rcx
  sub %rcx, %rdx
  jg .fail

  movq -24(%rsp), %rdx
  movq -16(%rsp), %rcx
  sub %rcx, %rdx
  jg .fail

  movq -16(%rsp), %rdx
  movq -8(%rsp), %rcx
  sub %rcx, %rdx
  jg .fail

  movq -8(%rsp), %rdx
  movq (%rsp), %rcx
  sub %rcx, %rdx
  jg .fail

  movq $0, %rax
  ret

.fail:
  movq $1, %rax
  ret
  

bubble_b:
  # last
  movq %rsi, %r8 
  subq $1, %r8 # init last
# outer loop
.oloop:
  jle .doneo
  # inner loop
  # i
  movq $0, %r9
.iloop:
  movq %r8, %r10 # copy last
  subq %r9, %r10
  jle .donei
  
  # Test
  # Get data[i]
  movq $0, %r10
  movq %rdi, %rdx # copy pointer to increment
.incrptr:
  movq %r9, %r11
  subq %r10, %r11
  jle .doneiptr
  addq $1, %r10
  addq $8, %rdx
  jmp .incrptr
.doneiptr:
  movq (%rdx), %rcx # data[i]
  movq 8(%rdx), %rbx # data[i+1]
  subq %rcx, %rbx  
  cmovl %rbx, %rbp # t
  cmovl %rcx, %rbx
  cmovl %rbp, %rcx

  movq %rcx, (%rdx)
  movq %rbx, 8(%rdx)

  addq $1, %r9 # increment i
  jmp .iloop

.donei:
  subq $1, %r8 # decrement last
  jmp .oloop
.doneo:
  ret

.section .note.GNU-stack,"",@progbits
