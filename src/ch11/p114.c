#define _POSIX_C_SOURCE 200112L

#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>

#define MAXLINE 256

int main(int argc, char **argv) {
  struct addrinfo *p, *listp, hints;
  char buf[MAXLINE];
  int rc, flags;

  if (argc != 2) {
    fprintf(stderr, "usage %s <domain name>\n", argv[0]);
    exit(0);
  }

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
    exit(1);
  }

  for (p = listp; p; p = p->ai_next) {
    struct sockaddr_in *sain = (struct sockaddr_in *)(p->ai_addr);
    struct in_addr addr = sain->sin_addr;

    if (!inet_ntop(AF_INET, &addr, buf, MAXLINE)) {
      printf("inet_ntop failed: %s\n", strerror(errno));
      return 1;
    }

    printf("%s\n", buf);
  }

  /*
  flags = NI_NUMERICHOST;
  for (p = listp; p; p = p->ai_next) {
    if ((rc = getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0,
                          flags)) != 0) {
      fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(rc));
      exit(1);
    }

    printf("%s\n", buf);
  }
  */

  freeaddrinfo(listp);

  exit(0);
}
