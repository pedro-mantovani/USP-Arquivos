#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "registro.h"
#include "header.h"
#include "busca.h"
#include "funcionalidades.h"
#include "utilitarias.h"
#include "arvoreB.h"

/* 
A função busca_parametrizada percorre o arquivo binário,
aplicando o filtro em cada registro e imprimindo aqueles que satisfazem as condições.
Para isso, ela utiliza funções auxiliares que manipulam uma struct Campos,
que armazena os campos a serem buscados e seus valores. 
*/

// Aloca a struct e os vetores internos com base no n fornecido (número de campos)
Campos* criar_campos(int n) {
    Campos *c = (Campos*) malloc(sizeof(Campos));
    if (!c) return NULL;

    c->n = n;
    c->chave_primaria = false;
    c->campos = (char**) malloc(n * sizeof(char*));
    c->valores = (char**) malloc(n * sizeof(char*));
    
    for (int i = 0; i < n; i++) {
        c->campos[i] = (char*) malloc(50 * sizeof(char));
        c->valores[i] = (char*) malloc(100 * sizeof(char));
    }
    
    return c;
}

// Libera toda a memória alocada dinamicamente
void apagar_campos(Campos **c) {
    if (!c || !*c) return;

    for (int i = 0; i < (*c)->n; i++) {
        free((*c)->campos[i]);
        free((*c)->valores[i]);
    }
    
    free((*c)->campos);
    free((*c)->valores);
    free(*c);
    *c = NULL;
}

// POO: Arrumar essa gambiarra
// Lê os n campos da entrada padrão e processa os valores
// Se um dos campos for o codEstação retorna 1, se não retorna 0
int preencher_campos(Campos *b) {
    int temcod = -1;

    for (int i = 0; i < b->n; i++) {
        scanf("%s", b->campos[i]);
        ScanQuoteString(b->valores[i]);
        if (strcmp(b->campos[i], "codEstacao") == 0) {
            temcod = atoi(b->valores[i]);
        }
    }

    return temcod;
}

//Verifica se o registro atual satisfaz TODOS os critérios da busca
int registro_passa_filtro(Registro *reg, Campos *c) {

    int ehid = 1;   //variavel para controle de codigo unico, para parar a busca

    // Verifica se o registro atende ao critério i
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
            // Se for, verifica se os valores são iguais
            if(reg_get_codEstacao(reg) == valor_inteiro){
                ehid = 2;   //2 sera o retorno da função caso ache o id, para a que a busca reconheça e seja parada na principal
                continue; // Se for passa para o próximo critério
            }   
            return 0; // Se não o registro não serve
        }
        
        // Verifica se o critério é "codEstação"
        if (strcmp(criterio, "nomeEstacao") == 0) {
            // Se for, verifica se os valores são iguais
            char* nomeEst = reg_get_nomeEstacao(reg);
            if (nomeEst != NULL && strcmp(nomeEst, valor_str) == 0) continue; // Caso os valores sejam iguais vai para o próximo critério
            else if (nomeEst == NULL && strcmp(valor_str, "") == 0) continue; // Caso os valores sejam nulos vai para o próximo critério
            return 0; // Se não o registro não serve
        }

        if (strcmp(criterio, "codLinha") == 0){
            if(reg_get_codLinha(reg) == valor_inteiro) continue;
            return 0;
        }

        if (strcmp(criterio, "nomeLinha") == 0) {
            char *nomeLinha = reg_get_nomeLinha(reg);
            if (nomeLinha && strcmp(nomeLinha, valor_str) == 0) continue; // Caso os valores sejam iguais
            else if (!nomeLinha && strcmp(valor_str, "") == 0) continue; // Caso os valores sejam iguais
            return 0;
        } 

        if (strcmp(criterio, "codProxEstacao") == 0){
            if(reg_get_codProxEstacao(reg) == valor_inteiro) continue;
            return 0;
        }
    
        if(strcmp(criterio, "distProxEstacao") == 0){
            if(reg_get_distProxEstacao(reg) == valor_inteiro) continue;
            return 0;
        }
    
        if (strcmp(criterio, "codLinhaIntegra") == 0){
            if (reg_get_codLinhaIntegra(reg) == valor_inteiro) continue;;
            return 0;
        }
    
        if (strcmp(criterio, "codEstIntegra") == 0){
            if (reg_get_codEstIntegra(reg) == valor_inteiro) continue;
            return 0;
        }

        // Critério não encontrado
        return 0;
    }

    // Se passou em todos os critérios retorna a variavel ehid
    // Caso valha 1 - verdadeiro, achou o registro mas nao tem codigo unico na busca
    // Caso valha 2 - verdadeiro MAS um campo de busca era codigo, logo é um sinal para parar a busca
    return ehid;
}

void busca_parametrizada_nova(char* arquivo_dados, char* arquivo_indice) {
    // Abre o arquivo de dados para leitura
    FILE* fp_dados = fopen(arquivo_dados, "rb");
    if(!verificarStatusArquivo(fp_dados)) return;
    Header* header = bin_to_header(fp_dados);
    
    // Abre o arquivo da árvore B para leitura
    FILE* fp_indice = fopen(arquivo_indice, "rb");
    if(!verificarStatusArquivo(fp_indice)) return;
    Arv_head* arv_head = bin_to_arv_head(fp_indice);
    
    //Lê quantas buscas serão feitas
    int n_buscas;
    if (scanf("%d", &n_buscas) != 1) {
        fclose(fp_indice);
        header_free(&header);
        arv_head_free(&arv_head);
        fclose(fp_dados);
        return;
    }

    while(n_buscas--){

        // Lê os m filtros
        int m_filtros;
        scanf("%d", &m_filtros);

        // Cria a estrutura de busca com m campos alocados dinamicamente
        Campos* b = criar_campos(m_filtros);

        // Faz a leitura dos campos e valores e reconhece se existe ou não o campo codEstacao como um dos criterios de busca (-1 se não tiver)
        int ehCodigo = preencher_campos(b);  

        int encontrou = 0; // Cria uma flag para verificar se algum registro foi encontrado

        // Se o codEstacao é um campo da busca, então faz a busca por indice arvore B
        if (ehCodigo != -1) {

            int byte_offset = arv_busca_chave(fp_indice, arv_head->noRaiz, ehCodigo);

            if (byte_offset != -1) {
                // Posiciona o ponteiro de leitura diretamente no byte offset recuperado da Árvore-B
                fseek(fp_dados, byte_offset, SEEK_SET);
                Registro* reg = bin_to_reg(fp_dados);
            
                if(reg == NULL) continue;
            
                // Testa os outros campos passados na busca
                int controle = registro_passa_filtro(reg, b);
                if (controle > 0) {
                    encontrou = 1;
                    print_reg(reg);
                }
            
                reg_free(&reg);
            }

        } else {

            Registro* reg; // Cria um registro temporário

            int proxRRN = header_get_proxRRN(header);

            fseek(fp_dados, tam_header, SEEK_SET); // Volta ao início dos dados (após o header de 17)

            // Percorre todos os registros
            for(int RRN_atual = 0; RRN_atual < proxRRN; RRN_atual ++) {
            
                // Lê o registro
                reg = bin_to_reg(fp_dados);

                // Ignora registros removidos
                if(reg == NULL) continue;

                // Verifica se o registo passa no filtro
                // 0 - nao passa (continua a buca)
                // 1 - passa (continua a busca)
                // 2 - passa E a busca é por codigo (para a busca)
                int controle = registro_passa_filtro(reg, b); 
                if (controle > 0) {
                    encontrou = 1;
                    print_reg(reg); // Imprime o registro
                }

                reg_free(&reg); // Libera a memória alocada
            }
        }

        if (encontrou == 0) {
            printf("Registro inexistente.\n");
        }

        // Libera a memória alocada para armazenar os campos antes da próxima iteração ou fim da função
        apagar_campos(&b);
        printf("\n");

    }

    // Libera a memória
    header_free(&header);
    arv_head_free(&arv_head);

    fclose(fp_dados);
    fclose(fp_indice);
}

// Antiga função de busca
void busca_parametrizada(char* nome_arquivo) {

    // Abre o arquivo existente para leitura
    FILE* fp = fopen(nome_arquivo, "rb");
    if(!verificarStatusArquivo(fp)) return;
    Header* header = bin_to_header(fp);
    
    //Lê quantas buscas serão feitas
    int n_buscas;
    if (scanf("%d", &n_buscas) != 1) {
        fclose(fp);
        return;
    }

    // Para cada busca faz as operações necessárias
    while (n_buscas--) {

        // Lê os m filtros
        int m_filtros;
        scanf("%d", &m_filtros);

        // Cria a estrutura de busca com m campos alocados dinamicamente
        Campos* b = criar_campos(m_filtros);
        preencher_campos(b);    //faz a leitura dos campos e valores

        Registro* reg; // Cria um registro temporário
        int encontrou = 0; // Cria uma flag para verificar se algum registro foi encontrado

        
        int proxRRN = header_get_proxRRN(header);
        
        fseek(fp, tam_header, SEEK_SET); // Volta ao início dos dados (após o header de 17 bytes)
        

        // Percorre todos os registros
        for(int RRN_atual = 0; RRN_atual < proxRRN; RRN_atual ++) {
            
            // Lê o registro
            reg = bin_to_reg(fp);

            // Ignora registros removidos
            if(reg == NULL) continue;

            // Verifica se o registo passa no filtro
            // 0 - nao passa (continua a buca)
            // 1 - passa (continua a busca)
            // 2 - passa E a busca é por codigo (para a busca)
            int controle = registro_passa_filtro(reg, b); 
            if (controle > 0) {
                encontrou = 1;
                print_reg(reg); // Imprime o registro
            }

            reg_free(&reg); // Libera a memória alocada

            if(controle == 2) break;   //se encontrou um registro e a busca envolvia codigo unico, para de procurar
        }

        if (encontrou == 0) {
            printf("Registro inexistente.\n");
        }

        // Libera a memória alocada para armazenar os campos antes da próxima iteração ou fim da função
        apagar_campos(&b);
        printf("\n"); 
    }

    // Libera a memória
    header_free(&header);

    fclose(fp);
}