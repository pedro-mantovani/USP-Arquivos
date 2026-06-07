#ifndef ARVOREB_H
    #define ARVOREB_H

    #include <stdio.h>
    #include <stdbool.h>
    #include <math.h>

    // Define ordem (m) da árvore
    #define ordem 4

    // Define o tamanho do cabeçalho
    #define tam_arv_head 17

    // Define tamanhos básicos com base no número de filhos
    #define nro_chaves (ordem-1)
    #define tam_arv_no (17+12*nro_chaves)
    #define min_chaves ((int)ceil(ordem/2.0) - 1)

    // Define as estruturas
    typedef struct arv_no Arv_no;
    typedef struct arv_head Arv_head;

    // Funções para criar structs
    Arv_head* criar_arv_head();
    Arv_no* criar_arv_no();

    // Funções para resgatar informações de um arquivo de árvore binária
    Arv_head* bin_to_arv_head(FILE* fp);
    Arv_no* bin_to_arv_no(FILE* fp, int RRN);

    // Funções de manipulação da árvore
    void remover_arv(FILE* fp_arvore, int chave);

    // Funções para colocar structs em um arquivo de árvore binária
    void arv_head_to_bin(FILE* fp, Arv_head* head);
    void arv_no_to_bin(FILE* fp, Arv_no* no, int RRN);

    // Funções de get
    int arv_head_get_nroNos(const Arv_head* h);
    
    // Funções de liberar memória
    void arv_no_free(Arv_no** no);
    void arv_head_free(Arv_head** head);
#endif