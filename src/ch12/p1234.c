#include "helpers.h"
#include "wrap.h"
#include <assert.h>
#include <bits/time.h>

typedef struct {
  int **A;
  int **B;
  int **res;
  int N1;
  int N2;
  int N;
  int M;
} matmul_input_t;

void *matmul_thread(void *inpp) {
  matmul_input_t *inp = (matmul_input_t *)inpp;

  for (int i = inp->N1; i < inp->N2; i++) {
    for (int j = 0; j < inp->N; j++) {
      int res_ij = 0;
      for (int k = 0; k < inp->M; k++) {
        // printf("i=%d, j=%d, k=%d\n", i, j, k);
        // printf("A[%d][%d]: %d\n", i, k, inp->A[i][k]);
        // printf("B[%d][%d]: %d\n", k, j, inp->B[k][j]);
        res_ij += inp->A[i][k] * inp->B[k][j];
      }
      inp->res[i][j] = res_ij;
    }
  }
  return NULL;
}

int **create_A(int N, int M) {
  int **A = malloc(N * sizeof(int *));
  for (int i = 0; i < N; i++) {
    A[i] = malloc(M * sizeof(int));
    for (int j = 0; j < M; j++) {
      A[i][j] = rand();
    }
  }

  return A;
}

int **create_B(int N, int M) { return create_A(M, N); }

void check(int **A, int **B, int **res, int N, int M) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      int res_ij = 0;
      for (int k = 0; k < M; k++) {
        res_ij += A[i][k] * B[k][j];
      }
      assert(res_ij == res[i][j]);
    }
  }
}

int main(int argc, char *argv[]) {
  int threads = 1;
  int N = 10;
  int M = 20;
  if (argc > 1) {
    threads = atoi(argv[1]);
  }
  if (argc > 2) {
    N = atoi(argv[2]);
  }
  if (argc > 3) {
    M = atoi(argv[3]);
  }

  int **A = create_A(N, M);
  int **B = create_B(N, M);
  int **res = create_A(N, N);

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  matmul_input_t *inputs = malloc(threads * sizeof(matmul_input_t));
  pthread_t *tids = malloc(threads * sizeof(pthread_t));

  int step = N / threads;
  for (int th = 0; th < threads; th++) {
    matmul_input_t inp;
    inp.A = A;
    inp.B = B;
    inp.res = res;
    inp.N = N;
    inp.M = M;
    inp.N1 = th * step;
    inp.N2 = th == threads - 1 ? N : (th + 1) * step;
    inputs[th] = inp;
    pthread_create(&tids[th], NULL, matmul_thread, (void *)&inputs[th]);
  }

  for (int th = 0; th < threads; th++) {
    pthread_join(tids[th], NULL);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  double elapsed =
      (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;
  printf("Time: %f usec\n", elapsed);

  check(A, B, res, N, M);

  for (int i = 0; i < N; i++) {
    free(A[i]);
    free(res[i]);
  }
  free(A);
  free(res);
  for (int j = 0; j < M; j++) {
    free(B[j]);
  }
  free(B);
  free(inputs);
  free(tids);
}
