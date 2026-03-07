#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* aux */
int signed_high_prod(int x, int y);

unsigned unsigned_high_prod(unsigned x, unsigned y) {
  unsigned sum = signed_high_prod(x, y);
  int x_w_1 = !!(x & INT_MIN);
  int y_w_1 = !!(y & INT_MIN);
  unsigned rem = (x_w_1 * y + y_w_1 * x);
  return rem + sum;
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    unsigned x = strtol(argv[1], NULL, 0);
    unsigned y = strtol(argv[2], NULL, 0);
    printf("unsigned_high_prod x=%d, y=%d: %d\n", x, y,
           unsigned_high_prod(x, y));
  } else {
    printf("Not enough args");
  }

  return 0;
}
