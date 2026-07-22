#include "memlib.h"
#include "mm.h"

int main(void) {
  mem_init();

  mm_init();

  void *ptr1 = mm_malloc(20);
  void *ptr2 = mm_malloc(30);
  void *ptr3 = mm_malloc(50);
  mm_free(ptr1);
  mm_free(ptr2);
  mm_free(ptr3);

  return 0;
}
