#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL.h"
#include "registro.h"
#include "header.h"
#include "busca.h"
#include "funcionalidades.h"
#include "utilities.h"

void inserir(char* nome_arquivo){

    FILE* fp = fopen(nome_arquivo, "rb+");
    if (!verificarStatusArquivo(fp)) return;

    // Marca o header como inconsistente e coloca a struct na memória
    char inconsistente = '0';
    fseek(fp, 0, SEEK_SET); // Volta o ponteiro para o início
    fwrite(&inconsistente, sizeof(char), 1, fp); // Coloca '0' no primeiro byte
    Header* h = bin_to_header(fp); 

    int n_insercoes;
    int novosPares = 0;
    if (scanf("%d", &n_insercoes) != 1) {
        fclose(fp); 
        head_free(&h); 
        return;
    }

    char valor[50];
    int valor_inteiro;
    long int offset;
    int prox;
    int topo;
    while (n_insercoes--) {
        Registro* reg = criar_registro();
        
        valor_inteiro = scan_int();
        reg_set_codEstacao(reg, valor_inteiro);
        
        ScanQuoteString(valor);
        reg_set_nomeEstacao(reg, valor);
        reg_set_tamNomeEstacao(reg, strlen(valor));

        valor_inteiro = scan_int();        
        reg_set_codLinha(reg, valor_inteiro);
        
        ScanQuoteString(valor);
        reg_set_nomeLinha(reg, valor);
        reg_set_tamNomeLinha(reg, strlen(valor));

        valor_inteiro = scan_int();
        reg_set_codProxEstacao(reg, valor_inteiro);
        if(valor_inteiro != -1)
            novosPares ++;
        
        valor_inteiro = scan_int();
        reg_set_distProxEstacao(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_codLinhaIntegra(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_codEstIntegra(reg, valor_inteiro);
        
        topo = header_get_topo(h);
        if(topo == -1){
            offset = RRN_to_offset(header_get_proxRRN(h));
            header_set_proxRRN(h, header_get_proxRRN(h) + 1);
        }else{
            offset = RRN_to_offset(topo);
            fseek(fp, offset+1, SEEK_SET);
            fread(&prox, sizeof(int), 1, fp);
            header_set_topo(h, prox);
        }

        reg_to_bin(reg, fp, offset);
        reg_free(&reg);
    }

    //salva o Header atualizado (nroEstacoes, nroPares) e consistente
    header_set_nroParesEstacao(h, header_get_nroParesEstacao(h) + novosPares);
    header_set_status(h, '1'); 
    header_to_bin(fp, h); 

    fclose(fp);
    head_free(&h);
    BinarioNaTela(nome_arquivo); 
}