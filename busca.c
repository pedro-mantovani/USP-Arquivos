#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "AVL.h"
#include "registro.h"
#include "header.h"
#include "busca.h"
#include "funcionalidades.h"
#include "utilities.h"

// Aloca a struct e os vetores internos com base no n fornecido (número de campos)
Campos* criar_campos(int n) {
    Campos *c = (Campos*) malloc(sizeof(Campos));
    if (!c) return NULL;

    c->n = n;
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

// Lê os n campos da entrada padrão e processa os valores
void preencher_campos(Campos *b) {
    for (int i = 0; i < b->n; i++) {
        scanf("%s", b->campos[i]);
        ScanQuoteString(b->valores[i]);
    }
}

//Verifica se o registro atual satisfaz TODOS os critérios da busca
bool registro_passa_filtro(Registro *reg, Campos *c) {
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
            if(reg_get_codEstacao(reg) == valor_inteiro) continue; // Se for passa para o próximo critério
            return false; // Se não o registro não serve
        }
        
        // Verifica se o critério é "codEstação"
        if (strcmp(criterio, "nomeEstacao") == 0) {
            // Se for, verifica se os valores são iguais
            char* nomeEst = reg_get_nomeEstacao(reg);
            if (nomeEst != NULL && strcmp(nomeEst, valor_str) == 0) continue; // Caso os valores sejam iguais vai para o próximo critério
            else if (nomeEst == NULL && strcmp(valor_str, "") == 0) continue; // Caso os valores sejam nulos vai para o próximo critério
            return false; // Se não o registro não serve
        }

        if (strcmp(criterio, "codLinha") == 0){
            if(reg_get_codLinha(reg) == valor_inteiro) continue;
            return false;
        }

        if (strcmp(criterio, "nomeLinha") == 0) {
            char *nomeLinha = reg_get_nomeLinha(reg);
            if (nomeLinha && strcmp(nomeLinha, valor_str) == 0) continue; // Caso os valores sejam iguais
            else if (!nomeLinha && strcmp(valor_str, "") == 0) continue; // Caso os valores sejam iguais
            return false;
        } 

        if (strcmp(criterio, "codProxEstacao") == 0){
            if(reg_get_codProxEstacao(reg) == valor_inteiro) continue;
            return false;
        }
    
        if(strcmp(criterio, "distProxEstacao") == 0){
            if(reg_get_distProxEstacao(reg) == valor_inteiro) continue;
            return false;
        }
    
        if (strcmp(criterio, "codLinhaIntegra") == 0){
            if (reg_get_codLinhaIntegra(reg) == valor_inteiro) continue;;
            return false;
        }
    
        if (strcmp(criterio, "codEstIntegra") == 0){
            if (reg_get_codEstIntegra(reg) == valor_inteiro) continue;
            return false;
        }

        // Critério não encontrado
        return false;
    }

    // Se passou em todos os critérios retorna verdadeiro
    return true;
}

// Função principal da busca
void busca_parametrizada(char* nome_arquivo) {
    // Abre o arquivo
    FILE* fp = fopen(nome_arquivo, "rb");
    if(!verificarStatusArquivo(fp)) return;

    int n_buscas;
    if (scanf("%d", &n_buscas) != 1) {
        fclose(fp);
        return;
    }

    // Realiza n buscas
    while (n_buscas--) {

        // Lê os m filtros
        int m_filtros;
        scanf("%d", &m_filtros);

        // Cria a estrutura de busca com m campos alocados dinamicamente
        Campos* b = criar_campos(m_filtros);
        preencher_campos(b);

        Registro* reg; // Cria um registro temporário
        int encontrou = 0; // Cria uma flag para verificar se algum registro foi encontrado

        // Lê o próximo RRN do registro
        fseek(fp, 5, SEEK_SET);
        int proxRRN;
        fread(&proxRRN, sizeof(int), 1, fp);

        fseek(fp, header_tam, SEEK_SET); // Volta ao início dos dados (após o header de 17 bytes)
        

        // Percorre todos os registros
        for(int RRN_atual = 0; RRN_atual < proxRRN; RRN_atual ++) {
            
            // Lê o registro
            reg = bin_to_reg(fp);

            // Ignora registros removidos
            if(reg == NULL) continue;

            // Verifica se o registo passa no filtro
            if (registro_passa_filtro(reg, b)) {
                encontrou = 1; // Atualiza a flag
                print_reg(reg); // Imprime o registro
            }

            reg_free(&reg); // Libera a memória alocada
        }

        if (!encontrou) {
            printf("Registro inexistente.\n");
        }

        // Libera a memória alocada para armazenar os campos antes da próxima iteração ou fim da função
        apagar_campos(&b);
        printf("\n"); 
    }
    fclose(fp);
}