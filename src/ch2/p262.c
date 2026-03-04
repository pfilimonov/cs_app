#include <stdio.h>

int int_shifts_are_arithmetic() {
  int x = ~0;
  return (x >> ((sizeof(int) - 1) << 3)) == x;
}

int main(void) {
  printf("Shifts are arithmetic: %d\n", int_shifts_are_arithmetic());
  return 0;
}
