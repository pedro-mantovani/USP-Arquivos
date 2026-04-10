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

    // Abre o arquivo
    FILE* fp = fopen(nome_arquivo, "rb+");
    if (!verificarStatusArquivo(fp)) return;

    // Marca o header como inconsistente e coloca a struct na memória
    char inconsistente = '0';
    fseek(fp, 0, SEEK_SET); // Volta o ponteiro para o início
    fwrite(&inconsistente, sizeof(char), 1, fp); // Coloca '0' no primeiro byte
    Header* h = bin_to_header(fp); 

    // Lê quantas inserções serão feitas
    int n_insercoes;
    if (scanf("%d", &n_insercoes) != 1) {
        fclose(fp); 
        head_free(&h); 
        return;
    }

    char valor_str[50]; // Variável temporária para armazenar strings
    int valor_inteiro; // Variável temporária para armazenar valores inteiros
    long int offset; // Byte offset em que o registro será inserido
    int prox; // Próximo elemento da pilha
    int topo; // Topo da pilha
    int novosPares = 0; // Número de novos pares inseridos

    // Faz n inserções
    while (n_insercoes--) {
        // Cria um novo registro
        Registro* reg = criar_registro();
        
        // Lê e preenche todos os campos do registro
        // Caso o campo seja um inteiro é utilizada a função scan_int
        // Caso o campo seja uma string é utilizada a função ScanQuoteString
        valor_inteiro = scan_int();
        reg_set_codEstacao(reg, valor_inteiro);
        
        ScanQuoteString(valor_str);
        reg_set_nomeEstacao(reg, valor_str);
        reg_set_tamNomeEstacao(reg, strlen(valor_str));

        valor_inteiro = scan_int();        
        reg_set_codLinha(reg, valor_inteiro);
        
        ScanQuoteString(valor_str);
        reg_set_nomeLinha(reg, valor_str);
        reg_set_tamNomeLinha(reg, strlen(valor_str));

        valor_inteiro = scan_int();
        reg_set_codProxEstacao(reg, valor_inteiro);
        // Se o código da próxima estação é válido, significa que um novo par válido e único foi inserido
        if(valor_inteiro != -1)
            novosPares ++;
        
        valor_inteiro = scan_int();
        reg_set_distProxEstacao(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_codLinhaIntegra(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_codEstIntegra(reg, valor_inteiro);
        
        /*Coloca o registro no topo da pilha e atualiza o topo*/

        topo = header_get_topo(h); // Armazena o topo atual da pilha

        // Se o topo for -1 nenhum registro foi removido
        if(topo == -1){ 
            offset = RRN_to_offset(header_get_proxRRN(h)); // O registro deve ser inserido no próximo RRN
            header_set_proxRRN(h, header_get_proxRRN(h) + 1); // O próximo RRN é incrementado
        }
        // Caso algum registro tenha sido removido
        else{ 
            offset = RRN_to_offset(topo); // O offset é aquele correspondente ao RRN do topo da pilha
            fseek(fp, offset+1, SEEK_SET); // Coloca fp no segundo byte do registro removido (próximo elemento da pilha)
            fread(&prox, sizeof(int), 1, fp); // Lê o valor e coloca em prox
            header_set_topo(h, prox); // Atualiza o topo
        }

        reg_to_bin(reg, fp, offset); // Coloca o registro no binário
        reg_free(&reg); // Libera a memória alocada para o registro
    }

    // Salva o Header com o nroPares atualizado e consistente
    // Note que pelas especificações do projeto é garantido que nenhuma estação com um novo nome será inserido
    header_set_nroParesEstacao(h, header_get_nroParesEstacao(h) + novosPares);
    header_set_status(h, '1'); 
    header_to_bin(fp, h);
    head_free(&h);

    fclose(fp); // Fecha o arquivo
    BinarioNaTela(nome_arquivo);
}