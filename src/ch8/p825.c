
#include "csapp.h"
#include <signal.h>

void unix_error(char *msg) {
  printf("Unix error: %s\n", msg);
  exit(-1);
}

sigjmp_buf buf;

void handler(int sig) { siglongjmp(buf, 1); }

char *tfgets(char *s, int n, FILE *stream) {

  alarm(5);
  if (!sigsetjmp(buf, 1)) {
    if (signal(SIGALRM, handler) == SIG_ERR) {
      unix_error("signal error");
    }
    return fgets(s, n, stream);
  } else {
    return NULL;
  }
}

int main(int argc, char *argv[]) {
  char s[100];
  printf("Got: %s\n", tfgets(s, 10, stdin));
  return 0;
}
