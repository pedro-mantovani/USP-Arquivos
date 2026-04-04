#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"

/*
Funções e estruturas relacionadas diretamente aos registros
*/

struct header{
    char status;
    int topo;
    int proxRRN;
    int nroEstacoes;
    int nroParesEstacao;
};

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

Registro* criar_registro(){
    Registro* reg = malloc(sizeof(Registro));
    if(reg == NULL){
        printf("Erro ao alocar memória\n");
        return NULL;
    }
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

Header* criar_header(){
    Header* head = malloc(sizeof(Header));
    if(head == NULL){
        printf("Erro ao alocar memória\n");
        return NULL;
    }
    
    head->status = '0';
    head->topo = -1;
    head->proxRRN = 0;
    head->nroEstacoes = 0;
    head->nroParesEstacao = 0;
    
    return head;
}

// Funções de set dos registros
void remover_registro(Registro* reg){
    if(reg == NULL) return;
    reg->removido = 1;
}

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

// Funções de get dos registros
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

// Funções de get do header
char header_get_status(const Header* h){
    if(h == NULL) return '0';
    return h->status;
}

int header_get_topo(const Header* h){
    if(h == NULL) return -1;
    return h->topo;
}

int header_get_proxRRN(const Header* h){
    if(h == NULL) return -1;
    return h->proxRRN;
}

int header_get_nroEstacoes(const Header* h){
    if(h == NULL) return -1;
    return h->nroEstacoes;
}

int header_get_nroParesEstacao(const Header* h){
    if(h == NULL) return -1;
    return h->nroParesEstacao;
}

// Funções de set do header
void header_set_status(Header* h, char status){
    if(h == NULL) return;
    h->status = status;
}

void header_set_topo(Header* h, int topo){
    if(h == NULL) return;
    h->topo = topo;
}

void header_set_proxRRN(Header* h, int proxRRN){
    if(h == NULL) return;
    h->proxRRN = proxRRN;
}

void header_set_nroEstacoes(Header* h, int nroEstacoes){
    if(h == NULL) return;
    h->nroEstacoes = nroEstacoes;
}

void header_set_nroParesEstacao(Header* h, int nroParesEstacao){
    if(h == NULL) return;
    h->nroParesEstacao = nroParesEstacao;
}

void print_reg(Registro* reg){
    if (reg == NULL) {
        printf("Registro NULL\n");
        return;
    }

    printf("=== Registro ===\n");

    printf("removido: %c\n", reg->removido);
    printf("proximo: %d\n", reg->proximo);
    printf("codEstacao: %d\n", reg->codEstacao);
    printf("codLinha: %d\n", reg->codLinha);
    printf("codProxEstacao: %d\n", reg->codProxEstacao);
    printf("distProxEstacao: %d\n", reg->distProxEstacao);
    printf("codLinhaIntegra: %d\n", reg->codLinhaIntegra);
    printf("codEstIntegra: %d\n", reg->codEstIntegra);

    printf("tamNomeEstacao: %d\n", reg->tamNomeEstacao);
    printf("nomeEstacao: %s\n", 
        (reg->nomeEstacao != NULL) ? reg->nomeEstacao : "(null)");

    printf("tamNomeLinha: %d\n", reg->tamNomeLinha);
    printf("nomeLinha: %s\n", 
        (reg->nomeLinha != NULL) ? reg->nomeLinha : "(null)");

    printf("================\n");
}

void reg_free(Registro** reg){
    if(*reg == NULL) return;
    if((*reg)->nomeEstacao != NULL)
        free((*reg)->nomeEstacao);
    if((*reg)->nomeLinha != NULL)
        free((*reg)->nomeLinha);
    free(*reg);
    *reg = NULL;
}

void head_free(Header** head){
    if(*head == NULL) return;
    free(*head);
    *head = NULL;
}

Registro* ler_registro(FILE* f){
    Registro* reg = NULL;
    return reg;
}

//funcionalidade 2: escrever todos os registros do arquivo binário
Registro* bin_to_reg(FILE* fp) {
    if (fp == NULL) return NULL;

    long pos_inicial = ftell(fp); // Guarda onde o registro começou
    Registro* reg = criar_registro();
    if (reg == NULL) return NULL;

    // 1. Campos fixos iniciais (1 + 4 + 4 + 4 + 4 + 4 + 4 + 4 = 29 bytes)
    if (fread(&(reg->removido), sizeof(char), 1, fp) != 1) {
        reg_free(&reg);
        return NULL;
    }
    fread(&(reg->proximo), sizeof(int), 1, fp);
    fread(&(reg->codEstacao), sizeof(int), 1, fp);
    fread(&(reg->codLinha), sizeof(int), 1, fp);
    fread(&(reg->codProxEstacao), sizeof(int), 1, fp);
    fread(&(reg->distProxEstacao), sizeof(int), 1, fp);
    fread(&(reg->codLinhaIntegra), sizeof(int), 1, fp);
    fread(&(reg->codEstIntegra), sizeof(int), 1, fp);

    // 2. Nome Estação (Tamanho + String)
    fread(&(reg->tamNomeEstacao), sizeof(int), 1, fp);
    if (reg->tamNomeEstacao > 0) {
        reg->nomeEstacao = (char*)malloc(reg->tamNomeEstacao + 1);
        fread(reg->nomeEstacao, sizeof(char), reg->tamNomeEstacao, fp);
        reg->nomeEstacao[reg->tamNomeEstacao] = '\0';
    }

    // 3. Nome Linha (Tamanho + String)
    fread(&(reg->tamNomeLinha), sizeof(int), 1, fp);
    if (reg->tamNomeLinha > 0) {
        reg->nomeLinha = (char*)malloc(reg->tamNomeLinha + 1);
        fread(reg->nomeLinha, sizeof(char), reg->tamNomeLinha, fp);
        reg->nomeLinha[reg->tamNomeLinha] = '\0';
    }

    // 4. GARANTIA DOS 80 BYTES: Salta para o início do próximo registro
    // Isso ignora o lixo '$' corretamente, não importa quantos bytes sobraram
    fseek(fp, pos_inicial + 80, SEEK_SET);

    return reg;
}