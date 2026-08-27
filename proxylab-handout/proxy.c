#include "csapp.h"
#include <stdio.h>

#include "parse.h"

void doit(int fd);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

int main(int argc, char *argv[]) {
  signal(SIGPIPE, SIG_IGN);

  int listenfd, client_connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(clientaddr);
    client_connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    if (client_connfd < 0)
      continue;

    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);

    doit(client_connfd);
    Close(client_connfd);
  }
}

void doit(int fd) {
  char buf[MAXLINE];
  int dest_fd;
  rio_t rio_in, rio_out;

  /* Read request line and headers */
  Rio_readinitb(&rio_in, fd);
  if (!Rio_readlineb(&rio_in, buf, MAXLINE)) {
    printf("Client hasn't sent request line\n");
    return;
  }

  struct req_line_parsed req_line;
  if (handle_request_line(buf, &req_line) != 0) {
    clienterror(fd, req_line.method, "400", "Bad Request",
                "Invalid request line");
  }

  printf("Request line received: %s %s %s\nHost: %s:%s\n", req_line.method,
         req_line.uri, req_line.version, req_line.host, req_line.port);

  dest_fd = Open_clientfd(req_line.host, req_line.port);

  sprintf(buf, "%s %s %s\r\n", req_line.method, req_line.uri, req_line.version);
  Rio_writen(dest_fd, buf, strlen(buf));
  handle_request_headers(&rio_in, &req_line, dest_fd);

  Rio_readinitb(&rio_out, dest_fd);
  handle_response(&rio_out, fd);

  Close(dest_fd);
}

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg) {
  char buf[MAXLINE];

  /* Print the HTTP response headers */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n\r\n");
  Rio_writen(fd, buf, strlen(buf));

  /* Print the HTTP response body */
  sprintf(buf, "<html><title>Tiny Error</title>");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<body bgcolor="
               "ffffff"
               ">\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
  Rio_writen(fd, buf, strlen(buf));
}
