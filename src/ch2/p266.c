#include <stdio.h>
#include <stdlib.h>

int leftmost_one(unsigned x) {
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;

  return (x >> 1) + (x && 1);
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    unsigned x = strtol(argv[1], NULL, 0);
    printf("Odd ones: %x\n", leftmost_one(x));
  }

  return 0;
}
