#include <stdio.h>
#include <stdlib.h>

int any_odd_one(unsigned x) {
  int mask = 0x55555555;
  return !!(x & mask);
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    unsigned x = strtol(argv[1], NULL, 0);
    printf("Any odd one: %d\n", any_odd_one(x));
  }

  return 0;
}
