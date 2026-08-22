#include "helpers.h"
#include "rio.h"
#include "wrap.h"
#include <signal.h>
#include <sys/select.h>
#include <unistd.h>

int echo(int connfd, rio_t *rio);
void command(void);

int running = 1;
sem_t mutex;

void sigint_handler(int _);

void sigint_handler(int _) {
  P(&mutex);
  running = 0;
  V(&mutex);
}

int main(int argc, char **argv) {
  if (signal(SIGINT, sigint_handler) == SIG_ERR) {
    app_error("Failed to install sigint_handler\n");
  }

  int listenfd, connfd;
  int maxfd;
  rio_t *rios;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  fd_set read_set, ready_set;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }
  listenfd = Open_listenfd(argv[1]);
  maxfd = listenfd;
  rios = malloc(maxfd * sizeof(rio_t));

  FD_ZERO(&read_set);              /* Clear read set */
  FD_SET(STDIN_FILENO, &read_set); /* Add stdin to read set */
  FD_SET(listenfd, &read_set);     /* Add listenfd to read set */

  while (1) {
    P(&mutex);
    if (!running)
      break;
    V(&mutex);

    ready_set = read_set;
    Select(maxfd + 1, &ready_set, NULL, NULL, NULL);
    if (FD_ISSET(STDIN_FILENO, &ready_set))
      command(); /* Read command line from stdin */
    if (FD_ISSET(listenfd, &ready_set)) {
      clientlen = sizeof(struct sockaddr_storage);
      connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
      printf("Connected new client %d\n", connfd);
      if (connfd > maxfd) {
        maxfd = connfd;
        void *tmp = realloc(rios, maxfd * sizeof(rio_t));
        if (!tmp) {
          fprintf(stderr, "Failed to allocate memory\n");
          exit(1);
        }
        rios = (rio_t *)tmp;
      }
      rio_readinitb(&rios[connfd - 1], connfd);
      FD_SET(connfd, &read_set);
    }

    for (int fd = listenfd + 1; fd <= maxfd; fd++) {
      if (!FD_ISSET(fd, &ready_set))
        continue;

      if (echo(fd, &rios[fd - 1]) == 0)
        continue;

      // eof
      Close(fd);
      FD_CLR(fd, &read_set);
    }
  }

  free(rios);
}

void command(void) {
  char buf[MAXLINE];
  if (!Fgets(buf, MAXLINE, stdin))
    exit(0);         /* EOF */
  printf("%s", buf); /* Process the input command */
}

int echo(int connfd, rio_t *rio) {
  size_t n;
  char buf[MAXLINE];
  n = rio_readlineb(rio, buf, MAXLINE);
  if (n == 0)
    return -1; // EOF
  else {
    printf("server received %d bytes\n", (int)n);
    Rio_writen(connfd, buf, n);
    return 0;
  }
}
