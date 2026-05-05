#include <stdio.h>
/* Example use of switch statement */
long switchv(long idx) {
  long result = 0;
  switch (idx) {
  case 0:
    result = 0xaaa;
    break;
  case 2:
  case 5:
    result = 0xbbb;
    break;
  case 3:
    result = 0xccc;
    break;
  default:
    result = 0xddd;
  }
  return result;
}
/* Testing Code */
#define CNT 8
#define MINVAL -1

/*
idx = -1, val = 0xddd
idx = 0, val = 0xaaa
idx = 1, val = 0xddd
idx = 2, val = 0xbbb
idx = 3, val = 0xccc
idx = 4, val = 0xddd
idx = 5, val = 0xbbb
idx = 6, val = 0xddd
*/
int main() {
  long vals[CNT];
  long i;
  for (i = 0; i < CNT; i++) {
    vals[i] = switchv(i + MINVAL);
    printf("idx = %ld, val = 0x%lx\n", i + MINVAL, vals[i]);
  }
  return 0;
}
