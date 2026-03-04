#include <stdio.h>

unsigned replace_byte(unsigned x, int i, unsigned char b) {
  int shift = i * 8;
  unsigned mask = ~(0xff << shift);
  printf("Mask: %#x\n", mask);
  printf("Shifted byte: %#x\n", b << shift);
  printf("X masked: %#x\n", x & mask);
  return (x & mask) | (b << shift);
}

int main(void) {
  printf("%#x\n", replace_byte(0x12345678, 2, 0xab));
  printf("%#x\n", replace_byte(0x12345678, 0, 0xAB));
  return 0;
}
