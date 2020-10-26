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

void big_sub(BigInt res, BigInt a, BigInt b) { // Only accepts a greater than b 
    int i;
    BigInt compA, compB, compRes;

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

void big_shl(BigInt res, BigInt a, int n) {
    BigInt aux;
    unsigned char ass, ass2, init;

    int rest, i, multiple;
   
    rest = n % 8;
  
    if (rest == 0) {
        multiple = n / 8;									
        while(i < 16) {
            if(i < multiple) {
                res[i] = 0x00;
            } else {
                res[i] = a[i - multiple];
            }
            i++;
        }
    } else {
        ass = 0xFF << (8 - rest);
	    init = 0x00;

        for(i = 0; i < 16; i++) {
            ass2 = a[i] & ass;
            res[i]=(a[i] << rest) | (init >> (8 - rest));
            init = ass2;
        }
        big_shl(res, res, n - rest);
    }

    return;
}

void big_shr(BigInt res, BigInt a, int n) {
    unsigned char aux, aux2, init;
    int rest, i, multiple;

    rest = n;
   
    rest = rest % 8;
    
    if(rest == 0) {
        multiple = n / 8;
       
        for(i = 16; i >= 0; i--) {
            if(i > (15 - multiple)) {
                res[i] = 0x00;
            } else {
                res[i] = a[i + multiple];
            }
        }
    } else {
        aux = 0xFF >> (8 - rest);
        init = 0x00;
        for(i = 16; i >= 0; i--) {
            aux2 = a[i] & aux;
            res[i] = (a[i] >> rest) | (init << (8 - rest));
            init = aux2;
        }
        big_shr(res, res, n - rest);
    }
    return;
}

void big_sar(BigInt res, BigInt a, int n) {
    unsigned char aux, aux2, init;
    int rest, i, multiple;

    rest = n;
   
    rest = rest % 8;
    
    if(rest == 0) {
        multiple = n / 8;
       
        for(i = 16; i >= 0; i--) {
            if(i > (15 - multiple)) {
                res[i] = 0xFF;
            } else {
                res[i] = a[i + multiple];
            }
        }
    } else {
        aux = 0xFF >> (8 - rest);
        init = 0xFF;
        for(i = 16; i >= 0; i--) {
            aux2 = a[i] & aux;
            res[i] = (a[i] >> rest) | (init << (8 - rest));
            init = aux2;
        }
        big_shr(res, res, n - rest);
    }
    return;
}
