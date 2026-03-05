#include <stdio.h>
#include <stdlib.h>

unsigned rotate_left(unsigned x, int n) {
  unsigned left = x << n;
  printf("Left: %#X\n", left);
  unsigned right = x >> ((sizeof(unsigned) << 3) - n - 1) >> 1;
  printf("Right: %#X\n", right);
  return left | right;
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    unsigned x = strtol(argv[1], NULL, 0);
    int n = strtol(argv[2], NULL, 0);
    printf("Rotate left for %#x and %d:\n%#x\n", x, n, rotate_left(x, n));
  } else {
    printf("Not enough argv\n");
  }

  return 0;
}
