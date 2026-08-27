#include "cache.h"
#include "csapp.h"
#include <string.h>
#include <strings.h>

struct req_line_parsed {
  char host[MAXLINE];
  char port[MAXLINE];
  char method[MAXLINE];
  char uri[MAXLINE];
  char version[MAXLINE];
};

// handles request line, converts it into the server request. Returns -1 on
// failure, 0 on success
int handle_request_line(char *req, struct req_line_parsed *parsed);

void handle_request_headers(rio_t *rp_in, struct req_line_parsed *req_line,
                            int out_fd);
void handle_response(rio_t *rio_in, char **data, size_t *data_size);
