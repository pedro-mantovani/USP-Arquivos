#include "arvoreB.h"
#include "utilitarias.h"
#include <stdbool.h>
#include <stdio.h>
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
void insercao_simples(Arv_no* no, int i, int p_chave, int p_offset, int p_filho_dir) {

    // Deslocamento para a direita para abrir espaço ordenado
    shift(no->chaves, i, no->nroChaves, false);
    shift(no->offsets, i, no->nroChaves, false);
    if(no->tipoNo != -1){
        shift(no->filhos, i, no->nroChaves + 1, false);
        no->filhos[i+1] = p_filho_dir;
    }

    // Insere a chave promovida no espaço aberto
    no->chaves[i] = p_chave;
    no->offsets[i] = p_offset;
    no->nroChaves++;
}

/*
Struct de retorno da inserção
*/
typedef struct{
    bool promocao; // Booleano indicando se ocorreu uma promocao
    Chave* promovida; // Chave promovida
} resultadoInsercao;


/*
Recebe o nó cheio e a chave que quer ser inserida
Realiza o split, 

Realiza o particionamento (split), caso o nó encontrado para a
chave não tenha espaço disponível

Cria vetores temporários, instancia o novo nó, limpa a memória 
e faz a reconstrução dos ponteiros
*/
void split(FILE* fp, Arv_head* head, Arv_no* no, int rrn_atual, int i, 
                     int p_chave, int p_offset, int p_filho_dir,
                     int* chave_promovida, int* offset_promovido, int* filho_dir_promovido) {
    
    int temp_chaves[4], temp_offsets[4], temp_filhos[5];

    // Extrai as chaves combinando as antigas e a nova promovida
    int idx = 0;
    for (int j = 0; j < 3; j++) {
        if (j == i) { 
            temp_chaves[idx] = p_chave; 
            temp_offsets[idx] = p_offset; 
            idx++; 
        }
        temp_chaves[idx] = no->chaves[j]; 
        temp_offsets[idx] = no->offsets[j]; 
        idx++;
    }
    if (i == 3) { 
        temp_chaves[3] = p_chave; 
        temp_offsets[3] = p_offset; 
    }

    // Reconstrução dos filhos
    int pos_filho = i + 1;
    for (int j = 0; j < pos_filho; j++) {
        temp_filhos[j] = no->filhos[j];
    }
    temp_filhos[pos_filho] = p_filho_dir;
    for (int j = pos_filho; j < 4; j++) {
        temp_filhos[j + 1] = no->filhos[j];
    }

    // Particionamento: criação do nó à direita (já inicializado)
    Arv_no* novo_no = criar_arv_no();
    
    // Ajuste de tipo do nó (Folha vs Intermediário)
    if (no->tipoNo == 0) {
        no->tipoNo = (no->filhos[0] == -1) ? -1 : 1;
    }
    novo_no->tipoNo = no->tipoNo; 
    
    int novo_rrn = rrn_insercao(fp, head);

    // Nó da esquerda fica com as 2 primeiras chaves
    no->nroChaves = 2;
    for (int j = 0; j < 2; j++) {
        no->chaves[j] = temp_chaves[j];
        no->offsets[j] = temp_offsets[j];
        no->filhos[j] = temp_filhos[j];
    }
    no->filhos[2] = temp_filhos[2];

    // Limpeza de lixo de memória do nó da esquerda
    for (int j = 2; j < nro_chaves; j++) {
        no->chaves[j] = -1; 
        no->offsets[j] = -1;
    }
    for (int j = 3; j <= nro_chaves; j++) { 
        no->filhos[j] = -1; 
    }

    // Nó da direita fica com a última chave
    novo_no->nroChaves = 1;
    novo_no->chaves[0] = temp_chaves[3];
    novo_no->offsets[0] = temp_offsets[3];
    novo_no->filhos[0] = temp_filhos[3];
    novo_no->filhos[1] = temp_filhos[4];

    // A chave promovida ao pai é a chave 3 (índice 2)
    *chave_promovida = temp_chaves[2];
    *offset_promovido = temp_offsets[2];
    *filho_dir_promovido = novo_rrn;

    // Escreve os dois nós resultantes no disco
    arv_no_to_bin(fp, no, rrn_atual);
    arv_no_to_bin(fp, novo_no, novo_rrn);

    arv_no_free(&novo_no); // Liberar a memória RAM da struct do novo nó
}

/*
Função auxiliar recursiva para inserção.

Retorna 1 se houve promoção (uma nova chave precisa ser inserida no nó pai).
Retorna 0 se a inserção foi resolvida de forma segura no nível atual ou inferior.
*/
int arv_inserir_recursivo(FILE* fp, Arv_head* head, int rrn_atual, int chave_inserir, int offset_inserir, 
                            int* chave_promovida, int* offset_promovido, int* filho_dir_promovido, int* inserido) {

    // POO: Chamada de recursão a toa, o correto seria verificar se o nó atual é folha
    // Condição de base: chegou abaixo de um nó folha
    if (rrn_atual == -1) {              // A chave deve ser "promovida" para o nó folha que chamou esta recursão
        *chave_promovida = chave_inserir;
        *offset_promovido = offset_inserir;
        *filho_dir_promovido = -1;      // Como é uma folha, não há nós filhos novos
        *inserido = 1;                  //A chave é nova portanto foi inserida de fato
        return 1;
    }

    // Carrega o nó atual para a RAM
    Arv_no* no = bin_to_arv_no(fp, rrn_atual);
    if (no == NULL) return 0; 

    // Busca sequencial dentro do nó para encontrar a posição da chave
    int i = 0;
    while (i < no->nroChaves && chave_inserir > no->chaves[i]) {
        i++;
    }

    // Prevenção de chaves duplicadas
    if (i < no->nroChaves && chave_inserir == no->chaves[i]) {
        arv_no_free(&no);
        *inserido = 0;
        return 0;                   // A chave já existe, encerra a inserção
    }

    // Chamada recursiva descendo para o filho correto
    int p_chave, p_offset, p_filho_dir;
    int promoveu = arv_inserir_recursivo(fp, head, no->filhos[i], chave_inserir, offset_inserir, &p_chave, &p_offset, &p_filho_dir, inserido);

    // Se falhou na inserção embaixo (chave duplicada), aborta aqui também
    if (*inserido == 0) {
        arv_no_free(&no);
        return 0;
    }

    // Se o filho absorveu a chave sem dar Overflow, encerra
    if (!promoveu) {
        arv_no_free(&no);
        return 0; 
    }

    // O nó atual tem espaço (nroChaves < 3)
    if (no->nroChaves < nro_chaves) {
        insercao_simples(no, i, p_chave, p_offset, p_filho_dir);
        arv_no_to_bin(fp, no, rrn_atual);
        arv_no_free(&no);
        return 0;
    }

    // Nó atual está cheio (nroChaves == 3), tem que dar split 
    split(fp, head, no, rrn_atual, i, p_chave, p_offset, p_filho_dir, 
                    chave_promovida, offset_promovido, filho_dir_promovido);
    
    arv_no_free(&no);
    return 1;   // Sinaliza a recursão acima que ocorreu uma promoção
}

/*
Função pública para inserir na Árvore-B

faz a leitura do cabeçalho e o crescimento da árvore
cria uma nova raiz caso a raiz atual sofra overflow
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
    int p_chave, p_offset, p_filho_dir;
    int inserido = 0;
    int overflow = arv_inserir_recursivo(fp_arvore, head, head->noRaiz, chave, offset_dados, &p_chave, &p_offset, &p_filho_dir, &inserido);

    // Se a recursão disse que a chave é duplicada, encerra e retorna falso
    if (inserido == 0) {
        return 0; 
    }

    // Se o retorno da raiz for 1, significa que o nó principal quebrou ao meio.
    // A árvore deve cresce em altura, criando uma nova raiz.
    if (overflow) {
        Arv_no* nova_raiz = criar_arv_no();
        nova_raiz->tipoNo = 0; 
        nova_raiz->nroChaves = 1;
        
        // A nova raiz recebe a chave promovida
        nova_raiz->chaves[0] = p_chave;
        nova_raiz->offsets[0] = p_offset;
        
        // Conecta as duas metades que sofreram o split
        nova_raiz->filhos[0] = head->noRaiz;     // O filho esquerdo é a antiga raiz
        nova_raiz->filhos[1] = p_filho_dir;      // O filho direito é o nó gerado pela quebra

        int novo_rrn_raiz = rrn_insercao(fp_arvore, head);
        head->noRaiz = novo_rrn_raiz;

        arv_no_to_bin(fp_arvore, nova_raiz, novo_rrn_raiz);
        arv_no_free(&nova_raiz);
    }

    return 1; //inserido com sucesso
}