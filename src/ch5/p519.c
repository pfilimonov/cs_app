#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static inline uint64_t rdtsc() {
  uint32_t lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
}

void measure_perf(void (*f)(float[], float[], long)) {
  long N = 1000000;
  float *arr = malloc(N * sizeof(float));
  float *p = malloc(N * sizeof(float));

  for (int i = 0; i < N; i++)
    arr[i] = (double)(i + 1);

  double min_cpe = 1e9;
  for (int iter = 0; iter < 10; iter++) {
    uint64_t s = rdtsc();
    f(arr, p, N);
    uint64_t e = rdtsc();
    double cpe = (double)(e - s) / N;
    if (cpe < min_cpe)
      min_cpe = cpe;
  }

  printf("res: %.1f, min CPE: %.5f\n", p[N - 1], min_cpe);

  free(arr);
}

void psum1(float a[], float p[], long n) {
  long i;
  p[0] = a[0];
  for (i = 1; i < n; i++)
    p[i] = p[i - 1] + a[i];
}

void psum2(float a[], float p[], long n) {
  long i;
  p[0] = a[0];
  for (i = 1; i < n - 1; i += 2) {
    float mid_val = p[i - 1] + a[i];
    p[i] = mid_val;
    p[i + 1] = mid_val + a[i + 1];
  }
  /* For even n, finish remaining element */
  if (i < n)
    p[i] = p[i - 1] + a[i];
}

void psum1a(float a[], float p[], long n) {
  long i;
  /* last_val holds p[i-1]; val holds p [i] */
  float last_val, val;
  last_val = p[0] = a[0];
  for (i = 1; i < n; i++) {
    val = last_val + a[i];
    p[i] = val;
    last_val = val;
  }
}

void psum_my(float a[], float p[], long n) {
  p[0] = a[0];
  p[1] = a[0] + a[1];
  for (int i = 2; i < n; i++) {
    p[i] = p[i - 2] + (a[i - 1] + a[i]);
  }
}

void psum_my2(float a[], float p[], long n) {
  p[0] = a[0];
  p[1] = a[0] + a[1];
  p[2] = p[1] + a[2];
  int i;
  for (i = 3; i < n - 2; i += 3) {
    p[i] = p[i - 3] + (a[i - 2] + a[i - 1] + a[i]);
    p[i + 1] = p[i - 2] + (a[i - 1] + a[i] + a[i + 1]);
    p[i + 2] = p[i - 1] + (a[i] + a[i + 1] + a[i + 2]);
  }

  for (; i < n; i++) {
    p[i] = p[i - 2] + (a[i - 1] + a[i]);
  }
}

void psum_4_1a(float a[], float p[], long n) {
  long i;
  float val, last_val;
  float tmp, tmp1, tmp2, tmp3;
  last_val = p[0] = a[0];

  for (i = 1; i < n - 4; i++) {
    tmp = last_val + a[i];
    tmp1 = tmp + a[i + 1];
    tmp2 = tmp1 + a[i + 2];
    tmp3 = tmp2 + a[i + 3];

    p[i] = tmp;
    p[i + 1] = tmp1;
    p[i + 2] = tmp2;
    p[i + 3] = tmp3;

    /* key point */
    last_val = last_val + (a[i] + a[i + 1] + a[i + 2] + a[i + 3]);
  }

  for (; i < n; i++) {
    last_val += a[i];
    p[i] = last_val;
  }
}

int main(int argc, char *argv[]) {
  printf("psum1\n");
  measure_perf(psum1);

  printf("psum2\n");
  measure_perf(psum2);

  printf("psum1a\n");
  measure_perf(psum1a);

  printf("psum_my\n");
  measure_perf(psum_my);

  printf("psum_my2\n");
  measure_perf(psum_my2);

  printf("psum_4_1a\n");
  measure_perf(psum_4_1a);

  return 0;
}
