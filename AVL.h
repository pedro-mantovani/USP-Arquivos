#ifndef AVL_H
    #define AVL_H

    #include <stdbool.h>

    typedef struct arv_avl AVL;

    // Cria a árvore vazia
    AVL* AVL_criar(void);

    // Apaga a árvore e todas as strings dela
    void AVL_apagar(AVL** arvore);

    // Insere uma string na árvore
    bool AVL_inserir(AVL* arvore, char* chave);

    // Remove uma string da árvore
    bool AVL_remover(AVL* arvore, char* chave);

    // Busca por string
    bool AVL_buscar(AVL* arvore, char* chave);

    // Imprime todas as strings da árvore em ordem alfabética
    void AVL_imprimir_em_ordem(AVL* arvore);

    // Retorna a quantidade total de strings
    int AVL_tamanho(AVL* arvore);

    // Verifica se está vazia
    bool AVL_vazia(AVL* arvore);

    int AVL_quantidade_ocorencias(AVL* arvore, char* chave);
#endif