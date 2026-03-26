#include <float.h>
#include <limits.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned float_bits;

int float_f2i(float_bits f, int debug) {
  unsigned sign = f >> 31;
  unsigned exp = (f >> 23) & 0xFF;
  unsigned frac = f & 0x007FFFFF;

  if (exp == 0xFF) {
    if (debug) {
      printf("Wrong value\n");
    }
    return 0x80000000;
  }

  if (exp) {
    if (debug) {
      printf("exp=%#x\n", exp);
    }
    //  Norm
    int E = exp - 127;
    if (E >= 31) {
      // printf("Too big exp E=%d\n", E);
      return 0x80000000;
    }
    if (E < 0) {
      if (debug) {
        printf("Too small exp\n");
      }
      return 0;
    }
    int first;
    frac = frac | (1 << 23);
    if (E <= 23) {
      first = frac >> (23 - E);
    } else {
      first = frac << (E - 23);
    }

    if (sign) {
      return (~first + 1); // -first
    } else {
      return first;
    }
  } else {
    // Denorm rounds toward zero
    return 0;
  }
}

int main() {
  float f;
  for (unsigned u = 0; u < UINT_MAX; u++) {
    memcpy(&f, &u, sizeof(u));
    int our_i = float_f2i((float_bits)u, 0);
    int std_i = (int)f;
    // printf("Our int: %d, std int: %d\n", our_i, std_i);

    if (std_i != our_i) {
      printf("Bits %#x integer conversion differs. our int=%d, std int=%d\n", u,
             our_i, std_i);
      float_f2i((float_bits)u, 1);
    }
  }

  return 0;
}
