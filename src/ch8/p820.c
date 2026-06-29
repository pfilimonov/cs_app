#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
  int e;
  if ((e = execve("/bin/ls", argv, envp))) {
    printf("Error: %s\n", strerror(errno));
    exit(1);
  }
}
