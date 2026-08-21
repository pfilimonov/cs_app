#ifndef WRAP
#define WRAP

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// ============= HEADERS ==============
int Pthread_create(pthread_t *thread, pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);
int Pthread_join(pthread_t thread, void **retval);
unsigned int Sleep(unsigned int seconds);
void P(sem_t *s);
void V(sem_t *s);

// ============= IMPLEMENTATION ==============

int Pthread_create(pthread_t *thread, pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg) {
  int res = pthread_create(thread, attr, start_routine, arg);
  if (res != 0) {
    fprintf(stderr, "Failed to create thread\n");
    exit(1);
  }
  return res;
}

int Pthread_join(pthread_t thread, void **retval) {
  int res = pthread_join(thread, retval);
  if (res != 0) {
    fprintf(stderr, "Failed to join thread\n");
    exit(1);
  }
  return res;
}

unsigned int Sleep(unsigned int seconds) { return sleep(seconds); }

void P(sem_t *s) {
  if (sem_wait(s) != 0) {
    fprintf(stderr, "Failed to wait a semaphore\n");
    exit(1);
  }
}

void V(sem_t *s) {
  if (sem_post(s) != 0) {
    fprintf(stderr, "Failed to post a semaphore\n");
    exit(1);
  }
}
#endif
