
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

int mul3div4(int x) {
  int big = x & (~0x3);
  int little = x & 0x3;
  printf("Big: %d, little: %d\n", big, little);
  printf("Big / 4: %d\n", divide_power2(big, 2));
  int res =
      divide_power2(big, 2) + divide_power2(big, 2) + divide_power2(big, 2);
  (((x & INT_MIN) == 0) && (res += (little + little + little) >> 2)) ||
      (((x & INT_MIN) != 0) &&
       (res += ((little + little + little) + (1 << 2) - 1) >> 2));
  return res;
}

int mul3div4_cool(int x) { return divide_power2(x, 1) + divide_power2(x, 2); }

int main(int argc, char *argv[]) {
  if (argc > 1) {
    int x = strtol(argv[1], NULL, 0);
    printf("3*%d/4 = %d\n", x, mul3div4_cool(x));
  } else {
    printf("Not enough args");
  }

  return 0;
}
