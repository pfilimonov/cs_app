#include "sbuf.h"

#define MAX_THREADS 16

void sbuf_init(sbuf_t *sp, int n) {
  sp->buf = calloc(n, sizeof(int));
  sp->n = n;                  /*Buffer holds maxofnitems */
  sp->front = sp->rear = 0;   /* Empty buffer iff front == rear */
  sem_init(&sp->mutex, 0, 1); /* Binary semaphore for locking */
  sem_init(&sp->slots, 0, n); /* Initially, buf has n empty slots */
  sem_init(&sp->items, 0, 0); /* Initially, buf has zero data items */
}

int sbuf_empty(sbuf_t *sp) {
  P(&sp->mutex);
  int res = sp->front == sp->rear;
  V(&sp->mutex);
  return res;
}
int sbuf_full(sbuf_t *sp) {
  P(&sp->mutex);
  int res = (sp->rear - sp->front) >= sp->n;
  V(&sp->mutex);
  return res;
}

/* Clean up buffer sp */
void sbuf_deinit(sbuf_t *sp) { free(sp->buf); }

/* Insert item onto the rear of shared buffer sp */
void sbuf_insert(sbuf_t *sp, int item) {
  P(&sp->slots);                          /* Wait for available slot */
  P(&sp->mutex);                          /*Lock the buffer */
  sp->buf[(++sp->rear) % (sp->n)] = item; /* Insert the item */
  V(&sp->mutex);                          /* Unlock the buffer */
  V(&sp->items);                          /* Announce available item */
}

/* Remove and return the first item from buffer sp */
int sbuf_remove(sbuf_t *sp) {
  int item;
  P(&sp->items);                           /* Wait for available item */
  P(&sp->mutex);                           /*Lock the buffer */
  item = sp->buf[(++sp->front) % (sp->n)]; /* Remove the item */
  V(&sp->mutex);                           /* Unlock the buffer */
  V(&sp->slots);                           /* Announce available slot */
  return item;
}

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
