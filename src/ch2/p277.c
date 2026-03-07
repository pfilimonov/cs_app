#include <limits.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

int m17(int x) { return x + (x << 4); }

int m_7(int x) { return x - (x << 3); }

int m60(int x) { return (x << 6) - (x << 2); }

int m_112(int x) { return (x << 4) - (x << 7); }

int main(int argc, char *argv[]) {
  if (argc > 1) {
    int x = strtol(argv[1], NULL, 0);
    printf("K = 17: %d\nK = -7: %d\nK = 60: %d\nK = -112: %d\n", m17(x), m_7(x),
           m60(x), m_112(x));
  } else {
    printf("Not enough args");
  }

  return 0;
}
