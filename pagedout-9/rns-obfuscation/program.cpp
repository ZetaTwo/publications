#include <cstdio>

#include "rns.hpp"

typedef RNS<uint8_t, uint16_t, 4, 5, 13> rns;

const rns damage(10);
const uint16_t damage2 = 10;

__attribute__((noinline)) void take_damage(rns &val) {
    val -= damage;
}

__attribute__((noinline)) void take_damage2(uint16_t &val) {
    val -= damage2;
}

int main() {

    uint16_t val;
    scanf("%hd", &val);

    rns health1(val);
    uint16_t health2 = val;
    
    printf("HP1: %hd, %hd\n", health1.decode(), health2);
    take_damage(health1);
    take_damage2(health2);
    printf("HP2: %hd, %hd\n", health1.decode(), health2);

    return 0;
}
