#define _POSIX_C_SOURCE 200112L

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <netdb.h>

#include "helpers.h"
#include "rio.h"

extern char **environ;
typedef struct sockaddr SA;

void sigchld_handler(int sig);
void doit(int fd);
int read_requesthdrs(rio_t *rp, char *method);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *method, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *method, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longsmsg);

int main(int argc, char **argv) {
  if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
    fprintf(stderr, "signal error\n");
    exit(1);
  }

  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);

  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = accept(listenfd, (SA *)(&clientaddr), &clientlen);
    if (connfd < 0) {
      if (errno == EINTR)
        continue;

      fprintf(stderr, "failed to accept connection %s\n", strerror(errno));
      exit(1);
    }

    int rc;
    if ((rc = getnameinfo((SA *)(&clientaddr), clientlen, hostname, MAXLINE,
                          port, MAXLINE, 0)) != 0) {
      fprintf(stderr, "failed to getnameinfo %s\n", gai_strerror(rc));
      exit(1);
    }
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    doit(connfd);
    if (close(connfd) != 0) {
      fprintf(stderr, "failed to close connfd: %s\n", strerror(errno));
      exit(1);
    }
  }
}

void doit(int fd) {
  int is_static;
  struct stat sbuf;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], versin[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  /*read req line and headers*/
  rio_readinitb(&rio, fd);
  rio_readlineb(&rio, buf, MAXLINE);
  printf("=== Request line ===\n");
  printf("%s", buf);
  sscanf(buf, "%s %s %s", method, uri, versin);

  if (strcasecmp(method, "GET") && strcasecmp(method, "HEAD") &&
      strcasecmp(method, "POST")) {
    clienterror(fd, method, "501", "Not implemented",
                "Tiny does not implement this method");
    return;
  }

  int param_len = read_requesthdrs(&rio, method);

  rio_readnb(&rio, buf, param_len);

  /* parse uri from get req */
  is_static = parse_uri(uri, filename, cgiargs);
  if (stat(filename, &sbuf) < 0) {
    clienterror(fd, filename, "404", "Not found",
                "Tiny couldn't find this file");
    return;
  }

  if (is_static) {
    // serve static content
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't read the file");
      return;
    }
    serve_static(fd, method, filename, sbuf.st_size);
  } else {
    // serve dynamic content
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't run the CGI program");
      return;
    }

    if (strcasecmp(method, "GET") == 0)
      serve_dynamic(fd, method, filename, cgiargs);
    else
      serve_dynamic(fd, method, filename, buf);
  }
}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg) {
  char buf[MAXLINE], body[MAXBUF];

  /* Build the HTTP response body */
  sprintf(body, "<html><title>Tiny Error</title>");
  sprintf(body,
          "%s<body bgcolor="
          "ffffff"
          ">\r\n",
          body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

  /* Print the HTTP response */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  rio_writen(fd, buf, strlen(buf));
  rio_writen(fd, body, strlen(body));
}

int read_requesthdrs(rio_t *rp, char *method) {
  char buf[MAXLINE];

  printf("=== Request headers ===\n");

  int len = 0;

  rio_readlineb(rp, buf, MAXLINE);
  while (strcmp(buf, "\r\n")) {
    if (strcasecmp(method, "POST") == 0 &&
        strncasecmp(buf, "Content-Length:", 15) == 0)
      sscanf(buf, "Content-Length: %d", &len);
    rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }
  return len;

  printf("========\n");
}

int parse_uri(char *uri, char *filename, char *cgiargs) {
  char *ptr;

  if (!strstr(uri, "cgi-bin")) { /* Static content */
    strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    if (uri[strlen(uri) - 1] == '/')
      strcat(filename, "home.html");
    return 1;
  } else { /* Dynamic content */
    ptr = index(uri, '?');
    if (ptr) {
      strcpy(cgiargs, ptr + 1);
      *ptr = '\0';
    } else
      strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    return 0;
  }
}

void serve_static(int fd, char *method, char *filename, int filesize) {
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];

  /* Send response headers to client */
  get_filetype(filename, filetype);
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
  sprintf(buf, "%sConnection: close\r\n", buf);
  sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
  sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
  rio_writen(fd, buf, strlen(buf));
  printf("Response headers:\n");
  printf("%s", buf);

  if (!strcasecmp(method, "HEAD")) {
    return;
  }

  /* Send response body to client */
  srcfd = open(filename, O_RDONLY, 0);
  if (srcfd == -1) {
    fprintf(stderr, "failed to open file %s: %s\n", filename, strerror(errno));
    exit(1);
  }

  srcp = malloc(sizeof(char) * filesize);
  if (srcp == NULL) {
    fprintf(stderr, "Failed to allocate memory\n");
    exit(1);
  }

  ssize_t rc = rio_readn(srcfd, srcp, filesize);
  if (rc <= 0) {
    fprintf(stderr, "failed to read file %s: %s\n", filename, strerror(errno));
    exit(1);
  }

  if (close(srcfd) != 0) {
    fprintf(stderr, "failed to close srcfd: %s\n", strerror(errno));
    exit(1);
  }
  rio_writen(fd, srcp, filesize);
  free(srcp);
}

/*
 * get_filetype - Derive file type from filename
 */
void get_filetype(char *filename, char *filetype) {
  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".png"))
    strcpy(filetype, "image/png");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg");
  else if (strstr(filename, ".mpeg"))
    strcpy(filetype, "video/mpg");
  else
    strcpy(filetype, "text/plain");
}

void serve_dynamic(int fd, char *method, char *filename, char *cgiargs) {
  char buf[MAXLINE], *emptylist[] = {NULL};

  /* Return first part of HTTP response */
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  rio_writen(fd, buf, strlen(buf));

  int pid = fork();
  if (pid == -1) {
    fprintf(stderr, "failed to fork %s\n", strerror(errno));
    exit(1);
  }
  if (pid == 0) { /* Child */
                  /* Real server would set all CGI vars here */
    setenv("QUERY_STRING", cgiargs, 1);
    if (strcasecmp(method, "HEAD") == 0)
      setenv("HEAD", "1", 1);
    else
      unsetenv("HEAD");
    if (dup2(fd, STDOUT_FILENO) == -1) {
      fprintf(stderr, "Failed to duplicate fd %s\n", strerror(errno));
      exit(1);
    } /* Redirect stdout to client */
    if (execve(filename, emptylist, environ) == -1) {
      fprintf(stderr, "failed to execute cgi prog: %s\n", strerror(errno));
      exit(1);
    }
  }
}

void sigchld_handler(int _) {
  int old_errno = errno;
  int status;
  pid_t pid;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
  }

  errno = old_errno;
}
