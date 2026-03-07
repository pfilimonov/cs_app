#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int saturating_add(int x, int y) {
  int x_pos = (x >> 31) == 0;
  int y_pos = (y >> 31) == 0;
  int x_y_pos = ((x + y) >> 31) == 0;
  int pos_of = (x_pos && y_pos && !x_y_pos);
  int neg_of = (!x_pos && !y_pos && x_y_pos);
  int no_of = !pos_of && !neg_of;

  return ((neg_of << 31) & INT_MIN) + ((pos_of << 31 >> 31) & INT_MAX) +
         ((no_of << 31 >> 31) & (x + y));
}

int saturating_add_cool(int x, int y) {
  int sum = x + y;
  int mask = INT_MIN;

  int pos_of = (sum & mask) && !(x & mask) && !(y & mask);
  int neg_of = !(sum & mask) && (x & mask) && (y & mask);

  (pos_of && (sum = INT_MAX) || neg_of && (sum = INT_MIN));

  return sum;
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    int x = strtol(argv[1], NULL, 0);
    int y = strtol(argv[2], NULL, 0);
    printf("saturating_add for x=%d, y=%d: %d\n", x, y,
           saturating_add_cool(x, y));
  } else {
    printf("Not enough args");
  }

  return 0;
}
