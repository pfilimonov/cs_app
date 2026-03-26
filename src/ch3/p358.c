long decode2(long x, long y, long z) {
  /*x in %rdi, y in %rsi, z in %rdx*/
  long t = (y - z);
  return (x * t) ^ (t << 63 >> 63);
}

int main(void) { return 0; }
