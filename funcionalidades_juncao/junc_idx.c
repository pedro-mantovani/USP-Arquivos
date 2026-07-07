#include <stdio.h>
#include "arvoreB.h"
#include "header.h"
#include "registro.h"
#include "utilitarias.h"
#include "funcionalidades.h"

/* 
A função juc_idx fará uma juncão com loop único
que consiste em percorrer o arquivo 1 e para cada registro 
buscar pelo arquivo de índice uma correspondência no arquivo 2
*/

void junc_idx(void){

    // Lê o nome do arquivo 1
    char arquivo1[100];
    scanf(" %s", arquivo1);

    // Abre o arquivo para leitura e verifica a consistênica
    FILE* fp1 = fopen(arquivo1, "rb");
    if(!verificarStatusArquivo(fp1)) return;

    // Queima o critério do arquivo 1 (garantido ser codProxEstacao)
    char lixo[100];
    scanf(" %s", lixo);

    // Lê o nome do arquivo 2
    char arquivo2[100];
    scanf(" %s", arquivo2);

    // Abre o arquivo para leitura e verifica a consistênica
    FILE* fp2 = fopen(arquivo2, "rb");
    if(!verificarStatusArquivo(fp2)) return;

    // Queima o critério do arquivo 2 (garantido ser codEstacao)
    scanf(" %s", lixo);

    // Lê o nome do arquivo 2
    char indice[100];
    scanf(" %s", indice);

    // Abre o arquivo para leitura e verifica a consistênica
    FILE* fp_indice = fopen(indice, "rb");
    if(!verificarStatusArquivo(fp_indice)) return;
    
    // Coloca os cabeçalhos dos arquivos na memória principal
    Header* header1 = bin_to_header(fp1);
    Arv_head* arv_head = bin_to_arv_head(fp_indice);
    
    // Cria dois registros temporários
    Registro* reg1;
    Registro* reg2;

    bool encontrou = false;
    int proxRRN1 = header_get_proxRRN(header1);
    
    // Percorre o arquivo 1
    for(int i = 0; i < proxRRN1; i ++){
        reg1 = bin_to_reg(fp1); // Lê o nó
        if(reg1 == NULL) continue; // Ignora registros removidos
        
        // Busca na árvore B
        long int offset = arv_busca_chave(fp_indice, arv_head->noRaiz, reg_get_codProxEstacao(reg1));
        // Se encontrou imprime o registro
        if(offset != -1){
            fseek(fp2, offset, SEEK_SET);
            reg2 = bin_to_reg(fp2);
            print_par_reg(reg1, reg2);
            reg_free(&reg2);
            encontrou = true;
        }
        
        // Libera a memória do registro temporário do arquivo 1
        reg_free(&reg1); // Libera a memória do registro
    }
    if(!encontrou) printf("Registro inexistente.\n");

    // Fecha os arquivos
    fclose(fp1);
    fclose(fp2);

    // Libera a memória
    header_free(&header1);
    arv_head_free(&arv_head);
}