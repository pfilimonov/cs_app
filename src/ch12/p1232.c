#include "helpers.h"
#include "wrap.h"

char *tfgets(char *s, int size, FILE *stream);

char *tfgets(char *s, int size, FILE *stream) {
  struct timeval t;
  t.tv_sec = 5;
  t.tv_usec = 0;

  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(0, &readfds);

  int cnt;
  if ((cnt = select(1, &readfds, NULL, NULL, &t)) == -1) {
    fprintf(stderr, "Failed select\n");
    exit(1);
  } else if (cnt == 0)
    return NULL;
  else
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
