// Antonio Mesquita Júnior - 1611715
// Yuri Lemos - 1610193
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "converte.h"

int main(void) {
	// FILE* utf8_pequeno_in = fopen("peq_utf.txt", "rb");
	// if (utf8_pequeno_in == NULL) {
    //     fputs("Erro ao abrir arquivo: peq_utf.txt", stderr);
    //     exit(1);
    // }

	// FILE* varint_pequeno_out = fopen("peq_varint_out.txt", "wb");
    // if (varint_pequeno_out == NULL) {
    //     fputs("Erro ao abrir arquivo: peq_varint_out.txt", stderr);
    //     exit(1);
    // }

    // utf_varint(utf8_pequeno_in, varint_pequeno_out);

	FILE* varint_pequeno_in = fopen("peq_varint_out.txt", "rb");
    if (varint_pequeno_in == NULL) {
        fputs("Erro ao abrir arquivo: peq_varint_out.txt", stderr);
        exit(1);
    }

	FILE* utf8_pequeno_out = fopen("peq_utf_out.txt", "wb");
    if (utf8_pequeno_out == NULL) {
        fputs("Erro ao abrir arquivo: peq_utf_out.txt", stderr);
        exit(1);
    }

    varint_utf(varint_pequeno_in, utf8_pequeno_out);

    return 1;
}