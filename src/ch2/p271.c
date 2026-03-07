#include <stdio.h>
#include <stdlib.h>

typedef unsigned packed_t;

int xbyte(packed_t word, int bytenum) {
  int remaining = (3 << 3) - (bytenum << 3);
  return (int)word << remaining >> (3 << 3);
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    int word = strtol(argv[1], NULL, 0);
    int bytenum = strtol(argv[2], NULL, 0);
    printf("xbyte for word=%#x, bytenum=%d: %d\n", word, bytenum,
           xbyte(word, bytenum));
  } else {
    printf("Not enough args");
  }

  return 0;
}
