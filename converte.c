#include <stdio.h>
#include <stdlib.h>

void dump(void *p, int n) {
	unsigned char *p1 = p;
	while (n--) {
		printf("\n0x%02X\n", *p1);
		p1++;
	}
}

int conta_bytes(char s) {
	int i = 0;
	char temp = s;
	while(temp & 0x80) {
		temp = temp << 1;
		i++;
	}
	return i;
}

int utf_varint(FILE *arq_entrada, FILE *arq_saida) {
    unsigned char current;
    int totalBytes, i, parse;
    unsigned int aux;

    if (arq_entrada == NULL || arq_saida == NULL) {
		fputs("erro de leitura de arquivo.", stderr);
		return 0;
	}


    while(fread(&current, 1, 1, arq_entrada)) {
        totalBytes = conta_bytes(current);
        aux = 0;

        // printf("\ntotalBytes: %d\n", totalBytes);

        if (totalBytes == 0) {
            printf("\nChar: %c\n", current);

            // Unify to 32 bits result
            aux = aux | current;
            printf("\nResult char: %.4X\n", aux);
        } else {
            for(i = totalBytes; i > 0; --i) {
                parse = 2;
                if (i == totalBytes)
                    parse =  totalBytes + 1;

                current = current | aux;
                
                // printf("\nStart:\n");
                // dump(&current, sizeof(current));

                // printf("\nParsed:\n");
                // dump(&current, sizeof(current));

                // clean up auxiliar bits from Unicode
                current = current << parse;
                current = current >> parse;

                // printf("\nClean up %d:\n", i);
                // dump(&current, sizeof(current));

                // printf("totalBytes: %d", totalBytes);

                aux = aux | current;
                if (i != 1)
                    aux = aux << 6;

                fread(&current, 1, 1, arq_entrada);
            }
            printf("\nResult unicode: %.4X\n", aux);
        }
        printf("\n=================>\n");
        // fread(&current, 1, 1, arq_entrada);
    }

    return 1;
}

// 0000 0000 0000 0000 0000 0000 0000 0000
// ---- ---- ---- ---- ---- ---- 0000 0010
// ---------------------------------------
// 0000 0000 0000 0000 0000 0000 0000 0010
// ---------------------------------------
// 0000 0000 0000 0000 0000 0000 1000 0000
// 0000 0000 0000 0000 0000 0000 0000 1001
// ---------------------------------------
// 0000 0000 0000 0000 0000 0000 1000 1001
// ---------------------------------------
// 0000 0000 0000 0000 0010 0010 0100 0000