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

Header* bin_to_header(FILE* fp) {
    Header* head = criar_header(); // Aloca a struct
    fseek(fp, 0, SEEK_SET);

    char status;
    int topo, proxRRN, nroEst, nroPares;

    fread(&status, sizeof(char), 1, fp);
    fread(&topo, sizeof(int), 1, fp);
    fread(&proxRRN, sizeof(int), 1, fp);
    fread(&nroEst, sizeof(int), 1, fp);
    fread(&nroPares, sizeof(int), 1, fp);

    header_set_status(head, status);
    header_set_topo(head, topo);
    header_set_proxRRN(head, proxRRN);
    header_set_nroEstacoes(head, nroEst);
    header_set_nroParesEstacao(head, nroPares);

    return head;
}

void header_to_bin(FILE* fp, const Header* head){
    if (fp == NULL || head == NULL) return;

    fseek(fp, 0, SEEK_SET);

    char status = header_get_status(head);
    int topo = header_get_topo(head);
    int proxRRN = header_get_proxRRN(head);
    int nroEstacoes = header_get_nroEstacoes(head);
    int nroPares = header_get_nroParesEstacao(head);

    fwrite(&status, sizeof(char), 1, fp);
    fwrite(&topo, sizeof(int), 1, fp);
    fwrite(&proxRRN, sizeof(int), 1, fp);
    fwrite(&nroEstacoes, sizeof(int), 1, fp);
    fwrite(&nroPares, sizeof(int), 1, fp);
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

void head_print(Header* h){
    printf("Status: %c\n", h->status);
    printf("Topo: %d\n", h->topo);
    printf("Prox RRN: %d\n", h->proxRRN);
    printf("Numero de Estacoes: %d\n", h->nroEstacoes);
    printf("Numero de Pares de Estacao: %d\n", h->nroParesEstacao);
}