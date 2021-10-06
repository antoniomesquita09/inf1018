// Antonio Mesquita Júnior - 1611715
// Yuri Lemos - 1610193
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "converte.h"

int main(void) {
	// FILE* file_in_utf8 = fopen("utf8_peq.txt", "rb");
	// if (file_in_utf8 == NULL) {
    //     fputs("Erro ao abrir arquivo: peq_utf.txt", stderr);
    //     exit(1);
    // }

	// FILE* file_out_varint = fopen("out_varint.txt", "wb");
    // if (file_out_varint == NULL) {
    //     fputs("Erro ao abrir arquivo: out_varint.txt", stderr);
    //     exit(1);
    // }

    // utf_varint(file_in_utf8, file_out_varint);

	FILE* file_in_varint = fopen("out_varint.txt", "rb");
	if (file_in_varint == NULL) {
        fputs("Erro ao abrir arquivo: out_varint.txt", stderr);
        exit(1);
    }

	FILE* file_out_utf8 = fopen("out_utf8.txt", "wb");
    if (file_out_utf8 == NULL) {
        fputs("Erro ao abrir arquivo: out_utf8.txt", stderr);
        exit(1);
    }

    varint_utf(file_in_varint, file_out_utf8);

    return 0;
}