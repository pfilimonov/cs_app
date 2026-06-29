#include <error.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sigint_handler(int sig);

void sigint_handler(int sig) {
  (void)sig;
  return;
}

unsigned int snooze(unsigned int secs);

unsigned int snooze(unsigned int secs) {
  unsigned int rc = sleep(secs);

  printf("Slept for %u of %u seconds\n", secs - rc, secs);
  return rc;
}

int main(int argc, char *argv[]) {
  if (signal(SIGINT, sigint_handler) == SIG_ERR)
    error(1, 1, "signal error\n");

  if (argc == 1) {
    return 1;
  }

  snooze(strtol(argv[1], NULL, 10));

  return 0;
}
