#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "arvoreB.h"
#include "header.h"
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

    // Ignorar espaços em branco, \r, \n, etc.
    while ((R = getchar()) != EOF && isspace(R))
        ; 

    // Se for uma string entre aspas
    if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { // Ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar(); // Ignorar aspas fechando
        
    } 
    
    // Se for uma palavra normal sem aspas
    else if (R != EOF) { 
        ungetc(R, stdin); // Devolve a primeira letra pro buffer
        scanf("%s", str); // Lê a palavra inteira
        
        // Verifica se a palavra inteira era "NULO"
        if (strcmp(str, "NULO") == 0 || strcmp(str, "nulo") == 0) {
            strcpy(str, ""); // Limpa a string
        }  
    } 
    // Se chegou no fim do arquivo
    else { 
        strcpy(str, "");
    }
}

// ----------------------------------------------

// Função para converter uma string para número
int convert_num(char* str_num){
    if(str_num == NULL || *str_num == '\0')
        return -1;
    else
        return atoi(str_num);
}

// Função calcular o byte offset correspondente a determinado RRN
long int RRN_to_offset(int RRN){
    return RRN*tam_reg + tam_header;
}

// Função calcular o RRN correspondente a determinado offset
int offset_to_RRN(int offset){
    return (offset-tam_header)/tam_reg;
}


// Função calcular o byte offset correspondente a determinado RRN
long int arv_RRN_to_offset(int RRN){
    return (RRN*tam_arv_no + tam_arv_head);
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

/*
Função padrão de busca binária

Parâmetros:
Vetor
Tamanho
Valor buscado

Retorno:
Índice do vetor que contém o valor buscado
-1 caso não encontre nada
*/
int busca_binaria(int vetor[], int tamanho, int alvo) {
    int inicio = 0;
    int fim = tamanho - 1;
    
    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        
        // Verifica se o alvo está no meio
        if (vetor[meio] == alvo)
            return meio;
        
        // Se o alvo for maior, ignora a metade esquerda
        if (vetor[meio] < alvo)
            inicio = meio + 1;
        
        // Se o alvo for menor, ignora a metade direita
        else
            fim = meio - 1;
    }
    
    // Alvo não encontrado
    return -1;
}

/*
Função para shiftar um vetor uma posição para tráz a partir do índice i

Parâmetros:
Vetor
Posição a partir da qual o vetor será deslocado
Tamanho total do vetor
*/
void shift_back(int vetor[], int pos, int tam){
    // Percorre da posição atual até o final do vetor 
    for(int i = pos; i < tam - 1; i++){
        int aux = vetor[i+1];
        vetor[i] = aux;
    }
    // Coloca o último elemento como o valor padrão
    vetor[tam-1] = -1;   
}