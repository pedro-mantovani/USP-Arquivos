#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "busca.h"
#include "IO.h" 


bool filter(Registro *reg, char* criterio, char* valor){
    int valor_inteiro;    
    if(strcmp(valor, "") == 0)
        valor_inteiro = -1;
    else 
        valor_inteiro = atoi(valor);

    if (strcmp(criterio, "codEstacao") == 0)
        return (reg_get_codEstacao(reg) == valor_inteiro);
        
    if (strcmp(criterio, "nomeEstacao") == 0) {
        char *res = reg_get_nomeEstacao(reg);
        if (res && strcmp(res, valor) == 0) return 1;
        else if (!res && strcmp(valor, "") == 0) return 1;
        return 0;
    }

    if (strcmp(criterio, "codLinha") == 0)
        return (reg_get_codLinha(reg) == valor_inteiro);

    if (strcmp(criterio, "nomeLinha") == 0) {
        char *res = reg_get_nomeLinha(reg);
        if (res && strcmp(res, valor) == 0) return 1;
        else if (!res && strcmp(valor, "") == 0) return 1;
        return 0;
    } 

    if (strcmp(criterio, "codProxEstacao") == 0)
        return (reg_get_codProxEstacao(reg) == valor_inteiro);
    
    if (strcmp(criterio, "distProxEstacao") == 0)
        return (reg_get_distProxEstacao(reg) == valor_inteiro);
    
    if (strcmp(criterio, "codLinhaIntegra") == 0)
        return (reg_get_codLinhaIntegra(reg) == valor_inteiro);
    
    if (strcmp(criterio, "codEstIntegra") == 0)
        return (reg_get_codEstIntegra(reg) == valor_inteiro);
    
    // Critério não encontrado
    return 0;
}

Registro** filtrar_registros(FILE* fp, char* criterio, char* valor, int* nroEstacoesFiltradas){
    int nroRegistros = 0;

    // Lê o número de RRNs do registro
    fseek(fp, 5, SEEK_SET);
    int totalRRN;
    fread(&totalRRN, sizeof(int), 1, fp);

    // Pula para o primeiro registro
    fseek(fp, 17, SEEK_SET);

    Registro** registros = malloc(totalRRN * sizeof(Registro*));
    if(registros == NULL){
        printf("Falha ao alocar a memória\n");
        return NULL;
    }

    for(int i = 0; i < totalRRN; i ++){
        Registro* reg_temp = bin_to_reg(fp);
        if(reg_temp == NULL)
            continue;
        if(filter(reg_temp, criterio, valor)){
            nroRegistros ++;
            reg_set_RRN(reg_temp, i);
            registros[nroRegistros - 1] = reg_temp;
        }else
            reg_free(&reg_temp);
    }
    *nroEstacoesFiltradas = nroRegistros;
    if(nroRegistros > 0){
        Registro** temp = realloc(registros, nroRegistros * sizeof(Registro*));
        if (temp != NULL)
            registros = temp;
        return registros;
    }else{
        free(registros);
        return NULL;
    }
}

Registro** filtrar_vetor(Registro** registros, char* criterio, char* valor, int* nroEstacoesFiltradas){
    if(registros == NULL) return NULL;

    int nroRegistros = 0;
    Registro** registrosFiltrados = malloc(*nroEstacoesFiltradas * sizeof(Registro*));
    if(registrosFiltrados == NULL){
        printf("Falha ao alocar a memória\n");
        for(int i = 0; i < *nroEstacoesFiltradas; i++){
            reg_free(&registros[i]);
        }
        free(registros);
        return NULL;
    }

    for(int i = 0; i < *nroEstacoesFiltradas; i++){
        if(filter(registros[i], criterio, valor)){
            registrosFiltrados[nroRegistros] = registros[i];
            nroRegistros ++;
        }else
            reg_free(&registros[i]);
    }
    free(registros);

    *nroEstacoesFiltradas = nroRegistros;
    if(nroRegistros > 0){
        Registro** temp = realloc(registrosFiltrados, nroRegistros * sizeof(Registro*));
        if (temp != NULL) {
            registrosFiltrados = temp;
        }
        return registrosFiltrados;
    }else{
        free(registrosFiltrados);
        return NULL;
    }
}

Registro** filtros_multiplos(FILE* fp, int* nroEstacoesFiltradas){
    char criterio[50];
    char valor[50];
    int n_filtros;
    scanf("%d", &n_filtros);

    scanf("%s", criterio);
    ScanQuoteString(valor);

    // Filtro inicial do arquivo todo
    Registro** registros = filtrar_registros(fp, criterio, valor, nroEstacoesFiltradas);
    n_filtros --;

    while(n_filtros--){
        scanf("%s", criterio);
        ScanQuoteString(valor);
        registros = filtrar_vetor(registros, criterio, valor, nroEstacoesFiltradas);
    }

    return registros;
}

void buscar(char* nome_arquivo){
    FILE* fp = fopen(nome_arquivo, "rb");
    if(!verificarStatusArquivo(fp)) return;

    int n_buscas;
    scanf("%d", &n_buscas);

    int nroEstacoesFiltradas = 0;
    while (n_buscas--) {
        
        Registro** registros = filtros_multiplos(fp, &nroEstacoesFiltradas);

        if (nroEstacoesFiltradas == 0)
            printf("Registro inexistente.\n");
        else{
            for(int i = 0; i < nroEstacoesFiltradas; i++){
                print_reg(registros[i]);
                reg_free(&registros[i]);
            }
        }
        if(registros != NULL)
            free(registros);
        printf("\n"); 
    }
    fclose(fp);
}