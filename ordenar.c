#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcionalidades.h"
#include "header.h"
#include "registro.h"
#include "utilitarias.h"

/* 
A função ordenar_RAM lê todo arquivo de dados sequencialmente e coloca todos os registros em um vetor
Depois ordena o vetor e coloca tudo em outro arquivo
*/

// Função para comparar pelo código da estação
int comparaCodEstacao(const void *a, const void *b){
    Registro *r1 = *(Registro **)a;
    Registro *r2 = *(Registro **)b;

    int c1 = reg_get_codEstacao(r1);
    int c2 = reg_get_codEstacao(r2);

    // Campos nulo fica no fim
    if(c1 == -1 && c2 != -1) return 1;
    if(c2 == -1 && c1 != -1) return -1;

    if(c1 < c2) return -1;
    if(c1 > c2) return 1;
    return 0;
}

// Função para comparar pelo código da próxima estação
int comparaCodProxEstacao(const void *a, const void *b){
    Registro *r1 = *(Registro **)a;
    Registro *r2 = *(Registro **)b;

    int c1 = reg_get_codProxEstacao(r1);
    int c2 = reg_get_codProxEstacao(r2);

    if(c1 == -1 && c2 != -1) return 1;
    if(c2 == -1 && c1 != -1) return -1;

    if(c1 < c2) return -1;
    if(c1 > c2) return 1;
    return 0;
}

// Função pricipal
bool ordenar_arquivo(char* arquivoEntrada, char* arquivoSaida, char* campo){

    // Abre o arquivo de entrada
    FILE *fpEntrada = fopen(arquivoEntrada, "rb");
    if(!verificarStatusArquivo(fpEntrada)) return false;

    // Abre o arquivo de saída
    FILE *fpSaida = fopen(arquivoSaida, "wb");
    if(fpSaida == NULL){
        printf("Falha no processamento do arquivo.\n");
        fclose(fpEntrada);
        return false;
    }

    // Coloca o header na memória principal
    Header *header = bin_to_header(fpEntrada);

    // Marca o cabeçalho como inconsistente
    header_set_status(header,'0');
    header_to_bin(fpSaida, header);

    int proxRRN = header_get_proxRRN(header);

    Registro **vetor = malloc(sizeof(Registro*) * proxRRN);

    int qtd = 0; // Contador do número de registros

    for(int i = 0; i < proxRRN; i++){

        Registro *reg = bin_to_reg(fpEntrada); // Lê o registro atual

        if(reg == NULL) continue; // Ignora registros removidos

        // Coloca o registro no vetor
        vetor[qtd] = reg;
        qtd ++;
    }

    // Ordena o arquivo
    if(strcmp(campo, "codEstacao") == 0)
        qsort(vetor,qtd,sizeof(Registro*),comparaCodEstacao);
    else
        qsort(vetor,qtd,sizeof(Registro*),comparaCodProxEstacao);

    // Coloca os registros no arquivo de saída
    for(int i = 0; i < qtd; i++){
        reg_to_bin(vetor[i],fpSaida,-1);
        reg_free(&vetor[i]);
    }
    free(vetor);

    // Marca o header como consistente e atualiza o proxRRN
    header_set_status(header,'1');
    header_set_proxRRN(header, qtd);
    header_to_bin(fpSaida,header);

    header_free(&header);

    // Fecha os arquivos
    fclose(fpEntrada);
    fclose(fpSaida);
    return true;
}

// Função chamada pela main (lê os parâmetros e passa para a outra função)
void ordenar_funcionalidade(void){
    char entrada[100], campo[50], saida[100];

    scanf("%s", entrada);
    scanf("%s", campo);
    scanf("%s", saida);

    if(!ordenar_arquivo(entrada, saida, campo)) return;

    BinarioNaTela(saida);
}