#include <stdio.h>
#include <stdlib.h>

int fits_bits(int x, int n) {
  int w = sizeof(int) << 3;
  int offset = w - n;
  return (x << offset >> offset) == x;
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    int x = strtol(argv[1], NULL, 0);
    int n = strtol(argv[2], NULL, 0);
    printf("Fits bits for x=%#x, n=%d: %d\n", x, n, fits_bits(x, n));
  } else {
    printf("Not enough args");
  }

  return 0;
}
