
#include <limits.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

void *calloc(size_t nmemb, size_t size) {
  size_t nbytes = nmemb * size;
  if (!nbytes || nbytes / nmemb != size) {
    return NULL;
  }
  printf("nbytes: %u\n", nbytes);
  void *mem = malloc(nbytes);
  if (mem == NULL) {
    return NULL;
  }
  return memset(mem, 0, nbytes);
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    unsigned nmemb = strtol(argv[1], NULL, 0);
    unsigned size = strtol(argv[2], NULL, 0);
    void *buf = calloc(nmemb, size);
    free(buf);
  } else {
    printf("Not enough args");
  }

  return 0;
}
