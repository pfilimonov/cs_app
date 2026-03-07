#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

void copy_int(int val, void *buf, int maxbytes) {
  if (maxbytes >= (int)sizeof(val)) {
    memcpy(buf, (void *)&val, sizeof(val));
  }
}
