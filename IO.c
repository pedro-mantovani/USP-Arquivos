#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "AVL.h"
#include "registro.h"
#include "busca.h"

/*
Funções relacionadas a entrada e saída
*/

void BinarioNaTela(char *arquivo) {
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }

    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);

    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }

    printf("%lf\n", (cs / (double)100));

    free(mb);
    fclose(fs);
}


void ScanQuoteString(char *str) {
    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n') { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();         // ignorar aspas fechando
    } else if (R != EOF) { 
        //em vez de str[0] = R e scanf devolve o caractere para o buffer e le a palavra inteira
        ungetc(R, stdin); 
        scanf("%s", str);
    } else { // EOF
        strcpy(str, "");
    }
}

// Caso a posição passada seja -1 ele não faz o seek e coloca como append para otimização
void reg_to_bin(Registro* reg, FILE* fp, int pos){
    char c;
    int x;
    char* s;
    int bytes_lixo = 43;

    if (fp == NULL || reg == NULL) return;

    if(pos != -1)
        fseek(fp, pos, SEEK_SET);

    c = reg_get_removido(reg);
    fwrite(&c, sizeof(char), 1, fp);

    x = reg_get_proximo(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codEstacao(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codLinha(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codProxEstacao(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_distProxEstacao(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codLinhaIntegra(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_codEstIntegra(reg);
    fwrite(&x, sizeof(int), 1, fp);

    x = reg_get_tamNomeEstacao(reg);
    bytes_lixo -= x;
    fwrite(&x, sizeof(int), 1, fp);

    s = reg_get_nomeEstacao(reg);
    if (x > 0 && s != NULL)
        fwrite(s, sizeof(char), x, fp);

    x = reg_get_tamNomeLinha(reg);
    bytes_lixo -= x;
    fwrite(&x, sizeof(int), 1, fp);

    s = reg_get_nomeLinha(reg);
    if (x > 0 && s != NULL)
        fwrite(s, sizeof(char), x, fp);

    char* lixo = malloc(sizeof(char)*bytes_lixo);
    memset(lixo, '$', bytes_lixo);
    fwrite(lixo, sizeof(char), bytes_lixo, fp);
    free(lixo);
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

int convert_num(char* str_num){
    if(str_num == NULL || *str_num == '\0')
        return -1;
    else
        return atoi(str_num);
}

//funcionalidade 1: leitura de csv e escrita em binário
void read_csv(char* arquivo_csv, char* arquivo_bin){
    FILE* fp_csv = fopen(arquivo_csv, "r");
    if(fp_csv == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    FILE* fp_bin = fopen(arquivo_bin, "wb");
    if(fp_bin == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }

    Header* head = criar_header();
    header_to_bin(fp_bin, head);

    char linha[256];
    char *ptr;
    char *token;
    char* codEstacao;
    char* codProxEstacao;
    char pair[21];
    int nroRegistos = 0;

    Registro* reg_temp = criar_registro();

    AVL* nomesEstacoes = AVL_criar();
    AVL* paresEstacoes = AVL_criar();

    if (fp_csv == NULL) return;

    fgets(linha, sizeof(linha), fp_csv); // Queima a linha de cabeçalho
    
    while (fgets(linha, sizeof(linha), fp_csv) != NULL) {
        ptr = linha;
        // Troca o \n por \0
        linha[strcspn(linha, "\n") - 1] = '\0';
        
        codEstacao = strsep(&ptr, ",");
        reg_set_codEstacao(reg_temp, atoi(codEstacao));

        token = strsep(&ptr, ",");
        reg_set_tamNomeEstacao(reg_temp, strlen(token));
        reg_set_nomeEstacao(reg_temp, token);
        AVL_inserir(nomesEstacoes, token);

        reg_set_codLinha(reg_temp, convert_num(strsep(&ptr, ",")));

        token = strsep(&ptr, ",");
        reg_set_tamNomeLinha(reg_temp, strlen(token));
        reg_set_nomeLinha(reg_temp, token);

        codProxEstacao = strsep(&ptr, ",");
        reg_set_codProxEstacao(reg_temp, convert_num(codProxEstacao));
        // Salva em ordem lexográfica (menor, maior)
        if(*codProxEstacao != '\0'){
            if (strcmp(codEstacao, codProxEstacao) < 0) {
                snprintf(pair, sizeof(pair), "%s,%s", codEstacao, codProxEstacao);
            } else {
                snprintf(pair, sizeof(pair), "%s,%s", codProxEstacao, codEstacao);
            }
            AVL_inserir(paresEstacoes, pair);
        }

        reg_set_distProxEstacao(reg_temp, convert_num(strsep(&ptr, ",")));
        reg_set_codLinhaIntegra(reg_temp, convert_num(strsep(&ptr, ",")));
        reg_set_codEstIntegra(reg_temp, convert_num(strsep(&ptr, ",")));

        reg_to_bin(reg_temp, fp_bin, -1);
        nroRegistos ++;
    }

    fclose(fp_csv);

    header_set_nroEstacoes(head, AVL_tamanho(nomesEstacoes));
    header_set_nroParesEstacao(head, AVL_tamanho(paresEstacoes));
    header_set_proxRRN(head, nroRegistos);
    header_set_status(head, '1');
    header_to_bin(fp_bin, head);
    reg_free(&reg_temp);
    head_free(&head);
    AVL_apagar(&nomesEstacoes);
    AVL_apagar(&paresEstacoes);
    fclose(fp_bin);
    BinarioNaTela(arquivo_bin);
}

//Função para verificar o status do arquivo antes de realizar buscas ou leituras
bool verificarStatusArquivo(FILE* fp) {

    if (fp == NULL) {
        printf("Falha no processamento do arquivo.");
        return 0;
    }

    char status;
    fread(&status, sizeof(char), 1, fp);
    if (status == '0') {
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return 0;
    }

    return 1;
}

// Funcionalidade 2: impressão de todos os registros do arquivo binário 
void select_all(char* nome_arquivo) {

    FILE* fp = fopen(nome_arquivo, "rb");
    // Verifica status de consistência no cabeçalho (byte 0)
    if(!verificarStatusArquivo(fp)) return;

    // Lê o número de RRNs do registro
    fseek(fp, 5, SEEK_SET);
    int totalRRN;
    fread(&totalRRN, sizeof(int), 1, fp);

    // Pula para o primeiro registro
    fseek(fp, 17, SEEK_SET);

    Registro* reg;
    int registros_lidos = 0;
    for(int i = 0; i < totalRRN; i ++){
        reg = bin_to_reg(fp);

        if (reg != NULL) {
            registros_lidos ++;
            print_reg(reg);
        }
        reg_free(&reg);
    }

    if (registros_lidos == 0)
        printf("Registro inexistente.\n");

    fclose(fp);
}

//funcionalidade 4: remoção lógica de registros

Header* ler_header_do_bin(FILE* fp) {
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

void criar_par(Registro* reg, char* pair){
    if(reg_get_codProxEstacao(reg) == -1){
        pair[0] = '\0';
        return;
    }
    if (reg_get_codEstacao(reg) < reg_get_codProxEstacao(reg))
        snprintf(pair, 20, "%d,%d", reg_get_codEstacao(reg), reg_get_codProxEstacao(reg));
    else
        snprintf(pair, 20, "%d,%d", reg_get_codProxEstacao(reg), reg_get_codEstacao(reg));
    return;
}

void povoa_avl(FILE* fp, AVL* nomesEstacoes, AVL* paresEstacoes){
    // Lê o número de RRNs do registro
    fseek(fp, 5, SEEK_SET);
    int totalRRN;
    fread(&totalRRN, sizeof(int), 1, fp);

    // Pula para o primeiro registro
    fseek(fp, 17, SEEK_SET);

    Registro* reg_aux;
    char pair[20];

    for(int i = 0; i < totalRRN; i ++) {
        reg_aux = bin_to_reg(fp);

        if (reg_aux != NULL) {
            AVL_inserir(nomesEstacoes, reg_get_nomeEstacao(reg_aux));
            
            if(reg_get_codProxEstacao(reg_aux) != -1){
                criar_par(reg_aux, pair);
                if(pair[0] != '\0') AVL_inserir(paresEstacoes, pair);
            }
            reg_free(&reg_aux);
        }
    }
    return;
}

long int offset_RRN(int RRN){
    return RRN*80 + tam_header;
}

void remover(char* nome_arquivo) {
    
    FILE* fp = fopen(nome_arquivo, "rb+");
    if (!verificarStatusArquivo(fp)) return;

    //carrega o Header para a memória e marca como inconsistente no início
    Header* h = ler_header_do_bin(fp);
    header_set_status(h, '0');
    header_to_bin(fp, h);

    //povoa a AVL com os nomes e pares das estações
    AVL* nomesEstacoes = AVL_criar();
    AVL* paresEstacoes = AVL_criar();
    povoa_avl(fp, nomesEstacoes, paresEstacoes);

    int n_remocoes; 
    if (scanf("%d", &n_remocoes) != 1) {
        fclose(fp); 
        head_free(&h); 
        AVL_apagar(&nomesEstacoes); 
        AVL_apagar(&paresEstacoes);
        return;
    }

    char removido = '1';
    long int offset;
    int RRN;
    int topo;
    while (n_remocoes--) {
        int nroEstacoesFiltradas;
        char pair[20];
        Registro** registrosFiltrados = filtros_multiplos(fp, &nroEstacoesFiltradas);
        
        for(int i = 0; i < nroEstacoesFiltradas; i ++){
            //se a quantidade na AVL for 1, esta é a última ocorrência desse nome no arquivo
            char* nomeEstacao = reg_get_nomeEstacao(registrosFiltrados[i]);
            AVL_remover(nomesEstacoes, nomeEstacao); //decrementa count ou remove nó
            
            // Faz a mesma coisa com os pares da estação
            criar_par(registrosFiltrados[i], pair);
            if(pair[0] != '\0') AVL_remover(paresEstacoes, pair); //decrementa count ou remove nó
            
            //Grava a alteração no registro diretamente no arquivo no offset correto
            RRN = reg_get_RRN(registrosFiltrados[i]);
            offset = offset_RRN(RRN);
            topo = header_get_topo(h);
            fseek(fp, offset, SEEK_SET);
            fwrite(&removido, sizeof(char), 1, fp);
            fwrite(&topo, sizeof(int), 1, fp);

            header_set_topo(h, RRN); // O novo topo da pilha vira este RRN
        }
    }
    //salva o Header atualizado (topo, nroEstacoes, nroPares) e consistente
    header_set_nroEstacoes(h, AVL_tamanho(nomesEstacoes));
    header_set_nroParesEstacao(h, AVL_tamanho(paresEstacoes));
    header_set_status(h, '1'); 
    header_to_bin(fp, h); 

    fclose(fp);
    head_free(&h);
    AVL_apagar(&nomesEstacoes);
    AVL_apagar(&paresEstacoes);
    BinarioNaTela(nome_arquivo); 
}

void update(char* nome_arquivo) {
    
    FILE* fp = fopen(nome_arquivo, "rb+");
    if (!verificarStatusArquivo(fp)) return;

    //carrega o Header para a memória e marca como inconsistente no início
    Header* h = ler_header_do_bin(fp);
    header_set_status(h, '0');
    header_to_bin(fp, h);

    //povoa a AVL com os nomes e pares das estações
    AVL* nomesEstacoes = AVL_criar();
    AVL* paresEstacoes = AVL_criar();
    povoa_avl(fp, nomesEstacoes, paresEstacoes);

    int n_updates; 
    if (scanf("%d", &n_updates) != 1) {
        fclose(fp); 
        head_free(&h); 
        AVL_apagar(&nomesEstacoes); 
        AVL_apagar(&paresEstacoes);
        return;
    }

    while (n_updates--) {
        int nroEstacoesFiltradas;
        Registro** registrosFiltrados = filtros_multiplos(fp, &nroEstacoesFiltradas);
        
        int nroCampos;
        scanf("%d", &nroCampos);
        for(int i = 0; i < nroCampos; i++){
            char campo[50];
            char valor[50];

            scanf("%s", campo);
            ScanQuoteString(valor);

            int valor_inteiro;
            if(strcmp(valor, "") == 0)
                valor_inteiro = -1;
            else 
                valor_inteiro = atoi(valor);

            if (strcmp(campo, "codEstacao") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    char pair[20];
                    criar_par(registrosFiltrados[i], pair);
                    if(pair[0] != '\0') AVL_remover(paresEstacoes, pair);
                    reg_set_codEstacao(registrosFiltrados[j], valor_inteiro);
                    if(pair[0] != '\0') criar_par(registrosFiltrados[i], pair);
                    AVL_inserir(paresEstacoes, pair);
                }
            }
                
            if (strcmp(campo, "nomeEstacao") == 0) {
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    AVL_remover(nomesEstacoes, reg_get_nomeEstacao(registrosFiltrados[j]));
                    reg_set_nomeEstacao(registrosFiltrados[j], valor);
                    AVL_inserir(nomesEstacoes, reg_get_nomeEstacao(registrosFiltrados[j]));
                }
            }

            if (strcmp(campo, "codLinha") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_codLinha(registrosFiltrados[j], valor_inteiro);
                }
            }

            if (strcmp(campo, "nomeLinha") == 0) {
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_nomeLinha(registrosFiltrados[j], valor);
                }
            } 

            if (strcmp(campo, "codProxEstacao") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    char pair[20];
                    criar_par(registrosFiltrados[i], pair);
                    if(pair[0] != '\0') AVL_remover(paresEstacoes, pair);
                    reg_set_codProxEstacao(registrosFiltrados[j], valor_inteiro);
                    criar_par(registrosFiltrados[i], pair);
                    if(pair[0] != '\0') AVL_inserir(paresEstacoes, pair);
                }
            }

            if (strcmp(campo, "distProxEstacao") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_distProxEstacao(registrosFiltrados[j], valor_inteiro);
                }
            }

            if (strcmp(campo, "codLinhaIntegra") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_codLinhaIntegra(registrosFiltrados[j], valor_inteiro);
                }
            }
            if (strcmp(campo, "codEstIntegra") == 0){
                for(int j = 0; j < nroEstacoesFiltradas; j++){
                    reg_set_codEstIntegra(registrosFiltrados[j], valor_inteiro);
                }
            }
        }
        
        // Atualiza os registros no arquivo
        int offset;
        for(int i = 0; i < nroEstacoesFiltradas; i++){
            offset = offset_RRN(reg_get_RRN(registrosFiltrados[i]));
            reg_to_bin(registrosFiltrados[i], fp, offset);
        }
    }

    //salva o Header atualizado (nroEstacoes, nroPares) e consistente
    header_set_nroEstacoes(h, AVL_tamanho(nomesEstacoes));
    header_set_nroParesEstacao(h, AVL_tamanho(paresEstacoes));
    header_set_status(h, '1'); 
    header_to_bin(fp, h); 

    fclose(fp);
    head_free(&h);
    AVL_apagar(&nomesEstacoes);
    BinarioNaTela(nome_arquivo); 
}

int scan_int(){
    char valor[50];
    int valor_inteiro;
    ScanQuoteString(valor);
    if(strcmp(valor, "") == 0)
        valor_inteiro = -1;
    else 
        valor_inteiro = atoi(valor);
    return valor_inteiro;
}

void insert(char* nome_arquivo){

    FILE* fp = fopen(nome_arquivo, "rb+");
    if (!verificarStatusArquivo(fp)) return;

    //carrega o Header para a memória e marca como inconsistente no início
    Header* h = ler_header_do_bin(fp);
    header_set_status(h, '0');
    header_to_bin(fp, h);

    //povoa a AVL com os nomes e pares das estações
    AVL* nomesEstacoes = AVL_criar();
    AVL* paresEstacoes = AVL_criar();
    povoa_avl(fp, nomesEstacoes, paresEstacoes);

    int n_insercoes; 
    if (scanf("%d", &n_insercoes) != 1) {
        fclose(fp); 
        head_free(&h); 
        AVL_apagar(&nomesEstacoes); 
        AVL_apagar(&paresEstacoes);
        return;
    }

    char valor[50];
    int valor_inteiro;
    long int offset;
    int prox;
    int topo;
    while (n_insercoes--) {
        Registro* reg = criar_registro();
        
        valor_inteiro = scan_int();
        reg_set_codEstacao(reg, valor_inteiro);
        
        ScanQuoteString(valor);
        reg_set_nomeEstacao(reg, valor);
        reg_set_tamNomeEstacao(reg, strlen(valor));

        valor_inteiro = scan_int();        
        reg_set_codLinha(reg, valor_inteiro);
        
        ScanQuoteString(valor);
        reg_set_nomeLinha(reg, valor);
        reg_set_tamNomeLinha(reg, strlen(valor));

        valor_inteiro = scan_int();
        reg_set_codProxEstacao(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_distProxEstacao(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_codLinhaIntegra(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_codEstIntegra(reg, valor_inteiro);
        
        AVL_inserir(nomesEstacoes, reg_get_nomeEstacao(reg));
        char pair[20];
        criar_par(reg, pair);
        if(pair[0] != '\0') AVL_inserir(paresEstacoes, pair);

        topo = header_get_topo(h);
        if(topo == -1){
            offset = offset_RRN(header_get_proxRRN(h));
            header_set_proxRRN(h, header_get_proxRRN(h) + 1);
        }else{
            offset = offset_RRN(topo);
            fseek(fp, offset+1, SEEK_SET);
            fread(&prox, sizeof(int), 1, fp);
            header_set_topo(h, prox);
        }

        reg_to_bin(reg, fp, offset);
    }

    //salva o Header atualizado (nroEstacoes, nroPares) e consistente
    header_set_nroEstacoes(h, AVL_tamanho(nomesEstacoes));
    header_set_nroParesEstacao(h, AVL_tamanho(paresEstacoes));
    header_set_status(h, '1'); 
    header_to_bin(fp, h); 

    fclose(fp);
    head_free(&h);
    AVL_apagar(&nomesEstacoes);
    BinarioNaTela(nome_arquivo); 
}