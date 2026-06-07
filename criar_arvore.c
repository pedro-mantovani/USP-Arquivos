#include <stdio.h>
#include "arvoreB.h"
#include "funcionalidades.h"
#include "utilitarias.h"

/* 
A função criar_arvore
POO: COLOCAR A EXPLICAÇÃO BEM BONITA DEPOIS
*/

void criar_arvore(char* arquivo_dados, char* arquivo_arvore){
    // Abre o arquivo binário de dados para leitura
    FILE* fp = fopen(arquivo_dados, "rb");

    // Abre o arquivo da árvore para escrita
    FILE* fp_a = fopen(arquivo_arvore, "wb");
    
    // Verifica status de consistência no cabeçalho (byte 0)
    if(!verificarStatusArquivo(fp)) return;

    Arv_head* head = bin_to_arv_head(fp);
    arv_head_to_bin(fp_a, head);

    Arv_no* no; // Cria um nó temporário

    for(int i = 0; i < arv_head_get_nroNos(head); i ++){
        no = bin_to_arv_no(fp, -1); // Lê o nó
        arv_no_to_bin(fp_a, no,-1);

        arv_no_free(&no); // Libera a memória do registro
    }

    fclose(fp);
    fclose(fp_a);
}