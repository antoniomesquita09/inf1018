// Antonio Mesquita Júnior - 1611715
// Yuri Lemos - 1610193
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "converte.h"

int main(void) {
	FILE* utf8 = fopen("peq_utf.txt", "rb");
	if (utf8 == NULL) {
        fputs("erro ao abrir arquivo", stderr);
        exit(1);
    }

	FILE* utf8out = fopen("peq_utf_out.txt", "wb");
    if (utf8out == NULL) {
        fputs ("erro ao abrir arquivo",stderr);
        exit(1);
    }

    utf_varint(utf8, utf8out);

    return 1;
}