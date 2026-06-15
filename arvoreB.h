/*
O arquivo arvoreB.h contém todas as funções públicas de manipulação na árvore B. 
Para facilitar a correção, a implementação foi separada em 4 arquivos:

- arvoreB.c: Funções de criação da árvore B e transição do arquivo binário para ram e vice versa
- bucarB.c:  Funções necessárias para a busca
- inserirB.c: Funções necessárias para a inserção
- removerB.c: Funções necessárias para a remoção

*/

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

    /*
    Sctructs para o cabeçalho, nós da árvore e conjunto que acompanha uma chave da árvore
    */

    struct arv_head{
        char status;
        int noRaiz;
        int topo;
        int proxRRN;
        int nroNos;
    };

    struct arv_no{
        char removido;
        int proximo;
        int tipoNo;
        int nroChaves;
        
        int* chaves;
        int* offsets;
        int* filhos;
    };

    typedef struct chave_{
        int chave;
        int offset;
        int filho_dir;
    } Chave;


    // Define as estruturas
    typedef struct arv_no Arv_no;
    typedef struct arv_head Arv_head;

    // Funções para criar structs
    Arv_head* criar_arv_head();
    Arv_no* criar_arv_no();

    // Funções para resgatar informações de um arquivo de árvore binária
    Arv_head* bin_to_arv_head(FILE* fp);
    Arv_no* bin_to_arv_no(FILE* fp, int RRN);

    // Funções de remoção da árvore
    int remover_arv(FILE* fp_arvore, Arv_head* head, int chave);

    // Funções para colocar structs em um arquivo de árvore binária
    void arv_head_to_bin(FILE* fp, Arv_head* head);
    void arv_no_to_bin(FILE* fp, Arv_no* no, int RRN);

    // Funções de get e set
    int arv_head_get_nroNos(const Arv_head* h);
    char arv_head_get_status(const Arv_head* h);
    void arv_head_set_status(Arv_head* h, char status);
    
    // Função de busca
    int arv_busca_chave(FILE* fp_arvore, int rrn_raiz, int chave_buscada);

    // Função de inserção
    bool arv_inserir_chave(FILE* fp_arvore, Arv_head* head, int chave, int offset_dados);
    
    // Funções de liberar memória
    void arv_no_free(Arv_no** no);
    void arv_head_free(Arv_head** head);

    // Funções utilitárias
    Chave* vectorize(Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir, int* tam, bool eh_folha);

#endif