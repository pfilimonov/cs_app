#include <stdio.h>
#include <stdlib.h>

int odd_ones(unsigned x) {
  x ^= x >> 16;
  x ^= x >> 8;
  x ^= x >> 4;
  x ^= x >> 2;
  x ^= x >> 1;
  return x & 1;
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    unsigned x = strtol(argv[1], NULL, 0);
    printf("Odd ones: %d\n", odd_ones(x));
  }

  return 0;
}
