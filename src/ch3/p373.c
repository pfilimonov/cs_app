
#include <assert.h>
#include <limits.h>
#include <memory.h>

typedef enum { NEG, ZERO, POS, OTHER } range_t;

extern range_t find_range_asm(float x);
range_t find_range(float x);

range_t find_range(float x) {
  int result;
  if (x < 0)
    result = NEG;
  else if (x == 0)
    result = ZERO;
  else if (x > 0)
    result = POS;
  else
    result = OTHER;
  return result;
}

int main(void) {
  for (unsigned u = 0; u < UINT_MAX; u++) {
    float f;
    memcpy(&f, &u, sizeof(float));
    assert(find_range(f) == find_range_asm(f));
  }
  return 0;
}
