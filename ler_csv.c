#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL.h"
#include "registro.h"
#include "header.h"
#include "busca.h"
#include "funcionalidades.h"
#include "utilities.h"

//funcionalidade 1: leitura de csv e escrita em binário
void ler_csv(char* arquivo_csv, char* arquivo_bin){
    FILE* fp_csv = fopen(arquivo_csv, "r");
    if(fp_csv == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    FILE* fp_bin = fopen(arquivo_bin, "wb");
    if(fp_bin == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Header* head = criar_header();
    header_to_bin(fp_bin, head);

    char linha[256];
    char *ptr;
    char *token;
    char* codEstacao;
    char* codProxEstacao;
    char pair[21];
    int nroRegistos = 0;

    Registro* reg_temp = criar_registro();

    AVL* nomesEstacoes = AVL_criar();
    AVL* paresEstacoes = AVL_criar();

    if (fp_csv == NULL) return;

    fgets(linha, sizeof(linha), fp_csv); // Queima a linha de cabeçalho
    
    while (fgets(linha, sizeof(linha), fp_csv) != NULL) {
        ptr = linha;
        // Troca o \n por \0
        linha[strcspn(linha, "\n") - 1] = '\0';
        
        codEstacao = strsep(&ptr, ",");
        reg_set_codEstacao(reg_temp, atoi(codEstacao));

        token = strsep(&ptr, ",");
        reg_set_tamNomeEstacao(reg_temp, strlen(token));
        reg_set_nomeEstacao(reg_temp, token);
        AVL_inserir(nomesEstacoes, token);

        reg_set_codLinha(reg_temp, convert_num(strsep(&ptr, ",")));

        token = strsep(&ptr, ",");
        reg_set_tamNomeLinha(reg_temp, strlen(token));
        reg_set_nomeLinha(reg_temp, token);

        codProxEstacao = strsep(&ptr, ",");
        reg_set_codProxEstacao(reg_temp, convert_num(codProxEstacao));
        // Salva em ordem lexográfica (menor, maior)
        if(*codProxEstacao != '\0'){
            if (strcmp(codEstacao, codProxEstacao) < 0) {
                snprintf(pair, sizeof(pair), "%s,%s", codEstacao, codProxEstacao);
            } else {
                snprintf(pair, sizeof(pair), "%s,%s", codProxEstacao, codEstacao);
            }
            AVL_inserir(paresEstacoes, pair);
        }

        reg_set_distProxEstacao(reg_temp, convert_num(strsep(&ptr, ",")));
        reg_set_codLinhaIntegra(reg_temp, convert_num(strsep(&ptr, ",")));
        reg_set_codEstIntegra(reg_temp, convert_num(strsep(&ptr, ",")));

        reg_to_bin(reg_temp, fp_bin, -1);
        nroRegistos ++;
    }

    fclose(fp_csv);

    header_set_nroEstacoes(head, AVL_tamanho(nomesEstacoes));
    header_set_nroParesEstacao(head, AVL_tamanho(paresEstacoes));
    header_set_proxRRN(head, nroRegistos);
    header_set_status(head, '1');
    header_to_bin(fp_bin, head);
    reg_free(&reg_temp);
    head_free(&head);
    AVL_apagar(&nomesEstacoes);
    AVL_apagar(&paresEstacoes);
    fclose(fp_bin);
    BinarioNaTela(arquivo_bin);
}
