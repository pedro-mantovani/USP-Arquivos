#include <stdio.h>
#include "arvoreB.h"
#include "funcionalidades.h"
#include "header.h"
#include "registro.h"
#include "utilitarias.h"

/* 
A função criar_arvore
POO: COLOCAR A EXPLICAÇÃO BEM BONITA DEPOIS
*/

void criar_arvore(char* arquivo_dados, char* arquivo_arvore){
    // Abre o arquivo binário de dados para leitura
    FILE* fp_dados = fopen(arquivo_dados, "rb");
    if(fp_dados == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abre o arquivo da árvore para escrita
    FILE* fp_arv = fopen(arquivo_arvore, "wb+");
    if(fp_arv == NULL){
        printf("Falha no processamento do arquivo.\n");
        fclose(fp_dados);
        return;
    }

    // Lê o cabeçalho do arquivo de dados
    Header* header = bin_to_header(fp_dados);
    if(header == NULL || header_get_status(header) == '0'){
        printf("Falha no processamento do arquivo.\n");
        header_free(&header);
        
        return;
    }

    // Cria um cabeçalho para a árvore
    Arv_head* head_arv = criar_arv_head();
    arv_head_to_bin(fp_arv, head_arv);

    Registro* reg_temp; // Cria um registro temporário

    // Percorre o arquivo de dados, inserindo os registros na árvore
    for(int i = 0; i < header_get_proxRRN(header); i ++){
        reg_temp = bin_to_reg(fp_dados); // Lê o nó
        if(reg_temp == NULL) continue; // Ignora registros removidos
        // Insere a chave do registro na árvore B
        arv_inserir_chave(fp_arv, reg_get_codEstacao(reg_temp),RRN_to_offset(i));
        reg_free(&reg_temp); // Libera a memória do registro
    }

    // Atualiza o cabeçalho
    // POO: MUDAR ISSO PELO AMOR DE DEUS
    arv_head_free(&head_arv);
    head_arv = bin_to_arv_head(fp_arv);
    arv_head_set_status(head_arv, '1'); 
    arv_head_to_bin(fp_arv, head_arv);
    arv_head_free(&head_arv);
    header_free(&header);

    // Fecha os arquivos
    fclose(fp_dados);
    fclose(fp_arv);

    // Utiliza a funcionalidade binário na tela
    BinarioNaTela(arquivo_arvore);
}