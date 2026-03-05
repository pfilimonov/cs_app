#include <stdio.h>
#include <stdlib.h>

int lower_one_mask(int n) {
  int first = -1 << (n - 1) << 1;
  return ~first;
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    int x = strtol(argv[1], NULL, 0);
    printf("lower one mask: %#x\n", lower_one_mask(x));
  }

  return 0;
}
