#include "tpool.h"

#define MAX_THREADS 16

void init_thread_pool(thread_pool_t *tp, int n_threads, void *(*f)(void *),
                      sbuf_t *sbuf) {
  tp->tids = malloc(n_threads * sizeof(pthread_t));
  tp->n_threads = n_threads;
  tp->f = f;
  tp->sbuf = sbuf;
  tp->n_busy = 0;
  sem_init(&tp->busy_mtx, NULL, 1);

  for (int i = 0; i < tp->n_threads; i++) {
    if (pthread_create(&tp->tids[i], NULL, tp->f, tp) != 0) {
      fprintf(stderr, "Failed to create thread %d\n", i);
      exit(1);
    }
  }
}

void close_thread_pool(thread_pool_t *tp) {
  for (int i = 0; i < tp->n_threads; i++) {
    P(&tp->sbuf->mutex); // make sure cancel occurs when no thread is working
                         // with sbuf
    if (pthread_cancel(tp->tids[i]) != 0) {
      fprintf(stderr, "Failed to cancel thread %d\n", i);
      exit(1);
    }
    V(&tp->sbuf->mutex);
    pthread_join(tp->tids[i], NULL);
  }
  free(tp->tids);
}

void expand_thread_pool(thread_pool_t *tp) {
  if (tp->n_threads == MAX_THREADS)
    return;

  printf("Expand thread pool to threads %d\n", tp->n_threads * 2);

  tp->tids = realloc(tp->tids, 2 * tp->n_threads * sizeof(pthread_t));
  tp->n_threads *= 2;

  for (int i = tp->n_threads / 2; i < tp->n_threads; i++) {
    if (pthread_create(&tp->tids[i], NULL, tp->f, tp) != 0) {
      fprintf(stderr, "Failed to create thread %d\n", i);
      exit(1);
    }
  }
}
void shrink_thread_pool(thread_pool_t *tp) {
  if (tp->n_threads == 1)
    return;

  printf("Shrink thread pool to threads %d\n", tp->n_threads / 2);

  for (int i = tp->n_threads / 2; i < tp->n_threads; i++) {
    P(&tp->sbuf->mutex); // make sure cancel occurs when no thread is working
                         // with sbuf
    if (pthread_cancel(tp->tids[i]) != 0) {
      fprintf(stderr, "Failed to cancel thread %d\n", i);
      exit(1);
    }
    V(&tp->sbuf->mutex);
    pthread_join(tp->tids[i], NULL);
  }

  tp->tids = realloc(tp->tids, tp->n_threads / 2 * sizeof(pthread_t));
  tp->n_threads = tp->n_threads / 2;
}

void notify_busy(thread_pool_t *tp) {
  P(&tp->busy_mtx);
  tp->n_busy++;
  V(&tp->busy_mtx);
}
void notify_free(thread_pool_t *tp) {
  P(&tp->busy_mtx);
  tp->n_busy--;
  V(&tp->busy_mtx);
}

int exhausted(thread_pool_t *tp) {
  int res;
  P(&tp->busy_mtx);
  res = tp->n_busy == tp->n_threads;
  V(&tp->busy_mtx);
  return res;
}
int chilling(thread_pool_t *tp) {
  int res;
  P(&tp->busy_mtx);
  res = tp->n_busy == 0;
  V(&tp->busy_mtx);
  return res;
}
