#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL.h"
#include "registro.h"
#include "header.h"
#include "busca.h"
#include "funcionalidades.h"
#include "utilitarias.h"

/* 
A função atualizar cria duas structs Campos, uma para os critérios de busca e outra para os campos a serem atualizados. 
Cada registro é verificado por uma função de filtro e, se satisfizer as condições, é enviado para atualizar_registro,
nela seus campos são modificados com base nos novos valores e, em seguida, 
o registro atualizado é escrito de volta no arquivo binário. 
*/

void atualizar_registro(Registro *reg, Campos *c) {

    // Atualiza critério por critério
    for (int i = 0; i < c->n; i++) {
        char* criterio = c->campos[i];
        char* valor_str = c->valores[i];

        // Converte o valor para inteiro
        int valor_inteiro;

        if(strcmp(valor_str, "") == 0)
            valor_inteiro = -1;
        else 
            valor_inteiro = atoi(valor_str);
        
        // Verifica se o critério é "codEstação"
        if (strcmp(criterio, "codEstacao") == 0){
            reg_set_codEstacao(reg, valor_inteiro); // Atualiza o registro
        }

        // Verifica se o critério é "codLinha"
        if (strcmp(criterio, "codLinha") == 0){
            reg_set_codLinha(reg, valor_inteiro); // Atualiza o registro
        }

        // Verifica se o critério é "codProxEstacao"
        if (strcmp(criterio, "codProxEstacao") == 0){
            reg_set_codProxEstacao(reg, valor_inteiro); // Atualiza o registro
        }
    
        // Verifica se o critério é "distProxEstacao"
        if (strcmp(criterio, "distProxEstacao") == 0){
            reg_set_distProxEstacao(reg, valor_inteiro); // Atualiza o registro
        }

        // Verifica se o critério é "codLinhaIntegra"
        if (strcmp(criterio, "codLinhaIntegra") == 0){
            reg_set_codLinhaIntegra(reg, valor_inteiro); // Atualiza o registro
        }
        
        // Verifica se o critério é "codEstIntegra"
        if (strcmp(criterio, "codEstIntegra") == 0){
            reg_set_codEstIntegra(reg, valor_inteiro); // Atualiza o registro
        }

        // Verifica se o critério é "nomeEstação"
        if (strcmp(criterio, "nomeEstacao") == 0) {
            // Se a string for válida atualiza o tamanho e o nome
            if(strcmp(valor_str, "") != 0){
                reg_set_tamNomeEstacao(reg, strlen(valor_str));
                reg_set_nomeEstacao(reg, valor_str);
            }else{ // Se não o tamanho é zero e o nome é um ponteiro nulo
                reg_set_tamNomeEstacao(reg, 0);
                reg_set_nomeEstacao(reg, NULL);
            }
        }

        // Verifica se o critério é "nomeLinha"
        if (strcmp(criterio, "nomeLinha") == 0) {
            // Se a string for válida atualiza o tamanho e o nome
            if(strcmp(valor_str, "") != 0){
                reg_set_tamNomeLinha(reg, strlen(valor_str));
                reg_set_nomeLinha(reg, valor_str);
            }else{ // Se não o tamanho é zero e o nome é um ponteiro nulo
                reg_set_tamNomeLinha(reg, 0);
                reg_set_nomeLinha(reg, NULL);
            }
        }
    }
}


void atualizar(char* nome_arquivo) {
    
    // Abre um arquivo existente para leitura e escrita
    FILE* fp = fopen(nome_arquivo, "rb+");
    if (!verificarStatusArquivo(fp)) return;

    // Marca o header como inconsistente e coloca a struct na memória
    char inconsistente = '0';
    fseek(fp, 0, SEEK_SET); // Volta o ponteiro para o início
    fwrite(&inconsistente, sizeof(char), 1, fp); // Coloca '0' no primeiro byte
    Header* h = bin_to_header(fp); 

    // Lê o número de atualizações
    int n_atualizacoes;
    if (scanf("%d", &n_atualizacoes) != 1) {
        fclose(fp); 
        header_free(&h); 
        return;
    }

    int novos_pares = 0;
    while (n_atualizacoes--) {
        // Lê os filtros da busca
        int m_filtros; 
        scanf("%d", &m_filtros);

        Campos* c_busca = criar_campos(m_filtros);
        preencher_campos(c_busca);

        // Lê os campos que serão atualizados
        int p_campos;
        scanf("%d", &p_campos);

        Campos* c_atualizar = criar_campos(p_campos);
        preencher_campos(c_atualizar);

        fseek(fp, header_tam, SEEK_SET); // Vai até o início dos registros
        Registro* reg; // Declara um registro temporário
        long int offset; // Declara uma variável para armazenar o offset do registro atual
        int prox_RRN = header_get_proxRRN(h);
        
        // Itera pelo arquivo verificando se o registro atende às condições de busca
        for(int RRN_atual = 0; RRN_atual < prox_RRN; RRN_atual++) {

            // Lê o registro do binário
            reg = bin_to_reg(fp);

            // Ignora registros removidos
            if(reg == NULL) continue;

            // Verifica se o registro atende aos critérios da struct Busca
            if (registro_passa_filtro(reg, c_busca)) {

                // Calcula o byte offset em que o registro encontrado está
                offset = RRN_to_offset(RRN_atual);

                int prox_estacao = reg_get_codProxEstacao(reg);

                // Atualiza os campos do registro
                atualizar_registro(reg, c_atualizar);

                // Se uma nova estação válida for adicionada, então os novos pares são incrementados
                if(prox_estacao == -1 && reg_get_codProxEstacao(reg) != -1)
                    novos_pares ++;
                // Coloca o registro no arquivo binário
                reg_to_bin(reg, fp, offset);

            }

            // Libera a memória do registro temporário
            reg_free(&reg);
        }

        // Libera memória da busca s atualização atual
        apagar_campos(&c_busca);
        apagar_campos(&c_atualizar);
    }

    //salva o Header atualizado (topo, nroEstacoes, nroPares) e consistente
    header_set_nroParesEstacao(h, header_get_nroParesEstacao(h) + novos_pares);
    header_set_status(h, '1'); 
    header_to_bin(fp, h); 

    // Fecha o arquivo
    fclose(fp);

    // Libera a memória do nó cabeça
    header_free(&h);
    BinarioNaTela(nome_arquivo); 
}