#include <stdio.h>
#include <stdlib.h>

void dump_bin(unsigned n) {
    unsigned i;
    for (i = 1 << 31; i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
}

void dump(void *p, int n) {
	unsigned char *p1 = p;
	while (n--) {
		// printf("\n%02X\n", *p1);
        dump_bin(*p1);
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
    unsigned char current, result;
    int totalBytes, i, parse;
    unsigned int aux;
    
    if (arq_entrada == NULL || arq_saida == NULL) {
		fputs("erro de leitura de arquivo.", stderr);
		return 0;
	}

    
    while(fread(&current, 1, 1, arq_entrada)) {
        totalBytes = conta_bytes(current);
        aux = 0;
        result = 0;


        // printf("\ntotalBytes: %d\n", totalBytes);

        if (totalBytes == 0) {
            // printf("\nChar: %c\n", current);
            // dump(&current, sizeof(current));

            // Unify to 32 bits result
            result = current | result;
        } else {

            for(i = totalBytes; i > 0; --i) {
                parse = 2;
                if (i == totalBytes)
                    parse = parse + totalBytes - 1;
                
                current = current | aux;

                // printf("\nParsed:\n");
                // dump(&current, sizeof(current));

                // clean up auxiliar bits from Unicode
                current = current << parse;
                current = current >> parse;

                // printf("\nClean up:\n");
                // dump(&current, sizeof(current));

                // Times to shift left after clean up
                parse = i == 1 ? 0 : 6;

                // Shift left to unify bits
                current = current << parse;

                // printf("\nShifted:\n");
                // dump(&current, sizeof(current));

                // Unify to 32 bits result
                result = current | result;

                // printf("\nResult:\n");
                // dump(&result, sizeof(result));

                fread(&current, 1, 1, arq_entrada);
            }
        }
        printf("\nResult unicode: 0x%.4X\n", result);
        printf("\n=================>\n");
        // numread = fread(&current, 1, 1, arq_entrada);
    }

    return 1;
}

// x = 1100 0000
//     &
// c = 1100 0000
//   =
//     1100 0000
// 

// x = 
//     &
// c = 1100 0000
//   =
//     1000 0000
// 0000 0000
// &
// 1000 0000