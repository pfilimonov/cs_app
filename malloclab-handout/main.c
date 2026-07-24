#include "memlib.h"
#include "mm.h"

int main(void) {
  mem_init();

  mem_reset_brk();

  mm_init();

  void *ptr1 = mm_malloc(2040);
  void *ptr2 = mm_malloc(2040);

  mm_free(ptr1);
  void *ptr3 = mm_malloc(2040);
  mm_free(ptr2);
  void *ptr4 = mm_malloc(2040);
  mm_free(ptr3);
  mm_free(ptr4);

  return 0;
}
