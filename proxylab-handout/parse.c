#include "parse.h"
#include "csapp.h"

static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

int handle_request_line(char *req, struct req_line_parsed *parsed) {
  char uri[MAXLINE];
  sscanf(req, "%s %s %s", parsed->method, uri, parsed->version);

  // method
  if (strcasecmp(parsed->method, "GET")) {
    fprintf(stderr, "INVALID METHOD %s\n", parsed->method);
    return -1;
  }

  // host and uri
  if (strlen(uri) < 7) { // http://
    fprintf(stderr, "INVALID URI %s - too short\n", uri);
    return -1;
  }

  if (strncmp(uri, "http://", 7) != 0) {
    fprintf(stderr, "INVALID URI %s - doesn't start with http://\n", uri);
    return -1;
  }

  char *stripped_uri = uri + 7;
  char *slash = strchr(stripped_uri, '/');
  if (slash != NULL) {
    *slash = '\0';
  }
  char host[MAXLINE];
  strcpy(host, stripped_uri);

  if (slash == NULL) {
    strcpy(parsed->uri, "/");
  } else {
    *slash = '/';
    strcpy(parsed->uri, slash);
  }

  char *colon = strchr(host, ':');
  if (colon == NULL) {
    strcpy(parsed->port, "80");
    strcpy(parsed->host, host);
  } else {
    strcpy(parsed->port, colon + 1);
    *colon = '\0';
    strcpy(parsed->host, host);
  }

  // version
  if (strcasecmp(parsed->version, "HTTP/1.1") == 0)
    strcpy(parsed->version, "HTTP/1.0");
  else if (strcasecmp(parsed->version, "HTTP/1.0") != 0) {
    fprintf(stderr, "INVALID VERSION %s\n", parsed->version);
    return -1;
  }

  return 0;
}

void handle_request_headers(rio_t *rp_in, struct req_line_parsed *req_line,
                            int out_fd) {
  int add_host = 1;
  int add_conn = 1;
  int add_proxy_conn = 1;
  int add_user_agent = 1;

  char buf[MAXLINE];

  while (1) {
    Rio_readlineb(rp_in, buf, MAXLINE);

    if (strcmp(buf, "\r\n"))
      break;

    if (strncmp(buf, "Host: ", 6) == 0)
      add_host = 0;

    if (strncmp(buf, "Connection: ", 12) == 0)
      add_conn = 0;

    if (strncmp(buf, "Proxy-Connection: ", 18) == 0)
      add_proxy_conn = 0;

    if (strncmp(buf, "User-Agent: ", 12) == 0)
      add_user_agent = 0;

    printf("Redirect header to fd=%d: %s", out_fd, buf);
    Rio_writen(out_fd, buf, strlen(buf));
  }

  if (add_host) {
    sprintf(buf, "Host: %s:%s\r\n", req_line->host, req_line->port);
    printf("Write header Host to fd=%d: %s", out_fd, buf);
    Rio_writen(out_fd, buf, strlen(buf));
  }

  if (add_conn) {
    sprintf(buf, "Connection: close\r\n");
    printf("Write header Connection to fd=%d: %s", out_fd, buf);
    Rio_writen(out_fd, buf, strlen(buf));
  }

  if (add_proxy_conn) {
    sprintf(buf, "Proxy-Connection: close\r\n");
    printf("Write header Proxy-Connection to fd=%d: %s", out_fd, buf);
    Rio_writen(out_fd, buf, strlen(buf));
  }

  if (add_user_agent) {
    sprintf(buf, "%s", user_agent_hdr);
    printf("Write header User-Agent to fd=%d: %s", out_fd, buf);
    Rio_writen(out_fd, buf, strlen(buf));
  }

  printf("Terminate headers to fd=%d\n", out_fd);
  Rio_writen(out_fd, "\r\n", 2);
}

void handle_response(rio_t *rio_in, char **data, size_t *data_size) {
  char buf[MAXLINE];
  int cl = 0;

  *data = NULL;
  *data_size = 0;

  while (1) {
    Rio_readlineb(rio_in, buf, MAXLINE);
    // printf("Send response string from server: %s", buf);
    // Rio_writen(out_fd, buf, strlen(buf));

    size_t size = strlen(buf);

    if (*data == NULL) {
      *data = malloc(sizeof(char) * size);
      memcpy(*data, buf, size);
      *data_size += size;
    } else {
      *data = realloc(*data, *data_size + size);
      memcpy(*data + *data_size, buf, size);
      *data_size += size;
    }

    if (strncasecmp(buf, "Content-Length: ", 16) == 0)
      cl = atoi(buf + 16);

    if (strcmp(buf, "\r\n") == 0)
      break;
  }

  if (cl > 0) {
    char body[cl];
    Rio_readnb(rio_in, body, cl);
    // Rio_writen(out_fd, body, cl);

    if (*data == NULL) {
      *data = malloc(sizeof(char) * cl);
      memcpy(*data, body, cl);
      *data_size += cl;
    } else {
      *data = realloc(*data, *data_size + cl);
      memcpy(*data + *data_size, body, cl);
      *data_size += cl;
    }
  }
}
