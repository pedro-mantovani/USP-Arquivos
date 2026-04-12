#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "AVL.h"
#include "registro.h"
#include "header.h"
#include "busca.h"
#include "utilitarias.h"

/*
Funções fornecidas
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
    } else if (R != EOF) { 
        //em vez de str[0] = R e scanf devolve o caractere para o buffer e le a palavra inteira
        ungetc(R, stdin); 
        scanf("%s", str);
    } else { // EOF
        strcpy(str, "");
    }
}

// Função para converter uma string para número
int convert_num(char* str_num){
    if(str_num == NULL || *str_num == '\0')
        return -1;
    else
        return atoi(str_num);
}

// Função calcular o byte offset correspondente a determinado RRN
long int RRN_to_offset(int RRN){
    return RRN*reg_tam + header_tam;
}

// Cria uma string que representa um par de estação
void criar_par(Registro* reg, char* pair){
    // Verifica se a próxima estação é válida
    if(reg_get_codProxEstacao(reg) == -1){
        pair[0] = '\0';
        return;
    }

    // Cria uma string do tipo "a,b", com a < b
    if (reg_get_codEstacao(reg) < reg_get_codProxEstacao(reg))
        snprintf(pair, 20, "%d,%d", reg_get_codEstacao(reg), reg_get_codProxEstacao(reg));
    else
        snprintf(pair, 20, "%d,%d", reg_get_codProxEstacao(reg), reg_get_codEstacao(reg));
    return;
}

//Função para verificar o status do arquivo antes de realizar buscas ou leituras
int verificarStatusArquivo(FILE* fp) {

    if (fp == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    char status;
    fread(&status, sizeof(char), 1, fp);
    if (status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return 0;
    }

    return 1;
}

// Função para ler um valor inteiro
int scan_int(){
    char valor[50];
    int valor_inteiro;
    ScanQuoteString(valor);
    if(strcmp(valor, "") == 0)
        valor_inteiro = -1;
    else 
        valor_inteiro = atoi(valor);
    return valor_inteiro;
}