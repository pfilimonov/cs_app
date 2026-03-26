
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>

typedef unsigned float_bits;

int bits_length(int i) {
  if ((i & INT_MIN) != 0) {
    return 32;
  }

  unsigned u = (unsigned)i;
  int length = 0;
  while (u >= (1 << length)) {
    length++;
  }
  return length;
}

float_bits float_i2f(int i, int debug) {
  int sig = 0;
  int absi = i;
  if (i & INT_MIN) {
    // negative i
    sig = 1;
    absi = ~i + 1;
  }

  if (debug) {
    printf("i=%d, absi=%d\n", i, absi);
  }

  if (absi == INT_MIN) {
    // INT MIN case
    if (debug) {
      printf("absi=INT_MIN\n");
    }
    return (1 << 31) | (127 + 31) << (23);
  }

  int l = bits_length(absi);

  if (l == 0) {
    return 0;
  }

  if (debug) {
    printf("absi: %b. L: %d\n", absi, l);
  }
  unsigned E = l - 1;
  unsigned exp = E + 127;
  unsigned frac = absi & ~(1 << E); // drop 1
  if (debug) {
    printf("Frac drop 1: %#x\n", frac);
  }
  int shift = E - 23;
  if (shift >= 0) {
    unsigned dropped = frac & ((1 << shift) - 1);
    unsigned half = 1 << (shift - 1);
    frac = (frac >> (E - 23));
    if (dropped > half || (dropped == half && (frac & 1))) {
      frac += 1;
      if (frac == 0x800000) {
        frac = 0;
        exp += 1;
      }
    }
  } else {
    frac = frac << (-shift);
  }

  if (debug) {
    printf("sig=%d\nexp=%b\nfrac=%b\n", sig, exp, frac);
  }

  return (sig << 31) | (exp << 23) | (frac & 0x007FFFFF);
}

int main() {
  float f;
  for (unsigned u = 0; u < UINT_MAX; u++) {
    memcpy(&f, &u, sizeof(u));
    float_bits our_fb = float_i2f((int)u, 0);
    float our_f;
    memcpy(&our_f, &our_fb, sizeof(our_f));
    float std_f = (float)(int)u;
    // printf("Our int: %d, std int: %d\n", our_i, std_i);

    float diff = fabs(our_f - std_f);
    if (diff > FLT_MIN) {
      printf("Bits %#x float (%f) conversion differs. Diff = %.10f\n", u, f,
             diff);
      printf("our_f=%.10f\nstd_f=%.10f\n", our_f, std_f);
      float_i2f((int)u, 1);
      assert(0);
    }
  }

  return 0;
}
