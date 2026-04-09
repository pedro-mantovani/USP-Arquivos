#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL.h"
#include "registro.h"
#include "header.h"
#include "busca.h"
#include "funcionalidades.h"
#include "utilities.h"

void atualizar(char* nome_arquivo) {
    
    FILE* fp = fopen(nome_arquivo, "rb+");
    if (!verificarStatusArquivo(fp)) return;

    // Marca o header como inconsistente e coloca a struct na memória
    char inconsistente = '0';
    fseek(fp, 0, SEEK_SET); // Volta o ponteiro para o início
    fwrite(&inconsistente, sizeof(char), 1, fp); // Coloca '0' no primeiro byte
    Header* h = bin_to_header(fp); 

    int n_atualizacoes;
    if (scanf("%d", &n_atualizacoes) != 1) {
        fclose(fp); 
        head_free(&h); 
        return;
    }

    while (n_atualizacoes--) {
        int m_filtros; 
        scanf("%d", &m_filtros);

        Busca* b = criar_busca(m_filtros);
        preencher_filtros(b);

        fseek(fp, header_tam, SEEK_SET); // Vai até o início dos registros
        Registro* reg;
        long int offset;
        
        // Itera pelo arquivo verificando se o registro atende às condições de busca
        for(int RRN_atual = 0; RRN_atual < header_get_proxRRN(h); RRN_atual++) {

            // Lê o registro do binário
            reg = bin_to_reg(fp);

            // Ignora registros removidos
            if(reg == NULL) continue;

            // Verifica se o registro atende aos critérios da struct Busca
            if (registro_passa_filtrob(reg, b)) {

                // Calcula o byte offset em que o registro encontrado está
                offset = RRN_to_offset(RRN_atual);

                // Verifica o topo atual da pilha
                topo = header_get_topo(h);

                fseek(fp, offset, SEEK_SET); // Vai para o primeiro byte offset do registro
                fwrite(&removido, sizeof(char), 1, fp); // Marca como removido
                fwrite(&topo, sizeof(int), 1, fp); // Coloca nos próximos 4 bytes o topo da pilha

                header_set_topo(h, RRN_atual); // O novo topo da pilha vira este RRN
                
                // Retorna o ponteiro para o próximo registro
                fseek(fp, offset + 80, SEEK_SET); 
            }else{
                // Se é a última remoção conta os casos únicos
                if(n_atualizacoes == 0){
                    // Como o registro é válido e não foi removido colocamos na AVL o nome e o par da estação
                    // Assim mantemos controle de quantos valores únicos existem no arquivo
                    AVL_inserir(nomes_estacoes, reg_get_nomeEstacao(reg)); // Insere o nome da estação na AVL
                    char pair[20];
                    criar_par(reg, pair); // Cria a string do par
                    if(pair[0] != '\0') // Caso ela exista insere na AVL
                    AVL_inserir(pares_estacoes, pair);
                }
            }

            // Libera a memória do registro temporário
            reg_free(&reg);
        }

        // Libera memória da busca atual
        apagar_busca(&b);
    }

    //salva o Header atualizado (topo, nroEstacoes, nroPares) e consistente
    header_set_nroEstacoes(h, AVL_tamanho(nomes_estacoes));
    header_set_nroParesEstacao(h, AVL_tamanho(pares_estacoes));
    header_set_status(h, '1'); 
    header_to_bin(fp, h); 

    // Fecha o arquivo
    fclose(fp);

    // Libera a memória do nó cabeça
    head_free(&h);
    AVL_apagar(&nomes_estacoes);
    AVL_apagar(&pares_estacoes);
    BinarioNaTela(nome_arquivo); 
}

void atualizar(char* nome_arquivo) {
    
    FILE* fp = fopen(nome_arquivo, "rb+");
    if (!verificarStatusArquivo(fp)) return;

    //carrega o Header para a memória e marca como inconsistente no início
    Header* h = ler_header_do_bin(fp);
    header_set_status(h, '0');
    header_to_bin(fp, h);

    int n_updates; 
    if (scanf("%d", &n_updates) != 1) {
        fclose(fp); 
        head_free(&h); 
        return;
    }

    int novosPares = 0;
    while (n_updates--) {
        int nroEstacoesFiltradas;
        Registro** registrosFiltrados = filtros_multiplos(fp, &nroEstacoesFiltradas);
        
        int nroCampos;
        scanf("%d", &nroCampos);
        for(int i = 0; i < nroCampos; i++){
            char campo[50];
            char valor[50];

            scanf("%s", campo);
            ScanQuoteString(valor);

            int valor_inteiro;
            if(strcmp(valor, "") == 0)
                valor_inteiro = -1;
            else 
                valor_inteiro = atoi(valor);

            if (strcmp(campo, "codEstacao") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_codEstacao(registrosFiltrados[j], valor_inteiro);
                }
            }
                
            if (strcmp(campo, "nomeEstacao") == 0) {
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_nomeEstacao(registrosFiltrados[j], valor);
                }
            }

            if (strcmp(campo, "codLinha") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_codLinha(registrosFiltrados[j], valor_inteiro);
                }
            }

            if (strcmp(campo, "nomeLinha") == 0) {
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_nomeLinha(registrosFiltrados[j], valor);
                }
            } 

            if (strcmp(campo, "codProxEstacao") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    if(reg_get_codProxEstacao(registrosFiltrados[i]) == -1){
                        if(valor_inteiro != -1)
                            novosPares ++;
                    }else{
                        if(valor_inteiro == -1)
                            novosPares --;
                    }
                    reg_set_codProxEstacao(registrosFiltrados[j], valor_inteiro);
                }
            }

            if (strcmp(campo, "distProxEstacao") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_distProxEstacao(registrosFiltrados[j], valor_inteiro);
                }
            }

            if (strcmp(campo, "codLinhaIntegra") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_codLinhaIntegra(registrosFiltrados[j], valor_inteiro);
                }
            }
            if (strcmp(campo, "codEstIntegra") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_codEstIntegra(registrosFiltrados[j], valor_inteiro);
                }
            }
        }
        
        // Atualiza os registros no arquivo
        int offset;
        for(int i = 0; i < nroEstacoesFiltradas; i++){
            offset = offset_RRN(reg_get_RRN(registrosFiltrados[i]));
            reg_to_bin(registrosFiltrados[i], fp, offset);
            reg_free(&registrosFiltrados[i]);
        }
        free(registrosFiltrados);
    }

    //salva o Header atualizado (nroEstacoes, nroPares) e consistente
    header_set_nroParesEstacao(h, header_get_nroParesEstacao(h) + novosPares);
    header_set_status(h, '1'); 
    header_to_bin(fp, h); 

    fclose(fp);
    head_free(&h);
    BinarioNaTela(nome_arquivo); 
}