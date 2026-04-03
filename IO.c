#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "registro.h"

/*
Funções relacionadas a entrada e saída
*/

void BinarioNaTela(char *arquivo) {
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }

    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);

    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }

    printf("%lf\n", (cs / (double)100));

    free(mb);
    fclose(fs);
}


void ScanQuoteString(char *str) {
    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n') { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();         // ignorar aspas fechando
    } else if (R != EOF) { // vc tá tentando ler uma string que não tá entre
                           // aspas! Fazer leitura normal %s então, pois deve
                           // ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else { // EOF
        strcpy(str, "");
    }
}

int convert_num(char* str_num){
    if(str_num == NULL || *str_num == '\0')
        return -1;
    else
        return atoi(str_num);
}

void reg_to_bin(Registro* reg, FILE* fp){
    char c;
    int x;
    char* s;
    int bytes_lixo = 47;
    char lixo = '$';

    if (fp == NULL || reg == NULL) return;

    c = reg_get_removido(reg);
    fwrite(&c, sizeof(char), 1, fp);

    x = reg_get_proximo(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codEstacao(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codLinha(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codProxEstacao(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_distProxEstacao(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codLinhaIntegra(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codEstIntegra(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_tamNomeEstacao(reg);
    bytes_lixo -= x;
    fwrite(&x, sizeof(int), 1, fp);

    s = reg_get_nomeEstacao(reg);
    if (x > 0 && s != NULL)
        fwrite(s, sizeof(char), x, fp);

    x = reg_get_tamNomeLinha(reg);
    bytes_lixo -= x;
    fwrite(&x, sizeof(int), 1, fp);

    s = reg_get_nomeLinha(reg);
    if (x > 0 && s != NULL)
        fwrite(s, sizeof(char), x, fp);

    fwrite(&lixo, sizeof(char), bytes_lixo, fp);
}

// Falta colocar o registro de cabeçalho
void read_csv(FILE* fp_csv, FILE* fp_bin){
    char linha[256];
    char *ptr;
    char *token;
    Registro* reg_temp = criar_registro();

    if (fp_csv == NULL) return;

    fgets(linha, sizeof(linha), fp_csv); // Queima a linha de cabeçalho
    
    while (fgets(linha, sizeof(linha), fp_csv) != NULL) {
        ptr = linha;
        reg_set_codEstacao(reg_temp, atoi(strsep(&ptr, ",")));

        token = strsep(&ptr, ",");
        reg_set_tamNomeEstacao(reg_temp, strlen(token));
        reg_set_nomeEstacao(reg_temp, token);

        reg_set_codLinha(reg_temp, convert_num(strsep(&ptr, ",")));

        token = strsep(&ptr, ",");
        reg_set_tamNomeLinha(reg_temp, strlen(token));
        reg_set_nomeLinha(reg_temp, token);
        reg_set_codProxEstacao(reg_temp, convert_num(strsep(&ptr, ",")));
        reg_set_distProxEstacao(reg_temp, convert_num(strsep(&ptr, ",")));
        reg_set_codLinhaIntegra(reg_temp, convert_num(strsep(&ptr, ",")));
        reg_set_codEstIntegra(reg_temp, convert_num(strsep(&ptr, ",")));

        reg_to_bin(reg_temp, fp_bin);
    }
    reg_free(&reg_temp);
}