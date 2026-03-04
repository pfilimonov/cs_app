#include <stdio.h>

int main(int argc, char *argv[]) {
  int x = 0x89abcdef;
  int y = 0x76543210;

  printf("%#x\n", (x & 0xff) | (y & ~0xff));

  return 0;
}
