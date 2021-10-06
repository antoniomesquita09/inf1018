#include <stdio.h>
#include <stdlib.h>

void dump(void *p, int n)
{
    unsigned char *p1 = p;
    while (n--)
    {
        printf("\n0x%02X\n", *p1);
        p1++;
    }
}

int isLittleEndian() {
    int b = 1;
    return *(char *) & b;
}

unsigned int inverte32(unsigned int x) {
    unsigned char d = x >> 24;
    unsigned char c = (x >> 16) & 0xFF;
    unsigned char b = (x >> 8) & 0xFF;
    unsigned char a = x & 0xFF;
    return (a << 24) + (b << 16) + (c << 8) + d;
}

int conta_bytes(char s)
{
    int i = 0;
    char temp = s;
    while (temp & 0x80)
    {
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
        printf("Erro de leitura de arquivo.", stderr);
        return -1;
    }

    while (fread(&current, 1, 1, arq_entrada)) {
        totalBytes = conta_bytes(current);
        aux = 0;

        if (totalBytes == 0) {

            // Unify to 32 bits result
            aux = aux | current;
            // printf("\nResult char: %.4X\n", aux);
        }
        else {
            for (i = totalBytes; i > 0; --i)
            {
                parse = 2;
                if (i == totalBytes)
                    parse = totalBytes + 1;

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
            // printf("\nResult unicode: %.4X\n", aux);
        }
        fwrite(&aux, sizeof(int), 1, arq_saida);
    }

    return 0;
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

// ===============================================================================================+>>>>>>>>>>

int varint_utf(FILE *arq_entrada, FILE *arq_saida) {
    long rSize;
    long wSize;
    int i, j;
    unsigned int *bufferRead;
    unsigned int *bufferWrite;
    unsigned char *p;
    char ordenacao;
    char qtdBitSig;
    char qtdZeros;
    unsigned int varUTF8;

    if (arq_entrada == NULL || arq_saida == NULL) {
        printf("Erro de leitura de arquivo.");
        return -1;
    }

    fseek(arq_entrada, 0, SEEK_END);
    rSize = ftell(arq_entrada);
    wSize = rSize - 4;
    printf("O tamanho de rSize eh: %ld\n", rSize);
    rewind(arq_entrada);

    bufferRead = (unsigned int *)malloc(sizeof(unsigned int) * rSize);
    if (bufferRead == NULL) {
        printf("Erro de memória.");
        exit(2);
    }
    bufferWrite = (unsigned int *)malloc((sizeof(unsigned int) * wSize));
    if (bufferWrite == NULL) {
        printf("Erro de memória.");
        exit(2);
    }

    ordenacao = isLittleEndian();

    fread(bufferRead, 4, rSize / 4, arq_entrada);
    printf("O tamanho de buffer eh: %ld bytes\n", rSize);

    //print e comparação com dump
    for (i = 0; i < rSize / 4; i++)
        printf("%c", bufferRead[i]);
    printf("\n\n");

    printf("int* buffer:\n");
    for (i = 0; i < rSize / 4; i++)
        printf("%08X|", bufferRead[i]);

    //dump (&buffer[0], lSize);
    printf("\n\n");

    for (i = 1; i < rSize / 4; i++)
        if (ordenacao)
            bufferWrite[i - 1] = inverte32(bufferRead[i]);

    printf("inverso int* buffer (sem BOM):\n");
    for (i = 0; i < wSize / 4; i++)
        printf("%02X|", bufferWrite[i]);
    printf("\n\n");

    p = &bufferWrite;
    wSize /= 4;
    wSize++;
    i = 0;
    j = 0;

    while(wSize--) {
        p = &bufferWrite[i - 1];

        if (bufferRead[i] <= 0x007F) {
            if ((*(p + 3) & 0x40) == 0x40)
                j = 1; // bit na posicao 7
            else if ((*(p + 3) & 0x20) == 0x20)
                j = 2; // bit na posicao 6
            else if ((*(p + 3) & 0x10) == 0x10)
                j = 3; // bit na posicao 5
            else if ((*(p + 3) & 0x08) == 0x08)
                j = 4; // bit na posicao 4
            else if ((*(p + 3) & 0x04) == 0x04)
                j = 5; // bit na posicao 3
            else if ((*(p + 3) & 0x02) == 0x02)
                j = 6; // bit na posicao 2
            else if ((*(p + 3) & 0x01) == 0x01)
                j = 7; // bit na posicao 1
            else
                j = 8; // zero

            qtdBitSig = 8 - j;
            qtdZeros = 7 - qtdBitSig;

            if (qtdZeros == 7) {
                varUTF8 = 0x00;
            }
            else if (qtdZeros == 6) {
                varUTF8 = 0x00; // 0(0)(0)(0)(0)(0)(0)x
                varUTF8 = varUTF8 | (bufferRead[i] & 0x01);
            }
            else if (qtdZeros == 5) {
                varUTF8 = 0x00; // 0(0)(0)(0)(0)(0)xx
                varUTF8 = bufferRead[i] & 0x03;
            }
            else if (qtdZeros == 4) {
                varUTF8 = 0x00; // 0(0)(0)(0)(0)xxx
                varUTF8 = bufferRead[i] & 0x07;
            }
            else if (qtdZeros == 3) {
                varUTF8 = 0x00; // 0(0)(0)(0)xxxx
                varUTF8 = bufferRead[i] & 0x0F;
            }
            else if (qtdZeros == 2) {
                varUTF8 = 0x00; // 0(0)(0)xxxxx
                varUTF8 = bufferRead[i] & 0x1F;
            }
            else if (qtdZeros == 1) {
                varUTF8 = 0x00; // 0(0)xxxxx
                varUTF8 = bufferRead[i] & 0x3F;
            }
            else if (qtdZeros == 0) {
                varUTF8 = 0x00; // 0xxxxxxx
                varUTF8 = bufferRead[i] & 0x7F;
            }
            else {
                printf("Erro na leitura do valor UTF-32 na seção 1.\n");
                return -1;
            }

            printf("O Valor UTF-32 0x%02X convertido para UTF-8 e: 0x%02X\n", bufferRead[i], varUTF8);
            //varutf8"U+"+varUTF8;
            if (fwrite(&varUTF8, 1, 1, arq_saida))
                printf("Caractere impresso no arquivo com sucesso!\n");
            else
                printf("erro de gravação do arquivo.");
        }

        if (bufferRead[i] >= 0x0080 && bufferRead[i] <= 0x07FF) {
            if ((*(p + 2) & 0x04) == 0x04)
                j = 5; // bit na posicao 4
            else if ((*(p + 2) & 0x02) == 0x02)
                j = 6; // bit na posicao 3
            else if ((*(p + 2) & 0x01) == 0x01)
                j = 7; // bit na posicao 2
            else
                j = 8; // bit na posicao 1

            qtdBitSig = 16 - j;
            qtdZeros = 11 - qtdBitSig;

            if (qtdZeros == 3) {
                varUTF8 = 0xC080 & 0xE3FF; // 110(0)(0)(0)xx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 6) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else if (qtdZeros == 2) {
                varUTF8 = 0xC080 & 0xE7FF; // 110(0)(0)xxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 6) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else if (qtdZeros == 1 || qtdZeros == 0) {
                varUTF8 = 0xC080 & 0xEFFF; // 110(0)xxxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 6) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else {
                printf("erro na leitura do valor UTF-32 na seção 2.\n");
                return -1;
            }

            printf("O Valor UTF-32 0x%04X convertido para UTF-8 e: 0x%04X\n", bufferRead[i], varUTF8);
            varUTF8 = inverte32(varUTF8);
            varUTF8 = varUTF8 >> 16;
            if (fwrite(&varUTF8, 2, 1, arq_saida))
                printf("Caractere impresso no arquivo com sucesso!\n");
            else
                printf("erro de gravação do arquivo.");
        }

        if (bufferRead[i] >= 0x0800 && bufferRead[i] <= 0xFFFF && bufferRead[i] != 0xFEFF) {
            if (((*p + 2) & 0x08) == 0x08)
                j = 6; // bit na posicao 5
            else if (((*p + 2) & 0x04) == 0x04)
                j = 7; // bit na posicao 4
            else if (((*p + 2) & 0x02) == 0x02)
                j = 8; // bit na posicao 3
            else if (((*p + 2) & 0x01) == 0x01)
                j = 9; // bit na posicao 2
            else if (((*p + 2) & 0x10) == 0x10)
                j = 10; // bit na posicao 1

            qtdBitSig = 24 - j;
            qtdZeros = 16 - qtdBitSig;

            if (qtdZeros == 4) {
                varUTF8 = 0xE08080;
                varUTF8 = varUTF8 | (bufferRead[i] >> 6) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else if (qtdZeros == 3) {
                varUTF8 = 0xE08080;
                varUTF8 = varUTF8 | (bufferRead[i] >> 12) << 16;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else if (qtdZeros == 2) {
                varUTF8 = 0xE08080;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 12) & 0x07) << 16;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else if (qtdZeros == 1) {
                varUTF8 = 0xE08080;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 12) & 0x07) << 16;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else if (qtdZeros == 0) {
                varUTF8 = 0xE08080;
                varUTF8 = varUTF8 | (bufferRead[i] >> 12) << 16;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else {
                printf("erro na leitura do valor UTF-32 na seção 3.\n");
                return -1;
            }

            printf("O Valor UTF-32 0x%06X convertido para UTF-8 e: 0x%06X\n", bufferRead[i], varUTF8);
            varUTF8 = inverte32(varUTF8);
            varUTF8 = varUTF8 >> 8;

            if (fwrite(&varUTF8, 3, 1, arq_saida))
                printf("Caractere impresso no arquivo com sucesso!\n");
            else
                printf("erro de gravação do arquivo.");
        }

        if (bufferRead[i] >= 0x10000 && bufferRead[i] <= 0x10FFFF) {
            if (((*p + 1) & 0x10) == 0x10)
                j = 3; // bit mais sig na 5a posicao
            else if (((*p + 1) & 0x08) == 0x08)
                j = 4; // bit mais sig na 4a posicao
            else if (((*p + 1) & 0x04) == 0x04)
                j = 5; // bit mais sig na 3a posicao
            else if (((*p + 1) & 0x02) == 0x02)
                j = 6; // bit mais sig na 2a posicao
            else
                j = 7; // bit mais sig na 1a posicao

            qtdBitSig = 24 - j;
            qtdZeros = 21 - qtdBitSig;

            if (qtdZeros == 4) {
                varUTF8 = 0xF0808080 & 0xF8DFFFFF; // 11110(000) 10(0)xxxxx 10xxxxxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 12) << 16;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else if (qtdZeros == 3) {
                varUTF8 = 0xF0808080 & 0xF8FFFFFF; // 11110(000) 10xxxxxx 10xxxxxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 12) << 16;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else if (qtdZeros == 2) {
                varUTF8 = 0xF0808080 & 0xF9FFFFFF; // 11110(00)x 10xxxxxx 10xxxxxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 18) << 24;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 12) & 0x3F) << 16;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else if (qtdZeros == 1) {
                varUTF8 = 0xF0808080 & 0xFBFFFFFF; // 11110(0)xx 10xxxxxx 10xxxxxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 18) << 24;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 12) & 0x3F) << 16;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else if (qtdZeros == 0) {
                varUTF8 = 0xF0808080; // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 18) << 24;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 12) & 0x3F) << 16;
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);
            }
            else {
                printf("Erro na leitura do valor UTF-32 na seção 4.\n");
                return -1;
            }


            printf("O Valor UTF-32 0x%06X convertido para UTF-8 e: 0x%06X\n", bufferRead[i], varUTF8);
            varUTF8 = inverte32(varUTF8);
            if (fwrite(&varUTF8, 4, 1, arq_saida))
                printf("Caractere impresso no arquivo com sucesso!\n");
            else
                printf("erro de gravação do arquivo.");
        }
        i++;
    }
    free(bufferRead);
    free(bufferWrite);
    return 0;
}