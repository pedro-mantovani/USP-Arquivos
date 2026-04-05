#ifndef BUSCA_H
#define BUSCA_H

#include "registro.h"

// Estrutura dinâmica para armazenar os critérios de uma busca
typedef struct {
    int m;               //Quantidade de campos na busca atual
    char **nomes;        //Vetor de strings para os nomes dos campos
    char **valoresStr;   //Vetor de strings para os valores (ou "" para NULO)
    int *valoresInt;     //Vetor de inteiros para os valores convertidos
} Busca;

Busca* criar_busca(int m);
void apagar_busca(Busca **b);

void preencher_filtros(Busca *b);
int registro_passa_filtrob(Registro *reg, Busca *b);

#endif