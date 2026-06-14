#include "arvoreB.h"
#include "utilitarias.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// /*
// Sctructs para o cabeçalho e para os nós da árvore
// */

// struct arv_head{
//     char status;
//     int noRaiz;
//     int topo;
//     int proxRRN;
//     int nroNos;
// };

// struct arv_no{
//     char removido;
//     int proximo;
//     int tipoNo;
//     int nroChaves;
//     int* chaves;
//     int* offsets;
//     int* filhos;
// };

// // Ver se tem um nome melhor
// typedef struct chave_{
//     int chave;
//     int offset;
//     int filho;
// } Chave;

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
    head->status = '0';     // 0 = inconsistente, 1 = consistente
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

    for (int i = 0; i < nro_chaves; i++) {
        no->chaves[i] = -1;
        no->offsets[i] = -1;
        no->filhos[i] = -1;
    }
    no->filhos[nro_chaves] = -1;

    // Inicializa os vetores com -1
    memset(no->chaves, -1, sizeof(int)*nro_chaves);
    memset(no->offsets, -1, sizeof(int)*nro_chaves);
    memset(no->filhos, -1, sizeof(int)*(nro_chaves+1));
    
    // Inicializa os campos da struct
    no->removido = '0';     // 0 = não removido, 1 = removido
    no->proximo = -1;
    no->tipoNo = -1;        // -1 = folha, 0 = raíz, 1 = intermediário
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

    // Se o nó foi removido ele pula para o próximo e devolve NULL
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

    // Caso o RRN passado seja -1 não é feito o seek e o nó é colocado na posição atual do ponteiro
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

char arv_head_get_status(const Arv_head* h) {
    if (h == NULL) return '0'; // Por segurança, se for nulo, trata como inconsistente
    return h->status;
}

void arv_head_set_status(Arv_head* h, char status) {
    if (h != NULL) {
        h->status = status;
    }
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

// /*
// Funções usadas na remoção
// */

// typedef struct{
//     bool underflow;
//     int offset;
//     Arv_no* buffer;
// } resultadoRemocao;

// /*
// Função de remoção simples, ou seja, simplesmente shifta os elementos
// */
// void remocao_simples(FILE*fp_arvore, Arv_no* no, int i, int rrn_atual, bool eh_folha){
//     // Shifta as chaves, offsets e filhos(se não for raíz)
//     shift_back(no->chaves, i, no->nroChaves);
//     shift_back(no->offsets, i, no->nroChaves);
//     if(!eh_folha){
//         shift_back(no->filhos, i+1, no->nroChaves + 1);
//     }
    
//     // Decrementa o número de chaves do nó
//     no->nroChaves --;
//     arv_no_to_bin(fp_arvore, no, rrn_atual); // Escreve em disco, só pra bater com o gabarito :P
// }

// /*
// Função para transformar uma chave pai + seus filhos em um vetor
// Usado na:
// Redistribuição
// Concatenação
// Inserção

// Retorna o vetor final e altera o conteúdo da variável tamanho para o tamanho do vetor

// Parâmetros:
// Nó pai
// Índice da chave pai no vetor pai
// Nó esquerdo
// Nó direito
// */
// Chave* vectorize(Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir, int* tam, bool eh_folha){
//     int n_esq, n_dir, total;
//     n_esq = f_esq->nroChaves;
//     n_dir = f_dir->nroChaves;
//     total = n_dir + n_esq + 1;

//     // Cria um vetor auxiliar para armazenar as chaves
//     Chave* vec;
//     if(eh_folha){
//         vec = (Chave*)malloc(sizeof(Chave)* total);
//     }else{
//         // Se não for raíz tem que armazenar um espaço a mais para salvar os filhos
//         vec = (Chave*)malloc(sizeof(Chave)* (1+total));
//     }
    
//     /*Preenche o vetor auxiliar*/
//     // Coloca os elementos do nó esquerdo
//     for(int i = 0; i < n_esq; i++){
//         vec[i].chave = f_esq->chaves[i];
//         vec[i].offset = f_esq->offsets[i];
//         if(!eh_folha){ // Se não é raíz salva os filhos
//             vec[i].filho = f_esq->filhos[i];
//         }
//     }
//     if(!eh_folha){ // Se não é raíz salva o filho mais a direita do nó à esquerda
//         vec[n_esq].filho = f_esq->filhos[n_esq];
//     }

//     // Coloca o elemento do nó pai
//     vec[n_esq].chave = pai->chaves[i_pai];
//     vec[n_esq].offset = pai->offsets[i_pai];
    
//     // Coloca os elementos do nó direito
//     for(int i = n_esq+1; i < total; i++){
//         vec[i].chave = f_dir->chaves[i - n_esq -1];
//         vec[i].offset = f_dir->offsets[i - n_esq -1];
//         if(!eh_folha){ // Se não é raíz salva os filhos
//             vec[i].filho = f_dir->filhos[i - n_esq - 1];
//         }
//     }
//     if(!eh_folha){ // Se não é raíz salva o filho mais a direita do nó à direita
//         vec[total].filho = f_dir->filhos[n_dir];
//     }

//     *tam = total;

//     return vec;
// }

// /*
// Função para redistribuir 3 nós
// Recebe o nó pai, filho esquerdo e filho direito
// Note que a única diferença entre uma redistribuição à direita e à esquerda
// é a posição relativa do nó com relação ao pai, ou seja, basta alterar os parâmetros

// Todas as chaves são colocadas em em um vetor de (chaves, offsets)
// Na esquerda ficam ceil((n-1)/2) elementos.

// Faz todas as alterações e escreve em disco os nós filhos e o nó pai, que agora estão estáveis
// Não libera a memória do pai, deixa a responsabilidade com quem chamou

// Parâmetros:
// Ponteiro para a árvore
// Nó pai
// Nó filho esquerdo
// Nó filho direito 
// RRN do filho esquerdo
// RRN do filho direito
// */
// void redistribuicao(FILE* fp_arvore, Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir,
//                      int RRN_esq, int RRN_dir, int RRN_pai){
//     // Transforma o conjunto chave pai + filhos em um vetor
//     int tam;
//     bool eh_folha = (f_esq->tipoNo == -1);
//     Chave* vec = vectorize(pai, i_pai, f_esq, f_dir, &tam, eh_folha);

//     // Cria um nó auxiliar temporário inicializado
//     Arv_no* temp = criar_arv_no();

//     // Define o tamanho do lado esquerdo
//     int tam_esq = (int)ceil((tam-1)/2.0);

//     // Coloca os elementos no nó esquerdo
//     for(int i = 0; i < tam_esq; i++){
//         temp->chaves[i] = vec[i].chave;
//         temp->offsets[i] = vec[i].offset;
//         if(!eh_folha)
//             temp->filhos[i] = vec[i].filho;
//     }
//     if(!eh_folha)
//         temp->filhos[tam_esq] = vec[tam_esq].filho;
//     temp->tipoNo = f_esq->tipoNo;
//     temp->nroChaves = tam_esq;
    
//     // Escreve o filho esquerdo em disco e libera a memória
//     arv_no_to_bin(fp_arvore, temp, RRN_esq);
//     arv_no_free(&temp);
//     arv_no_free(&f_esq);

//     // Edita o elemento i_pai do nó pai
//     pai->chaves[i_pai] = vec[tam_esq].chave;
//     pai->offsets[i_pai] = vec[tam_esq].offset;

//     // Escreve o pai em disco pois ele está estável
//     arv_no_to_bin(fp_arvore, pai, RRN_pai);

//     // Cria um novo nó temporário
//     temp = criar_arv_no();

//     // Coloca os elementos no nó direito
//     for(int i = tam_esq+1, j = 0; i < tam; i++, j++){
//         temp->chaves[j] = vec[i].chave;
//         temp->offsets[j] = vec[i].offset;
//         if(!eh_folha)
//             temp->filhos[j] = vec[i].filho;
//     }
//     if(!eh_folha)
//         temp->filhos[tam - tam_esq -1] = vec[tam].filho;
//     temp->nroChaves = tam - tam_esq - 1;
//     temp->tipoNo = f_dir->tipoNo;

//     // Escreve o nó direito em disco e libera as memórias
//     arv_no_to_bin(fp_arvore, temp, RRN_dir);
//     arv_no_free(&temp);
//     arv_no_free(&f_dir);
//     free(vec);
// }

// /*
// Função para concatenar dois filhos e a chave pai

// Note que a única diferença entre uma concatenação à direita e à esquerda
// é a posição relativa do nó com relação ao pai, ou seja, basta passar alterar alterar os parâmetros

// Sempre marca o filho da direita como logicamente removido e escreve o nó concatenado à esquerda em disco
// Só não escreve o pai em disco se ele tiver underflow
// Nunca libera a memória do pai, deixa a responsabilidade com quem chamou

// */
// void merge(FILE* fp_arvore, Arv_head* head, Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir, int RRN_esq,
//              int RRN_dir, int RRN_pai, resultadoRemocao* res){
    
//     // Transforma o conjunto chave pai + filhos em um vetor
//     int tam;
//     bool eh_folha = (f_esq->tipoNo == -1);
//     Chave* vec = vectorize(pai, i_pai, f_esq, f_dir, &tam, eh_folha);

//     // Cria um nó com o conjunto
//     Arv_no* temp = criar_arv_no();
//     for(int i = 0; i < tam; i++){
//         temp->chaves[i] = vec[i].chave;
//         temp->offsets[i] = vec[i].offset;
//         if(!eh_folha)
//             temp->filhos[i] = vec[i].filho;
//     }
//     if(!eh_folha)
//         temp->filhos[tam] = vec[tam].filho;
//     temp->nroChaves = tam;
//     temp->tipoNo = f_esq->tipoNo;

//     // Coloca na esquerda
//     arv_no_to_bin(fp_arvore, temp, RRN_esq);

//     // Marca o nó da direita como removido e atualiza o topo da pilha
//     // Note que isso é feito diretamente em disco, dessa forma, todo conteúdo anterior ao merge do nó se mantém
//     char removido = '1';
//     fseek(fp_arvore, arv_RRN_to_offset(RRN_dir), SEEK_SET); // Vai para o primeiro byte offset do nó (que é o removido)
//     fwrite(&removido, sizeof(char), 1, fp_arvore); // Marca como removido
//     fwrite(&(head->topo), sizeof(int), 1, fp_arvore); // Coloca nos próximos 4 bytes o topo da pilha
//     head->topo = RRN_dir; // Atualiza o topo da pilha
//     head->nroNos --; // Atualiza o número de nós
    
//     // Libera a memória
//     arv_no_free(&temp);
//     arv_no_free(&f_dir);
//     arv_no_free(&f_esq);
//     free(vec);

//     // Remove a chave pai no nó pai, visto que agora ela está no nó filho 
//     remocao_simples(fp_arvore, pai, i_pai, RRN_pai, false); // Note que nunca o pai será uma folha
//     res->underflow = pai->nroChaves < min_chaves;
//     if(res->underflow){ // Se der underflow deixa para o avô resolver
//         res->buffer = pai;
//     }else{ // Se não escreve em disco
//         arv_no_to_bin(fp_arvore, pai, RRN_pai);
//     }
// }

// /*
// Função para tratar o underflow de um nó, fazendo redistribuição ou merge

// Sempre escreve os nós filhos, que agora estarão estáveis
// A responsabilidade de escrever o nó pai é da função que chamou, visto que ele pode estar em underflow

// Parâmetros
// Ponteiro para o arquivo da árvore
// Cabeçalho da árvore
// Índice do elemento em underflow
// Nó pai
// Struct de resultado da remoção
// RRN do filho
// RRN do pai
// */
// void tratar_underflow(FILE* fp_arvore, Arv_head* head, int i, Arv_no* no,
//     resultadoRemocao* res, int rrn_filho, int RRN){
//     /*
//     1. Tenta redistribuição à direita do filho em underflow

//     Note que a versão mais recente do nó com underflow está em RAM no buffer, 
//     sendo ele o filho i do nó atual, que acabou de voltar da recursão.
//     Dessa forma, a redistribuição é com o nó atual, nó buffer (filho i) e filho i-1 (à esquerda daquele com underflow)
//     */
    
//     // Inicializa as informações do filho direito para potencialmente usar os valores que serão lidos dentro do if fora dele
//     int rrn_dir = -1;
//     Arv_no* f_dir = NULL;
//     if(i < no->nroChaves){ // Se o nó em underflow não é o último filho da direita
//         // Lê o irmão direito
//         rrn_dir = no->filhos[i+1];
//         f_dir = bin_to_arv_no(fp_arvore, rrn_dir);
//         // Verifica se a redistribuição é possível
//         if(f_dir->nroChaves > min_chaves){
//             // Faz a redistribuição e escreve os nós filhos em disco
//             redistribuicao(fp_arvore, no, i, res->buffer, f_dir, rrn_filho, rrn_dir, RRN);
//             // Como redistribuição não gera underflow retorna essa informação
//             res->underflow = false;
//             return;
//         }
//     }
    
//     /*
//     2. Tenta redistribuição à esquerda do filho em underflow
//     */
//     if(i > 0){ // Tem irmão esquerdo
//         // Libera o nó da direita, pois não será usado
//         // A função verifica se o nó é NULL antes de dar free
//         arv_no_free(&f_dir);
        
//         // Pega o nó à esquerda daquele com underflow
//         int rrn_esq = no->filhos[i-1];
//         Arv_no* f_esq = bin_to_arv_no(fp_arvore, rrn_esq);
        
//         if(f_esq->nroChaves > min_chaves){ // Se é possível a redistribuição    
//             // Faz a redistribuição e escreve os filhos em disco
//             redistribuicao(fp_arvore, no, i-1, f_esq, res->buffer, rrn_esq, rrn_filho, RRN);
//             // Retorna que não aconteceu underflow
//             res->underflow = false;
//             return;
//         }

//         // 3. Se não funcionar faz concatenação à esquerda
//         else{
//             merge(fp_arvore, head, no, i-1, f_esq, res->buffer, rrn_esq, rrn_filho, RRN, res);
//             return;
//         }
//     }

//     /*
//     4. Se o nó atual não tiver um vizinho esquerdo, faz concatenação à direita

//     Note que se chegar nesse caso ele vai ter entrado no primeiro if e inicializado o filho direito
//     Visto que a única forma dos dois ifs falharem é no caso do número máximo de nós ser um, o que não faz sentido
//     Dessa forma não é necessário ler duas vezes o filho direito
//     */
//     merge(fp_arvore, head, no, i, res->buffer, f_dir, rrn_filho, rrn_dir, RRN, res);
//     return;
// }

// /*
// Função para remover um nó sucessor o nó sucessor
// */

// Chave remover_sucessor(FILE* fp_arvore, Arv_head* head, resultadoRemocao* res, int RRN) {

//     Chave sucessor;

//     Arv_no* no = bin_to_arv_no(fp_arvore, RRN);

//     // Caso base: chegou na folha
//     if (no->tipoNo == -1) {
//         // Salva os dados do sucessor para retornar ao nó interno
//         sucessor.chave = no->chaves[0];
//         sucessor.offset = no->offsets[0];

//         // Remove do nó a chave sucessora
//         remocao_simples(fp_arvore, no, 0, RRN, true);
//         arv_no_to_bin(fp_arvore, no, RRN); // Escrita em disco só pra passar no runcodes :P

//         // Verifica underflow
//         res->underflow = no->nroChaves < min_chaves;
//         if (res->underflow){
//             res->buffer = no;
//         }else{
//             arv_no_to_bin(fp_arvore, no, RRN);
//             arv_no_free(&no);
//         }
//         return sucessor;
//     }

//     // Desce sempre pelo filho mais à esquerda (filhos[0])
//     int rrn_filho = no->filhos[0];
//     sucessor = remover_sucessor(fp_arvore, head, res, rrn_filho);

//     // Na volta da recursão, se deu underflow, arruma 
//     if (res->underflow) {
//         tratar_underflow(fp_arvore, head, 0, no, res, rrn_filho, RRN);
//     }
//     if(!res->underflow){
//         arv_no_free(&no); // Se o não deu underflow libera da memória
//     }

//     return sucessor;
// }

// // Função auxiliar recursiva para remover um nó
// resultadoRemocao arv_remocao_aux(FILE* fp_arvore, Arv_head* head, int RRN, int chave) {
    
//     // Inicia o resultado como uma falha
//     resultadoRemocao res;
//     res.offset = -1;
//     res.underflow = false;
        
//     // Condição de parada: Chegou em um nó folha e não encontrou a chave
//     if (RRN == -1) {
//         // Nó não encontrado
//         return res;
//     }

//     // Carrega o nó atual para a memória RAM
//     Arv_no* no = bin_to_arv_no(fp_arvore, RRN);
//     if (no == NULL) {
//         return res;      // Falha na leitura ou nó inexistente
//     }

//     // Busca sequencial dentro do nó
//     // POO: trocar por busca binária
//     int i = 0;
//     while (i < no->nroChaves && chave > no->chaves[i]) {
//         i++;
//     }

//     // Se encontrou o nó atual
//     if (i < no->nroChaves && chave == no->chaves[i]) {
//         // Salva o offset
//         res.offset = no->offsets[i];

//         /*
//         Remove o nó
//         */

//         // Se não for folha, troca de posição com o sucessor
//         if(no->tipoNo != -1){
//             // Descobre o sucessor e remove
//             int rrn_dir = no->filhos[i+1];
//             Chave sucessor = remover_sucessor(fp_arvore, head, &res, rrn_dir);
//             // Atualiza o nó atual
//             no->chaves[i] = sucessor.chave;
//             no->offsets[i] = sucessor.offset;
            
//             // Concerta um potencial underflow propagado
//             if(res.underflow){
//                 tratar_underflow(fp_arvore, head, i+1, no, &res, rrn_dir, RRN);
//             }
//         }else{
//             // Faz a remoção simples
//             remocao_simples(fp_arvore, no, i, RRN, true);

//             // Salva a informação de underflow
//             res.underflow = no->nroChaves < min_chaves;
//         }

//         // Se tiver underflow faz o buffer apontar para o nó em questão
//         if(res.underflow){
//             res.buffer = no;
//         }else{ // Caso contrário salva o nó em disco
//             arv_no_to_bin(fp_arvore, no, RRN);
//             arv_no_free(&no);
//         }
//         return res;
//     }

//     /*
//     Não encontrou
//     */

//     // Chama o filho i
//     int rrn_filho = no->filhos[i];

//     res = arv_remocao_aux(fp_arvore, head, rrn_filho, chave);

//     // Se tiver underflow arruma
//     if(res.underflow){
//         tratar_underflow(fp_arvore, head, i, no, &res, rrn_filho, RRN);
//     }

//     if(!res.underflow){
//         // Se o filho não teve underflow, só libera a memória
//         arv_no_free(&no);
//     }

//     return res;
// }

// /*
// Função para remoção da árvore B

// Cuida dos 6 casos possíveis de remoção:

// 1. Remoção de uma chave em um nó folha, sem
// causar underflow
// 2. Remoção de uma chave em um nó não folha
// 3. Remoção de uma chave em um nó, causando
// underflow
// 4. Remoção de uma chave em um nó, causando
// underflow e a redistribuição não pode ser aplicada
// 5. Underflow no nó pai causado pela remoção de
// uma chave em um nó filho
// 6. Diminuição da altura da árvore

// Parâmetros:
// Arquivo
// Chave a ser removida
// */
// int remover_arv(FILE* fp_arvore, Arv_head* header, int chave){
//     if (fp_arvore == NULL || header == NULL) return -1;

//     // Se a árvore estiver vazia
//     if (header->nroNos == 0) {
//         return -1; 
//     }

//     // Remove o nó
//     resultadoRemocao res = arv_remocao_aux(fp_arvore, header, header->noRaiz, chave);
//     // Caso tenha dado underflow na raíz
//     if(res.underflow == true){
//         if(res.buffer->nroChaves > 0){ // A raíz tem chaves, a estrutura se mantém
//             arv_no_to_bin(fp_arvore, res.buffer, header->noRaiz);
//         }else{ 
//             int rrn_raiz_antiga = header->noRaiz;

//             // Se a raiz é uma folha a árvore ficou completamente vazia
//             if(res.buffer->tipoNo == -1){ 
//                 header->noRaiz = -1;
//             } 
//             // Caso contrário, a altura diminui (O filho esquerdo vira a nova raíz)
//             else { 
//                 header->noRaiz = res.buffer->filhos[0];
//                 // Atualiza o tipo do nó que agora é raíz
//                 Arv_no* temp = bin_to_arv_no(fp_arvore, header->noRaiz);
//                 temp->tipoNo = 0;
//                 arv_no_to_bin(fp_arvore, temp, header->noRaiz);
//                 arv_no_free(&temp);
//             }

//             // Remove a raiz antiga
//             char removido = '1';
//             fseek(fp_arvore, arv_RRN_to_offset(rrn_raiz_antiga), SEEK_SET); 
//             fwrite(&removido, sizeof(char), 1, fp_arvore); 
//             fwrite(&(header->topo), sizeof(int), 1, fp_arvore); 
//             header->topo = rrn_raiz_antiga;
//             header->nroNos--;
//         }
//         arv_no_free(&(res.buffer)); // Libera a raiz da memória RAM
//     }

//     return res.offset;
// }

// /*
// Função recursiva para buscar uma chave na Árvore-B.

// Recebe o ponteiro do arquivo de índice, o RRN do nó atual a ser analisado, 
// e a chave procurada.

// Retorna o offset correspondente à chave, ou -1 se não encontrar.
// */
// int arv_busca_recursiva(FILE* fp_arvore, int rrn_atual, int chave_buscada) {
//     // Condição de parada: Chegou em um nó folha e não encontrou a chave
//     if (rrn_atual == -1) {
//         return -1;       // Não encontrou
//     }

//     // Carrega o nó atual para a memória RAM
//     Arv_no* no = bin_to_arv_no(fp_arvore, rrn_atual);
//     if (no == NULL) {
//         return -1;      // Falha na leitura ou nó inexistente
//     }

//     // Busca sequencial dentro do nó para encontrar a posição da chave
//     int i = 0;
//     while (i < no->nroChaves && chave_buscada > no->chaves[i]) {
//         i++;
//     }

//     // Condição de parada: Encontrou a chave no nó atual
//     if (i < no->nroChaves && chave_buscada == no->chaves[i]) {
//         int offset_dados = no->offsets[i];      // Salva a referência do registro de dados
//         arv_no_free(&no);                       // Libera a memória alocada
//         return offset_dados;
//     }

//     // Se não encontrou, desce na árvore pelo ponteiro (filho) adequado
//     int rrn_filho = no->filhos[i];
    
//     // Libera o nó atual da memória RAM antes de fazer a chamada recursiva
//     arv_no_free(&no);

//     // Chamada recursiva para o nó filho
//     return arv_busca_recursiva(fp_arvore, rrn_filho, chave_buscada);
// }

// /*
// Função principal que inicia a busca na Árvore-B.
// Abre/Lê o cabeçalho, verifica a consistência e dispara a busca recursiva a partir da Raiz.
// */
// int arv_busca_chave(FILE* fp_arvore, int chave_buscada) {
//     if (fp_arvore == NULL) return -1;
//     // Lê o cabeçalho para descobrir quem é o nó raiz
//     Arv_head* head = bin_to_arv_head(fp_arvore);

//     if (head == NULL) return -1;

//     int rrn_raiz = head->noRaiz;
//     arv_head_free(&head); // Libera o cabeçalho da RAM

//     // Se a árvore estiver vazia
//     if (rrn_raiz == -1) {
//         return -1; 
//     }

//     // Inicia a pesquisa recursiva
//     int oqretorna = arv_busca_recursiva(fp_arvore, rrn_raiz, chave_buscada);
//     return oqretorna;
// }

// /*
// Função auxiliar recursiva para inserção.

// Retorna 1 se houve promoção (uma nova chave precisa ser inserida no nó pai).
// Retorna 0 se a inserção foi resolvida de forma segura no nível atual ou inferior.
// */

// //MODULARIZAR POR CASO (SE VAI FAZER SPLIT OU NAO E OS OUTROS FODASES)
// int arv_inserir_recursivo(FILE* fp, Arv_head* head, int rrn_atual, int chave_inserir, int offset_inserir, 
//                             int* chave_promovida, int* offset_promovido, int* filho_dir_promovido, int* inserido) {

//     // Condição de base: chegou abaixo de um nó folha
//     if (rrn_atual == -1) {          // A chave deve ser "promovida" para o nó folha que chamou esta recursão
//         *chave_promovida = chave_inserir;
//         *offset_promovido = offset_inserir;
//         *filho_dir_promovido = -1;  // Como é uma folha, não há nós filhos novos
//         *inserido = 1; //A chave é nova 
//         return 1;
//     }

//     // Carrega o nó atual para a RAM
//     Arv_no* no = bin_to_arv_no(fp, rrn_atual);
//     if (no == NULL) return 0; 

//     // Busca sequencial dentro do nó para encontrar a posição da chave
//     int i = 0;
//     while (i < no->nroChaves && chave_inserir > no->chaves[i]) {
//         i++;
//     }

//     // Prevenção de chaves duplicadas
//     if (i < no->nroChaves && chave_inserir == no->chaves[i]) {
//         //printf("o codEstacao: %d já existe em %d\n", chave_inserir, no->offsets[i]);
//         arv_no_free(&no);
//         *inserido = 0;
//         return 0;                   // A chave já existe, encerra a inserção
//     }

//     // Chamada recursiva descendo para o filho correto
//     int p_chave, p_offset, p_filho_dir;
//     int promoveu = arv_inserir_recursivo(fp, head, no->filhos[i], chave_inserir, offset_inserir, &p_chave, &p_offset, &p_filho_dir, inserido);

//     // Se falhou na inserção embaixo (chave duplicada), aborta aqui também
//     if (*inserido == 0) {
//         arv_no_free(&no);
//         return 0;
//     }

//     // Se o filho absorveu a chave sem dar Overflow, não precisa fazer nada neste nó
//     if (!promoveu) {
//         arv_no_free(&no);
//         return 0; 
//     }

//     // O nó atual tem espaço (nroChaves < 3)
//     if (no->nroChaves < nro_chaves) {
//         // Deslocamento para a direita para abrir espaço ordenado
//         for (int j = no->nroChaves; j > i; j--) {
//             no->chaves[j] = no->chaves[j-1];
//             no->offsets[j] = no->offsets[j-1];
//             no->filhos[j+1] = no->filhos[j];    // Os filhos acompanham as chaves à direita
//         }
        
//         // Insere a chave promovida
//         no->chaves[i] = p_chave;
//         no->offsets[i] = p_offset;
//         no->filhos[i+1] = p_filho_dir;
//         no->nroChaves++;

//         // Atualiza no disco e encerra a cadeia de promoções
//         arv_no_to_bin(fp, no, rrn_atual);
//         arv_no_free(&no);
//         return 0; 
//     }

//     // Nó atual está cheio (nroChaves == 3) split 
//     // Buffers temporários para organizar as 4 chaves e 5 filhos virtuais
//     int temp_chaves[4], temp_offsets[4], temp_filhos[5];

//     // Extrai as chaves combinando as antigas e a nova promovida
//     int idx = 0;
//     for (int j = 0; j < 3; j++) {
//         if (j == i) { 
//             temp_chaves[idx] = p_chave; 
//             temp_offsets[idx] = p_offset; idx++; 
//         }

//         temp_chaves[idx] = no->chaves[j]; 
//         temp_offsets[idx] = no->offsets[j]; 
//         idx++;
//     }

//     if (i == 3) { 
//         temp_chaves[3] = p_chave; 
//         temp_offsets[3] = p_offset; 
//     }

//     // Extrai os filhos combinando os antigos e o novo RRN
//     idx = 0;
//     for (int j = 0; j < 4; j++) {
//         if (j == i + 1) { 
//             temp_filhos[idx] = p_filho_dir; 
//             idx++; 
//         }

//         temp_filhos[idx] = no->filhos[j]; 
//         idx++;
//     }

//     if (i + 1 == 4) { 
//         temp_filhos[4] = p_filho_dir; 
//     }

//     // Particionamento: criação do nó à direita
//     Arv_no* novo_no = criar_arv_no();
    
//     // Se quebrou a raiz (0), o da esquerda e o da direita viram intermediários (1) ou folhas (-1)
//     if (no->tipoNo == 0) {
//         no->tipoNo = (no->filhos[0] == -1) ? -1 : 1;
//     }
//     novo_no->tipoNo = no->tipoNo; 
    
//     int novo_rrn = obter_rrn_livre_arvore(fp, head);

//     // Nó da esquerda fica com as 2 primeiras chaves
//     no->nroChaves = 2;
//     for (int j = 0; j < 2; j++) {
//         no->chaves[j] = temp_chaves[j];
//         no->offsets[j] = temp_offsets[j];
//         no->filhos[j] = temp_filhos[j];
//     }
//     no->filhos[2] = temp_filhos[2];

//     // Limpeza de lixo de memória do nó da esquerda
//     for (int j = 2; j < nro_chaves; j++) {
//         no->chaves[j] = -1; 
//         no->offsets[j] = -1;
//     }

//     for (int j = 3; j <= nro_chaves; j++) { 
//         no->filhos[j] = -1; 
//     }

//     // Nó da direita fica com a última chave
//     novo_no->nroChaves = 1;
//     novo_no->chaves[0] = temp_chaves[3];
//     novo_no->offsets[0] = temp_offsets[3];
//     novo_no->filhos[0] = temp_filhos[3];
//     novo_no->filhos[1] = temp_filhos[4];

//     // A chave promovida ao pai é a chave 3
//     *chave_promovida = temp_chaves[2];
//     *offset_promovido = temp_offsets[2];
//     *filho_dir_promovido = novo_rrn;

//     // Escreve os dois nós resultantes no disco
//     arv_no_to_bin(fp, no, rrn_atual);
//     arv_no_to_bin(fp, novo_no, novo_rrn);

//     arv_no_free(&no);
//     arv_no_free(&novo_no);

//     return 1; // Sinaliza a recursão que uma chave foi para cima
// }

// /*
// Função pública para inserir na Árvore-B
// faz a leitura do cabeçalho e o crescimento da árvore
// cria uma nova raiz caso a raiz atual sofra overflow
// */
// // POO: Atualizar o header apenas em RAM
// int arv_inserir_chave(FILE* fp_arvore, int chave, int offset_dados) {
//     if (fp_arvore == NULL) return 0;

//     Arv_head* head = bin_to_arv_head(fp_arvore);
//     if (head == NULL) return 0;

//     // Caso a Árvore esteja completamente vazia
//     if (head->noRaiz == -1) {
//         Arv_no* raiz = criar_arv_no();
//         raiz->tipoNo = -1;
//         raiz->nroChaves = 1;
//         raiz->chaves[0] = chave;
//         raiz->offsets[0] = offset_dados;
        
//         int rrn = head->proxRRN++;
//         head->noRaiz = rrn;
//         head->nroNos++;
        
//         arv_no_to_bin(fp_arvore, raiz, rrn);
//         arv_head_to_bin(fp_arvore, head);

//         arv_no_free(&raiz);
//         arv_head_free(&head);
//         return 1; //inserido com sucesso
//     }

//     // Caso a árvore já exista, começa a inserção recursiva
//     int p_chave, p_offset, p_filho_dir;
//     int inserido = 0;
//     int overflow = arv_inserir_recursivo(fp_arvore, head, head->noRaiz, chave, offset_dados, &p_chave, &p_offset, &p_filho_dir, &inserido);

//     // Se a recursão disse que a chave é duplicada, encerra e retorna falso
//     if (inserido == 0) {
//         arv_head_free(&head);
//         return 0; 
//     }

//     // Se o retorno da raiz for 1, significa que o nó principal quebrou ao meio.
//     // A árvore deve cresce em altura, criando uma nova raiz.
//     if (overflow) {
//         Arv_no* nova_raiz = criar_arv_no();
//         nova_raiz->tipoNo = 0; 
//         nova_raiz->nroChaves = 1;
        
//         // A nova raiz recebe a chave promovida
//         nova_raiz->chaves[0] = p_chave;
//         nova_raiz->offsets[0] = p_offset;
        
//         // Conecta as duas metades que sofreram o split
//         nova_raiz->filhos[0] = head->noRaiz;     // O filho esquerdo é a antiga raiz
//         nova_raiz->filhos[1] = p_filho_dir;      // O filho direito é o nó gerado pela quebra

//         int novo_rrn_raiz = obter_rrn_livre_arvore(fp_arvore, head);
//         head->noRaiz = novo_rrn_raiz;

//         arv_no_to_bin(fp_arvore, nova_raiz, novo_rrn_raiz);
//         arv_no_free(&nova_raiz);
//     }

//     // Atualiza contadores e ponteiros no cabeçalho
//     arv_head_to_bin(fp_arvore, head);
//     arv_head_free(&head);

//     return 1; //inserido com sucesso
// }

// //ISSO NAO FAZ SENTIDO MAS NAO VOU MEXER PQ TA FUNCIONANDO
// int obter_rrn_livre_arvore(FILE* fp_arvore, Arv_head* head) {
//     int rrn_livre;
//     if (head->topo == -1) {
//         rrn_livre = head->proxRRN++;
//     } else {
//         rrn_livre = head->topo;

//         fseek(fp_arvore, 17 + (rrn_livre * 53) + 1, SEEK_SET);
//         int prox_topo;
//         fread(&prox_topo, sizeof(int), 1, fp_arvore);
//         head->topo = prox_topo; // Atualiza o topo com o próximo da pilha
//     }
//     head->nroNos++;
//     return rrn_livre;
// }