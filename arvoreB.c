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
Funções usadas na remoção
*/

typedef struct{
    bool underflow;
    int offset;
    Arv_no* buffer;
} resultadoRemocao;

/*
Função de remoção simples, ou seja, simplesmente shifta os elementos
*/
void remocao_simples(FILE*fp_arvore, Arv_no* no, int i, int rrn_atual, bool eh_folha){
    // Shifta as chaves, offsets e filhos(se não for raíz)
    shift_back(no->chaves, i, no->nroChaves);
    shift_back(no->offsets, i, no->nroChaves);
    if(!eh_folha){
        shift_back(no->filhos, i+1, no->nroChaves + 1);
    }
    
    // Decrementa o número de chaves do nó
    no->nroChaves --;
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
Chave* vectorize(Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir, int* tam, bool eh_folha){
    int n_esq, n_dir, total;
    n_esq = f_esq->nroChaves;
    n_dir = f_dir->nroChaves;
    total = n_dir + n_esq + 1;

    // Cria um vetor auxiliar para armazenar as chaves
    Chave* vec;
    if(eh_folha){
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
        if(!eh_folha){ // Se não é raíz salva os filhos
            vec[i].filho = f_esq->filhos[i];
        }
    }
    if(!eh_folha){ // Se não é raíz salva o filho mais a direita do nó à esquerda
        vec[n_esq].filho = f_esq->filhos[n_esq];
    }

    // Coloca o elemento do nó pai
    vec[n_esq].chave = pai->chaves[i_pai];
    vec[n_esq].offset = pai->offsets[i_pai];
    
    // Coloca os elementos do nó direito
    for(int i = n_esq+1; i < total; i++){
        vec[i].chave = f_dir->chaves[i - n_esq -1];
        vec[i].offset = f_dir->offsets[i - n_esq -1];
        if(!eh_folha){ // Se não é raíz salva os filhos
            vec[i].filho = f_dir->filhos[i - n_esq];
        }
    }
    if(!eh_folha){ // Se não é raíz salva o filho mais a direita do nó à direita
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

Faz todas as alterações e já escreve em disco os 3 nós, visto que todos são estáveis

Parâmetros:
Nó pai
Nó filho esquerdo
Nó filho direito 
*/
void redistribuicao(FILE* fp_arvore, Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir,
                     int RRN_esq, int RRN_dir, int RRN_pai){
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

Sempre marca o filho da direita como logicamente removido e escreve o nó concatenado à esquerda em disco
Como o nó pai ainda pode ser alterado ele não é escrito em disco
*/
void merge(FILE* fp_arvore, Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir, int RRN_esq,
             int RRN_dir, int RRN_pai, int* topo, resultadoRemocao* res){
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
    arv_no_to_bin(fp_arvore, temp, RRN_esq);

    // Marca o nó da direita como removido e atualiza o topo da pilha
    // Note que isso é feito diretamente em disco, dessa forma, todo conteúdo anterior ao merge do nó se mantém
    char removido = '1';
    fseek(fp_arvore, arv_RRN_to_offset(RRN_dir), SEEK_SET); // Vai para o primeiro byte offset do nó (que é o removido)
    fwrite(&removido, sizeof(char), 1, fp_arvore); // Marca como removido
    fwrite(topo, sizeof(int), 1, fp_arvore); // Coloca nos próximos 4 bytes o topo da pilha
    *topo = RRN_dir; // Atualiza o topo da pilha
    
    // Libera a memória
    arv_no_free(&temp);
    arv_no_free(&f_dir);
    arv_no_free(&f_esq);
    free(vec);

    // Remove a chave pai no nó pai, visto que agora ela está no nó filho 
    // Porém, não escreve o nó arquivo pois ainda pode ter alteração (caso de underflow)
    remocao_simples(fp_arvore, pai, i_pai, RRN_pai, false);
    
    // Agora a função que chamou vai decidir se coloca ou não o pai em disco
    // a depender do estabilidade dele (caso tenha ou não underflow)
    res->buffer = pai;
}

/*
Função para tratar o underflow de um nó, fazendo redistribuição ou merge
*/
void tratar_underflow(FILE* fp_arvore, int i, Arv_no* no, resultadoRemocao* res, int rrn_filho, int RRN, int* topo){
    /*
    1. Tenta redistribuição à direita do filho em underflow

    Note que a versão mais recente do nó com underflow está em RAM no buffer, 
    sendo ele o filho i do nó atual, que acabou de voltar da recursão.
    Dessa forma, a redistribuição é com o nó atual, nó buffer (filho i) e filho i-1 (à esquerda daquele com underflow)
    */
    
    // Inicializa as informações do filho direito para potencialmente usar os valores que serão lidos dentro do if fora dele
    int rrn_dir = -1;
    Arv_no* f_dir = NULL;
    if(i < no->nroChaves){ // Se não é o último filho da direita
        // Lê o irmão direito
        rrn_dir = no->filhos[i+1];
        f_dir = bin_to_arv_no(fp_arvore, rrn_dir);
        // Verifica se a redistribuição é possível
        if(f_dir->nroChaves > min_chaves){
            // Faz a redistribuição (já escreve os nós em disco e libera a memória pois são todos estáveis)
            redistribuicao(fp_arvore, no, i, res->buffer, f_dir, rrn_filho, rrn_dir, RRN);
            printf("Redistribuição à direita realizada com sucesso\n");
            // Como redistribuição não gera underflow retorna essa informação
            res->underflow = false;
            return;
        }
    }
    
    /*
    2. Tenta redistribuição à esquerda do filho em underflow
    */
    if(i > 0){ // Tem irmão esquerdo
        // Libera o nó da direita, pois não será usado
        // A função verifica se o nó é NULL antes de dar free
        arv_no_free(&f_dir);
        
        // Pega o nó à esquerda daquele com underflow
        int rrn_esq = no->filhos[i-1];
        Arv_no* f_esq = bin_to_arv_no(fp_arvore, rrn_esq);
        
        if(f_esq->nroChaves > min_chaves){ // Se é possível a redistribuição    
            // Faz a redistribuição
            redistribuicao(fp_arvore, no, i-1, f_esq, res->buffer, rrn_esq, rrn_filho, RRN);
            printf("Redistribuição à esquerda realizada com sucesso\n");
            // Retorna que não aconteceu underflow
            res->underflow = false;
            return;
        }

        // 3. Se não funcionar faz concatenação à esquerda
        else{
            merge(fp_arvore, no, i-1, f_esq, res->buffer, rrn_esq, rrn_filho, RRN, topo, res);
            printf("Merge feito à esquerda\n");
            // Retorna se o merge gerou underflow
            res->underflow = no->nroChaves < min_chaves;
            return;
        }
    }

    /*
    4. Se o nó atual não tiver um vizinho esquerdo, faz concatenação à direita

    Note que se chegar nesse caso ele vai ter entrado no primeiro if e inicializado o filho direito
    Visto que a única forma dos dois ifs falharem é no caso do número máximo de nós ser um, o que não faz sentido
    Dessa forma não é necessário ler duas vezes o filho direito
    */
    merge(fp_arvore, no, i, res->buffer, f_dir, rrn_filho, rrn_dir, RRN, topo, res);
    printf("Merge feito à direita\n");
    res->underflow = no->nroChaves < min_chaves;
    return;
}

/*
Função para remover um nó sucessor o nó sucessor
*/

Chave remover_sucessor(FILE* fp_arvore, resultadoRemocao* res, int RRN, int* topo) {

    Chave sucessor;

    Arv_no* no = bin_to_arv_no(fp_arvore, RRN);

    // Caso base: chegou na folha
    if (no->tipoNo == -1) {
        // Salva os dados do sucessor para retornar ao nó interno
        sucessor.chave = no->chaves[0];
        sucessor.offset = no->offsets[0];

        // Remove do disco a chave da folha
        remocao_simples(fp_arvore, no, 0, RRN, true);

        // Verifica underflow
        res->underflow = no->nroChaves < min_chaves;
        if (res->underflow){
            res->buffer = no;
        }else{
            arv_no_to_bin(fp_arvore, no, RRN);
            arv_no_free(&no);
        }
        return sucessor;
    }

    // Desce sempre pelo filho mais à esquerda (filhos[0])
    int rrn_filho = no->filhos[0];
    sucessor = remover_sucessor(fp_arvore, res, rrn_filho, topo);

    // Na volta da recursão, se deu underflow, arruma 
    if (res->underflow) {
        tratar_underflow(fp_arvore, 0, no, res, rrn_filho, RRN, topo);
        
        if (res->underflow) {
            res->buffer = no; // Pai teve underflow
        } else {
            // Pai estável
            arv_no_to_bin(fp_arvore, no, RRN);
            arv_no_free(&no);
        }
    } else {
        // Caso não tenha underflow apenas libera a memória
        arv_no_free(&no);
    }

    return sucessor;
}

// Função auxiliar recursiva para remover um nó
resultadoRemocao arv_remocao_aux(FILE* fp_arvore, int RRN, int chave, int* topo) {
    
    // Inicia o resultado como uma falha
    resultadoRemocao res;
    res.offset = -1;
    res.underflow = false;
        
    // Condição de parada: Chegou em um nó folha e não encontrou a chave
    if (RRN == -1) {
        // Nó não encontrado
        printf("Nó não encontrado\n");
        return res;
    }

    // Carrega o nó atual para a memória RAM
    Arv_no* no = bin_to_arv_no(fp_arvore, RRN);
    if (no == NULL) {
        printf("Erro no processamento do arquivo\n");
        return res;      // Falha na leitura ou nó inexistente
    }

    // Busca sequencial dentro do nó
    // POO: trocar por busca binária
    int i = 0;
    while (i < no->nroChaves && chave > no->chaves[i]) {
        i++;
    }

    // Se encontrou o nó atual
    if (i < no->nroChaves && chave == no->chaves[i]) {
        // Salva o offset
        res.offset = no->offsets[i];

        /*
        Remove o nó
        */

        // Se não for folha, troca de posição com o sucessor
        if(no->tipoNo != -1){
            // Descobre o sucessor e remove
            int rrn_dir = no->filhos[i+1];
            Chave sucessor = remover_sucessor(fp_arvore, &res, rrn_dir, topo);
            // Atualiza o nó atual
            no->chaves[i] = sucessor.chave;
            no->offsets[i] = sucessor.offset;
            
            // Concertaum potencial underflow propagado
            if(res.underflow){
                tratar_underflow(fp_arvore, i+1, no, &res, rrn_dir, RRN, topo);
            }
        }else{
            // Faz a remoção simples
            remocao_simples(fp_arvore, no, i, RRN, true);

            // Salva a informação de underflow
            res.underflow = no->nroChaves < min_chaves;
        }

        // Se tiver underflow faz o buffer apontar para o nó em questão
        if(res.underflow){
            res.buffer = no;
        }else{ // Caso contrário salva o nó em disco
            arv_no_to_bin(fp_arvore, no, RRN);
            arv_no_free(&no);
        }
        return res;
    }

    /*
    Não encontrou
    */

    // Chama o filho i
    int rrn_filho = no->filhos[i];

    res = arv_remocao_aux(fp_arvore, rrn_filho, chave, topo);

    // Se tiver underflow arruma
    if(res.underflow){
        tratar_underflow(fp_arvore, i, no, &res, rrn_filho, RRN, topo);
        
        // Se arrumar o underflow no filho causou underflow no pai
        if(res.underflow){
            res.buffer = no; // Salva o nó no buffer para o avô resolver
        }else{
            // Se o pai está estável salva em disco
            arv_no_to_bin(fp_arvore, no, RRN);
            arv_no_free(&no);
        }
    }else{
        // Se o filho não teve underflow, só libera a memória
        arv_no_free(&no);
    }

    return res;
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
void remover_arv(FILE* fp_arvore, Arv_head* header, int chave){
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
    resultadoRemocao res = arv_remocao_aux(fp_arvore, head->noRaiz, chave, &(head->topo));
    // Caso tenha dado underflow na raíz
    if(res.underflow == true){
        if(res.buffer->nroChaves > 0){ // A raíz tem chaves, a estrutura se mantém
            arv_no_to_bin(fp_arvore, res.buffer, head->noRaiz);
        }else{ 
            int rrn_raiz_antiga = head->noRaiz;

            // Se a raiz é uma folha a árvore ficou completamente vazia
            if(res.buffer->tipoNo == -1){ 
                head->noRaiz = -1;
            } 
            // Caso contrário, a altura diminui (O filho esquerdo vira a nova raíz)
            else { 
                head->noRaiz = res.buffer->filhos[0];
            }

            // Remove a raiz antiga
            char removido = '1';
            fseek(fp_arvore, arv_RRN_to_offset(rrn_raiz_antiga), SEEK_SET); 
            fwrite(&removido, sizeof(char), 1, fp_arvore); 
            fwrite(&(head->topo), sizeof(int), 1, fp_arvore); 
            head->topo = rrn_raiz_antiga; 
            }
            arv_no_free(&(res.buffer)); // Libera a raiz da memória RAM
    }

    if(res.offset != -1){
        printf("Nó removido com sucesso\n");
    }else{
        printf("Nó não encontrado\n");
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