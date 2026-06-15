#include "arvoreB.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*
Função recursiva para buscar uma chave na Árvore-B.

Recebe o ponteiro do arquivo de índice, o RRN do nó atual a ser analisado, 
e a chave procurada.

Retorna o offset correspondente à chave, ou -1 se não encontrar.
*/
int arv_busca_recursiva(FILE* fp_arvore, int rrn_atual, int chave_buscada) {
    // Condição de parada: Chegou em um nó folha e não encontrou a chave
    if (rrn_atual == -1) {
        return -1;       // Não encontrou
    }

    // Carrega o nó atual para a memória RAM
    Arv_no* no = bin_to_arv_no(fp_arvore, rrn_atual);
    if (no == NULL) {
        return -1;      // Falha na leitura ou nó inexistente
    }

    // Busca sequencial dentro do nó para encontrar a posição da chave
    int i = 0;
    while (i < no->nroChaves && chave_buscada > no->chaves[i]) {
        i++;
    }

    // Condição de parada: Encontrou a chave no nó atual
    if (i < no->nroChaves && chave_buscada == no->chaves[i]) {
        int offset_dados = no->offsets[i];      // Salva a referência do registro de dados
        arv_no_free(&no);                       // Libera a memória alocada
        return offset_dados;
    }

    // Se não encontrou, desce na árvore pelo ponteiro (filho) adequado
    int rrn_filho = no->filhos[i];
    
    // Libera o nó atual da memória RAM antes de fazer a chamada recursiva
    arv_no_free(&no);

    // Chamada recursiva para o nó filho
    return arv_busca_recursiva(fp_arvore, rrn_filho, chave_buscada);
}

/*
Função principal que inicia a busca na Árvore-B.
Abre/Lê o cabeçalho, verifica a consistência e dispara a busca recursiva a partir da Raiz.
*/
int arv_busca_chave(FILE* fp_arvore, int rrn_raiz, int chave_buscada) {
    if (fp_arvore == NULL) return -1;
    
    // Se a árvore estiver vazia
    if (rrn_raiz == -1) {
        return -1; 
    }

    // Inicia a pesquisa recursiva
    return arv_busca_recursiva(fp_arvore, rrn_raiz, chave_buscada);
}