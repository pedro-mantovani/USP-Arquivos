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

// Função auxiliar da remoção
NO* AVL_remover_aux(NO *raiz, char* chave, bool remover){

    // Não encontrado
    if(raiz == NULL)
        return NULL;
    
    // Encontrado
    if(strcmp(chave, raiz->chave) == 0){

        // Um ou nenhum filho
        if(raiz->esq == NULL || raiz->dir == NULL){
            NO* aux = raiz;
            NO* substituto = (raiz->esq != NULL) ? raiz->esq : raiz->dir;
            // substitui raiz pelo seu único filho (ou NULL se folha)
            raiz = substituto;

            if(remover) // Apaga o registro
                free(aux->chave);
            
            // Libera o nó
            free(aux);
            aux = NULL;
        }

        // Ambos os filhos: troca pelo máximo da esquerda
        else {
            NO* aux = raiz->esq;
            // Encontra maior valor da sub-arvore esquerda
            while(aux->dir != NULL)
                aux = aux->dir;

            // Removendo o registro
            free(raiz->chave);

            // Troca raiz e aux
            raiz->chave = aux->chave;

            // Remove o nó auxiliar
            // Note que o item não é apagado pois isso o apagaria da raíz também (são os mesmos ponteiros)
            raiz->esq = AVL_remover_aux(raiz->esq, raiz->chave, false);
        }
    }

    // Caso não tenha encontrado continua procurando
    else if(strcmp(chave, raiz->chave) < 0)
        raiz->esq = AVL_remover_aux(raiz->esq, chave, remover);
    else
        raiz->dir = AVL_remover_aux(raiz->dir, chave, remover);

    if (raiz != NULL){
        // Arruma a altura
        raiz->altura = max(AVL_altura_no(raiz->esq), AVL_altura_no(raiz->dir)) + 1;

        // Arruma o balanceamento
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

// Função principal da remoção
bool AVL_remover(AVL* arv, char* chave){
    // verifica se a árvore e a chave existem antes de tentar remover
    if(AVL_buscar(arv, chave) == 0)
        return false;
    
    arv->raiz = AVL_remover_aux(arv->raiz, chave, true);
    arv->tamanho--;
    return true;
}