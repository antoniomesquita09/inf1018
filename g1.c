#include "g1.h"

void big_val(BigInt res, long val) {
    printf("long: %ld\n", val);

    res[0] = val & 0xFF;
    res[1] = (val >> 8) & 0xFF;
    res[2] = (val >> 16) & 0xFF;
    res[3] = (val >> 24) & 0xFF;
    res[4] = (val >> 32) & 0xFF;
    res[5] = (val >> 40) & 0xFF;
    res[6] = (val >> 48) & 0xFF;
    res[7] = (val >> 56) & 0xFF;
    if ((val >> 56) & 0xFF > 0xEF) { // negative complement
        res[8] = 0xFF;
        res[9] = 0xFF;
        res[10] = 0xFF;
        res[11] = 0xFF;
        res[12] = 0xFF;
        res[13] = 0xFF;
        res[14] = 0xFF;
        res[15] = 0xFF;
        return;
    }
    // positive complement
    res[8] = 0x00;
    res[9] = 0x00;
    res[10] = 0x00;
    res[11] = 0x00;
    res[12] = 0x00;
    res[13] = 0x00;
    res[14] = 0x00;
    res[15] = 0x00;
    return;
}

// long b = 0x 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0001;
//            |          |         |         |         |         |         |         |  
int main(void) {
    int i;
    long b = 1;
    // BigInt a = {0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    BigInt a;

    big_val(a, b);
    for (i = 0; i < 16; i++) {
        printf("value %d: %x\n", i, a[i]);
    };
    return 0;
}