#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL.h"
#include "registro.h"
#include "header.h"
#include "busca.h"
#include "funcionalidades.h"
#include "utilities.h"

void remover(char* nome_arquivo) {
    
    FILE* fp = fopen(nome_arquivo, "rb+");
    if (!verificarStatusArquivo(fp)) return;

    // Marca o header como inconsistente e coloca a struct na memória
    char inconsistente = '0';
    fseek(fp, 0, SEEK_SET); // Volta o ponteiro para o início
    fwrite(&inconsistente, sizeof(char), 1, fp); // Coloca '0' no primeiro byte
    Header* h = bin_to_header(fp); 

    int n_buscas;
    if (scanf("%d", &n_buscas) != 1) {
        fclose(fp); 
        head_free(&h); 
        return;
    }

    // Cria as AVLs que irão armazenar os nomes das estações e os pares
    AVL* nomes_estacoes = AVL_criar();
    AVL* pares_estacoes = AVL_criar();

    while (n_buscas--) {
        int m_filtros; 
        scanf("%d", &m_filtros);

        Campos* c_busca = criar_campos(m_filtros);
        preencher_campos(c_busca);

        fseek(fp, header_tam, SEEK_SET); // Vai até o início dos registros
        Registro* reg;
        long int offset;
        int topo;
        char removido = '1';

        // Itera pelo arquivo verificando se o registro atende às condições de busca
        for(int RRN_atual = 0; RRN_atual < header_get_proxRRN(h); RRN_atual++) {

            // Lê o registro do binário
            reg = bin_to_reg(fp);

            // Ignora registros removidos
            if(reg == NULL) continue;

            // Verifica se o registro atende aos critérios da struct Busca
            if (registro_passa_filtro(reg, c_busca)) {

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
                if(n_buscas == 0){
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
        apagar_campos(&c_busca);
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