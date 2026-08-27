#ifndef SBUF_H
#define SBUF_H
#include "csapp.h"

typedef struct {
  int *buf;    /* Buffer array */
  int n;       /* Maximum number of slots */
  int front;   /* buf[(front+1)%n] is first item */
  int rear;    /* buf[rear%n] is last item */
  sem_t mutex; /* Protects accesses to buf */
  sem_t slots; /* Counts available slots */
  sem_t items; /* Counts available items */
} sbuf_t;

int sbuf_empty(sbuf_t *sp);
int sbuf_full(sbuf_t *sp);

void sbuf_init(sbuf_t *sp, int n);

/* Clean up buffer sp */
void sbuf_deinit(sbuf_t *sp);

/* Insert item onto the rear of shared buffer sp */
void sbuf_insert(sbuf_t *sp, int item);
/* Remove and return the first item from buffer sp */
int sbuf_remove(sbuf_t *sp);

typedef struct thread_pool_t {
  pthread_t *tids;
  int n_threads;
  void *(*f)(void *);
  sbuf_t *sbuf;
  int n_busy;
  sem_t busy_mtx;
} thread_pool_t;

void init_thread_pool(thread_pool_t *tp, int n_threads, void *(*f)(void *),
                      sbuf_t *sbuf);
void expand_thread_pool(thread_pool_t *tp);
void shrink_thread_pool(thread_pool_t *tp);
void close_thread_pool(thread_pool_t *tp);

void notify_busy(thread_pool_t *tp);
void notify_free(thread_pool_t *tp);
int exhausted(thread_pool_t *tp);
int chilling(thread_pool_t *tp);

#endif
