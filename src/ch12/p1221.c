#include "wrap.h"

/* Global variables */
int readcnt = 0, writecnt = 0;
sem_t mutex1, mutex2, r, w;

void reader(void) {
  while (1) {
    P(&r);
    P(&mutex1);
    readcnt++;
    if (readcnt == 1)
      P(&w);
    V(&mutex1);
    V(&r);

    // reading

    P(&mutex1);
    readcnt--;
    if (readcnt == 0)
      V(&w);
    V(&mutex1);
  }
}

void writer(void) {
  while (1) {
    P(&mutex2);
    writecnt++;
    if (writecnt == 1)
      P(&r);
    V(&mutex2);

    P(&w);
    // writing
    V(&w);

    P(&mutex2);
    writecnt--;
    if (writecnt == 0)
      V(&r);
    V(&mutex2);
  }
}
