
.globl find_range_asm
# NEG, ZERO, POS, OTHER
find_range_asm:
  vxorps %xmm1, %xmm1, %xmm1 # set %xmm1 to 0
  ucomiss %xmm1, %xmm0 # compare x against 0 (x - 0)

  jp .L3
  ja .L2
  je .L1
  jb .L0

.L0:
  movl $0, %eax # neg
  ret
.L1:
  movl $1, %eax # zero
  ret
.L2:
  movl $2, %eax # pos
  ret
.L3:
  movl $3, %eax # nan
  ret

.section .note.GNU-stack,"",@progbits
