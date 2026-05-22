#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static inline uint64_t rdtsc() {
  uint32_t lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
}

double poly_base(double a[], double x, long degree) {
  long i;
  double result = a[0];
  double xpwr = x; /* Equals x⁁i at start of loop */
  for (i = 1; i <= degree; i++) {
    result += a[i] * xpwr;
    xpwr = x * xpwr;
  }
  return result;
}

double poly_unroll6(double a[], double x, long degree) {
  long K = 6;
  long i;
  double result;
  long limit = degree - K + 1;
  double xpwr0 = 1.0f;
  double xpwr1 = x;
  double xpwr2 = x * x;
  double xpwr3 = xpwr2 * x;
  double xpwr4 = xpwr3 * x;
  double xpwr5 = xpwr4 * x;
  double x6 = xpwr5 * x;
  double d0 = 0.0f, d1 = 0.0f, d2 = 0.0f, d3 = 0.0f, d4 = 0.0f, d5 = 0.0f;
  for (i = 0; i <= limit; i += K) {
    d0 += a[i] * xpwr0;
    d1 += a[i + 1] * xpwr1;
    d2 += a[i + 2] * xpwr2;
    d3 += a[i + 3] * xpwr3;
    d4 += a[i + 4] * xpwr4;
    d5 += a[i + 5] * xpwr5;

    xpwr0 = x6 * xpwr0;
    xpwr1 = x6 * xpwr1;
    xpwr2 = x6 * xpwr2;
    xpwr3 = x6 * xpwr3;
    xpwr4 = x6 * xpwr4;
    xpwr5 = x6 * xpwr5;
  }

  result = d0 + d1 + d2 + d3 + d4 + d5;

  for (; i <= degree; i++) {
    result += a[i] * xpwr0;
    xpwr0 = x * xpwr0;
  }
  return result;
}

double poly_unroll8(double a[], double x, long degree) {
  long K = 8;
  long i;
  double result;
  long limit = degree - K + 1;
  double xpwr0 = 1.0f;
  double xpwr1 = x;
  double xpwr2 = x * x;
  double xpwr3 = xpwr2 * x;
  double xpwr4 = xpwr3 * x;
  double xpwr5 = xpwr4 * x;
  double xpwr6 = xpwr5 * x;
  double xpwr7 = xpwr6 * x;
  double x8 = xpwr7 * x;
  double d0 = 0.0f, d1 = 0.0f, d2 = 0.0f, d3 = 0.0f, d4 = 0.0f, d5 = 0.0f,
         d6 = 0.0f, d7 = 0.0f;
  for (i = 0; i <= limit; i += K) {
    d0 += a[i] * xpwr0;
    d1 += a[i + 1] * xpwr1;
    d2 += a[i + 2] * xpwr2;
    d3 += a[i + 3] * xpwr3;
    d4 += a[i + 4] * xpwr4;
    d5 += a[i + 5] * xpwr5;
    d6 += a[i + 6] * xpwr6;
    d7 += a[i + 7] * xpwr7;

    xpwr0 = x8 * xpwr0;
    xpwr1 = x8 * xpwr1;
    xpwr2 = x8 * xpwr2;
    xpwr3 = x8 * xpwr3;
    xpwr4 = x8 * xpwr4;
    xpwr5 = x8 * xpwr5;
    xpwr6 = x8 * xpwr6;
    xpwr7 = x8 * xpwr7;
  }

  result = d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7;

  for (; i <= degree; i++) {
    result += a[i] * xpwr0;
    xpwr0 = x * xpwr0;
  }
  return result;
}

double poly_6_3a(double a[], double x, long degree) {
  long i = 1;
  double result = a[0];
  double result1 = 0;
  double result2 = 0;

  double xpwr = x;
  double xpwr1 = x * x * x;
  double xpwr2 = x * x * x * x * x;

  double xpwr_step = x * x * x * x * x * x;
  for (; i <= degree - 6; i += 6) {
    result = result + (a[i] * xpwr + a[i + 1] * xpwr * x);
    result1 = result1 + (a[i + 2] * xpwr1 + a[i + 3] * xpwr1 * x);
    result2 = result2 + (a[i + 4] * xpwr2 + a[i + 5] * xpwr2 * x);

    xpwr *= xpwr_step;
    xpwr1 *= xpwr_step;
    xpwr2 *= xpwr_step;
  }

  for (; i <= degree; i++) {
    result = result + a[i] * xpwr;
    xpwr *= x;
  }

  return result + result1 + result2;
}

int main(int argc, char *argv[]) {
  long N = 3;
  double *arr = malloc(N * sizeof(double));
  for (int i = 0; i < N; i++) {
    arr[i] = (double)(i + 1);
  }

  double ref = poly_base(arr, 2, N - 1);
  assert(fabs(poly_unroll6(arr, 2, N - 1) - ref) < 1e-6);
  assert(fabs(poly_unroll8(arr, 2, N - 1) - ref) < 1e-6);

  free(arr);

  N = 1000000;
  arr = malloc(N * sizeof(double));
  for (int i = 0; i < N; i++) {
    arr[i] = (double)(i + 1);
  }

  double min_cpe = 1e9;
  double acc = 0.0f;
  for (int iter = 0; iter < 10; iter++) {
    uint64_t s = rdtsc();
    acc += poly_unroll6(arr, 2, N - 1);
    uint64_t e = rdtsc();
    double cpe = (double)(e - s) / N;
    if (cpe < min_cpe)
      min_cpe = cpe;
  }

  printf("K=6. res: %.1f, min CPE: %.5f\n", acc, min_cpe);

  min_cpe = 1e9;
  acc = 0.0f;
  for (int iter = 0; iter < 10; iter++) {
    uint64_t s = rdtsc();
    acc += poly_unroll8(arr, 2, N - 1);
    uint64_t e = rdtsc();
    double cpe = (double)(e - s) / N;
    if (cpe < min_cpe)
      min_cpe = cpe;
  }

  printf("K=8. res: %.1f, min CPE: %.5f\n", acc, min_cpe);

  min_cpe = 1e9;
  acc = 0.0f;
  for (int iter = 0; iter < 10; iter++) {
    uint64_t s = rdtsc();
    acc += poly_6_3a(arr, 2, N - 1);
    uint64_t e = rdtsc();
    double cpe = (double)(e - s) / N;
    if (cpe < min_cpe)
      min_cpe = cpe;
  }

  printf("K=6(3a). res: %.1f, min CPE: %.5f\n", acc, min_cpe);

  free(arr);
  return 0;
}
