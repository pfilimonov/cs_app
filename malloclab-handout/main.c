#include "memlib.h"
#include "mm.h"

int main(void) {
  mem_init();

  mm_init();

  void *ptr1 = mm_malloc(20);
  void *ptr2 = mm_malloc(30);
  void *ptr3 = mm_malloc(50);
  printf("3 pointers allocated: %p, %p, %p\n", ptr1, ptr2, ptr3);

  mm_free(ptr1);
  mm_free(ptr2);
  mm_free(ptr3);

  printf("Pointers freed\n");

  return 0;
}
