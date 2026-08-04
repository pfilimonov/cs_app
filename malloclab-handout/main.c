#include "memlib.h"
#include "mm.h"

int main(void) {
  mem_init();

  mem_reset_brk();

  mm_init();

  void *ptr1 = mm_malloc(2040);
  void *ptr2 = mm_realloc(ptr1, 2050);
  void *ptr3 = mm_realloc(ptr2, 4000);
  mm_free(ptr3);

  return 0;
}
