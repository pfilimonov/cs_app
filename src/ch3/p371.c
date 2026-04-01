#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFFER_BLOCK_LEN 4

void good_echo() {
  printf("BUFFER_BLOCK_LEN = %d\n", BUFFER_BLOCK_LEN);

  char *string = malloc(BUFFER_BLOCK_LEN);
  if (string == NULL) {
    return;
  }

  int n_blocks = 1;
  int buff_start = 0;

  while (fgets(&string[buff_start], BUFFER_BLOCK_LEN, stdin) != NULL) {
    printf("Buff start on block %d: %d\n", n_blocks, buff_start);

    if (strchr(&string[buff_start], '\n') != NULL) {
      printf("Found backslash n on block %d\n", n_blocks);
      break;
    }

    n_blocks++;
    char *tmp = realloc(string, BUFFER_BLOCK_LEN * n_blocks);
    if (tmp == NULL) {
      free(string);
      return;
    }

    buff_start += (BUFFER_BLOCK_LEN - 1);

    string = tmp;
  }

  printf("Len of string: %lu\n", strlen(string));

  fputs(string, stdout);
  free(string);
}

/* https://dreamanddead.github.io/CSAPP-3e-Solutions/chapter3/3.71/ */
void good_echo_from_solutions(void) {
  char buf[BUFFER_BLOCK_LEN];
  while (1) {
    /* function fgets is interesting */
    char *p = fgets(buf, BUFFER_BLOCK_LEN, stdin);
    if (p == NULL) {
      break;
    }
    printf("%s", p);
  }
  return;
}

int main(void) {
  good_echo();
  return 0;
}
