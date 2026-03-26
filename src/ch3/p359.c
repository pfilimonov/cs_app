#include <stdint.h>

typedef __int128 int128_t;

void store_prod(int128_t *dest, int64_t x, int64_t y) {
  /* dest - %rdi, x - %rsi, y - %rdx */
  *dest = x * (int128_t)y;
}

/*
1	store_prod:
2	movq	%rdx, %rax
3	cqto (int128_t)y = R[%rdx]:R[%rax]. R[%rdx] = -y_63, R[%rax] = u(y)
4	movq	%rsi, %rcx
5	sarq	$63, %rcx R[%rcx] = -x_63
6	imulq	%rax, %rcx R[%rcx] = (u(y) * -x_63)
7	imulq	%rsi, %rdx R[%rdx] = (-y_63 * u(x))
8	addq	%rdx, %rcx R[%rcx] = p_h
9	mulq	%rsi = %rsi * %rax => R[%rdx]:R[%rax] = (ux * uy) = p_l
10 addq	%rcx, %rdx all high-order bytes
11 movq	%rax, (%rdi) move to dest (low order)
12 movq	%rdx, 8(%rdi) move to dest (high-order)
13 ret

*/
