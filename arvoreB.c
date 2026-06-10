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

/*
Função recursiva para buscar uma chave na Árvore-B.

Recebe o ponteiro do arquivo de índice, o RRN do nó atual a ser analisado, 
e a chave procurada.

Retorna o offset correspondente à chave, ou -1 se não encontrar.
*/
int arv_busca_recursiva(FILE* fp_arvore, int rrn_atual, int chave_buscada) {
    // Condição de parada: Chegou em um nó folha e não encontrou a chave
    if (rrn_atual == -1) {
        return -1;       // Não encontrou
    }

    // Carrega o nó atual para a memória RAM
    Arv_no* no = bin_to_arv_no(fp_arvore, rrn_atual);
    if (no == NULL) {
        return -1;      // Falha na leitura ou nó inexistente
    }

    // Busca sequencial dentro do nó para encontrar a posição da chave
    int i = 0;
    while (i < no->nroChaves && chave_buscada > no->chaves[i]) {
        i++;
    }

    // Condição de parada: Encontrou a chave no nó atual
    if (i < no->nroChaves && chave_buscada == no->chaves[i]) {
        int offset_dados = no->offsets[i];      // Salva a referência do registro de dados
        arv_no_free(&no);                       // Libera a memória alocada
        return offset_dados;
    }

    // Se não encontrou, desce na árvore pelo ponteiro (filho) adequado
    int rrn_filho = no->filhos[i];
    
    // Libera o nó atual da memória RAM antes de fazer a chamada recursiva
    arv_no_free(&no);

    // Chamada recursiva para o nó filho
    return arv_busca_recursiva(fp_arvore, rrn_filho, chave_buscada);
}

/*
Função principal que inicia a busca na Árvore-B.
Abre/Lê o cabeçalho, verifica a consistência e dispara a busca recursiva a partir da Raiz.
*/
int arv_busca_chave(FILE* fp_arvore, int chave_buscada) {
    if (fp_arvore == NULL) return -1;
    // Lê o cabeçalho para descobrir quem é o nó raiz
    Arv_head* head = bin_to_arv_head(fp_arvore);

    if (head == NULL) return -1;

    // Verifica se o arquivo de índice está em estado consistente
    if (head->status == '0') {
        printf("Falha no processamento do arquivo.\n");
        arv_head_free(&head);
        return -1;
    }

    int rrn_raiz = head->noRaiz;
    arv_head_free(&head); // Libera o cabeçalho da RAM

    // Se a árvore estiver vazia
    if (rrn_raiz == -1) {
        return -1; 
    }

    // Inicia a pesquisa recursiva
    int oqretorna = arv_busca_recursiva(fp_arvore, rrn_raiz, chave_buscada);
    return oqretorna;
}

/*
Função auxiliar recursiva para inserção.

Retorna 1 se houve promoção (uma nova chave precisa ser inserida no nó pai).
Retorna 0 se a inserção foi resolvida de forma segura no nível atual ou inferior.
*/

//MODULARIZAR POR CASO (SE VAI FAZER SPLIT OU NAO E OS OUTROS FODASES)
int arv_inserir_recursivo(FILE* fp, Arv_head* head, int rrn_atual, int chave_inserir, int offset_inserir, 
                            int* chave_promovida, int* offset_promovido, int* filho_dir_promovido, int* inserido) {

    // Condição de base: chegou abaixo de um nó folha
    if (rrn_atual == -1) {          // A chave deve ser "promovida" para o nó folha que chamou esta recursão
        *chave_promovida = chave_inserir;
        *offset_promovido = offset_inserir;
        *filho_dir_promovido = -1;  // Como é uma folha, não há nós filhos novos
        *inserido = 1; //A chave é nova 
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
        //printf("o codEstacao: %d já existe em %d\n", chave_inserir, no->offsets[i]);
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

    // Se o filho absorveu a chave sem dar Overflow, não precisa fazer nada neste nó
    if (!promoveu) {
        arv_no_free(&no);
        return 0; 
    }

    // O nó atual tem espaço (nroChaves < 3)
    if (no->nroChaves < nro_chaves) {
        // Deslocamento para a direita para abrir espaço ordenado
        for (int j = no->nroChaves; j > i; j--) {
            no->chaves[j] = no->chaves[j-1];
            no->offsets[j] = no->offsets[j-1];
            no->filhos[j+1] = no->filhos[j];    // Os filhos acompanham as chaves à direita
        }
        
        // Insere a chave promovida
        no->chaves[i] = p_chave;
        no->offsets[i] = p_offset;
        no->filhos[i+1] = p_filho_dir;
        no->nroChaves++;

        // Atualiza no disco e encerra a cadeia de promoções
        long int byte_offset = rrn_atual * tam_arv_no + tam_arv_head;
        arv_no_to_bin(fp, no, byte_offset);
        arv_no_free(&no);
        return 0; 
    }

    // Nó atual está cheio (nroChaves == 3) split 
    // Buffers temporários para organizar as 4 chaves e 5 filhos virtuais
    int temp_chaves[4], temp_offsets[4], temp_filhos[5];

    // Extrai as chaves combinando as antigas e a nova promovida
    int idx = 0;
    for (int j = 0; j < 3; j++) {
        if (j == i) { 
            temp_chaves[idx] = p_chave; 
            temp_offsets[idx] = p_offset; idx++; 
        }

        temp_chaves[idx] = no->chaves[j]; 
        temp_offsets[idx] = no->offsets[j]; 
        idx++;
    }

    if (i == 3) { 
        temp_chaves[3] = p_chave; 
        temp_offsets[3] = p_offset; 
    }

    // Extrai os filhos combinando os antigos e o novo RRN
    idx = 0;
    for (int j = 0; j < 4; j++) {
        if (j == i + 1) { 
            temp_filhos[idx] = p_filho_dir; 
            idx++; 
        }

        temp_filhos[idx] = no->filhos[j]; 
        idx++;
    }

    if (i + 1 == 4) { 
        temp_filhos[4] = p_filho_dir; 
    }

    // Particionamento: criação do nó à direita
    Arv_no* novo_no = criar_arv_no();
    
    // Se quebrou a raiz (0), o da esquerda e o da direita viram intermediários (1) ou folhas (-1)
    if (no->tipoNo == 0) {
        no->tipoNo = (no->filhos[0] == -1) ? -1 : 1;
    }
    novo_no->tipoNo = no->tipoNo; 
    
    int novo_rrn = obter_rrn_livre_arvore(fp, head);

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

    // A chave promovida ao pai é a chave 3
    *chave_promovida = temp_chaves[2];
    *offset_promovido = temp_offsets[2];
    *filho_dir_promovido = novo_rrn;

    // Escreve os dois nós resultantes no disco
    arv_no_to_bin(fp, no, rrn_atual * tam_arv_no + tam_arv_head);
    arv_no_to_bin(fp, novo_no, novo_rrn * tam_arv_no + tam_arv_head);

    arv_no_free(&no);
    arv_no_free(&novo_no);

    return 1; // Sinaliza a recursão que uma chave foi para cima
}

/*
Função pública para inserir na Árvore-B
faz a leitura do cabeçalho e o crescimento da árvore
cria uma nova raiz caso a raiz atual sofra overflow
*/
int arv_inserir_chave(FILE* fp_arvore, int chave, int offset_dados) {
    if (fp_arvore == NULL) return 0;

    Arv_head* head = bin_to_arv_head(fp_arvore);
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
        
        arv_no_to_bin(fp_arvore, raiz, rrn * tam_arv_no + tam_arv_head);
        arv_head_to_bin(fp_arvore, head);

        arv_no_free(&raiz);
        arv_head_free(&head);
        return 1; //inserido com sucesso
    }

    // Caso a árvore já exista, começa a inserção recursiva
    int p_chave, p_offset, p_filho_dir;
    int inserido = 0;
    int overflow = arv_inserir_recursivo(fp_arvore, head, head->noRaiz, chave, offset_dados, &p_chave, &p_offset, &p_filho_dir, &inserido);

    // Se a recursão disse que a chave é duplicada, encerra e retorna falso
    if (inserido == 0) {
        arv_head_free(&head);
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

        int novo_rrn_raiz = obter_rrn_livre_arvore(fp_arvore, head);
        head->noRaiz = novo_rrn_raiz;

        arv_no_to_bin(fp_arvore, nova_raiz, novo_rrn_raiz * tam_arv_no + tam_arv_head);
        arv_no_free(&nova_raiz);
    }

    // Atualiza contadores e ponteiros no cabeçalho
    arv_head_to_bin(fp_arvore, head);
    arv_head_free(&head);

    return 1; //inserido com sucesso
}

//ISSO NAO FAZ SENTIDO MAS NAO VOU MEXER PQ TA FUNCIONANDO
int obter_rrn_livre_arvore(FILE* fp_arvore, Arv_head* head) {
    int rrn_livre;
    if (head->topo == -1) {
        rrn_livre = head->proxRRN++;
    } else {
        rrn_livre = head->topo;

        fseek(fp_arvore, 17 + (rrn_livre * 53) + 1, SEEK_SET);
        int prox_topo;
        fread(&prox_topo, sizeof(int), 1, fp_arvore);
        head->topo = prox_topo; // Atualiza o topo com o próximo da pilha
    }
    head->nroNos++;
    return rrn_livre;
}