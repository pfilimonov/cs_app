	.file	"p447.c"
	.text
	.section	.rodata
.LC0:
	.string	"%ld\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movl	%edi, -68(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	$3, -48(%rbp)
	movq	$1, -40(%rbp)
	movq	$4, -32(%rbp)
	movq	$2, -24(%rbp)
	movq	$5, -16(%rbp)
	leaq	-48(%rbp), %rax
	movl	$5, %esi
	movq	%rax, %rdi
	call	bubble_b
	movl	$0, -52(%rbp)
	jmp	.L2
.L3:
	movl	-52(%rbp), %eax
	cltq
	movq	-48(%rbp,%rax,8), %rax
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -52(%rbp)
.L2:
	cmpl	$4, -52(%rbp)
	jle	.L3
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L5
	call	__stack_chk_fail@PLT
.L5:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.globl	bubble_a
	.type	bubble_a, @function
bubble_a:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	-48(%rbp), %rax
	subq	$1, %rax
	movq	%rax, -16(%rbp)
	jmp	.L7
.L11:
	movq	$0, -24(%rbp)
	jmp	.L8
.L10:
	movq	-24(%rbp), %rax
	addq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rdx
	movq	-24(%rbp), %rax
	leaq	0(,%rax,8), %rcx
	movq	-40(%rbp), %rax
	addq	%rcx, %rax
	movq	(%rax), %rax
	cmpq	%rax, %rdx
	jge	.L9
	movq	-24(%rbp), %rax
	addq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	-24(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movq	-24(%rbp), %rdx
	addq	$1, %rdx
	leaq	0(,%rdx,8), %rcx
	movq	-40(%rbp), %rdx
	addq	%rcx, %rdx
	movq	(%rax), %rax
	movq	%rax, (%rdx)
	movq	-24(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-40(%rbp), %rax
	addq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	%rax, (%rdx)
.L9:
	addq	$1, -24(%rbp)
.L8:
	movq	-24(%rbp), %rax
	cmpq	-16(%rbp), %rax
	jl	.L10
	subq	$1, -16(%rbp)
.L7:
	cmpq	$0, -16(%rbp)
	jg	.L11
	nop
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	bubble_a, .-bubble_a
	.globl	bubble_b
	.type	bubble_b, @function
bubble_b:
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	-48(%rbp), %rax
	subq	$1, %rax
	movq	%rax, -16(%rbp)
	jmp	.L13
.L17:
	movq	$0, -24(%rbp)
	jmp	.L14
.L16:
	movq	-24(%rbp), %rax
	addq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rdx
	movq	-24(%rbp), %rax
	leaq	0(,%rax,8), %rcx
	movq	-40(%rbp), %rax
	addq	%rcx, %rax
	movq	(%rax), %rax
	cmpq	%rax, %rdx
	jge	.L15
	movq	-24(%rbp), %rax
	addq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	-24(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movq	-24(%rbp), %rdx
	addq	$1, %rdx
	leaq	0(,%rdx,8), %rcx
	movq	-40(%rbp), %rdx
	addq	%rcx, %rdx
	movq	(%rax), %rax
	movq	%rax, (%rdx)
	movq	-24(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-40(%rbp), %rax
	addq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	%rax, (%rdx)
.L15:
	addq	$1, -24(%rbp)
.L14:
	movq	-24(%rbp), %rax
	cmpq	-16(%rbp), %rax
	jl	.L16
	subq	$1, -16(%rbp)
.L13:
	cmpq	$0, -16(%rbp)
	jg	.L17
	nop
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	bubble_b, .-bubble_b
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
