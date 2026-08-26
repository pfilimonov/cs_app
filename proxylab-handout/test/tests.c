#include "../parse.h"
#include <assert.h>

// ================ parse.c ==================

void test_handle_request_line_1() {
  char *REQ_LINE = "GET http://www.cmu.edu/hub/index.html HTTP/1.1";

  struct req_line_parsed parsed;
  assert(handle_request_line(REQ_LINE, &parsed) == 0);
  assert(strcmp(parsed.host, "www.cmu.edu") == 0);
  assert(strcmp(parsed.method, "GET") == 0);
  assert(strcmp(parsed.uri, "/hub/index.html") == 0);
  assert(strcmp(parsed.version, "HTTP/1.0") == 0);
  assert(strcmp(parsed.port, "80") == 0);
}

void test_handle_request_line_2() {
  char *REQ_LINE = "GET http://www.cmu.edu HTTP/1.0";

  struct req_line_parsed parsed;
  assert(handle_request_line(REQ_LINE, &parsed) == 0);
  assert(strcmp(parsed.host, "www.cmu.edu") == 0);
  assert(strcmp(parsed.method, "GET") == 0);
  assert(strcmp(parsed.uri, "/") == 0);
  assert(strcmp(parsed.version, "HTTP/1.0") == 0);
  assert(strcmp(parsed.port, "80") == 0);
}

void test_handle_request_line_3() {
  char *REQ_LINE = "GETT http://www.cmu.edu HTTP/1.0";

  struct req_line_parsed parsed;
  assert(handle_request_line(REQ_LINE, &parsed) == -1);
}

void test_handle_request_line_4() {
  char *REQ_LINE = "GET htp://www.cmu.edu HTTP/1.0";

  struct req_line_parsed parsed;
  assert(handle_request_line(REQ_LINE, &parsed) == -1);
}

void test_handle_request_line_5() {
  char *REQ_LINE = "GET http://www.cmu.edu HTTP/1.2";

  struct req_line_parsed parsed;
  assert(handle_request_line(REQ_LINE, &parsed) == -1);
}

void test_handle_request_line_6() {
  char *REQ_LINE = "GET http:/ HTTP/1.0";

  struct req_line_parsed parsed;
  assert(handle_request_line(REQ_LINE, &parsed) == -1);
}

void test_handle_request_line_7() {
  char *REQ_LINE = "GET http://www.cmu.edu:8080 HTTP/1.0";

  struct req_line_parsed parsed;
  assert(handle_request_line(REQ_LINE, &parsed) == 0);
  assert(strcmp(parsed.host, "www.cmu.edu") == 0);
  assert(strcmp(parsed.method, "GET") == 0);
  assert(strcmp(parsed.uri, "/") == 0);
  assert(strcmp(parsed.version, "HTTP/1.0") == 0);
  assert(strcmp(parsed.port, "8080") == 0);
}

void test_handle_request_line_8() {
  char *REQ_LINE = "GET http://www.cmu.edu:8080/hub/gavrik.html HTTP/1.0";

  struct req_line_parsed parsed;
  assert(handle_request_line(REQ_LINE, &parsed) == 0);
  assert(strcmp(parsed.host, "www.cmu.edu") == 0);
  assert(strcmp(parsed.method, "GET") == 0);
  assert(strcmp(parsed.uri, "/hub/gavrik.html") == 0);
  assert(strcmp(parsed.version, "HTTP/1.0") == 0);
  assert(strcmp(parsed.port, "8080") == 0);
}

int main(void) {
  // parse.c
  test_handle_request_line_1();
  test_handle_request_line_2();
  test_handle_request_line_3();
  test_handle_request_line_4();
  test_handle_request_line_5();
  test_handle_request_line_6();
  test_handle_request_line_7();
  test_handle_request_line_8();

  return 0;
}
