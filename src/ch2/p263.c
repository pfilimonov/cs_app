#include <assert.h>
#include <stdio.h>

unsigned srl(unsigned x, int k) {
  /* Perform shift arithmetically */
  unsigned xsra = (int)x >> k;
  unsigned mask = (~0 << (sizeof(int) * 8 - k)) & xsra;
  return xsra ^ mask;
}

int sra(int x, int k) {
  /* Perform shift logically */
  int xsrl = (unsigned)x >> k;
  int is_pos_x = !((1 << ((sizeof(int) << 3) - 1)) & x);
  int mask = is_pos_x - 1;
  int mask_shifted = mask << (sizeof(int) * 8 - k);
  return xsrl ^ mask_shifted;
}

int main(void) {
  // test srl

  assert(srl(12345, 5) == (unsigned)12345 >> 5);

  // test sra
  assert(sra(12345, 5) == 12345 >> 5);
  assert(sra(-12345, 5) == -12345 >> 5);

  printf("Success");

  return 0;
}
