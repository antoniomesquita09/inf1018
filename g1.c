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

void big_sum(BigInt res, BigInt a, BigInt b) {
    int i;
    BigInt compA, compB, compRes;
    char rest, sub, sub2;
    rest = 0x00;

    if (a[0] < 0xEF & b[0] < 0xEF) { // sum positive nums
        for (i = 0; i < 16; i++) { // sum BigInt
            res[i] = a[i] + b[i] + rest;

            sub = 0xFF - a[i];
            sub2 = b[i] + sub;
            if (sub == 0x00 | sub2 == 0x00) { // if its too big
                rest = 0x01;
            } else {
                rest = 0x00;
            }
        };
        return;
    }
    // else sum negative nums
    big_comp2(compA, a);
    big_comp2(compB, b);

    for (i = 0; i < 16; i++) { // sum BigInt
        compRes[i] = compA[i] + compB[i] + rest;

        sub = 0xFF - compA[i];
        sub2 = compB[i] + sub;
        if (sub == 0x00 | sub2 == 0x00) { // if its too big
            rest = 0x01;
        } else {
            rest = 0x00;
        }
    };
    big_comp2(res, compRes);
    return;
}

void big_sub(BigInt res, BigInt a, BigInt b) {
    int i;
    BigInt compB;
    char rest, sub, sub2;
    rest = 0x00;

    if (a[0] < 0xEF & b[0] < 0xEF) { // a and b positives
        for (i = 0; i < 16; i++) {
            if (a[i] < b[i]) { // a smaller than b
                a[i+1] -= 0x01; // borrow from next
                res[i] = 0xFF + a[i] - b[i];
            }
            res[i] = a[i] - b[i];
        };
        return;
    }
}

// long b = 0x 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0001;
//            |          |         |         |         |         |         |         |

int main(void) {
    int i, j;
    long c = -4;
    long d = -8;
    BigInt a = {0x07, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BigInt b = {0x04, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BigInt res, long4, long8;

    printf("Sub sum =================>\n");
    big_sub(res, b, a);
    for (i = 0; i < 16; i++) {
        printf("Result value %d: 0x%x\n", i, res[i]);
    };
    return 0;
}