#include "helpers.h"
#include "wrap.h"
#include <pthread.h>

sem_t mutex;
pthread_t read_tid = -1;
int finished = 0;

char *tfgets(char *s, int size, FILE *stream);

struct tfgets_input {
  char *s;
  int size;
  FILE *stream;
  pthread_t sleep_tid;
};

void *sleep_thread(void *vargp) {
  sleep(5);
  P(&mutex);
  if (!finished) {
    finished = 1;
    pthread_cancel(read_tid);
  }
  V(&mutex);
  return NULL;
}

void *read_thread(void *vargp) {
  struct tfgets_input *input = (struct tfgets_input *)vargp;

  char *s = fgets(input->s, input->size, input->stream);

  P(&mutex);
  if (!finished) {
    finished = 1;
    pthread_cancel(input->sleep_tid);
  }
  V(&mutex);
  return (void *)s;
}

char *tfgets(char *s, int size, FILE *stream) {
  pthread_t sleep_tid;

  if (pthread_create(&sleep_tid, NULL, sleep_thread, NULL) != 0) {
    app_error("Failed to create thread");
  }
  struct tfgets_input input = {s, size, stream, sleep_tid};

  P(&mutex);
  if (pthread_create(&read_tid, NULL, read_thread, &input) != 0) {
    app_error("Failed to create thread");
  }
  V(&mutex);

  char *res;
  pthread_join(read_tid, (void **)&res);
  pthread_join(sleep_tid, NULL);
  if ((void *)res == PTHREAD_CANCELED)
    return NULL;

  return res;
}

int main() {
  char buf[MAXLINE];

  sem_init(&mutex, 0, 1);

  if (tfgets(buf, MAXLINE, stdin) == NULL)
    printf("BOOM!\n");
  else
    printf("%s", buf);

  exit(0);
}
