#include "arvoreB.h"
#include "utilitarias.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Scruct de retorno da remoção
typedef struct{
    bool underflow; // Booleano indicando se ocorreu um underflow
    int offset; // Byte offset da chave removida no arquivo de registros
    Arv_no* buffer; // Nó instável que será mantido na RAM pois sofrerá modificações
} resultadoRemocao;

/*
Funções usadas na remoção
*/
  
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
            vec[i].filho_dir = f_esq->filhos[i];
        }
    }
    if(!eh_folha){ // Se não é raíz salva o filho mais a direita do nó à esquerda
        vec[n_esq].filho_dir = f_esq->filhos[n_esq];
    }

    // Coloca o elemento do nó pai
    vec[n_esq].chave = pai->chaves[i_pai];
    vec[n_esq].offset = pai->offsets[i_pai];
    
    // Coloca os elementos do nó direito
    for(int i = n_esq+1; i < total; i++){
        vec[i].chave = f_dir->chaves[i - n_esq -1];
        vec[i].offset = f_dir->offsets[i - n_esq -1];
        if(!eh_folha){ // Se não é raíz salva os filhos
            vec[i].filho_dir = f_dir->filhos[i - n_esq - 1];
        }
    }
    if(!eh_folha){ // Se não é raíz salva o filho mais a direita do nó à direita
        vec[total].filho_dir = f_dir->filhos[n_dir];
    }

    *tam = total;

    return vec;
}

/*
Função de remoção simples, ou seja, simplesmente shifta os elementos
*/
void remocao_simples(FILE*fp_arvore, Arv_no* no, int i, int rrn_atual, bool eh_folha){
    // Shifta as chaves, offsets e filhos(se não for raíz)
    shift(no->chaves, i, no->nroChaves, true);
    shift(no->offsets, i, no->nroChaves, true);
    if(!eh_folha){
        shift(no->filhos, i+1, no->nroChaves + 1, true);
    }
    
    // Decrementa o número de chaves do nó
    no->nroChaves --;
    arv_no_to_bin(fp_arvore, no, rrn_atual); // Escreve em disco só pra bater com o gabarito :P
}

/*
Função para redistribuir 3 nós
Recebe o nó pai, filho esquerdo e filho direito
Note que a única diferença entre uma redistribuição à direita e à esquerda
é a posição relativa do nó com relação ao pai, ou seja, basta alterar os parâmetros

Todas as chaves são colocadas em em um vetor de (chaves, offsets)
Na esquerda ficam ceil((n-1)/2) elementos.

Faz todas as alterações e escreve em disco os nós filhos e o nó pai, que agora estão estáveis
Não libera a memória do pai, deixa a responsabilidade com quem chamou

Parâmetros:
Ponteiro para a árvore
Nó pai
Nó filho esquerdo
Nó filho direito 
RRN do filho esquerdo
RRN do filho direito
*/
void redistribuicao(FILE* fp_arvore, Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir,
                     int RRN_esq, int RRN_dir, int RRN_pai){
    // Transforma o conjunto chave pai + filhos em um vetor
    int tam;
    bool eh_folha = (f_esq->tipoNo == -1);
    Chave* vec = vectorize(pai, i_pai, f_esq, f_dir, &tam, eh_folha);

    // Cria um nó auxiliar temporário inicializado
    Arv_no* temp = criar_arv_no();

    // Define o tamanho do lado esquerdo
    int tam_esq = (int)ceil((tam-1)/2.0);

    // Coloca os elementos no nó esquerdo
    for(int i = 0; i < tam_esq; i++){
        temp->chaves[i] = vec[i].chave;
        temp->offsets[i] = vec[i].offset;
        if(!eh_folha)
            temp->filhos[i] = vec[i].filho_dir;
    }
    if(!eh_folha)
        temp->filhos[tam_esq] = vec[tam_esq].filho_dir;
    temp->tipoNo = f_esq->tipoNo;
    temp->nroChaves = tam_esq;
    
    // Escreve o filho esquerdo em disco e libera a memória
    arv_no_to_bin(fp_arvore, temp, RRN_esq);
    arv_no_free(&temp);
    arv_no_free(&f_esq);

    // Edita o elemento i_pai do nó pai
    pai->chaves[i_pai] = vec[tam_esq].chave;
    pai->offsets[i_pai] = vec[tam_esq].offset;

    // Escreve o pai em disco pois ele está estável
    arv_no_to_bin(fp_arvore, pai, RRN_pai);

    // Cria um novo nó temporário
    temp = criar_arv_no();

    // Coloca os elementos no nó direito
    for(int i = tam_esq+1, j = 0; i < tam; i++, j++){
        temp->chaves[j] = vec[i].chave;
        temp->offsets[j] = vec[i].offset;
        if(!eh_folha)
            temp->filhos[j] = vec[i].filho_dir;
    }
    if(!eh_folha)
        temp->filhos[tam - tam_esq -1] = vec[tam].filho_dir;
    temp->nroChaves = tam - tam_esq - 1;
    temp->tipoNo = f_dir->tipoNo;

    // Escreve o nó direito em disco e libera as memórias
    arv_no_to_bin(fp_arvore, temp, RRN_dir);
    arv_no_free(&temp);
    arv_no_free(&f_dir);
    free(vec);
}

/*
Função para concatenar dois filhos e a chave pai

Note que a única diferença entre uma concatenação à direita e à esquerda
é a posição relativa do nó com relação ao pai, ou seja, basta passar alterar alterar os parâmetros

Sempre marca o filho da direita como logicamente removido e escreve o nó concatenado à esquerda em disco
Só não escreve o pai em disco se ele tiver underflow
Nunca libera a memória do pai, deixa a responsabilidade com quem chamou

*/
void merge(FILE* fp_arvore, Arv_head* head, Arv_no* pai, int i_pai, Arv_no* f_esq, Arv_no* f_dir, int RRN_esq,
             int RRN_dir, int RRN_pai, resultadoRemocao* res){
    
    // Transforma o conjunto chave pai + filhos em um vetor
    int tam;
    bool eh_folha = (f_esq->tipoNo == -1);
    Chave* vec = vectorize(pai, i_pai, f_esq, f_dir, &tam, eh_folha);

    // Cria um nó com o conjunto
    Arv_no* temp = criar_arv_no();
    for(int i = 0; i < tam; i++){
        temp->chaves[i] = vec[i].chave;
        temp->offsets[i] = vec[i].offset;
        if(!eh_folha)
            temp->filhos[i] = vec[i].filho_dir;
    }
    if(!eh_folha)
        temp->filhos[tam] = vec[tam].filho_dir;
    temp->nroChaves = tam;
    temp->tipoNo = f_esq->tipoNo;

    // Coloca na esquerda
    arv_no_to_bin(fp_arvore, temp, RRN_esq);

    // Marca o nó da direita como removido e atualiza o topo da pilha
    // Note que isso é feito diretamente em disco, dessa forma, todo conteúdo anterior ao merge do nó se mantém
    char removido = '1';
    fseek(fp_arvore, arv_RRN_to_offset(RRN_dir), SEEK_SET); // Vai para o primeiro byte offset do nó (que é o removido)
    fwrite(&removido, sizeof(char), 1, fp_arvore); // Marca como removido
    fwrite(&(head->topo), sizeof(int), 1, fp_arvore); // Coloca nos próximos 4 bytes o topo da pilha
    head->topo = RRN_dir; // Atualiza o topo da pilha
    head->nroNos --; // Atualiza o número de nós
    
    // Libera a memória
    arv_no_free(&temp);
    arv_no_free(&f_dir);
    arv_no_free(&f_esq);
    free(vec);

    // Remove a chave pai no nó pai, visto que agora ela está no nó filho 
    remocao_simples(fp_arvore, pai, i_pai, RRN_pai, false); // Note que nunca o pai será uma folha
    res->underflow = pai->nroChaves < min_chaves;
    if(res->underflow){ // Se der underflow deixa para o avô resolver
        res->buffer = pai;
    }else{ // Se não escreve em disco
        arv_no_to_bin(fp_arvore, pai, RRN_pai);
    }
}

/*
Função para tratar o underflow de um nó, fazendo redistribuição ou merge

Sempre escreve os nós filhos, que agora estarão estáveis
A responsabilidade de escrever o nó pai é da função que chamou, visto que ele pode estar em underflow

Parâmetros
Ponteiro para o arquivo da árvore
Cabeçalho da árvore
Índice do elemento em underflow
Nó pai
Struct de resultado da remoção
RRN do filho
RRN do pai
*/
void tratar_underflow(FILE* fp_arvore, Arv_head* head, int i, Arv_no* no,
    resultadoRemocao* res, int rrn_filho, int RRN){
    /*
    1. Tenta redistribuição à direita do filho em underflow

    Note que a versão mais recente do nó com underflow está em RAM no buffer, 
    sendo ele o filho i do nó atual, que acabou de voltar da recursão.
    Dessa forma, a redistribuição é com o nó atual, nó buffer (filho i) e filho i-1 (à esquerda daquele com underflow)
    */
    
    // Inicializa as informações do filho direito para potencialmente usar os valores que serão lidos dentro do if fora dele
    int rrn_dir = -1;
    Arv_no* f_dir = NULL;
    if(i < no->nroChaves){ // Se o nó em underflow não é o último filho da direita
        // Lê o irmão direito
        rrn_dir = no->filhos[i+1];
        f_dir = bin_to_arv_no(fp_arvore, rrn_dir);
        // Verifica se a redistribuição é possível
        if(f_dir->nroChaves > min_chaves){
            // Faz a redistribuição e escreve os nós filhos em disco
            redistribuicao(fp_arvore, no, i, res->buffer, f_dir, rrn_filho, rrn_dir, RRN);
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
            // Faz a redistribuição e escreve os filhos em disco
            redistribuicao(fp_arvore, no, i-1, f_esq, res->buffer, rrn_esq, rrn_filho, RRN);
            // Retorna que não aconteceu underflow
            res->underflow = false;
            return;
        }

        // 3. Se não funcionar faz concatenação à esquerda
        else{
            merge(fp_arvore, head, no, i-1, f_esq, res->buffer, rrn_esq, rrn_filho, RRN, res);
            return;
        }
    }

    /*
    4. Se o nó atual não tiver um vizinho esquerdo, faz concatenação à direita

    Note que se chegar nesse caso ele vai ter entrado no primeiro if e inicializado o filho direito
    Visto que a única forma dos dois ifs falharem é no caso do número máximo de nós ser um, o que não faz sentido
    Dessa forma não é necessário ler duas vezes o filho direito
    */
    merge(fp_arvore, head, no, i, res->buffer, f_dir, rrn_filho, rrn_dir, RRN, res);
    return;
}

/*
Função para remover um nó sucessor o nó sucessor
*/

Chave remover_sucessor(FILE* fp_arvore, Arv_head* head, resultadoRemocao* res, int RRN) {

    Chave sucessor;

    Arv_no* no = bin_to_arv_no(fp_arvore, RRN);

    // Caso base: chegou na folha
    if (no->tipoNo == -1) {
        // Salva os dados do sucessor para retornar ao nó interno
        sucessor.chave = no->chaves[0];
        sucessor.offset = no->offsets[0];

        // Remove do nó a chave sucessora
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
    sucessor = remover_sucessor(fp_arvore, head, res, rrn_filho);

    // Na volta da recursão, se deu underflow, arruma 
    if (res->underflow) {
        tratar_underflow(fp_arvore, head, 0, no, res, rrn_filho, RRN);
    }
    if(!res->underflow){
        arv_no_free(&no); // Se o não deu underflow libera da memória
    }

    return sucessor;
}

// Função auxiliar recursiva para remover um nó
resultadoRemocao arv_remocao_aux(FILE* fp_arvore, Arv_head* head, int RRN, int chave) {
    
    // Inicia o resultado como uma falha
    resultadoRemocao res;
    res.offset = -1;
    res.underflow = false;
        
    // Condição de parada: Chegou em um nó folha e não encontrou a chave
    if (RRN == -1) {
        // Nó não encontrado
        return res;
    }

    // Carrega o nó atual para a memória RAM
    Arv_no* no = bin_to_arv_no(fp_arvore, RRN);
    if (no == NULL) {
        return res;      // Falha na leitura ou nó inexistente
    }

    // Busca binária dentro do nó
    int i = busca_binaria(no->chaves, no->nroChaves, chave);

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
            Chave sucessor = remover_sucessor(fp_arvore, head, &res, rrn_dir);
            // Atualiza o nó atual
            no->chaves[i] = sucessor.chave;
            no->offsets[i] = sucessor.offset;
            
            // Concerta um potencial underflow propagado
            if(res.underflow){
                tratar_underflow(fp_arvore, head, i+1, no, &res, rrn_dir, RRN);
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

    res = arv_remocao_aux(fp_arvore, head, rrn_filho, chave);

    // Se tiver underflow arruma
    if(res.underflow){
        tratar_underflow(fp_arvore, head, i, no, &res, rrn_filho, RRN);
    }

    if(!res.underflow){
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
int remover_arv(FILE* fp_arvore, Arv_head* header, int chave){
    if (fp_arvore == NULL || header == NULL) return -1;

    // Se a árvore estiver vazia
    if (header->nroNos == 0) {
        return -1; 
    }

    // Remove o nó
    resultadoRemocao res = arv_remocao_aux(fp_arvore, header, header->noRaiz, chave);
    // Caso tenha dado underflow na raíz
    if(res.underflow == true){
        if(res.buffer->nroChaves > 0){ // A raíz tem chaves, a estrutura se mantém
            arv_no_to_bin(fp_arvore, res.buffer, header->noRaiz);
        }else{ 
            int rrn_raiz_antiga = header->noRaiz;

            // Se a raiz é uma folha a árvore ficou completamente vazia
            if(res.buffer->tipoNo == -1){ 
                header->noRaiz = -1;
            } 
            // Caso contrário, a altura diminui (O filho esquerdo vira a nova raíz)
            else { 
                header->noRaiz = res.buffer->filhos[0];
                // Atualiza o tipo do nó que agora é raíz
                Arv_no* temp = bin_to_arv_no(fp_arvore, header->noRaiz);
                temp->tipoNo = 0;
                arv_no_to_bin(fp_arvore, temp, header->noRaiz);
                arv_no_free(&temp);
            }

            // Remove a raiz antiga
            char removido = '1';
            fseek(fp_arvore, arv_RRN_to_offset(rrn_raiz_antiga), SEEK_SET); 
            fwrite(&removido, sizeof(char), 1, fp_arvore); 
            fwrite(&(header->topo), sizeof(int), 1, fp_arvore); 
            header->topo = rrn_raiz_antiga;
            header->nroNos--;
        }
        arv_no_free(&(res.buffer)); // Libera a raiz da memória RAM
    }

    return res.offset;
}