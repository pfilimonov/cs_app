#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *thread(void *vargp);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <n>\n", argv[0]);
    exit(0);
  }

  int n = atoi(argv[1]);

  pthread_t tids[n];

  for (int i = 0; i < n; i++) {
    if (pthread_create(&tids[i], NULL, thread, (void *)(long)i) != 0) {
      printf("Failed to create thread %d\n", i);
      exit(1);
    }

    if (pthread_join(tids[i], NULL) != 0) {
      printf("Failed to join thread %d\n", i);
      exit(1);
    }
  }

  exit(0);
}

void *thread(void *vargp) {
  printf("Hello world from thread %ld!\n", (long)vargp);
  return NULL;
}
