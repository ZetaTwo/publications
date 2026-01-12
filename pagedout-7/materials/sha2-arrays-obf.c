#include <stdint.h>
#include <stddef.h>

#define N 11
#define P 312
#define H 8
#define K 64
#define M (1LL<<32)

void init(uint32_t *h, uint32_t *k) {
  size_t pi = 0;
  uint8_t ip[P-2];
  for (size_t p = 0; p < (P-2); p++) {
    ip[p] = 1;
  }
  for (size_t p = 0; p < (P-2); p++) {
    if (ip[p] == 0) {
      continue;
    }
    if (pi < H) {
      double x = p + 2;
      double xn = x / 2;
      for (size_t i = 0; i < N; i++) {
        xn = 1.0/2*xn+x/(2*xn);
      }
      h[pi] = (uint32_t)(uint64_t)(xn*M);
    }
    if (pi < K) {
      double x = p + 2;
      double xn = x / 3;
      for (size_t i = 0; i < N; i++) {
        xn = 2.0/3*xn+x/(3*xn*xn);
      }
      k[pi] = (uint32_t)(uint64_t)(xn*M);
    }
    pi++;
    for (size_t n = p+2; n < (P-2); n++) {
      size_t val = (p + 2) * n - 2;
      if (val + 1 > (P-2)) {
        break;
      }
      ip[val] = 0;
    }
  }
}

static uint32_t h[H];
static uint32_t k[K];

int main() {
    init(h, k);

    // h & k used as part of an algorithm later
    // ...
}
