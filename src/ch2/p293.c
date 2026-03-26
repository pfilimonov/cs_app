#include <limits.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#define EPS 1e-6

typedef unsigned float_bits;

float_bits float_absval(float_bits f) {
  unsigned sign = f >> 31;
  unsigned exp = (f >> 23) & 0xFF;
  unsigned frac = f & 0x007FFFFF;

  if (exp == 0xFF && !!frac) {
    return f;
  }

  return (exp << 23) | (frac);
}

int main() {
  float f;
  for (unsigned u = 0; u < UINT_MAX; u++) {
    memcpy(&f, &u, sizeof(u));
    float_bits our_abs = float_absval((float_bits)u);
    float std_abs = fabs(f);
    float our_abs_float;
    memcpy(&our_abs_float, &our_abs, sizeof(our_abs));

    float diff = fabs(our_abs_float - std_abs);
    if (diff > EPS) {
      printf("Bits %#x abs differs by %.10f\n", u, diff);
    }
  }

  return 0;
}
