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

int isLittleEndian()
{
    int b = 1;

    return *(char *)&b;
}

unsigned int inverte32(unsigned int x)
{
    //printf("x = 0x%08X\n", x);
    //printf("01-D:\n");
    unsigned char d = x >> 24;
    //dump (&d, sizeof(d));
    //printf("---\n02-C:\n");
    unsigned char c = (x >> 16) & 0xFF;
    //dump (&c, sizeof(c));
    //printf("---\n03-B:\n");
    unsigned char b = (x >> 8) & 0xFF;
    //dump (&b, sizeof(b));
    //printf("---\n04-A:\n");
    unsigned char a = x & 0xFF;
    //dump (&a, sizeof(a));

    /*printf("\nX:\n");
	
		dump (&x, sizeof(x));
		unsigned int y = (a<<24) + (b<<16) + (c<<8) + d;
		printf("\nY:\n");
		dump (&y, sizeof(y));*/

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

int utf_varint(FILE *arq_entrada, FILE *arq_saida)
{
    unsigned char current;
    int totalBytes, i, parse;
    unsigned int aux;

    if (arq_entrada == NULL || arq_saida == NULL)
    {
        fputs("Erro de leitura de arquivo.", stderr);
        return -1;
    }

    while (fread(&current, 1, 1, arq_entrada))
    {
        totalBytes = conta_bytes(current);
        aux = 0;

        // printf("\ntotalBytes: %d\n", totalBytes);

        if (totalBytes == 0)
        {
            // printf("\nChar: %c\n", current);

            // Unify to 32 bits result
            aux = aux | current;
            // printf("\nResult char: %.4X\n", aux);
        }
        else
        {
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

int varint_utf(FILE *arq_entrada, FILE *arq_saida)
{
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

    if (arq_entrada == NULL || arq_saida == NULL)
    {
        fputs("erro de leitura de arquivo.", stderr);
        return -1;
    }

    // LEITURA
    // determina tamanho do arquivo de entrada e de saída
    fseek(arq_entrada, 0, SEEK_END);
    rSize = ftell(arq_entrada);
    wSize = rSize - 4; // texto sem BOM
    printf("O tamanho de rSize eh: %ld\n", rSize);
    rewind(arq_entrada);

    // aloca memória para conter todo o arquivo
    bufferRead = (unsigned int *)malloc(sizeof(unsigned int) * rSize);
    if (bufferRead == NULL)
    {
        fputs("erro de memória.", stderr);
        exit(2);
    }
    bufferWrite = (unsigned int *)malloc((sizeof(unsigned int) * wSize));
    if (bufferWrite == NULL)
    {
        fputs("erro de memória.", stderr);
        exit(2);
    }

    ordenacao = isLittleEndian();

    // insere BOM + texto do arquivo no buffer
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
        if (ordenacao)                                     // se Little Endian
            bufferWrite[i - 1] = inverte32(bufferRead[i]); // transfere texto invertido sem BOM, melhor para apontar bytes

    printf("inverso int* buffer (sem BOM):\n");
    for (i = 0; i < wSize / 4; i++)
        printf("%02X|", bufferWrite[i]);
    printf("\n\n");

    /*printf("dump - tamanho do buffer: %ld\n", wSize);
			dump (&bufferWrite[0], wSize);*/

    p = &bufferWrite; // ponteiro para percorrer invertido
                      //printf("\n%p - %02X\n", p+3, *p+3);
    wSize /= 4;
    wSize++;
    i = 0;
    j = 0;
    while (wSize--)
    {
        p = &bufferWrite[i - 1]; //evita BOM

        // determina a qual seção UTF-8 o número pertence
        if (bufferRead[i] <= 0x007F)
        {

            //printf("bufferRead[%d] eh: %02X\n", i, bufferRead[i]);
            //printf("*p eh: %02X\n", *(p+3));

            // determina 0s a esquerda/posição do bit mais significativo
            if ((*(p + 3) & 0x40) == 0x40)
            {
                j = 1; // bit mais sig na 7a posicao
            }
            else if ((*(p + 3) & 0x20) == 0x20)
            {
                j = 2; // bit mais sig na 6a posicao
            }
            else if ((*(p + 3) & 0x10) == 0x10)
            {
                j = 3; // bit mais sig na 5a posicao
            }
            else if ((*(p + 3) & 0x08) == 0x08)
            {
                j = 4; // bit mais sig na 4a posicao
            }
            else if ((*(p + 3) & 0x04) == 0x04)
            {
                j = 5; // bit mais sig na 3a posicao
            }
            else if ((*(p + 3) & 0x02) == 0x02)
            {
                j = 6; // bit mais sig na 2a posicao
            }
            else if ((*(p + 3) & 0x01) == 0x01)
            {
                j = 7; // bit mais sig na 1a posicao, e é 1
            }
            else
                j = 8; // número é 0x00

            //printf("saiu.\n");
            //printf("j saida = %d\n", j);
            qtdBitSig = 8 - j;
            //printf("qtdBitSig = %d\n", qtdBitSig);

            qtdZeros = 7 - qtdBitSig;
            //printf("qtdZeros = %d\n", qtdZeros);
            switch (qtdZeros)
            {
            case 7:
                varUTF8 = 0x00;
                break;

            case 6:
                varUTF8 = 0x00;                             // molde"+0's na frente"   	          0(0)(0)(0)(0)(0)(0)x
                varUTF8 = varUTF8 | (bufferRead[i] & 0x01); // anterior mais 4o byte preenchido
                break;

            case 5:
                varUTF8 = 0x00;                 // molde"+0's na frente"   	          0(0)(0)(0)(0)(0)xx
                varUTF8 = bufferRead[i] & 0x03; // anterior mais byte preenchido
                break;

            case 4:
                varUTF8 = 0x00;                 // molde"+0's na frente"   	          0(0)(0)(0)(0)xxx
                varUTF8 = bufferRead[i] & 0x07; // anterior mais byte preenchido
                break;

            case 3:
                varUTF8 = 0x00;                 // molde"+0's na frente"   	          0(0)(0)(0)xxxx
                varUTF8 = bufferRead[i] & 0x0F; // anterior mais byte preenchido

            case 2:
                varUTF8 = 0x00;                 // molde"+0's na frente"   	          0(0)(0)xxxxx
                varUTF8 = bufferRead[i] & 0x1F; // anterior mais byte preenchido
                break;

            case 1:
                varUTF8 = 0x00;                 // molde"+0's na frente"   	          0(0)xxxxx
                varUTF8 = bufferRead[i] & 0x3F; // anterior mais byte preenchido
                break;

            case 0:
                varUTF8 = 0x00;                 // molde"+0's na frente"   	          0xxxxxxx
                varUTF8 = bufferRead[i] & 0x7F; // anterior mais byte preenchido
                break;

            default:
            {
                fputs("erro na leitura do valor UTF-32 na seção 1.\n", stderr);
                return -1;
            }
            }

            printf("O Valor UTF-32 0x%02X convertido para UTF-8 e: 0x%02X\n", bufferRead[i], varUTF8);
            //varutf8"U+"+varUTF8;
            if (fwrite(&varUTF8, 1, 1, arq_saida))
                printf("Caractere impresso no arquivo com sucesso!\n");
            else
                fputs("erro de gravação do arquivo.", stderr);
        }

        if (bufferRead[i] >= 0x0080 && bufferRead[i] <= 0x07FF)
        {
            // 00000 00000 00000 00000 0000 0|000 1|000 0000
            // 00000 00000 00000 00000,0000 0|111 1|111 1111

            //printf("bufferRead[i] eh: %02X\n", bufferRead[i]);
            //printf("*p eh: %02X\n", *p+2);
            if ((*(p + 2) & 0x04) == 0x04)
            {
                j = 5; // bit mais sig na 4a posicao
            }
            else if ((*(p + 2) & 0x02) == 0x02)
            {
                j = 6; // bit mais sig na 3a posicao
            }
            else if ((*(p + 2) & 0x01) == 0x01)
            {
                j = 7; // bit mais sig na 2a posicao
            }
            else
                j = 8; // bit mais sig na 1a posicao

            //printf("saiu.\n");
            //printf("j saida = %d\n", j);
            qtdBitSig = 16 - j; // 8 a 11, j de 8 a 5
                                //printf("qtdBitSig = %d\n", qtdBitSig);

            qtdZeros = 11 - qtdBitSig;
            //printf("qtdZeros = %d\n", qtdZeros);
            switch (qtdZeros)
            {
            case 3:
                varUTF8 = 0xC080 & 0xE3FF;                     // molde+0's na frente                 110(0)(0)(0)xx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 6) << 8; // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);    // anterior mais 4o byte preenchido

                break;

            case 2:
                varUTF8 = 0xC080 & 0xE7FF;                     // molde+0's na frente                 110(0)(0)xxx   10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 6) << 8; // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);    // anterior mais 4o byte preenchido
                break;

            case 1:
                varUTF8 = 0xC080 & 0xEFFF;                     // molde+0's na frente                   110(0)xxxx   10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 6) << 8; // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);    // anterior mais 4o byte preenchido
                break;

            case 0:
                varUTF8 = 0xC080 & 0xEFFF;                     // molde+0's na frente                    110xxxxx   10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 6) << 8; // anterior mais 1o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);    // anterior mais 4o byte preenchido
                break;

            default:
            {
                fputs("erro na leitura do valor UTF-32 na seção 2.\n", stderr);
                return -1;
            }
            }
            //varUTF8 = "U+"+varUTF8;
            printf("O Valor UTF-32 0x%04X convertido para UTF-8 e: 0x%04X\n", bufferRead[i], varUTF8);
            varUTF8 = inverte32(varUTF8);
            varUTF8 = varUTF8 >> 16;
            if (fwrite(&varUTF8, 2, 1, arq_saida))
                printf("Caractere impresso no arquivo com sucesso!\n");
            else
                fputs("erro de gravação do arquivo.", stderr);
        }

        if (bufferRead[i] >= 0x0800 && bufferRead[i] <= 0xFFFF && bufferRead[i] != 0xFEFF)
        {
            // sig 12 a 16 0000 0000 0000 0000 |0000 1|000 0000 0000
            //             0000 0000 0000 0000 |1000 0|000 0000 0000

            //printf("bufferRead[i] eh: %02X\n", bufferRead[i]);
            //printf("*p eh: %02X\n", *(p+2));
            if (((*p + 2) & 0x08) == 0x08)
            {
                j = 6; // bit mais sig na 5a posicao
            }
            else if (((*p + 2) & 0x04) == 0x04)
            {
                j = 7; // bit mais sig na 4a posicao
            }
            else if (((*p + 2) & 0x02) == 0x02)
            {
                j = 8; // bit mais sig na 3a posicao
            }
            else if (((*p + 2) & 0x01) == 0x01)
            {
                j = 9; // bit mais sig na 2a posicao
            }
            else if (((*p + 2) & 0x10) == 0x10)
            {
                j = 10; // bit mais sig na 1a posicao
            }

            //printf("saiu.\n");
            //printf("j saida = %d\n", j);
            qtdBitSig = 24 - j;
            //printf("qtdBitSig = %d\n", qtdBitSig);

            qtdZeros = 16 - qtdBitSig;
            //printf("qtdZeros = %d\n", qtdZeros);
            switch (qtdZeros)
            {
            case 4:
                varUTF8 = 0xE08080;
                varUTF8 = varUTF8 | (bufferRead[i] >> 6) << 8; // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);    // anterior mais 4o byte preenchido

                break;

            case 3:
                varUTF8 = 0xE08080;
                varUTF8 = varUTF8 | (bufferRead[i] >> 12) << 16;        // anterior mais 2o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8; // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);             // anterior mais 4o byte preenchido
                break;

            case 2:
                varUTF8 = 0xE08080;
                varUTF8 = varUTF8 | (bufferRead[i] >> 12) << 16;        // anterior mais 2o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8; // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);             // anterior mais 4o byte preenchido
                break;

            case 1:
                varUTF8 = 0xE08080;                                       //& 0xF0FFFF
                varUTF8 = varUTF8 | ((bufferRead[i] >> 12) & 0x07) << 16; // anterior mais 2o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;   // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);               // anterior mais 4o byte preenchido
                break;

            case 0:
                varUTF8 = 0xE08080;
                varUTF8 = varUTF8 | (bufferRead[i] >> 12) << 16;        // anterior mais 2o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8; // anterior mais 3o byte preenchido

                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F); // anterior mais 4o byte preenchido
                break;

            default:
            {
                fputs("erro na leitura do valor UTF-32 na seção 3.\n", stderr);
                return -1;
            }
            }

            printf("O Valor UTF-32 0x%06X convertido para UTF-8 e: 0x%06X\n", bufferRead[i], varUTF8);
            varUTF8 = inverte32(varUTF8);
            varUTF8 = varUTF8 >> 8;

            if (fwrite(&varUTF8, 3, 1, arq_saida))
                printf("Caractere impresso no arquivo com sucesso!\n");
            else
                fputs("erro de gravação do arquivo.", stderr);
        }

        if (bufferRead[i] >= 0x10000 && bufferRead[i] <= 0x10FFFF)
        {

            //printf("bufferRead[%d] eh: %02X\n", i, bufferRead[i]);
            //printf("*p eh: %02X\n", *p+1);
            if (((*p + 1) & 0x10) == 0x10)
            {
                j = 3; // bit mais sig na 5a posicao
            }
            else if (((*p + 1) & 0x08) == 0x08)
            {
                j = 4; // bit mais sig na 4a posicao
            }
            else if (((*p + 1) & 0x04) == 0x04)
            {
                j = 5; // bit mais sig na 3a posicao
            }
            else if (((*p + 1) & 0x02) == 0x02)
            {
                j = 6; // bit mais sig na 2a posicao
            }
            else
                j = 7; // bit mais sig na 1a posicao

            //printf("saiu.\n");
            //printf("j saida = %d\n", j);
            qtdBitSig = 24 - j;
            //printf("qtdBitSig = %d\n", qtdBitSig);

            qtdZeros = 21 - qtdBitSig;
            //printf("qtdZeros = %d\n", qtdZeros);
            switch (qtdZeros)
            {
            case 4:
                varUTF8 = 0xF0808080 & 0xF8DFFFFF;                      // molde+0s na frente 11110(000) 10(0)xxxxx 10xxxxxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 12) << 16;        // anterior mais 2o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8; // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);             // anterior mais 4o byte preenchido

                break;

            case 3:
                varUTF8 = 0xF0808080 & 0xF8FFFFFF;                      //   	 	         11110(000) 10xxxxxx 10xxxxxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 12) << 16;        // anterior mais 2o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8; // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);             // anterior mais 4o byte preenchido
                break;

            case 2:
                varUTF8 = 0xF0808080 & 0xF9FFFFFF;                        //    	                 11110(00)x 10xxxxxx 10xxxxxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 18) << 24;          // anterior mais 1o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 12) & 0x3F) << 16; // anterior mais 2o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;   // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);               // anterior mais 4o byte preenchido
                break;

            case 1:
                varUTF8 = 0xF0808080 & 0xFBFFFFFF;                        //		         11110(0)xx 10xxxxxx 10xxxxxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 18) << 24;          // anterior mais 1o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 12) & 0x3F) << 16; // anterior mais 2o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;   // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);               // anterior mais 4o byte preenchido
                break;

            case 0:
                varUTF8 = 0xF0808080;                                     //		Só molda sem inserir 0s   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                varUTF8 = varUTF8 | (bufferRead[i] >> 18) << 24;          // anterior mais 1o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 12) & 0x3F) << 16; // anterior mais 2o byte preenchido
                varUTF8 = varUTF8 | ((bufferRead[i] >> 6) & 0x3F) << 8;   // anterior mais 3o byte preenchido
                varUTF8 = varUTF8 | (bufferRead[i] & 0x3F);               // anterior mais 4o byte preenchido
                break;

            default:
            {
                fputs("erro na leitura do valor UTF-32 na seção 4.\n", stderr);
                return -1;
            }
            }

            printf("O Valor UTF-32 0x%06X convertido para UTF-8 e: 0x%06X\n", bufferRead[i], varUTF8);
            varUTF8 = inverte32(varUTF8);
            if (fwrite(&varUTF8, 4, 1, arq_saida))
                printf("Caractere impresso no arquivo com sucesso!\n");
            else
                fputs("erro de gravação do arquivo.", stderr);
        }
        i++;
    }
    printf("\nrepetições: %d\n", i);
    free(bufferRead);
    free(bufferWrite);
    return 0;
}