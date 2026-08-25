#include "helpers.h"

uint64_t rdtsc(void) {
  uint32_t lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
}

// Returns: descriptor if OK, −1 on error
int open_listenfd(char *port) {
  struct addrinfo hints, *listp, *p;
  int listenfd, optval = 1;
  int rc;

  /* Get a list of potential server addresses */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;             /* Accept connections */
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
  hints.ai_flags |= AI_NUMERICSERV;            /* ... using port number */

  if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
    exit(1);
  }

  /* Walk the list for one that we can bind to */
  for (p = listp; p; p = p->ai_next) {
    /* Create a socket descriptor */
    if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
      continue; /* Socket failed, try the next */

    /* Eliminates "Address already in use" error from bind */
    if ((rc = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                         (const void *)&optval, sizeof(int))) == -1) {
      fprintf(stderr, "failed to setsockopt: %s\n", strerror(errno));
      exit(1);
    }

    /* Bind the descriptor to the address */
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
      break; /* Success */

    /* Bind failed, try the next */
    if (close(listenfd) != 0) {
      fprintf(stderr, "failed to close listenfd: %s\n", strerror(errno));
      exit(1);
    }
  }
  /* Clean up */
  freeaddrinfo(listp);
  if (!p) /* No address worked */
    return -1;

  /* Make it a listening socket ready to accept connection requests */
  if (listen(listenfd, LISTENQ) < 0) {
    if (close(listenfd) != 0) {
      fprintf(stderr, "failed to close listenfd: %s\n", strerror(errno));
      exit(1);
    }
    return -1;
  }
  return listenfd;
}

int Open_listenfd(char *port) {
  int fd = open_listenfd(port);
  if (fd < 0) {
    fprintf(stderr, "open_listenfd");
    exit(1);
  }

  return fd;
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout) {
  int res = select(nfds, readfds, writefds, exceptfds, timeout);
  if (res == -1) {
    fprintf(stderr, "Failed to select: %s\n", strerror(errno));
    exit(1);
  }

  return res;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  int res = accept(sockfd, addr, addrlen);
  if (res == -1) {
    fprintf(stderr, "Failed to accept: %s\n", strerror(errno));
    exit(1);
  }
  return res;
}

int Close(int fd) {
  int res = close(fd);
  if (res == -1) {
    fprintf(stderr, "Failed to close: %s\n", strerror(errno));
    exit(1);
  }
  return res;
}

char *Fgets(char *s, int n, FILE *stream) {
  char *res = fgets(s, n, stream);
  if (res == NULL) {
    if (!feof(stream) && ferror(stream))
      perror("fgets");
  }
  return res;
}

int app_error(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}
