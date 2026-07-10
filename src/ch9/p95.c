#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Specify filename\n");
    return 1;
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    printf("Could not open file %s\n", argv[1]);
    return 1;
  }

  int len = lseek(fd, 0, SEEK_END);

  void *bufp = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
  if (bufp == MAP_FAILED) {
    printf("Mmap failed\n");
    return 1;
  }

  printf("%s\n", (char *)bufp);

  if (munmap(bufp, len) == -1) {
    printf("Unmap failed\n");
    return 1;
  }
}
