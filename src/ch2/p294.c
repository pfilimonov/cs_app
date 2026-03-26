
#include <float.h>
#include <limits.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned float_bits;

float_bits float_twice(float_bits f) {
  unsigned sign = f >> 31;
  unsigned exp = (f >> 23) & 0xFF;
  unsigned frac = f & 0x007FFFFF;

  if (exp == 0xFF) {
    return f;
  }

  if (exp) {
    // Norm
    exp += 1;
  } else if (frac & (1 << 22)) {
    // Denorm to norm
    exp = 1;
    frac <<= 1;
    frac = frac & 0x007FFFFF;
  } else {
    // Denorm
    frac <<= 1;
  }
  return sign << 31 | (exp << 23) | frac;
}

int main() {
  float f;
  for (unsigned u = 0; u < UINT_MAX; u++) {
    memcpy(&f, &u, sizeof(u));
    float_bits our_tw = float_twice((float_bits)u);
    float std_tw = 2.0 * f;
    float our_tw_float;
    memcpy(&our_tw_float, &our_tw, sizeof(our_tw));

    float diff = fabs(our_tw_float - std_tw);
    if (diff > FLT_MIN) {
      printf("Bits %#x twice differs by %.10f\n", u, diff);
    }
  }

  return 0;
}
