

#include <limits.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#define EPS 1e-23

typedef unsigned float_bits;

float_bits float_half(float_bits f) {
  unsigned sign = f >> 31;
  unsigned exp = (f >> 23) & 0xFF;
  unsigned frac = f & 0x007FFFFF;
  unsigned rest = f & 0x7FFFFFFF;

  if (exp == 0xFF) {
    return f;
  }

  int add = (frac & 0x3) == 0x3;

  if (exp == 1) {
    // Norm to denorm
    rest >>= 1;
    rest += add;
    exp = (rest >> 23) & 0xFF;
    frac = rest & 0x007FFFFF;
  } else if (exp) {
    // Norm
    exp -= 1;
  } else {
    // Denorm
    frac >>= 1;
    frac += add;
  }
  return sign << 31 | (exp << 23) | frac;
}

int main() {
  float f;
  for (unsigned u = 0; u < UINT_MAX; u++) {
    memcpy(&f, &u, sizeof(u));
    float_bits our_hf = float_half((float_bits)u);
    float std_hf = 0.5 * f;
    float our_hf_float;
    memcpy(&our_hf_float, &our_hf, sizeof(our_hf));

    float diff = fabs(our_hf_float - std_hf);
    if (diff > EPS) {
      printf("Bits %#x half differs by %.10f\n", u, diff);
    }
  }

  return 0;
}
