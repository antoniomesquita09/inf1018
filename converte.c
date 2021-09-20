#include <stdio.h>

void dump_bin(unsigned n) {
    unsigned i;
    for (i = 1 << 7; i > 0; i = i / 2)
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
    char current;
    int numread, totalBytes;
    
    if (arq_entrada == NULL || arq_saida == NULL) {
		fputs("erro de leitura de arquivo.", stderr);
		return 0;
	}

    numread = fread(&current, 1, 1, arq_entrada);

    while(numread == 1) {
        totalBytes = conta_bytes(current);

        if (totalBytes == 0) {
            dump(&current, sizeof(current));
        } else {
            printf("current: ");
            dump(&current, sizeof(current));
            printf("\ntotalBytes: %d\n", totalBytes);

            for(;totalBytes != 0; --totalBytes) {
                dump(&current, sizeof(current));
                printf("\n");
                // fread(&current, 1, 1, arq_entrada);
            }

        }
        printf("\n=================>\n");
        numread = fread(&current, 1, 1, arq_entrada);
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