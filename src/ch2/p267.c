#include <stdio.h>
#include <stdlib.h>

int int_size_is_32() {
  int set_msb = 1 << 31;
  int beyond_msb = set_msb << 1;

  return set_msb && (!beyond_msb);
}

int int_size_is_16() {
  int set_msb = 1 << 15 << 15 << 1;
  int beyond_msb = set_msb << 1;
  return set_msb && (!beyond_msb);
}

int main(int argc, char *argv[]) {
  printf("Bad int size: %d\n", int_size_is_32());

  return 0;
}
