#define _DEFAULT_SOURCE

#include <errno.h>
#include <error.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 2

pid_t Fork(void);
int main(int, char *[]);

pid_t Fork(void) {
  pid_t pid;
  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error: %s\n", strerror(errno));
    exit(0);
  }
  return pid;
}

int main(int argc, char *argv[]) {
  int status, i;
  pid_t pid;

  for (i = 0; i < N; i++)
    if ((pid = Fork()) == 0)
      *((char *)0x1) = 'a';

  while ((pid = waitpid(-1, &status, 0)) > 0) {
    if (WIFSIGNALED(status)) {
      char *msg = malloc(100);
      sprintf(msg, "child %d terminated bby signal %d", pid, WTERMSIG(status));
      psignal(WTERMSIG(status), msg);
    } else
      printf("child %d terminated normally\n", pid);
  }

  if (errno != ECHILD)
    error(1, 1, "waitpid error");

  exit(0);
}
