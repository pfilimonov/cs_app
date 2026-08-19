#include "wrap.h"
#include <pthread.h>
#include <stdlib.h>

void *thread(void *vargp);

int main(void) {
  pthread_t tid;

  Pthread_create(&tid, NULL, thread, NULL);
  Pthread_join(tid, NULL);
  exit(0);
}

/* Thread routine */
void *thread(void *_) {
  Sleep(1);
  printf("Hello, world!\n");
  return NULL;
}
