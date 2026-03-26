#include <stdio.h>

int fact32(int n) {
  int fact = 1;
  do {
    fact *= (n--);
  } while (n > 1);
  return fact;
}

long fact64(long n) {
  long fact = 1;
  do {
    fact *= (n--);
  } while (n > 1);
  return fact;
}

int main(void) {
  // printf("%d\n", fact32(10));
  // return 0;
  int n = 10;
  while (fact32(n) > 0) {
    printf("Check 32bit: %d\n", n);
    printf("Fact: %d\n", fact32(n));
    n++;
  }
  printf("Max 32bit integer: %d\n", n - 1);
  long m = 10;
  while (fact64(m) > 0) {
    printf("Check 64bit: %ld\n", m);
    m++;
  }
  printf("Max 64bit integer: %ld\n", m - 1);
  return 0;
}
