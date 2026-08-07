#include "arpa/inet.h"
#include "stdio.h"
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <hex inet addr>\n", argv[0]);
    return 1;
  }

  uint32_t addr = strtol(argv[1], NULL, 16);
  uint32_t addr_n = htonl(addr);

  struct in_addr addr_struct;
  addr_struct.s_addr = addr_n;

  char presentation[INET_ADDRSTRLEN];
  if (!inet_ntop(AF_INET, &addr_struct, presentation, sizeof(presentation))) {
    printf("inet_ntop failed: %s\n", strerror(errno));
    return 1;
  }

  printf("%s\n", presentation);
  return 0;
}
