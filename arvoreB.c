#include "arvoreB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
Sctructs para o cabeçalho e para os nós da árvore
*/

struct arv_head{
    char status;
    int noRaiz;
    int topo;
    int proxRRN;
    int nroNos;
};

struct arv_no{
    char removido;
    int proximo;
    int tipoNo;
    int nroChaves;
    int* chaves;
    int* offsets;
    int* filhos;
};

/*
Funções para criar e inicializar as structs
*/

Arv_head* criar_arv_head(){
    // Aloca memória para o cabeçalho
    Arv_head* head = malloc(sizeof(Arv_head));
    if(head == NULL){
        printf("Erro ao alocar memória\n");
        return NULL;
    }
    
    // Inicializa os campos da struct
    head->status = '0';  // 0 = inconsistente, 1 = consistente
    head->noRaiz = -1;
    head->topo = -1;
    head->proxRRN = 0;
    head->nroNos = 0;
    
    return head;
}

Arv_no* criar_arv_no(){
    // Aloca memória para o nó
    Arv_no* no = malloc(sizeof(Arv_no));
    if(no == NULL){
        printf("Erro ao alocar memória\n");
        return NULL;
    }

    // Aloca memória para as chaves e os offsets
    no->chaves = malloc(sizeof(int)*nro_chaves);
    no->offsets = malloc(sizeof(int)*nro_chaves);
    no->filhos = malloc(sizeof(int)*(nro_chaves+1));
    if(no->chaves == NULL || no->offsets == NULL || no->filhos == NULL){
        printf("Erro ao alocar memória\n");
        arv_no_free(&no);
        return NULL;
    }

    // Inicializa os vetores com -1
    memset(no->chaves, -1, sizeof(int)*nro_chaves);
    memset(no->offsets, -1, sizeof(int)*nro_chaves);
    memset(no->filhos, -1, sizeof(int)*(nro_chaves+1));
    
    // Inicializa os campos da struct
    no->removido = '0';  // 0 = não removido, 1 = removido
    no->proximo = -1;
    no->tipoNo = -1; // -1 = folha, 0 = raíz, 1 = intermediário
    no->nroChaves = 0;
    
    return no;
}

/*
Funções para ler de um arquivo binário
*/

/*
Função para ler o cabeçalho de uma árvore B
Sempre cria uma struct de cabeçalho
Sempre coloca o ponteiro no início do arquivo antes de ler
Recebe como parâmetro apenas o ponteiro para o arquivo
*/
Arv_head* bin_to_arv_head(FILE* fp) {
    if (fp == NULL) return NULL;

    Arv_head* head = criar_arv_head(); // Aloca a struct
    if(head == NULL) return NULL;
    
    fseek(fp, 0, SEEK_SET); // Coloca o ponteiro no início do arquivo    

    // Preenche os campos da struct
    fread(&(head->status), sizeof(char), 1, fp);
    fread(&(head->noRaiz), sizeof(int), 1, fp);
    fread(&(head->topo), sizeof(int), 1, fp);
    fread(&(head->proxRRN), sizeof(int), 1, fp);
    fread(&(head->nroNos), sizeof(int), 1, fp);

    return head;
}

/*
Função para ler um nó de árvore B
Sempre cria uma struct de nó
Recebe como parâmetros:
Ponteiro para o arquivo
RRN do nó buscado (-1 para posição atual)
*/
Arv_no* bin_to_arv_no(FILE* fp, int RRN) {
    if (fp == NULL) return NULL;

    // Cria um nó
    Arv_no* no = criar_arv_no();
    if(no == NULL) return NULL;

    // Coloca o ponteiro na posição correta
    if(RRN != -1){
        long int offset = RRN*tam_arv_no + tam_arv_head;
        fseek(fp, offset, SEEK_SET);
    }

    // Tenta ler se o nó foi removido
    if (fread(&(no->removido), sizeof(char), 1, fp) != 1) {
        // Se não conseguir significa que o arquivo acabou, nesse caso, a função retorna um ponteiro NULL
        arv_no_free(&no);
        return NULL;
    }

    // Se o registro foi removido ele pula para o próximo e devolve NULL
    if (no->removido == '1') {
        fseek(fp, 79, SEEK_CUR);
        arv_no_free(&no);
        return NULL;
    }

    // 1. Campos iniciais
    fread(&(no->proximo), sizeof(int), 1, fp);
    fread(&(no->tipoNo), sizeof(int), 1, fp);
    fread(&(no->nroChaves), sizeof(int), 1, fp);
    
    // 2. Campos que dependem do nroChaves
    for(int i = 0; i < no->nroChaves; i++){
        fread(&(no->chaves[i]), sizeof(int), 1, fp);
        fread(&(no->offsets[i]), sizeof(int), 1, fp);
    }
    
    // Pula os as chaves nulas
    fseek(fp, 8*(nro_chaves-no->nroChaves), SEEK_CUR);

    // POO: caso nroChaves == 0 ele vai ler um int a toa, será que vale o if? 
    for(int i = 0; i < no->nroChaves + 1; i++){
        fread(&(no->filhos[i]), sizeof(int), 1, fp);
    }

    // Pula os filhos nulos
    fseek(fp, 4*(nro_chaves-no->nroChaves), SEEK_CUR);

    return no;
}

/*
Funções para escrever no arquivo binário
*/

/*
Escreve o cabeçalho de uma árvore do arquivo binário
Sempre coloca o ponteiro no início do arquivo antes de ler
Recebe como parâmetros:

Ponteiro para o arquivo

Struct de cabeçalho
*/
void arv_head_to_bin(FILE* fp, Arv_head* head){
    if (fp == NULL) return; // Verifica se ponteiro para o arquivo é válido

    fseek(fp, 0, SEEK_SET); // Coloca o ponteiro no início do arquivo

    // Escreve os campos da struct no arquivo
    fwrite(&(head->status), sizeof(char), 1, fp);
    fwrite(&(head->noRaiz), sizeof(int), 1, fp);
    fwrite(&(head->topo), sizeof(int), 1, fp);
    fwrite(&(head->proxRRN), sizeof(int), 1, fp);
    fwrite(&(head->nroNos), sizeof(int), 1, fp);
}

/*
Escreve um nó em um arquivo de árvore binária, 
recebe como parâmetros: 

Ponteiro para o arquivo 

Struct nó que será colocada no arquivo 

Byte offset do nó (-1 para posição atual)
*/
void arv_no_to_bin(FILE* fp, Arv_no* no, long int offset){
    if (fp == NULL || no == NULL) return;

    // Caso a posição passada seja -1 não é feito o seek e o registro é colocado na posição atual do ponteiro
    if(offset != -1)
        fseek(fp, offset, SEEK_SET); // Caso contrário ele vai para o byte offset passado como parâmetro

    // Escreve os campos fixos
    fwrite(&(no->removido), sizeof(char), 1, fp);
    fwrite(&(no->proximo), sizeof(int), 1, fp);
    fwrite(&(no->tipoNo), sizeof(int), 1, fp);
    fwrite(&(no->nroChaves), sizeof(int), 1, fp);

    // Escreve as chaves
    for(int i = 0; i < nro_chaves; i++){
        fwrite(&(no->chaves[i]), sizeof(int), 1, fp);
        fwrite(&(no->offsets[i]), sizeof(int), 1, fp);
    }

    // Escreve os filhos
    for(int i = 0; i < nro_chaves+1; i++){
        fwrite(&(no->filhos[i]), sizeof(int), 1, fp);
    }
}

/*
Funções de get e set
*/

int arv_head_get_nroNos(const Arv_head* h){
    if(h == NULL) return -1;
    return h->nroNos;
}

/*
Funções para liberar memória
*/
void arv_no_free(Arv_no** no){
    // Se o nó já é nulo retorna
    if(*no == NULL) return;
    // Libera os vetores internos caso existam
    if((*no)->chaves != NULL) free((*no)->chaves);
    if((*no)->offsets != NULL) free((*no)->offsets);
    if((*no)->filhos != NULL) free((*no)->filhos);
    
    // Libera o nó
    free(*no);
    *no = NULL;
}

void arv_head_free(Arv_head** head){
    // Se o cabeçalho já é nulo retorna
    if(*head == NULL) return;

    // Libera o cabeçalho
    free(*head);
    *head = NULL;
}
