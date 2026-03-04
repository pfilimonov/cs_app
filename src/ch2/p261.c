#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int x = (argc > 1) ? strtol(argv[1], NULL, 0) : INT_MIN;

  // A
  printf("Any bit of x equals 1: %d\n", !!(x & ~0));

  // B
  printf("Any bit of x equals 0: %d\n", !!(x ^ ~0));

  // C
  printf("ANy bit in the least significant byte of x equals 1: %d\n",
         !!(x & 0xff));

  // D
  int shift_val = (sizeof(int) - 1) << 3;
  int xright = x >> shift_val;
  int msb = x & 0xff;
  printf("%#x\n", xright);
  printf("Any bit in the most significant byte of x equals 0: %d\n",
         !!(msb ^ 0xff));

  return 0;
}
