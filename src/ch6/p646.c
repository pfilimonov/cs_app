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

int check(int *G, int N) {
  int i, j;

  for (i = 0; i < N; i++)
    for (j = i; j < N; j++)
      if (G[N * i + j] != G[N * j + i])
        return 0;

  return 1;
}

void measure_perf(void (*f)(int *, int), int N) {
  printf("Test performance for N=%d\n", N);
  int *G = malloc(N * N * sizeof(int));

  for (int i = 0; i < N * N; i++)
    G[i] = i % 2;

  double min_cpe = 1e9;
  for (int iter = 0; iter < 20; iter++) {
    uint64_t s = rdtsc();
    f(G, N);
    uint64_t e = rdtsc();
    double cpe = (double)(e - s) / N / N;
    if (cpe < min_cpe)
      min_cpe = cpe;
  }

  printf("min CPE: %.5f\n", min_cpe);
  printf("Correct %d\n", check(G, N));

  free(G);
}

void col_convert(int *G, int dim) {
  int i, j;

  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      G[j * dim + i] = G[j * dim + i] || G[i * dim + j];
}

void col_convert_perm(int *G, int dim) {
  int i, j;

  for (j = 0; j < dim; j++)
    for (i = 0; i < dim; i++)
      G[j * dim + i] = G[j * dim + i] || G[i * dim + j];
}

void col_convert_perm_unroll21(int *G, int dim) {
  int i, j;

  for (j = 0; j < dim; j++) {
    for (i = 0; i < dim - 1; i += 2) {
      G[j * dim + i] = G[j * dim + i] || G[i * dim + j];
      G[j * dim + (i + 1)] = G[j * dim + (i + 1)] || G[(i + 1) * dim + j];
    }
    for (; i < dim; i++) {
      G[j * dim + i] = G[j * dim + i] || G[i * dim + j];
    }
  }
}

void col_convert_halved(int *G, int dim) {
  int i, j;

  for (j = 0; j < dim; j++)
    for (i = j; i < dim; i++) {
      int val = G[j * dim + i] || G[i * dim + j];
      G[j * dim + i] = G[i * dim + j] = val;
    }
}

void printG(int *G, int dim) {
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      printf("%d ", G[i * dim + j]);
    }
    printf("\n");
  }
}

void col_convert_block4(int *G, int dim) {
  int i, j, k, l;

  int BLOCK = 128;

  for (j = 0; j <= dim - BLOCK; j += BLOCK) {
    for (i = j; i <= dim - BLOCK; i += BLOCK) {
      // printf("j=%d, i=%d\n", j, i);
      // printG(G, dim);
      for (k = j; k < j + BLOCK; k++) {
        for (l = i; l < i + BLOCK; l++) {
          // printf("k=%d, l=%d\n", k, l);
          int val = G[k * dim + l] || G[l * dim + k];
          G[k * dim + l] = G[l * dim + k] = val;
        }
      }
    }
  }

  for (; j < dim; j++) {
    for (i = 0; i <= dim - BLOCK; i += BLOCK) {
      for (l = i; l < i + BLOCK; l++) {
        int val = G[l * dim + j] || G[j * dim + l];
        G[l * dim + j] = G[j * dim + l] = val;
      }
    }

    for (; i < dim; i++) {
      int val = G[i * dim + j] || G[j * dim + i];
      G[i * dim + j] = G[j * dim + i] = val;
    }
  }
}

int main(int argc, char *argv[]) {

  int N = 5000;
  // printf("\nBaseline:\n"); // 13.5 CPE
  // measure_perf(col_convert, N);

  // printf("\nPermutated i and j:\n"); // ~7.00 CPE
  // measure_perf(col_convert_perm, N);

  // printf("\nPermutated i and j + inner loop unroll 2x1:\n"); // 6.94 CPE
  // measure_perf(col_convert_perm_unroll21, N);

  printf("\nHalved:\n");
  measure_perf(col_convert_halved, N); // CPE 6.26

  printf("\nBlock 4:\n");
  measure_perf(col_convert_block4, N); // CPE 6.26

  return 0;
}
