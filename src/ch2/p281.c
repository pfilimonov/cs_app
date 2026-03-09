#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int A(int k) { return -1 << k; }

int B(int k, int j) { return ~A(k) << j; }

int main(int argc, char *argv[]) {
  if (argc > 2) {
    int k = strtol(argv[1], NULL, 0);
    int j = strtol(argv[2], NULL, 0);
    printf("A: %b\nB: %b\n", A(k), B(k, j));
  } else {
    printf("Not enough args\n");
  }
  return 0;
}
