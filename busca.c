#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "busca.h"
#include "IO.h" 

//Aloca a struct e os vetores internos com base no m fornecido
Busca* criar_busca(int m) {
    Busca *b = (Busca*) malloc(sizeof(Busca));
    if (!b) return NULL;

    b->m = m;
    b->nomes = (char**) malloc(m * sizeof(char*));
    b->valoresStr = (char**) malloc(m * sizeof(char*));
    b->valoresInt = (int*) malloc(m * sizeof(int));

    for (int i = 0; i < m; i++) {
        b->nomes[i] = (char*) malloc(50 * sizeof(char));
        b->valoresStr[i] = (char*) malloc(100 * sizeof(char));
    }
    return b;
}

//Libera toda a memória alocada dinamicamente
void apagar_busca(Busca **b) {
    if (!b || !*b) return;
    for (int i = 0; i < (*b)->m; i++) {
        free((*b)->nomes[i]);
        free((*b)->valoresStr[i]);
    }
    free((*b)->nomes);
    free((*b)->valoresStr);
    free((*b)->valoresInt);
    free(*b);
    *b = NULL;
}

//Lê os m campos da entrada padrão e processa os valores
void preencher_filtros(Busca *b) {
    for (int i = 0; i < b->m; i++) {
        scanf("%s", b->nomes[i]);
        ScanQuoteString(b->valoresStr[i]); 

        if (strcmp(b->valoresStr[i], "") == 0) {
            b->valoresInt[i] = -1; 
        } else {
            b->valoresInt[i] = atoi(b->valoresStr[i]);
        }
    }
}

//Verifica se o registro atual satisfaz TODOS os critérios da busca
int registro_passa_filtrob(Registro *reg, Busca *b) {
    for (int i = 0; i < b->m; i++) {
        int match = 0;
        char *nomeCampo = b->nomes[i];

        if (strcmp(nomeCampo, "codEstacao") == 0) {
            if (reg_get_codEstacao(reg) == b->valoresInt[i]) match = 1;
        } 
        else if (strcmp(nomeCampo, "nomeEstacao") == 0) {
            char *res = reg_get_nomeEstacao(reg);
            if (res && strcmp(res, b->valoresStr[i]) == 0) match = 1;
            else if (!res && strcmp(b->valoresStr[i], "") == 0) match = 1;
        } 
        else if (strcmp(nomeCampo, "codLinha") == 0) {
            if (reg_get_codLinha(reg) == b->valoresInt[i]) match = 1;
        } 
        else if (strcmp(nomeCampo, "nomeLinha") == 0) {
            char *res = reg_get_nomeLinha(reg);
            if (res && strcmp(res, b->valoresStr[i]) == 0) match = 1;
            else if (!res && strcmp(b->valoresStr[i], "") == 0) match = 1;
        } 
        else if (strcmp(nomeCampo, "codProxEstacao") == 0) {
            if (reg_get_codProxEstacao(reg) == b->valoresInt[i]) match = 1;
        } 
        else if (strcmp(nomeCampo, "distProxEstacao") == 0) {
            if (reg_get_distProxEstacao(reg) == b->valoresInt[i]) match = 1;
        } 
        else if (strcmp(nomeCampo, "codLinhaIntegra") == 0) {
            if (reg_get_codLinhaIntegra(reg) == b->valoresInt[i]) match = 1;
        } 
        else if (strcmp(nomeCampo, "codEstIntegra") == 0) {
            if (reg_get_codEstIntegra(reg) == b->valoresInt[i]) match = 1;
        }

        if (!match) return 0; //se um critério falhar, o registro não serve
    }
    return 1;
}