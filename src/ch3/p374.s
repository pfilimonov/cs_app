.globl find_range_asm
# NEG, ZERO, POS, OTHER
find_range_asm:
  movl $0, %edi
  movl $1, %esi
  movl $2, %edx
  movl $3, %ecx
  movl %edi, %eax # default = neg
  vxorps %xmm1, %xmm1, %xmm1 # set %xmm1 to 0
  ucomiss %xmm1, %xmm0 # compare x against 0 (x - 0)
  cmova %edx, %eax # if x > 0, %eax = 2
  cmove %esi, %eax # if x == 0, %eax = 1
  cmovb %edi, %eax # if x < 0, %eax = 0
  cmovp %ecx, %eax # NaN
  ret

.section .note.GNU-stack,"",@progbits
