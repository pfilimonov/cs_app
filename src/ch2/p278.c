
#include <limits.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

int divide_power2(int x, int k) {
  int x_neg = x & INT_MIN;
  int res = 1;
  (!x_neg && (res = x >> k)) || (x_neg && (res = (x + (1 << k) - 1) >> k));
  return res;
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    int x = strtol(argv[1], NULL, 0);
    int k = strtol(argv[2], NULL, 0);
    printf("%d / (2 ^ %d) = %d\n", x, k, divide_power2(x, k));
  } else {
    printf("Not enough args");
  }

  return 0;
}
