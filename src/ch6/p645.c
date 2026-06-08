#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 10

static inline uint64_t rdtsc() {
  uint32_t lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
}

int check(int *dst, int *src, int dim) {
  int i, j;

  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      if (dst[j * dim + i] != src[i * dim + j])
        return 0;

  return 1;
}

void measure_perf(void (*f)(int *, int *, int), int N) {
  printf("Test performance for N=%d\n", N);
  int *dst = malloc(N * N * sizeof(int));
  int *src = malloc(N * N * sizeof(int));

  for (int i = 0; i < N * N; i++)
    src[i] = i;

  double min_cpe = 1e9;
  for (int iter = 0; iter < 10; iter++) {
    uint64_t s = rdtsc();
    f(dst, src, N);
    uint64_t e = rdtsc();
    double cpe = (double)(e - s) / N;
    if (cpe < min_cpe)
      min_cpe = cpe;
  }

  printf("min CPE: %.5f\n", min_cpe);
  printf("Correct %d\n", check(dst, src, N));

  free(dst);
  free(src);
}

void transpose_base(int *dst, int *src, int dim) {
  int i, j;

  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      dst[j * dim + i] = src[i * dim + j];
}

void transpose_opt(int *dst, int *src, int dim) {
  int i, j;

  for (i = 0; i < dim; i++) {
    for (j = 0; j < dim; j += 2) {
      dst[j * dim + i] = src[i * dim + j];
      dst[(j + 1) * dim + i] = src[i * dim + j + 1];
    }
    for (; j < dim; j++) {
      dst[j * dim + i] = src[i * dim + j];
    }
  }
}

void transpose_opt_perm(int *dst, int *src, int dim) {
  int i, j;

  for (j = 0; j < dim; j++) {
    for (i = 0; i < dim; i += 2) {
      dst[j * dim + i] = src[i * dim + j];
      dst[j * dim + i + 1] = src[(i + 1) * dim + j];
    }
    for (; i < dim; i++) {
      dst[j * dim + i] = src[i * dim + j];
    }
  }
}

void transpose_block2(int *dst, int *src, int dim) {
  int i, j;

  for (j = 0; j < dim; j += 2) {
    for (i = 0; i < dim; i += 2) {
      dst[j * dim + i] = src[i * dim + j];
      dst[j * dim + i + 1] = src[(i + 1) * dim + j];
      dst[(j + 1) * dim + i] = src[i * dim + (j + 1)];
      dst[(j + 1) * dim + i + 1] = src[(i + 1) * dim + (j + 1)];
    }
    for (; i < dim; i++) {
      dst[j * dim + i] = src[i * dim + j];
      dst[(j + 1) * dim + i] = src[i * dim + (j + 1)];
    }
  }

  for (; j < dim; j++) {
    for (i = 0; i < dim; i += 2) {
      dst[j * dim + i] = src[i * dim + j];
      dst[j * dim + i + 1] = src[(i + 1) * dim + j];
    }
    for (; i < dim; i++) {
      dst[j * dim + i] = src[i * dim + j];
    }
  }
}

void transpose_block(int *dst, int *src, int dim) {
  int i, j, k, l;
  int offset;
  for (i = 0; i < dim - BLOCK_SIZE; i += BLOCK_SIZE) {
    for (j = 0; j < dim - BLOCK_SIZE; j += BLOCK_SIZE) {
      for (k = i; k < i + BLOCK_SIZE; k++) {
        for (l = j; l < j + BLOCK_SIZE; l++) {
          dst[l * dim + k] = src[k * dim + l];
        }
      }
    }
  }

  offset = i;

  for (; i < dim; i++) {
    for (j = 0; j < offset; j += BLOCK_SIZE) {
      for (l = j; l < j + BLOCK_SIZE; l++) {
        dst[l * dim + i] = src[i * dim + l];
      }
    }
  }

  for (i = 0; i < dim; i++) {
    for (j = offset; j < dim; j++) {
      dst[j * dim + i] = src[i * dim + j];
    }
  }
}

int main(int argc, char *argv[]) {

  int N = 10000;
  // printf("\nBaseline:\n");
  // measure_perf(transpose_base, N);

  // printf("\nOptimized:\n");
  // measure_perf(transpose_opt, N);

  // printf("\nOptimized with loop permutation:\n");
  // measure_perf(transpose_opt_perm, N);

  printf("\nOptimized with block 2:\n");
  measure_perf(transpose_block2, N);

  printf("\nOptimized with block %d:\n", BLOCK_SIZE);
  measure_perf(transpose_block, N);

  return 0;
}
