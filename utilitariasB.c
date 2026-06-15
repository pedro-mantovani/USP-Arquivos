#include "arvoreB.h"
#include <stdbool.h>
#include <stdlib.h>

/*
Função para shiftar um vetor uma posição para tráz a partir do índice i

Parâmetros:
Vetor
Posição a partir da qual o vetor será deslocado
Tamanho total do vetor
Booleano indicando se é um deslocamento para frente ou para trás
Quando true desloca todos os elementos a parir da posição indicada uma posição para trás
Quando false desloca todos os elementos a partir da posição indicada uma posição para frente
*/
void shift(int vetor[], int pos, int tam, bool back){
    // Percorre da posição atual até o final do vetor 
    if(back){
        for(int i = pos; i < tam - 1; i++){
            vetor[i] = vetor[i+1];
        }    
    }else{
        for(int i = tam; i > pos; i--){
            vetor[i] = vetor[i-1];
        }
    }
    
    // Coloca o último elemento como o valor padrão
    if(back) vetor[tam-1] = -1;   
}

/*
Função para transformar uma chave pai + seus filhos em um vetor
Usado na:
Redistribuição
Concatenação
Inserção

Retorna o vetor final e altera o conteúdo da variável tamanho para o tamanho do vetor

Parâmetros:
Nó pai
Índice da chave pai no vetor pai
Nó esquerdo
Nó direito
*/
Chave* vectorize(Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir, int* tam, bool eh_folha){
    int n_esq, n_dir, total;
    n_esq = f_esq->nroChaves;
    n_dir = f_dir->nroChaves;
    total = n_dir + n_esq + 1;

    // Cria um vetor auxiliar para armazenar as chaves
    Chave* vec;
    if(eh_folha){
        vec = (Chave*)malloc(sizeof(Chave)* total);
    }else{
        // Se não for raíz tem que armazenar um espaço a mais para salvar os filhos
        vec = (Chave*)malloc(sizeof(Chave)* (1+total));
    }
    
    /*Preenche o vetor auxiliar*/
    // Coloca os elementos do nó esquerdo
    for(int i = 0; i < n_esq; i++){
        vec[i].chave = f_esq->chaves[i];
        vec[i].offset = f_esq->offsets[i];
        if(!eh_folha){ // Se não é raíz salva os filhos
            vec[i].filho = f_esq->filhos[i];
        }
    }
    if(!eh_folha){ // Se não é raíz salva o filho mais a direita do nó à esquerda
        vec[n_esq].filho = f_esq->filhos[n_esq];
    }

    // Coloca o elemento do nó pai
    vec[n_esq].chave = pai->chaves[i_pai];
    vec[n_esq].offset = pai->offsets[i_pai];
    
    // Coloca os elementos do nó direito
    for(int i = n_esq+1; i < total; i++){
        vec[i].chave = f_dir->chaves[i - n_esq -1];
        vec[i].offset = f_dir->offsets[i - n_esq -1];
        if(!eh_folha){ // Se não é raíz salva os filhos
            vec[i].filho = f_dir->filhos[i - n_esq - 1];
        }
    }
    if(!eh_folha){ // Se não é raíz salva o filho mais a direita do nó à direita
        vec[total].filho = f_dir->filhos[n_dir];
    }

    *tam = total;

    return vec;
}
