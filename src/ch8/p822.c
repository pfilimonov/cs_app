#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXARGS 100
#define MAXLINE 1000

pid_t Fork(void);

pid_t Fork(void) {
  pid_t pid;
  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error: %s\n", strerror(errno));
    exit(0);
  }
  return pid;
}

int mysystem(char *command);

int mysystem(char *command) {
  int pid = Fork();
  if (pid == 0) {
    char *argv[] = {"sh", "-c", command, NULL};

    int execv_ret = execv("/bin/sh", argv);
    if (execv_ret < 0) {
      printf("%s: Command not found\n", command);
      return 1;
    }
  }

  int status;
  if (waitpid(pid, &status, 0) < 0) {
    fprintf(stderr, "waitpid error: %s\n", strerror(errno));
    return 1;
  }

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }

  if (WIFSIGNALED(status)) {
    return WTERMSIG(status);
  }

  return 1;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    fprintf(stderr, "Provide command\n");
    exit(0);
  }

  char buf[MAXLINE];
  char *p = buf;

  int i = 1;
  while (i < argc) {
    strcpy(p, argv[i]);
    p += strlen(argv[i]);
    *p = ' ';
    p++;
    i++;
  }

  *p = '\0';

  printf("Len(command)=%lu\n", strlen(buf));
  printf("Execute command:\n%s\n", buf);

  return mysystem(buf);
}
