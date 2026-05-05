	.file	"p450.c"
	.text
	.globl	switchv
	.type	switchv, @function
switchv:
.LFB23:
	.cfi_startproc
	movl	$3276, %eax
	cmpq	$3, %rdi
	je	.L1
	jg	.L3
	movl	$2730, %eax
	testq	%rdi, %rdi
	je	.L1
	cmpq	$2, %rdi
	movl	$3003, %eax
	movl	$3549, %edx
	cmovne	%rdx, %rax
	ret
.L3:
	cmpq	$5, %rdi
	movl	$3003, %eax
	movl	$3549, %edx
	cmovne	%rdx, %rax
.L1:
	ret
	.cfi_endproc
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
	cmpq	$7, %rbx
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
