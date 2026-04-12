#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

struct header{
    char status;
    int topo;
    int proxRRN;
    int nroEstacoes;
    int nroParesEstacao;
};

// Função para criar um cabeçalho com valores inicializados
Header* criar_header(){
    // Aloca memória para o cabeçalho
    Header* head = malloc(sizeof(Header));
    if(head == NULL){
        printf("Erro ao alocar memória\n");
        return NULL;
    }
    
    // Inicializa os campos da struct
    head->status = '0';  // 0 = inconsistente, 1 = consistente
    head->topo = -1;
    head->proxRRN = 0;
    head->nroEstacoes = 0;
    head->nroParesEstacao = 0;
    
    return head;
}

// Função para colocar um cabeçalho no arquivo binário
void header_to_bin(FILE* fp, const Header* head){
    if (fp == NULL || head == NULL) return;

    fseek(fp, 0, SEEK_SET); // Coloca o ponteiro no início do arquivo

    // Escreve os campos da struct no arquivo
    fwrite(&(head->status), sizeof(char), 1, fp);
    fwrite(&(head->topo), sizeof(int), 1, fp);
    fwrite(&(head->proxRRN), sizeof(int), 1, fp);
    fwrite(&(head->nroEstacoes), sizeof(int), 1, fp);
    fwrite(&(head->nroParesEstacao), sizeof(int), 1, fp);
}

// Função para ler o cabeçalho de um arquivo binário
Header* bin_to_header(FILE* fp) {
    Header* head = criar_header(); // Aloca a struct
    fseek(fp, 0, SEEK_SET); // Coloca o ponteiro no início do arquivo

    // Preenche os campos da struct
    fread(&(head->status), sizeof(char), 1, fp);
    fread(&(head->topo), sizeof(int), 1, fp);
    fread(&(head->proxRRN), sizeof(int), 1, fp);
    fread(&(head->nroEstacoes), sizeof(int), 1, fp);
    fread(&(head->nroParesEstacao), sizeof(int), 1, fp);

    return head;
}

// Função para para imprimir os campos do cabeçalho
// Usada apenas para debug
void header_print(Header* h){
    printf("Status: %c\n", h->status);
    printf("Topo: %d\n", h->topo);
    printf("Prox RRN: %d\n", h->proxRRN);
    printf("Numero de Estacoes: %d\n", h->nroEstacoes);
    printf("Numero de Pares de Estacao: %d\n", h->nroParesEstacao);
}

// Função para liberar a memória alocada para o cabeçalho
void header_free(Header** head){
    if(*head == NULL) return;
    free(*head);
    *head = NULL;
}

/*
Funções de get do header
*/
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

/*
Funções de set do header
*/
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