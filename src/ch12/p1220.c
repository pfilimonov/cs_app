#include "wrap.h"

#define N 10

/* Global variables */
sem_t mutex, counter; /* Both initially = 1 */

int main(void) {
  sem_init(&mutex, 0, 1);
  sem_init(&counter, 0, N);
  return 0;
}

void reader(void) {
  while (1) {
    P(&counter);
    /* Critical section */
    /* Reading happens */
    V(&counter);
  }
}
void writer(void) {
  while (1) {
    P(&mutex);
    for (int i = 0; i < N; i++)
      P(&counter);
    /* Critical section */
    /* Writing happens */
    for (int i = 0; i < N; i++)
      V(&counter);
    V(&mutex);
  }
}
