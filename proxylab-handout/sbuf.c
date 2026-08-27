#include "sbuf.h"

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
