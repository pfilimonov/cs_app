#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned long K = sizeof(unsigned long);

static inline uint64_t rdtsc() {
  uint32_t lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
}

void *memset(void *s, int c, size_t n) {
  unsigned char cb = (unsigned char)c;
  size_t cnt = 0;
  unsigned char *schar = s;
  while (cnt < n && (unsigned long)schar % sizeof(unsigned long) != 0) {
    *schar++ = cb;
    cnt++;
  }

  unsigned long pack = 0;
  for (size_t i = 0; i < K; i++) {
    pack += (cb << (K * i));
  }

  unsigned long *slong = (unsigned long *)schar;
  size_t i;
  size_t lim = n - cnt - K + 1;
  for (i = 0; i < lim; i += K) {
    *slong++ = pack;
  }

  schar = (unsigned char *)slong;
  for (; i < n; i++) {
    *schar++ = cb;
  }
}

void *basic_memset(void *s, int c, size_t n) {
  size_t cnt = 0;
  unsigned char *schar = s;
  while (cnt < n) {
    *schar++ = (unsigned char)c;
    cnt++;
  }
  return s;
}

int main(int argc, char *argv[]) {
  long N = 100000;
  long *arr = malloc(N * sizeof(long));

  double min_cpe = 1e9;
  for (int iter = 0; iter < 10; iter++) {
    uint64_t s = rdtsc();
    memset(arr, 0, N * sizeof(long));
    uint64_t e = rdtsc();
    double cpe = (double)(e - s) / (N * sizeof(long));
    if (cpe < min_cpe)
      min_cpe = cpe;
  }

  for (int i = 0; i < N; i++)
    assert(arr[i] == 0);

  printf("min CPE: %.2f\n", min_cpe);
  return 0;
}
