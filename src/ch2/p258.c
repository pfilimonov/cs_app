#include <stdio.h>

int is_little_endian() {
  int test_val = 1;
  return *(unsigned char *)&test_val == 1;
}

int main(int argc, char *argv[]) {
  printf("Is little endian: %d\n", is_little_endian());
  return 0;
}
