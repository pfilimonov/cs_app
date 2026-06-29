#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

unsigned int snooze(unsigned int secs);

unsigned int snooze(unsigned int secs) {
  for (unsigned int i = 1; i <= secs; i++) {
    if (sleep(1) > 0)
      return secs - i + 1;
    printf("Slept for %u of %u secs\n", i, secs);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  unsigned int secs = 0;
  if (argc > 1)
    secs = strtol(argv[1], NULL, 10);

  printf("Returned %u\n", snooze(secs));
  return 0;
}
