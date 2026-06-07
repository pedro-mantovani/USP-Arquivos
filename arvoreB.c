#include "arvoreB.h"
#include "utilitarias.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

// Ver se tem um nome melhor
typedef struct chave_{
    int chave;
    int offset;
    int filho;
} Chave;

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
        long int offset = arv_RRN_to_offset(RRN);
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

RRN do nó (-1 para posição atual)
*/
void arv_no_to_bin(FILE* fp, Arv_no* no, int RRN){
    if (fp == NULL || no == NULL) return;

    // Caso o RRN passado seja -1 não é feito o seek e o registro é colocado na posição atual do ponteiro
    if(RRN != -1){
        long int offset = arv_RRN_to_offset(RRN);
        fseek(fp, offset, SEEK_SET); // Caso contrário ele vai para o byte offset passado como parâmetro
    }
        
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


/*
Funções de debug
*/

void print_no(Arv_no* no){
    int i;

    if (no == NULL) {
        printf("Nó nulo\n");
        return;
    }

    printf("===== NO =====\n");

    printf("removido  : %c\n", no->removido);
    printf("proximo   : %d\n", no->proximo);
    printf("tipoNo    : %d\n", no->tipoNo);
    printf("nroChaves : %d\n", no->nroChaves);

    // chaves
    printf("chaves    : [ ");
    for (i = 0; i < nro_chaves; i++) {
        printf("%d ", no->chaves[i]);
    }
    printf("]\n");

    // offsets
    printf("offsets   : [ ");
    for (i = 0; i < nro_chaves; i++) {
        printf("%d ", no->offsets[i]);
    }
    printf("]\n");

    // filhos
    printf("filhos    : [ ");
    for (i = 0; i < ordem; i++) {
        printf("%d ", no->filhos[i]);
    }
    printf("]\n");

    printf("================\n");
}

/*
Função de remoção simples, ou seja: 
remoção em nó folha que não gera underflow
*/
void remocao_simples(FILE*fp_arvore, Arv_no* no, int i, int rrn_atual, bool escrita_bin, bool eh_raiz){
    // Shifta as chaves, offsets e filhos(se não for raíz)
    shift_back(no->chaves, i, no->nroChaves);
    shift_back(no->offsets, i, no->nroChaves);
    if(!eh_raiz){
        shift_back(no->filhos, i+1, no->nroChaves + 1);
    }
    
    // Decrementa o número de chaves do nó
    no->nroChaves --;
    
    // Reescreve o nó no binário
    if(escrita_bin){
        arv_no_to_bin(fp_arvore, no, rrn_atual);
    }
}

/*
Função para trocar o nó de posição com seu sucessor
Ou seja, o menor filho da direita do nó
Escreve em disco o nó de origem e 
atualiza o nó e o RRN passados como parâmetro para o nó raíz
Dessa forma a remoção pode ocorrer antes da escrita em disco
Evitando reescritas desnecessárias

Parâmetros:
Arquivo
Nó atual
*/
int swap(FILE* fp_arvore, Arv_no* atual, int RRN_atual, int i_atual){
    // Vai para a direita do nó atual
    int RRN_folha = atual->filhos[i_atual+1];
    Arv_no* prox = bin_to_arv_no(fp_arvore, RRN_folha);
    // Enquanto não for nó folha vai para o primeiro filho
    while (prox->tipoNo != -1){
        RRN_folha = prox->filhos[0];
        arv_no_free(&prox);
        prox = bin_to_arv_no(fp_arvore, RRN_folha);
    }

    // Salva a chave que será removida
    int chave_rem = atual->chaves[i_atual];
    
    // Sobe a chave sucessora
    atual->chaves[i_atual] = prox->chaves[0];
    atual->offsets[i_atual] = prox->offsets[0];
    
    // Atualiza o arquivo
    arv_no_to_bin(fp_arvore, atual, RRN_atual);

    // Coloca a chave a ser removida no nó folha
    prox->chaves[0] = chave_rem;
    
    // Atualiza o nó atual para a folha
    *atual = *prox;

    // Retorna o RRN atual
    return RRN_folha;
}

/*
Função para transformar uma chave pai + seus filhos em um vetor
Usado na:
Redistribuição
Concatenação
Inserção

Retorna o vetor final e altera o conteúdo da variável tamanho para o tamanho do vetor

Parâmetros:
Nó pai
Índice da chave pai no vetor pai
Nó esquerdo
Nó direito
*/
Chave* vectorize(Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir, int* tam, bool eh_raiz){
    int n_esq, n_dir, total;
    n_esq = f_esq->nroChaves;
    n_dir = f_dir->nroChaves;
    total = n_dir + n_esq + 1;

    // Cria um vetor auxiliar para armazenar as chaves
    Chave* vec;
    if(eh_raiz){
        vec = (Chave*)malloc(sizeof(Chave)* total);
    }else{
        // Se não for raíz tem que armazenar um espaço a mais para salvar os filhos
        vec = (Chave*)malloc(sizeof(Chave)* (1+total));
    }
    
    
    /*Preenche o vetor auxiliar*/
    // Coloca os elementos do nó esquerdo
    for(int i = 0; i < n_esq; i++){
        vec[i].chave = f_esq->chaves[i];
        vec[i].offset = f_esq->offsets[i];
        if(!eh_raiz){ // Se não é raíz salva os filhos
            vec[i].filho = f_esq->filhos[i];
        }
    }
    if(!eh_raiz){ // Se não é raíz salva o filho mais a direita do nó à esquerda
        vec[n_esq].filho = f_esq->filhos[n_esq];
    }

    // Coloca o elemento do nó pai
    vec[n_esq].chave = pai->chaves[i_pai];
    vec[n_esq].offset = pai->offsets[i_pai];
    
    // Coloca os elementos do nó direito
    for(int i = n_esq+1; i < total; i++){
        vec[i].chave = f_dir->chaves[i - n_esq -1];
        vec[i].offset = f_dir->offsets[i - n_esq -1];
        if(!eh_raiz){ // Se não é raíz salva os filhos
            vec[i].filho = f_dir->filhos[i - n_esq];
        }
    }
    if(!eh_raiz){ // Se não é raíz salva o filho mais a direita do nó à direita
        vec[total].filho = f_dir->filhos[total];
    }

    *tam = total;

    return vec;
}

/*
Função para redistribuir 3 nós
Recebe o nó pai, filho esquerdo e filho direito
Note que a única diferença entre uma redistribuição à direita e à esquerda
é a posição relativa do nó com relação ao pai, ou seja, basta alterar os parâmetros

Todas as chaves são colocadas em em um vetor de (chaves, offsets)
Os índices 0 a ceil((n-2)/2) ficam na esquerda 
O índice i = ceil(n/2) fica no nó pai
Os índices ceil(n/2) até o fim ficam na direita

Faz todas as alterações e já escreve em disco os 3 nós

Parâmetros:
Nó pai
Nó filho esquerdo
Nó filho direito 
*/
void redistribuicao(FILE* fp_arvore, Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir, int RRN_esq, int RRN_dir, int RRN_pai){
    // Transforma o conjunto chave pai + filhos em um vetor
    int tam;
    Chave* vec = vectorize(pai, i_pai, f_esq, f_dir, &tam, true);

    // Cria um nó auxiliar temporário inicializado
    Arv_no* temp = criar_arv_no();

    // Define o tamanho   
    int tam_esq = (int)ceil((tam-2)/2.0);

    // Coloca os elementos no nó esquerdo
    for(int i = 0; i < tam_esq; i++){
        temp->chaves[i] = vec[i].chave;
        temp->offsets[i] = vec[i].offset;
    }
    temp->nroChaves = tam_esq;

    // Escreve em disco e libera a memória
    arv_no_to_bin(fp_arvore, temp, RRN_esq);
    arv_no_free(&temp);
    arv_no_free(&f_esq);

    // Edita o elemento i_pai do nó pai
    pai->chaves[i_pai] = vec[tam_esq].chave;
    pai->offsets[i_pai] = vec[tam_esq].offset;

    // Escreve em disco
    arv_no_to_bin(fp_arvore, pai, RRN_pai);
    arv_no_free(&pai);

    // Cria um novo vetor temporário
    temp = criar_arv_no();

    // Coloca os elementos no nó direito
    for(int i = tam_esq+1, j = 0; i < tam; i++, j++){
        temp->chaves[j] = vec[i].chave;
        temp->offsets[j] = vec[i].offset;
    }
    temp->nroChaves = tam - tam_esq - 1;

    // Escreve em disco e libera as memórias
    arv_no_to_bin(fp_arvore, temp, RRN_dir);
    arv_no_free(&temp);
    arv_no_free(&f_dir);
    free(vec);
}

/*
Função para concatenar dois filhos e a chave pai

Note que a única diferença entre uma redistribuição à direita e à esquerda
é a posição relativa do nó com relação ao pai, ou seja, basta passar alterar alterar os parâmetros
*/
void merge(FILE* fp_arvore, Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir, int RRN_esq, int RRN_dir, int RRN_pai, int* topo){
    // Transforma o conjunto chave pai + filhos em um vetor
    int tam;
    Chave* vec = vectorize(pai, i_pai, f_esq, f_dir, &tam, false);

    // Cria um nó com o conjunto
    Arv_no* temp = criar_arv_no();
    for(int i = 0; i < tam; i++){
        temp->chaves[i] = vec[i].chave;
        temp->offsets[i] = vec[i].offset;
        temp->filhos[i] = vec[i].filho;
    }
    temp->filhos[tam] = vec[tam].filho;
    temp->nroChaves = tam;

    // Coloca na esquerda
    printf("Nó da esquerda, RRN %d:\n", RRN_esq);
    print_no(temp);
    //arv_no_to_bin(fp_arvore, temp, RRN_esq);

    // Marca o nó da direita como removido e atualiza o topo da pilha
    // Note que isso é feito diretamente em disco, dessa forma, todo conteúdo anterior ao merge do nó se mantém
    //char removido = '1';
    //fseek(fp_arvore, arv_RRN_to_offset(RRN_dir), SEEK_SET); // Vai para o primeiro byte offset do registro (que é o removido)
    //fwrite(&removido, sizeof(char), 1, fp_arvore); // Marca como removido
    //fwrite(topo, sizeof(int), 1, fp_arvore); // Coloca nos próximos 4 bytes o topo da pilha
    //*topo = RRN_dir; // Atualiza o topo da pilha
    
    // Libera a memória
    arv_no_free(&temp);
    arv_no_free(&f_dir);
    arv_no_free(&f_esq);
    free(vec);

    // Remove a chave pai no nó pai, visto que agora ela está no nó filho 
    // Porém, não escreve o nó arquivo pois ainda pode ter alteração (caso de underflow)
    remocao_simples(fp_arvore, pai, i_pai, RRN_pai, false, false);
    printf("Nó pai, RRN %d:\n", RRN_pai);
    print_no(pai);

    // Agora a função que chamou vai decidir se aconteceu um underflow com o pai ou não, caso tenha acontecido ele chama a função de novo
}

// Função auxiliar recursiva para remover um nó
bool arv_remocao_aux(FILE* fp_arvore, int rrn_atual, int rrn_anterior, int i_anterior, int chave, int* topo) {
    // Condição de parada: Chegou em um nó folha e não encontrou a chave
    if (rrn_atual == -1) {
        printf("Nó não encontrado\n");
        return 0; // Não encontrou
    }

    // Carrega o nó atual para a memória RAM
    Arv_no* no = bin_to_arv_no(fp_arvore, rrn_atual);
    if (no == NULL) {
        printf("Erro no processamento do arquivo\n");
        return 0;      // Falha na leitura ou nó inexistente
    }

    // Busca sequencial dentro do nó
    int i = 0;
    while (i < no->nroChaves && chave > no->chaves[i]) {
        i++;
    }

    // Condição de parada: Encontrou a chave no nó atual
    if (i < no->nroChaves && chave == no->chaves[i]) {
        // Se não for folha, troca de posição com o sucessor
        if(no->tipoNo != -1){
            rrn_atual = swap(fp_arvore, no, rrn_atual, i);
            i = 0; // Agora a chave removida está na posição 0
            // Ainda não escreve o nó no arquivo pq ele vai ser alterado
        }

        // Caso não tenha overflow (casos 1 e 2)
        if(no->nroChaves > min_chaves){
            remocao_simples(fp_arvore, no, i, rrn_atual, true, true);
            printf("Remoção simples realizada com sucesso!\n");
            return 1;
        }

        // Caso tenha underflow
        // Faz a remoção simples com shiftada, mas não escreve o nó no arquivo, pois ele será modificado
        remocao_simples(fp_arvore, no, i, rrn_atual, false, true);

        /*
        2. Tenta redistribuição à direita
        Note que nessa configuração:
        - Nó atual é o nó da esquerda
        - Nó da direita é o filho i_anterior + 1
        */
        Arv_no* pai = bin_to_arv_no(fp_arvore, rrn_anterior);
        // Se o filho da direita é ele mesmo, siginifica que é impossível a redistribuição à direita
        int rrn_dir = pai->filhos[i_anterior+1];
        Arv_no* f_dir;
        if(rrn_dir != rrn_atual){
            f_dir = bin_to_arv_no(fp_arvore, rrn_dir);
            if(f_dir->nroChaves > min_chaves){
                redistribuicao(fp_arvore, pai, i_anterior, no, f_dir, rrn_atual, rrn_dir, rrn_anterior);
                printf("Redistribuição à direita realizada com sucesso\n");
                return 1;
            }
        }
        
        /*
        2. Tenta redistribuição à esquerda
        Note que nessa configuração:
        - Nó atual é o nó da direita
        - Nó da esquerda é o filho i_anterior - 1
        */
        // Se ele tem um filho esquerdo
        Arv_no* f_esq;
        if(i_anterior != 0){
            // Tenta fazer redistribuição à esquerda
            arv_no_free(&f_dir); // Libera o filho direito que não vai ser usado
            int rrn_esq = pai->filhos[i_anterior-1];
            f_esq = bin_to_arv_no(fp_arvore, rrn_esq);
            if(f_esq->nroChaves > min_chaves){
                redistribuicao(fp_arvore, pai, i_anterior-1, f_esq, no, rrn_esq, rrn_atual, rrn_anterior);
                printf("Redistribuição à esquerda realizada com sucesso\n");
                return 1;
            }
            // Se não funcionar faz concatenação à esquerda
            else{
                merge(fp_arvore, pai, i_anterior-1, f_esq, no, rrn_esq, rrn_atual, rrn_anterior, topo);
                printf("Merge feito à esquerda\n");
                return 1;
            }
        }

        /*
        Se nada funcionar concatena à direita
        */
        merge(fp_arvore, pai, i_anterior, no, f_dir, rrn_atual, rrn_dir, rrn_anterior, topo);
        printf("Merge feito à direita\n");
        return 1;
    }

    // Se não encontrou, desce na árvore pelo ponteiro (filho) adequado
    int rrn_filho = no->filhos[i];
    
    // Libera o nó atual da memória RAM antes de fazer a chamada recursiva
    arv_no_free(&no);

    // Chamada recursiva para o nó filho
    return arv_remocao_aux(fp_arvore, rrn_filho, rrn_atual, i, chave, topo);
}

/*
Função para remoção da árvore B

Cuida dos 6 casos possíveis de remoção:

1. Remoção de uma chave em um nó folha, sem
causar underflow
2. Remoção de uma chave em um nó não folha
3. Remoção de uma chave em um nó, causando
underflow
4. Remoção de uma chave em um nó, causando
underflow e a redistribuição não pode ser aplicada
5. Underflow no nó pai causado pela remoção de
uma chave em um nó filho
6. Diminuição da altura da árvore

Parâmetros:
Arquivo
Chave a ser removida
*/
void remover_arv(FILE* fp_arvore, int chave){
    if (fp_arvore == NULL) return;

    // Lê o cabeçalho
    Arv_head* head = bin_to_arv_head(fp_arvore);
    if (head == NULL) return;

    // Verifica se o arquivo de índice está em estado consistente
    if (head->status == '0') {
        printf("Falha no processamento do arquivo.\n");
        arv_head_free(&head);
        return;
    }

    // Se a árvore estiver vazia
    if (head->nroNos == 0) {
        return; 
    }

    // Remove o(s) nó(s)
    // POO
    if(arv_remocao_aux(fp_arvore, head->noRaiz, -1, -1, chave, &(head->topo))){
        printf("Nó removido com sucesso\n");
    }else{
        printf("Falha ao remover o nó\n");
    }

    // Atualiza o cabeçalho
    // POO

    // Coloca o cabeçalho de volta no arquivo
    // POO

    // Libera a memória
    arv_head_free(&head);

    // Fecha o arquivo
    fclose(fp_arvore);
}