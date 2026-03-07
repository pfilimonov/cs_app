#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int tsub_ok(int x, int y) {
  int sub = x - y;

  int res = 1;

  (y == INT_MIN) && (res = 0);
  printf("sub: %d\n", sub);
  int neg_of = !(sub & INT_MIN) && (x & INT_MIN) && !(y & INT_MIN);
  int pos_of = (sub & INT_MIN) && !(x & INT_MIN) && (y & INT_MIN);
  printf("neg_of: %d, pos_of: %d\n", neg_of, pos_of);
  return res && !pos_of && !neg_of;
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    int x = strtol(argv[1], NULL, 0);
    int y = strtol(argv[2], NULL, 0);
    printf("tsub_ok x=%d, y=%d: %d\n", x, y, tsub_ok(x, y));
  } else {
    printf("Not enough args");
  }

  return 0;
}
