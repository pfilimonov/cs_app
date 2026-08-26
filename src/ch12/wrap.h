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

#endif
