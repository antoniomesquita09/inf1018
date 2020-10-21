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

void big_comp2(BigInt res, BigInt a) {
    int i, j;
    for (i = 0; i < 16; i++) { // Invert bits
        res[i] = ~a[i];
    };
    for (j = 0; j < 16; j++) { // sum 0x01
        if (res[j] == 0xFF) {
            res[j] = 0x00;
        } else {
            res[j] = res[j] + 0x01;
            break;
        }
    };
}

// long b = 0x 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0001;
//            |          |         |         |         |         |         |         |  
int main(void) {
    int i;
    long b = -2;
    BigInt a = {0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    BigInt res;

    big_comp2(res, a);
    for (i = 0; i < 16; i++) {
        printf("Result value %d: 0x%x\n", i, res[i]);
    };
    return 0;
}