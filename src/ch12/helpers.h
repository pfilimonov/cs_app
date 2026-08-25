#ifndef HELPERS
#define HELPERS

#define _POSIX_C_SOURCE 200112L

#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <netdb.h>

extern char **environ;
typedef struct sockaddr SA;

uint64_t rdtsc(void);

#define LISTENQ 1024
#define MAXLINE 1024
#define MAXBUF 1024

int open_listenfd(char *port);
int Open_listenfd(char *port);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout);
int Close(int fd);
char *Fgets(char *s, int n, FILE *stream);

int app_error(char *msg);

#endif
