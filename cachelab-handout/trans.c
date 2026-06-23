/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include "cachelab.h"
#include <assert.h>
#include <stdio.h>

#define K 16

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */

void transpose32(int M, int N, int A[N][M], int B[M][N]) {
  int BLOCK_SIZE = 8;

  int i, j, k, l, tmp, diag;
  int diag_touched = 0;

  for (i = 0; i <= N - BLOCK_SIZE; i += BLOCK_SIZE) {
    for (j = 0; j <= M - BLOCK_SIZE; j += BLOCK_SIZE) {
      for (k = i; k < i + BLOCK_SIZE; k++) {
        for (l = j; l < j + BLOCK_SIZE; l++) {
          if (k == l) {
            diag_touched = 1;
            diag = A[k][l];
            continue;
          }
          tmp = A[k][l];
          B[l][k] = tmp;
        }
        if (diag_touched)
          B[k][k] = diag;
        diag_touched = 0;
      }
    }
  }
}

void transpose64(int M, int N, int A[N][M], int B[M][N]) {
  int BLOCK_SIZE = 4;

  int i, j, k, l, tmp, diag;
  int diag_touched = 0;

  int even_block[BLOCK_SIZE][BLOCK_SIZE];

  for (i = 0; i <= N - BLOCK_SIZE; i += BLOCK_SIZE) {
    for (j = 0; j <= M - BLOCK_SIZE; j += 2 * BLOCK_SIZE) {
      for (k = i; k < i + BLOCK_SIZE; k++) {
        for (l = j; l < j + BLOCK_SIZE; l++) {
          if (k == l) {
            diag_touched = 1;
            diag = A[k][l];
            continue;
          }
          tmp = A[k][l];
          B[l][k] = tmp;
        }

        for (int m = 0; m < BLOCK_SIZE; m++) {
          even_block[k % 4][m] = A[k][j + BLOCK_SIZE + m];
        }

        if (diag_touched)
          B[k][k] = diag;
        diag_touched = 0;
      }

      for (int n = 0; n < BLOCK_SIZE; n++) {
        for (int m = 0; m < BLOCK_SIZE; m++) {
          B[j + BLOCK_SIZE + m][i + n] = even_block[n][m];
        }
      }
    }
  }
}
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
  /*
  if (M == 32 && N == 32)
    transpose32(M, N, A, B);
  else if (M == 64 && N == 64)
    transpose64(M, N, A, B);
  */

  int tmp[K][K], tmp_transpose[K][K];
  int i, j, u, v;

  for (i = 0; i < N / K; i++) {
    for (j = 0; j < M / K; j++) {
      for (u = 0; u < K; u++) {
        for (v = 0; v < K; v++) {
          tmp[u][v] = A[i * K + u][j * K + v];
        }
      }

      for (u = 0; u < K; u++) {
        for (v = 0; v < K; v++) {
          tmp_transpose[v][u] = tmp[u][v];
        }
      }

      for (u = 0; u < K; u++) {
        for (v = 0; v < K; v++) {
          B[j * K + u][i * K + v] = tmp_transpose[u][v];
        }
      }
    }
  }

  if (i * K < N) {
    for (j = 0; j < M / K; j++) {
      for (u = 0; u < N % K; u++) {
        for (v = 0; v < K; v++) {
          tmp[u][v] = A[i * K + u][j * K + v];
        }
      }

      for (u = 0; u < N % K; u++) {
        for (v = 0; v < K; v++) {
          tmp_transpose[v][u] = tmp[u][v];
        }
      }

      for (u = 0; u < N % K; u++) {
        for (v = 0; v < K; v++) {
          B[j * K + v][i * K + u] = tmp_transpose[v][u];
        }
      }
    }
  }

  if (j * K < M) {
    for (i = 0; i < N / K; i++) {
      for (u = 0; u < K; u++) {
        for (v = 0; v < M % K; v++) {
          tmp[u][v] = A[i * K + u][j * K + v];
        }
      }

      for (u = 0; u < K; u++) {
        for (v = 0; v < M % K; v++) {
          tmp_transpose[v][u] = tmp[u][v];
        }
      }

      for (u = 0; u < K; u++) {
        for (v = 0; v < M % K; v++) {
          B[j * K + v][i * K + u] = tmp_transpose[v][u];
        }
      }
    }
  }

  if (j * K < M && i * K < N) {
    for (u = 0; u < N % K; u++) {
      for (v = 0; v < M % K; v++) {
        tmp[u][v] = A[i * K + u][j * K + v];
      }
    }

    for (u = 0; u < N % K; u++) {
      for (v = 0; v < M % K; v++) {
        tmp_transpose[v][u] = tmp[u][v];
      }
    }

    for (u = 0; u < N % K; u++) {
      for (v = 0; v < M % K; v++) {
        B[j * K + v][i * K + u] = tmp_transpose[v][u];
      }
    }
  }
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, tmp;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; j++) {
      tmp = A[i][j];
      B[j][i] = tmp;
    }
  }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
  /* Register your solution function */
  // registerTransFunction(transpose_submit, transpose_submit_desc);

  /* Register any additional transpose functions */
  registerTransFunction(trans, trans_desc);
  registerTransFunction(transpose_submit, transpose_submit_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
  int i, j;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; ++j) {
      if (A[i][j] != B[j][i]) {
        return 0;
      }
    }
  }
  return 1;
}
