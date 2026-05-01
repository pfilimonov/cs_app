
.globl main
main:
	subq $40, %rsp
  movq $3, 0(%rsp)
  movq $1, 8(%rsp)
  movq $4, 16(%rsp)
  movq $2, 24(%rsp)
  movq $5, 32(%rsp)
  movq %rsp, %rdi 
	movl	$5, %esi
	call	bubble_b
  
  # Test
  movq (%rsp), %rdx
  movq 8(%rsp), %rcx
  sub %rcx, %rdx
  jg .fail

  movq 8(%rsp), %rdx
  movq 16(%rsp), %rcx
  sub %rcx, %rdx
  jg .fail

  movq 16(%rsp), %rdx
  movq 24(%rsp), %rcx
  sub %rcx, %rdx
  jg .fail

  movq 24(%rsp), %rdx
  movq 32(%rsp), %rcx
  sub %rcx, %rdx
  jg .fail

  addq $40, %rsp
  movq $0, %rax
  ret

.fail:
  addq $40, %rsp
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
  movq %rdi, %rdx # copy pointer to increment
.iloop:
  movq %r8, %r10 # copy last
  subq %r9, %r10
  jle .donei
  
  # Test condition
  # Get data[i]
  movq (%rdx), %rcx # data[i]
  movq 8(%rdx), %rbx # data[i+1]
  movq %rbx, %rbp
  subq %rcx, %rbp  
  cmovl %rbx, %rbp # t
  cmovl %rcx, %rbx
  cmovl %rbp, %rcx

  movq %rcx, (%rdx)
  movq %rbx, 8(%rdx)

  # End test condition

  addq $1, %r9 # increment i
  addq $8, %rdx
  jmp .iloop

.donei:
  subq $1, %r8 # decrement last
  jmp .oloop
.doneo:
  ret

.section .note.GNU-stack,"",@progbits
