#include <stdio.h>
#include "header.h"
#include "registro.h"
#include "utilitarias.h"
#include "funcionalidades.h"

/* 
A função juc_loop fará uma juncão de loop alinhado
que consiste em percorrer o arquivo 1 e para cada registro 
verificar todos os registros do arquivo 2 imprimindo aqueles tenham 
o valor do campo buscado igual ao registro do arquivo 1
*/

void junc_loop(void){

    // POO: LER TUDO PRIMEIRO E DEPOIS VERIFICAR

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
    
    // Coloca os cabeçalhos dos arquivos na memória principal
    Header* header1 = bin_to_header(fp1);
    Header* header2 = bin_to_header(fp2);

    // Cria dois registros temporários
    Registro* reg1;
    Registro* reg2;

    bool encontrou = false;
    int proxRRN1 = header_get_proxRRN(header1);
    int proxRRN2 = header_get_proxRRN(header2);
    // Percorre o arquivo 1
    for(int i = 0; i < proxRRN1; i ++){
        reg1 = bin_to_reg(fp1); // Lê o nó
        if(reg1 == NULL) continue; // Ignora registros removidos
        // Percorre o segundo arquivo
        for(int i = 0; i < proxRRN2; i++){
            // Lê o próximo registro do segundo arquivo
            reg2 = bin_to_reg(fp2);
            // Se o códgigo estação do registro 2 for igual ao codigo estação do arquivo 1
            if(reg_get_codProxEstacao(reg1) == reg_get_codEstacao(reg2)){
                encontrou = true; // Marca a flag de encontrado como verdadeira
                // Imprime os campos
                print_par_reg(reg1, reg2);
                reg_free(&reg2);
                break;
            }
            // Libera a memória do registro do arquivo temporário 2
            reg_free(&reg2);
        }
        // Volta o segundo arquivo para o início
        fseek(fp2, tam_header, SEEK_SET);

        // Libera a memória do registro temporário do arquivo 1
        reg_free(&reg1); // Libera a memória do registro
    }
    if(!encontrou) printf("Registro inexistente.\n");

    // Fecha os arquivos
    fclose(fp1);
    fclose(fp2);

    // Libera a memória
    header_free(&header1);
    header_free(&header2);
}