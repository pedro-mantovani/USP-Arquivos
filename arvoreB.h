#ifndef ARVOREB_H
    #define ARVOREB_H

    #include <stdio.h>
    #include <math.h>

    // Define o número de filhos (m) de cada nó da árvore
    #define nro_filhos 4

    // Define o tamanho do cabeçalho
    #define tam_arv_head 17

    // Define tamanhos básicos com base no número de filhos
    #define nro_chaves nro_filhos-1
    #define tam_arv_no 17+12*nro_chaves
    #define min_chaves (int)ceil(nro_filhos/2.0) - 1

    // Define as estruturas
    typedef struct arv_no Arv_no;
    typedef struct arv_head Arv_head;

    // Funções para criar structs
    Arv_head* criar_arv_head();
    Arv_no* criar_arv_no();

    // Funções para resgatar informações de um arquivo de árvore binária
    Arv_head* bin_to_arv_head(FILE* fp);
    Arv_no* bin_to_arv_no(FILE* fp, int RRN);

    // Funções para colocar structs em um arquivo de árvore binária
    void arv_head_to_bin(FILE* fp, Arv_head* head);
    void arv_no_to_bin(FILE* fp, Arv_no* no, long int offset);

    // Funções de get
    int arv_head_get_nroNos(const Arv_head* h);
    
    // Funções de liberar memória
    void arv_no_free(Arv_no** no);
    void arv_head_free(Arv_head** head);
#endif