#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int main(void) {

  int fd = open("hello.txt", O_RDWR);
  if (fd == -1) {
    printf("Could not open file hello.txt");
    return 1;
  }

  printf("fd=%d\n", fd);

  int len = lseek(fd, 0, SEEK_END);

  printf("len=%d\n", len);

  void *bufp = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd, 0);
  printf("bufp=%p\n", bufp);
  if (bufp == MAP_FAILED) {
    printf("Mmap failed\n");
    return 1;
  }

  char *bufch = (char *)bufp;

  bufch[0] = 'J';

  printf("Content=%s\n", (char *)bufp);

  msync(bufp, len, 0);

  if (munmap(bufp, len) == -1) {
    printf("Unmap failed\n");
    return 1;
  }
}
