#include "arvoreB.h"
#include "utilitarias.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Função para definir onde um novo nó será inserido

Verifica a pilha de removidos e já faz as atualizações necessárias no cabeçalho

Retorno:
RRN em que o novo nó deve ser inserido
*/
int rrn_insercao(FILE* fp_arvore, Arv_head* head) {
    int rrn_livre;

    if (head->topo == -1) { // Se não existe espaço a se reaproveitar
        rrn_livre = head->proxRRN++; // Insere no fim do arquivo
    } else {
        rrn_livre = head->topo; // Verifica o topo da pilha

        // Lê o novo topo da pilha
        fseek(fp_arvore, arv_RRN_to_offset(rrn_livre) + 1, SEEK_SET);
        int prox_topo;
        fread(&prox_topo, sizeof(int), 1, fp_arvore);
        head->topo = prox_topo; // Atualiza o topo com o próximo da pilha
    }
    head->nroNos++; // aumenta o número de nós
    return rrn_livre;
}

/* 
Função para realizar a inserção da chave 
caso o nó encontrado esteja com espaço disponível
*/
void insercao_simples(Arv_no* no, int i, Chave chave_inser) {

    // Deslocamento para a direita para abrir espaço ordenado
    shift(no->chaves, i, no->nroChaves, false);
    shift(no->offsets, i, no->nroChaves, false);
    if(no->tipoNo != -1){
        shift(no->filhos, i, no->nroChaves + 1, false);
        no->filhos[i+1] = chave_inser.filho_dir;
    }

    // Insere a chave promovida no espaço aberto
    no->chaves[i] = chave_inser.chave;
    no->offsets[i] = chave_inser.offset;
    no->nroChaves++;
}

/*
Struct de retorno da inserção
*/
typedef struct{
    bool promocao; // Booleano indicando se ocorreu uma promocao
    bool inseriu; // Booleano indicando se ocorreu a insercao
    Chave promovida; // Chave promovida
} resultadoInsercao;


/*
Recebe o nó cheio e a chave que quer ser inserida
Realiza o split, deixando:
Metade das chaves na direita
O nó central é promovido (em caso de número par de chaves o primeiro da direita)
O restante fica em um novo nó criado à direita daquele passado como parâmetro

Retorna a sctruct de resultado da inserção, sinalizando que:
Houve promoção
Houve insersão
A chave que foi promovida
*/
resultadoInsercao split(FILE* fp, Arv_head* head, Arv_no* no, int rrn_atual, int i, Chave nova_chave) {

    // Cria um vetor auxiliar de chaves com tamanho suficiente para alocar as chaves e seus filhos
    Chave* vec_temp = malloc((nro_chaves+2)*sizeof(Chave));
    
    // Extrai as chaves combinando as antigas e a nova promovida
    int idx = 0;
    for (int j = 0; j < nro_chaves; j++) {
        if (j == i) { 
            vec_temp[idx].chave = nova_chave.chave; 
            vec_temp[idx].offset = nova_chave.offset; 
            idx++; 
        }
        vec_temp[idx].chave = no->chaves[j]; 
        vec_temp[idx].offset = no->offsets[j]; 
        idx++;
    }
    if (i == nro_chaves) { 
        vec_temp[nro_chaves].chave = nova_chave.chave; 
        vec_temp[nro_chaves].offset = nova_chave.offset; 
    }

    // Reconstrução dos filhos
    int pos_filho = i + 1;
    for (int j = 0; j < pos_filho; j++) {
        vec_temp[j].filho_dir = no->filhos[j];
    }
    vec_temp[pos_filho].filho_dir = nova_chave.filho_dir;
    for (int j = pos_filho; j < nro_chaves + 1; j++) {
        vec_temp[j+1].filho_dir = no->filhos[j];
    }

    // Define o índice da chave que será promovida
    int i_promovido = (nro_chaves+1)/2;

    // Particionamento: criação do nó à direita (já inicializado)
    Arv_no* novo_no = criar_arv_no();
    
    // Ajuste de tipo do nó (Folha vs Intermediário)
    if (no->tipoNo == 0) {
        no->tipoNo = (no->filhos[0] == -1) ? -1 : 1;
    }
    novo_no->tipoNo = no->tipoNo; 
    
    int novo_rrn = rrn_insercao(fp, head);

    // Nó da esquerda fica com as chaves anteriores à promovida
    no->nroChaves = i_promovido;
    for (int j = 0; j < i_promovido; j++) {
        no->chaves[j] = vec_temp[j].chave;
        no->offsets[j] = vec_temp[j].offset;
        no->filhos[j] = vec_temp[j].filho_dir;
    }
    no->filhos[i_promovido] = vec_temp[i_promovido].filho_dir;

    // Limpeza de lixo de memória do nó da esquerda
    for (int j = i_promovido; j < nro_chaves; j++) {
        no->chaves[j] = -1; 
        no->offsets[j] = -1;
        no->filhos[j+1] = -1;
    }

    // Nó da direita fica com as chaves posteriores à promovida
    int chaves_dir = nro_chaves-i_promovido;
    novo_no->nroChaves = chaves_dir;
    for (int i = 0, j = i_promovido + 1; i < chaves_dir; i++, j++) {
        novo_no->chaves[i] = vec_temp[j].chave;
        novo_no->offsets[i] = vec_temp[j].offset;
        novo_no->filhos[i] = vec_temp[j].filho_dir;
    }
    novo_no->filhos[chaves_dir] = vec_temp[i_promovido + 1 + chaves_dir].filho_dir;

    // Salva a chave promovida
    resultadoInsercao res;
    res.promovida.chave = vec_temp[i_promovido].chave;
    res.promovida.offset = vec_temp[i_promovido].offset;
    res.promovida.filho_dir = novo_rrn;
    res.promocao = true;
    res.inseriu = true;
    
    // Escreve os dois nós resultantes no disco
    arv_no_to_bin(fp, no, rrn_atual);
    arv_no_to_bin(fp, novo_no, novo_rrn);
    arv_no_free(&novo_no); // Libera a memória RAM da struct do novo nó
    free(vec_temp);

    return res;
}

/*
Função auxiliar recursiva para inserção.

Retorna a sctruct de resultado da inserção, indicando:
Se teve promoção
Se teve insersão
A chave que foi promovida

Quando a chave é inserida em um nó cheio acontece o split
em que a chave promovida fica na struct
Assim o pai cuida de uma possível superlotação no nó e pode propagar splits
*/
resultadoInsercao arv_inserir_recursivo(FILE* fp, Arv_head* head, int rrn_atual, int chave_inserir, int offset_inserir) {

    // Inicializa o resultado da inserção
    resultadoInsercao res;

    // Condição de base: chegou abaixo de um nó folha
    if (rrn_atual == -1) {              // A chave deve ser "promovida" para o nó folha que chamou esta recursão
        res.promovida.chave = chave_inserir;
        res.promovida.offset = offset_inserir;
        res.promovida.filho_dir = -1;      // Como é uma folha, não há nós filhos novos
        res.promocao = true;                  //A chave é nova portanto foi inserida de fato
        res.inseriu = true;
        return res;
    }

    // Carrega o nó atual para a RAM
    Arv_no* no = bin_to_arv_no(fp, rrn_atual);
    if (no == NULL){
        res.inseriu = false;
        res.promocao = false;
        return res; 
    }

    // Busca binária dentro do nó para encontrar a posição da chave
    int i = busca_binaria(no->chaves, no->nroChaves, chave_inserir);
    
    // Prevenção de chaves duplicadas
    if (i < no->nroChaves && chave_inserir == no->chaves[i]) {
        arv_no_free(&no);
        res.inseriu = false;
        res.promocao = false;
        return res;                   // A chave já existe, encerra a inserção
    }

    // Chamada recursiva descendo para o filho correto
    res = arv_inserir_recursivo(fp, head, no->filhos[i], chave_inserir, offset_inserir);

    // Se falhou na inserção embaixo (chave duplicada), aborta aqui também
    if (res.inseriu == false) {
        arv_no_free(&no);
        return res;
    }

    // Se o filho absorveu a chave sem dar Overflow, encerra
    if (res.promocao == false) {
        arv_no_free(&no);
        res.promocao = false;
        res.inseriu = true;
        return res; 
    }

    // O nó atual tem espaço
    if (no->nroChaves < nro_chaves) {
        insercao_simples(no, i, res.promovida);
        arv_no_to_bin(fp, no, rrn_atual);
        arv_no_free(&no);
        res.inseriu = true;
        res.promocao = false;
        return res;
    }

    // Nó atual está cheio, tem que dar split 
    res = split(fp, head, no, rrn_atual, i, res.promovida);
    
    arv_no_free(&no);
    return res;
}

/*
Função pública para inserir na Árvore-B

Faz as atualizações de cabeçalho em RAM, evitando escritas em disco desnecessárias

Parâmetros:
Ponteiro do arquivo da árvore
Cabeçalho da árvore
Chave a ser inserida
Offset em que a chave será inserida

Retorno:
Booleano indicando se a inserção foi bem sucedida
*/
bool arv_inserir_chave(FILE* fp_arvore, Arv_head* head, int chave, int offset_dados) {
    if (fp_arvore == NULL) return 0;
    if (head == NULL) return 0;

    // Caso a Árvore esteja completamente vazia
    if (head->noRaiz == -1) {
        Arv_no* raiz = criar_arv_no();
        raiz->tipoNo = -1;
        raiz->nroChaves = 1;
        raiz->chaves[0] = chave;
        raiz->offsets[0] = offset_dados;
        
        int rrn = head->proxRRN++;
        head->noRaiz = rrn;
        head->nroNos++;
        
        arv_no_to_bin(fp_arvore, raiz, rrn);

        arv_no_free(&raiz);
        return 1; //inserido com sucesso
    }

    // Caso a árvore já exista, começa a inserção recursiva
    resultadoInsercao res = arv_inserir_recursivo(fp_arvore, head, head->noRaiz, chave, offset_dados);

    // Se a recursão disse que a chave é duplicada, encerra e retorna falso
    if (res.inseriu == false) {
        return 0; 
    }

    // Se aconteceu uma promoção na raíz a árvore cresce em altura, criando uma nova raiz
    if (res.promocao == true) {
        Arv_no* nova_raiz = criar_arv_no();
        nova_raiz->tipoNo = 0; 
        nova_raiz->nroChaves = 1;
        
        // A nova raiz recebe a chave promovida
        nova_raiz->chaves[0] = res.promovida.chave;
        nova_raiz->offsets[0] = res.promovida.offset;
        
        // Conecta as duas metades que sofreram o split
        nova_raiz->filhos[0] = head->noRaiz;     // O filho esquerdo é a antiga raiz
        nova_raiz->filhos[1] = res.promovida.filho_dir;      // O filho direito é o nó gerado pela quebra

        int novo_rrn_raiz = rrn_insercao(fp_arvore, head);
        head->noRaiz = novo_rrn_raiz;

        arv_no_to_bin(fp_arvore, nova_raiz, novo_rrn_raiz);
        arv_no_free(&nova_raiz);
    }

    return 1; //inserido com sucesso
}