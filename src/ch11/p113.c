#include "arpa/inet.h"
#include "stdio.h"
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <ip>\n", argv[0]);
    return 1;
  }

  struct in_addr inet_addr;

  int res = inet_pton(AF_INET, argv[1], &(inet_addr.s_addr));
  if (res == 0) {
    printf("Invalid dotted decimal address: %s\n", argv[1]);
    return 1;
  }

  if (res == -1) {
    printf("Error: %s\n", strerror(errno));
    return 1;
  }

  printf("0x%x\n", ntohl(inet_addr.s_addr));
  return 0;
}
