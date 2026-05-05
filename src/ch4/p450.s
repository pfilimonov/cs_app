.section .rodata
table:
 .quad .L0
 .quad .L1
 .quad .L2
 .quad .L3
 .quad .L4
 .quad .L5

	.file	"p450.c"
	.text
	.globl	switchv
	.type	switchv, @function
switchv:
  movq $0xddd, %rax
  # cmp $5, %rdi
  movq %rdi, %r8
  subq $0, %r8
  jl .done
  subq $5, %r8
  # end cmp
  jg .done # default


  # Calculate addr offset 8 * %rdi
  movq $0, %r8 # addr offset var   
  movq %rdi, %r9 # counter
.mulloop:
  subq $1, %r9
  jl .endmulloop
  addq $8, %r8
  jmp .mulloop
.endmulloop:

  # Jump to switch case
  movq table(%r8), %r9
  pushq %r9
  ret

.L0:
  movq $0xaaa, %rax
  jmp .done
.L1:
  jmp .done 
.L2:
  jmp .L5
.L3:
  movq $0xccc, %rax
  jmp .done
.L4:
  jmp .done 
.L5:
  movq $0xbbb, %rax
  jmp .done
.done:
  ret


.LFE23:
	.size	switchv, .-switchv
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"idx = %ld, val = 0x%lx\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB24:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	$-1, %rbx
.L9:
	movq	%rbx, %rdi
	call	switchv
	movq	%rax, %rcx
	movq	%rbx, %rdx
	movl	$.LC0, %esi
	movl	$2, %edi
	movl	$0, %eax
	call	__printf_chk
	addq	$1, %rbx
  movq %rbx, %r11
	subq	$7, %r11
	jne	.L9
	movl	$0, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE24:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
  
