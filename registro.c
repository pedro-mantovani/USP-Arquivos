#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"

/*
Funções e estruturas relacionadas diretamente aos registros
*/

struct registro{
    char removido;
    int proximo;
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    int tamNomeEstacao;
    char* nomeEstacao;
    int tamNomeLinha;
    char* nomeLinha;
};

// Função para criar um registro com valores inicializados como nulos
Registro* criar_registro(){
    // Aloca memória para o registro
    Registro* reg = malloc(sizeof(Registro));
    if(reg == NULL){
        printf("Erro ao alocar memória\n");
        return NULL;
    }
    // Inicializa os campos como nulos
    reg->removido = '0';
    reg->proximo = -1;
    reg->codEstacao = -1;
    reg->codLinha = -1;
    reg->codProxEstacao = -1;
    reg->distProxEstacao = -1;
    reg->codLinhaIntegra = -1;
    reg->codEstIntegra = -1;
    reg->tamNomeEstacao = 0;
    reg->nomeEstacao = NULL;
    reg->tamNomeLinha = 0;
    reg->nomeLinha = NULL;

    return reg;
}

// Função para colocar um registro no arquivo binário
void reg_to_bin(Registro* reg, FILE* fp, long int offset){
    char c;
    int x;
    char* s;
    int bytes_lixo = 43; // Número máximo de bytes de lixo (80 - campos fixos)

    if (fp == NULL || reg == NULL) return;

    // Caso a posição passada seja -1 não é feito o seek e o registro é colocado na posição atual do ponteiro
    if(offset != -1)
        fseek(fp, offset, SEEK_SET); // Caso contrário ele vai para o byte offset passado como parâmetro

    // Coloca os campos do registro no arquivo binário
    c = reg_get_removido(reg);
    fwrite(&c, sizeof(char), 1, fp);

    x = reg_get_proximo(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codEstacao(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codLinha(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codProxEstacao(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_distProxEstacao(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codLinhaIntegra(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codEstIntegra(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_tamNomeEstacao(reg);
    bytes_lixo -= x; // Tira do lixo os bytes do nome da estação
    fwrite(&x, sizeof(int), 1, fp);

    s = reg_get_nomeEstacao(reg);
    if (x > 0 && s != NULL)
        fwrite(s, sizeof(char), x, fp);

    x = reg_get_tamNomeLinha(reg);
    bytes_lixo -= x; // Tira do lixo os bytes da linha
    fwrite(&x, sizeof(int), 1, fp);

    s = reg_get_nomeLinha(reg);
    if (x > 0 && s != NULL)
        fwrite(s, sizeof(char), x, fp);

    // preenche o espaço restante com lixo ('$')
    char* lixo = malloc(sizeof(char)*bytes_lixo);
    memset(lixo, '$', bytes_lixo);
    fwrite(lixo, sizeof(char), bytes_lixo, fp);
    free(lixo);
}

// Função para ler um registro do arquivo binário
Registro* bin_to_reg(FILE* fp) {
    if (fp == NULL) return NULL;

    // Cria um registro
    Registro* reg = criar_registro();
    if(reg == NULL) return NULL;

    // Tenta ler se o registro foi removido
    if (fread(&(reg->removido), sizeof(char), 1, fp) != 1) {
        // Se não conseguir significa que o arquivo acabou, nesse caso, a função retorna um ponteiro NULL
        reg_free(&reg);
        return NULL;
    }

    // Se o registro foi removido ele pula para o próximo e devolve NULL
    if (reg->removido == '1') {
        fseek(fp, 79, SEEK_CUR);
        reg_free(&reg);
        return NULL;
    }

    // 1. Campos fixos iniciais (1 + 4 + 4 + 4 + 4 + 4 + 4 + 4 = 29 bytes)
    fread(&(reg->proximo), sizeof(int), 1, fp);
    fread(&(reg->codEstacao), sizeof(int), 1, fp);
    fread(&(reg->codLinha), sizeof(int), 1, fp);
    fread(&(reg->codProxEstacao), sizeof(int), 1, fp);
    fread(&(reg->distProxEstacao), sizeof(int), 1, fp);
    fread(&(reg->codLinhaIntegra), sizeof(int), 1, fp);
    fread(&(reg->codEstIntegra), sizeof(int), 1, fp);

    // 2. Campos de tamanho variável
    // Lê o tamanho do nome da estação
    fread(&(reg->tamNomeEstacao), sizeof(int), 1, fp);
    // Se o tamanho do nome da estação for maior que zero a string é copiada para o registro
    if (reg->tamNomeEstacao > 0) {
        reg->nomeEstacao = (char*)malloc(reg->tamNomeEstacao + 1); // Aloca memória para a string + '/0'
        fread(reg->nomeEstacao, sizeof(char), reg->tamNomeEstacao, fp); // Lê a string
        reg->nomeEstacao[reg->tamNomeEstacao] = '\0'; // Coloca o terminador '\0'
    } // Caso contrário, o nome da string permanece NULL

    fread(&(reg->tamNomeLinha), sizeof(int), 1, fp);
    if (reg->tamNomeLinha > 0) {
        reg->nomeLinha = (char*)malloc(reg->tamNomeLinha + 1);
        fread(reg->nomeLinha, sizeof(char), reg->tamNomeLinha, fp);
        reg->nomeLinha[reg->tamNomeLinha] = '\0';
    }

    // Ignora o lixo '$' pulando o número de bytes restantes (80 - 37 - tamStrings)
    fseek(fp, 43 - reg->tamNomeEstacao - reg->tamNomeLinha, SEEK_CUR);

    return reg;
}

// Função auxiliar para printar um campo do tipo inteiro
void print_campo_int(int valor) {
    if (valor == -1) printf("NULO");
    else printf("%d", valor);
}

// Função auxiliar para printar um campo do tipo string
void print_campo_str(char* str) {
    if (str == NULL || strlen(str) == 0) printf("NULO");
    else printf("%s", str);
}

// Função para printar um registro
void print_reg(Registro* reg){
    if (reg == NULL) {
        printf("Registro NULL\n");
        return;
    }

    print_campo_int(reg->codEstacao); printf(" ");
    print_campo_str(reg->nomeEstacao); printf(" ");
    print_campo_int(reg->codLinha); printf(" ");
    print_campo_str(reg->nomeLinha); printf(" ");
    print_campo_int(reg->codProxEstacao); printf(" ");
    print_campo_int(reg->distProxEstacao); printf(" ");
    print_campo_int(reg->codLinhaIntegra); printf(" ");
    print_campo_int(reg->codEstIntegra);
    printf("\n");
}

// Função para liberar a memória alocada para um registro
void reg_free(Registro** reg){
    if(*reg == NULL) return;
    if((*reg)->nomeEstacao != NULL)
        free((*reg)->nomeEstacao);
    if((*reg)->nomeLinha != NULL)
        free((*reg)->nomeLinha);
    free(*reg);
    *reg = NULL;
}

/*
Funções de set dos registros
*/
void reg_set_proximo(Registro* reg, int proximo){
    if(reg == NULL) return;
    reg->proximo = proximo;
}

void reg_set_codEstacao(Registro* reg, int codEstacao){
    if(reg == NULL) return;
    reg->codEstacao = codEstacao;
}

void reg_set_codLinha(Registro* reg, int codLinha){
    if(reg == NULL) return;
    reg->codLinha = codLinha;
}

void reg_set_codProxEstacao(Registro* reg, int codProxEstacao){
    if(reg == NULL) return;
    reg->codProxEstacao = codProxEstacao;
}

void reg_set_distProxEstacao(Registro* reg, int distProxEstacao){
    if(reg == NULL) return;
    reg->distProxEstacao = distProxEstacao;
}

void reg_set_codLinhaIntegra(Registro* reg, int codLinhaIntegra){
    if(reg == NULL) return;
    reg->codLinhaIntegra = codLinhaIntegra;
}

void reg_set_codEstIntegra(Registro* reg, int codEstIntegra){
    if(reg == NULL) return;
    reg->codEstIntegra = codEstIntegra;
}

void reg_set_tamNomeEstacao(Registro* reg, int tamNomeEstacao){
    if(reg == NULL) return;
    reg->tamNomeEstacao = tamNomeEstacao;
}

void reg_set_nomeEstacao(Registro* reg, char* nomeEstacao){
    if(reg == NULL) return;

    free(reg->nomeEstacao);

    if(nomeEstacao != NULL){
        reg->nomeEstacao = malloc(strlen(nomeEstacao) + 1);
        strcpy(reg->nomeEstacao, nomeEstacao);
    } else {
        reg->nomeEstacao = NULL;
    }
}

void reg_set_tamNomeLinha(Registro* reg, int tamNomeLinha){
    if(reg == NULL) return;
    reg->tamNomeLinha = tamNomeLinha;
}

void reg_set_nomeLinha(Registro* reg, char* nomeLinha){
    if(reg == NULL) return;

    free(reg->nomeLinha);

    if(nomeLinha != NULL){
        reg->nomeLinha = malloc(strlen(nomeLinha) + 1);
        strcpy(reg->nomeLinha, nomeLinha);
    } else {
        reg->nomeLinha = NULL;
    }
}

void reg_set_removido(Registro* reg, char removido) {
    if (reg != NULL) {
        reg->removido = removido;
    }
}

/*
Funções de get dos registros
*/
char reg_get_removido(Registro* reg){
    if(reg == NULL) return '\0';
    return reg->removido;
}

int reg_get_proximo(Registro* reg){
    if(reg == NULL) return -1;
    return reg->proximo;
}

int reg_get_codEstacao(Registro* reg){
    if(reg == NULL) return -1;
    return reg->codEstacao;
}

int reg_get_codLinha(Registro* reg){
    if(reg == NULL) return -1;
    return reg->codLinha;
}

int reg_get_codProxEstacao(Registro* reg){
    if(reg == NULL) return -1;
    return reg->codProxEstacao;
}

int reg_get_distProxEstacao(Registro* reg){
    if(reg == NULL) return -1;
    return reg->distProxEstacao;
}

int reg_get_codLinhaIntegra(Registro* reg){
    if(reg == NULL) return -1;
    return reg->codLinhaIntegra;
}

int reg_get_codEstIntegra(Registro* reg){
    if(reg == NULL) return -1;
    return reg->codEstIntegra;
}

int reg_get_tamNomeEstacao(Registro* reg){
    if(reg == NULL) return 0;
    return reg->tamNomeEstacao;
}

char* reg_get_nomeEstacao(Registro* reg){
    if(reg == NULL) return NULL;
    return reg->nomeEstacao;
}

int reg_get_tamNomeLinha(Registro* reg){
    if(reg == NULL) return 0;
    return reg->tamNomeLinha;
}

char* reg_get_nomeLinha(Registro* reg){
    if(reg == NULL) return NULL;
    return reg->nomeLinha;
}