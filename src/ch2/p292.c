#include <limits.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#define EPS 1e-6

typedef unsigned float_bits;

float_bits float_negate(float_bits f) {
  unsigned sign = f >> 31;
  unsigned exp = (f >> 23) & 0xFF;
  unsigned frac = f & 0x007FFFFF;

  if (exp == 0xFF && !!frac) {
    return f;
  }

  return (!sign << 31) | (exp << 23) | (frac);
}

int main() {
  float f;
  for (unsigned u = 0; u < UINT_MAX; u++) {
    memcpy(&f, &u, sizeof(u));
    float_bits our_neg = float_negate((float_bits)u);
    float std_neg = -f;
    float our_neg_float;
    memcpy(&our_neg_float, &our_neg, sizeof(our_neg));

    float diff = fabs(our_neg_float - std_neg);
    if (diff > EPS) {
      printf("Bits %#x negation differs by %.10f\n", u, diff);
    }
  }

  return 0;
}
