#define _POSIX_C_SOURCE 200809L

#include "helpers.h"
#include "wrap.h"
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

sigjmp_buf buf;

void handler(int sig) { siglongjmp(buf, 1); }

char *tfgets(char *s, int size, FILE *stream);

char *tfgets(char *s, int size, FILE *stream) {

  pid_t pid;
  pid_t parent = getpid();

  if (!sigsetjmp(buf, 1)) {
    signal(SIGABRT, handler);
  } else {
    return NULL;
  }

  if ((pid = fork()) == 0) {
    sleep(5);
    kill(parent, SIGABRT);
    exit(0);
  }

  return fgets(s, size, stream);
}

int main() {
  char buf[MAXLINE];

  if (tfgets(buf, MAXLINE, stdin) == NULL)
    printf("BOOM!\n");
  else
    printf("%s", buf);

  exit(0);
}
