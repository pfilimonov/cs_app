#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]) {
  int i = 0;
  printf("Command line arguments:\n");
  while (argv[i] != NULL) {
    printf("argv[%d]: %s\n", i, argv[i]);
    i++;
  }

  i = 0;
  printf("Environment variables:\n");
  while (envp[i] != NULL) {
    printf("envp[%d]: %s\n", i, envp[i]);
    i++;
  }

  return 0;
}
