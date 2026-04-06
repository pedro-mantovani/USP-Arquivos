#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "AVL.h"

#define max(a, b) (( (a) > (b) ) ? (a) : (b))
#define ERRO -1

typedef struct no_{
    char* chave;
    struct no_* esq;
    struct no_* dir;
    int altura;
    int quantidade;
} NO;

struct arv_avl{
    NO* raiz;
    int tamanho;
};

// Função para criar e inicializar uma árvore
AVL* AVL_criar(){
    AVL* avl = (AVL*)(malloc(sizeof(AVL)));
    if(avl != NULL){
        avl->raiz = NULL;
        avl->tamanho = 0;
    }
    return avl;
}

// Função auxiliar para apagar recursivamente uma árvore
void AVL_apagar_aux(NO *no){
    if(no != NULL){
        AVL_apagar_aux(no->esq);
        AVL_apagar_aux(no->dir);
        free(no->chave);
        free(no);
    }
}

// Função principal para apagar uma árvore
void AVL_apagar(AVL **arv){
    if(arv == NULL || *arv == NULL) return;
    AVL_apagar_aux((*arv)->raiz);
    free(*arv);
    *arv = NULL;
}

// Função que retorna o tamanho de uma árvore
int AVL_tamanho(AVL* arvore){
    if(arvore != NULL)
        return arvore->tamanho;
    else return ERRO;
}

// Função para verificar se uma árvore estpa vazia
bool AVL_vazia(AVL* arvore){
    if(!arvore) return true;
    else return (arvore->tamanho == 0);
}

// Função para acessar a altura de um nó
int AVL_altura_no(NO* no){
    if(no == NULL)
        return -1; // convenção: altura de nó nulo = -1
    return no->altura;
}

// Função auxiliar para imprimir uma árvore em ordem crescente
void AVL_imprimir_nos_em_ordem(NO* raiz){
    if(raiz != NULL){
        AVL_imprimir_nos_em_ordem(raiz->esq);
        printf("%s: %d ocorrencia(s)\n", raiz->chave, raiz->quantidade);
        AVL_imprimir_nos_em_ordem(raiz->dir);
    }
}

// Função principal para imprimir uma árvore
void AVL_imprimir_em_ordem(AVL* arvore){
    if(arvore != NULL)
        AVL_imprimir_nos_em_ordem(arvore->raiz);
}

// Rotações
NO* rotacao_direita(NO* A){
    if(A == NULL || A->esq == NULL) return A;
    NO* B = A->esq;
    A->esq = B->dir;
    B->dir = A;

    // atualiza alturas
    A->altura = max(AVL_altura_no(A->esq), AVL_altura_no(A->dir)) + 1;
    B->altura = max(AVL_altura_no(B->esq), AVL_altura_no(B->dir)) + 1;
    return B; // nova raiz da subárvore
}

NO* rotacao_esquerda(NO* A){
    if(A == NULL || A->dir == NULL) return A;
    NO* B = A->dir;
    A->dir = B->esq;
    B->esq = A;

    // atualiza alturas
    A->altura = max(AVL_altura_no(A->esq), AVL_altura_no(A->dir)) + 1;
    B->altura = max(AVL_altura_no(B->esq), AVL_altura_no(B->dir)) + 1;
    return B; // nova raiz da subárvore
}

// Rotações duplas
NO* rotacao_esquerda_direita(NO* A){
    if(A == NULL) return A;
    A->esq = rotacao_esquerda(A->esq);
    return rotacao_direita(A);
}

NO* rotacao_direita_esquerda(NO* A){
    if(A == NULL) return A;
    A->dir = rotacao_direita(A->dir);
    return rotacao_esquerda(A);
}

// Função para criar um nó
NO* criar_no(char* chave){
    NO* no = (NO*)malloc(sizeof(NO));
    if(no != NULL){
        no->altura = 0;
        no->quantidade = 1;
        no->dir = NULL;
        no->esq = NULL;
        char* chave_copia = malloc(sizeof(char)*(strlen(chave)+1));
        no->chave = strcpy(chave_copia, chave);
    }
    return no;
}

// Função auxiliar para inserir um nó na árvore
NO *AVL_inserir_no(NO *raiz, char* chave, int* add){
    // Encontrou a posição
    if(raiz == NULL){
        raiz = criar_no(chave);
        if(raiz)
            *add = 1;
    }
    // Procura na direita se a chave é maior que a raiz e na esquerda caso contrário
    else if(strcmp(chave, raiz->chave) > 0)
        raiz->dir = AVL_inserir_no(raiz->dir, chave, add);
    else if(strcmp(chave, raiz->chave) < 0)
        raiz->esq = AVL_inserir_no(raiz->esq, chave, add);
    else
        raiz->quantidade ++;
    
    if(raiz != NULL){
        // Corrige a altura
        raiz->altura = max(AVL_altura_no(raiz->esq), AVL_altura_no(raiz->dir)) + 1;

        // Corrige o balanceamento
        int fb = AVL_altura_no(raiz->esq) - AVL_altura_no(raiz->dir);
        if(fb == -2){
            int fb_dir = AVL_altura_no(raiz->dir->esq) - AVL_altura_no(raiz->dir->dir);
            if(fb_dir <= 0)
                raiz = rotacao_esquerda(raiz);
            else
                raiz = rotacao_direita_esquerda(raiz);
        }
        else if(fb == 2){
            int fb_esq = AVL_altura_no(raiz->esq->esq) - AVL_altura_no(raiz->esq->dir);
            if(fb_esq >= 0)
                raiz = rotacao_direita(raiz);
            else
                raiz = rotacao_esquerda_direita(raiz);
        }
    }

    return raiz;
}

// Função principal para inserir um registro
bool AVL_inserir(AVL *arv, char* chave){
    if(arv == NULL || chave == NULL) return false;
    int add = 0;
    arv->raiz = AVL_inserir_no(arv->raiz, chave, &add);
    if(add)
        arv->tamanho ++;
    return add;
}

// Função auxiliar para realizar uma busca
bool AVL_busca_no(NO* raiz, char* chave){
    // Não encontrou
    if(raiz == NULL)
        return 0;
    
    // Encontrou
    if(strcmp(chave, raiz->chave) == 0)
        return 1;
    
    // Procura na direita se a chave é maior que o nó e na esquerda caso contrário
    if(strcmp(chave, raiz->chave) > 0)
        return AVL_busca_no(raiz->dir, chave);
    else
        return AVL_busca_no(raiz->esq, chave);
}

// Função principal para realizar uma busca
bool AVL_buscar(AVL* arv, char* chave){
    if(arv == NULL) return 0;
    return AVL_busca_no(arv->raiz, chave);
}

/*
Raciocínio da remoção:
- 0 filhos: libera nó
- 1 filho: substitui pelo filho
- 2 filhos: troca com o máximo da subárvore esquerda
*/


NO* AVL_remover_aux(NO *raiz, char* chave, bool remover_registro, bool* removeu_no) {
    if (raiz == NULL)
        return NULL;

    int cmp = strcmp(chave, raiz->chave);

    if (cmp < 0) {
        raiz->esq = AVL_remover_aux(raiz->esq, chave, remover_registro, removeu_no);
    }
    else if (cmp > 0) {
        raiz->dir = AVL_remover_aux(raiz->dir, chave, remover_registro, removeu_no);
    }
    else {
        /* Encontrou a chave */

        /* Se for remoção real de registro e este nó representa mais de uma ocorrência,
           apenas decrementa a quantidade e NÃO remove o nó da árvore. */
        if (remover_registro && raiz->quantidade > 1) {
            raiz->quantidade--;
            return raiz;
        }

        /* Caso com 0 ou 1 filho */
        if (raiz->esq == NULL || raiz->dir == NULL) {
            NO* filho = (raiz->esq != NULL) ? raiz->esq : raiz->dir;
            NO* aux = raiz;

            /* Só libera a chave se for remoção real do registro.
               Na remoção interna de reorganização (remover_registro == false),
               a chave pode ter sido transferida para outro nó. */
            if (remover_registro) {
                free(aux->chave);
            }

            free(aux);
            *removeu_no = true;
            return filho;
        }

        /* Caso com 2 filhos:
           troca com o predecessor (maior da subárvore esquerda) */
        NO* pred = raiz->esq;
        while (pred->dir != NULL)
            pred = pred->dir;

        /* A chave atual será substituída */
        free(raiz->chave);

        /* Transfere os dados do predecessor para a raiz */
        raiz->chave = pred->chave;
        raiz->quantidade = pred->quantidade;

        /* Remove fisicamente o predecessor da subárvore esquerda.
           Note que remover_registro = false para NÃO decrementar quantidade
           e NÃO liberar pred->chave, pois ela agora pertence à raiz. */
        raiz->esq = AVL_remover_aux(raiz->esq, pred->chave, false, removeu_no);
    }

    /* Rebalanceamento */
    if (raiz != NULL) {
        raiz->altura = max(AVL_altura_no(raiz->esq), AVL_altura_no(raiz->dir)) + 1;

        int fb = AVL_altura_no(raiz->esq) - AVL_altura_no(raiz->dir);

        if (fb == -2) {
            int fb_dir = AVL_altura_no(raiz->dir->esq) - AVL_altura_no(raiz->dir->dir);
            if (fb_dir <= 0)
                raiz = rotacao_esquerda(raiz);
            else
                raiz = rotacao_direita_esquerda(raiz);
        }
        else if (fb == 2) {
            int fb_esq = AVL_altura_no(raiz->esq->esq) - AVL_altura_no(raiz->esq->dir);
            if (fb_esq >= 0)
                raiz = rotacao_direita(raiz);
            else
                raiz = rotacao_esquerda_direita(raiz);
        }
    }

    return raiz;
}

/* Função principal */
bool AVL_remover(AVL* arv, char* chave) {
    if (arv == NULL || chave == NULL)
        return false;

    if (AVL_buscar(arv, chave) == 0)
        return false;

    bool removeu_no = false;
    arv->raiz = AVL_remover_aux(arv->raiz, chave, true, &removeu_no);

    /* tamanho = número de nós */
    if (removeu_no)
        arv->tamanho--;

    return true;
}

// Retorna a quantidade de ocorrências registradas para uma chave
int AVL_obter_quantidade(NO* raiz, char* chave) {
    if (raiz == NULL) return 0;
    int comp = strcmp(chave, raiz->chave);
    if (comp == 0) return raiz->quantidade;
    return (comp > 0) ? AVL_obter_quantidade(raiz->dir, chave) : AVL_obter_quantidade(raiz->esq, chave);
}   

// Função auxiliar para chamar a partir da struct AVL
int AVL_quantidade_ocorencias(AVL* arv, char* chave) {
    if (arv == NULL) return 0;
    return AVL_obter_quantidade(arv->raiz, chave);
}

