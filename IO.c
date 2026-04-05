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

// Funcionalidade 2: 
void select_all(char* nome_arquivo) {

    FILE* fp = fopen(nome_arquivo, "rb");
    // Verifica status de consistência no cabeçalho (byte 0)
    if(!verificarStatusArquivo(fp)) return;

    // Pula o resto do cabeçalho para chegar no primeiro registro (17 bytes total)
    fseek(fp, 17, SEEK_SET);

    Registro* reg;
    int encontrou_pelo_menos_um = 0; //controle para casa nao existam registro
    while ((reg = bin_to_reg(fp)) != NULL) {
        //ignora registros logicamente removidos ATENCAO NESSE IGNORA SE REALMENTE EH ISSO QUE QUEREM DIZR COM IGNORAR
        if (reg_get_removido(reg) == '0') {
            encontrou_pelo_menos_um = 1;
            print_registro(reg);
        }
        reg_free(&reg);
    }

    if (!encontrou_pelo_menos_um) {
        printf("Registro inexistente.\n");
    }

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


AVL* povoa_avl(FILE* fp){
    fseek(fp, 17, SEEK_SET); //pula o Header de 17 bytes 
    Registro* reg_aux;
    AVL* nomesEstacoes = AVL_criar();

    LEMBRO DELES FALANDO ALGUMA COISA SOBRE NAO PODER LER O REGISTRO INTEIRO SE ELE TIVER SIDO
    EXCLUIDO, TIPO, A GENTE TEM QUE PEGAR DE ALGUMA FORMA SO O CAMPO REMOVIDO EM VEZ DE RECUPERAR TUDO
    MAS 1. NAO SEI SE ENTENDI CERTO E 2. NAO SEI COMO FAZER ISSO
    while ((reg_aux = bin_to_reg(fp)) != NULL) {
        if (reg_get_removido(reg_aux) == '0') {
            AVL_inserir(nomesEstacoes, reg_get_nomeEstacao(reg_aux));

        }
        reg_free(&reg_aux);
    }

    return nomesEstacoes;
}

void remover(char* nome_arquivo) {
    
    FILE* fp = fopen(nome_arquivo, "rb+");
    if (!verificarStatusArquivo(fp)) return;

    //carrega o Header para a memória e marca como inconsistente no início
    Header* h = ler_header_do_bin(fp);
    header_set_status(h, '0');
    header_to_bin(fp, h); 

    //povoa a AVL com os nomes das estações existentes para controle de nroEstacoes
    AVL* nomesEstacoes = povoa_avl(fp);

    int n_buscas; 
    if (scanf("%d", &n_buscas) != 1) {
        fclose(fp); 
        head_free(&h); 
        AVL_apagar(&nomesEstacoes); 
        return;
    }

    while (n_buscas--) {
        int m_filtros; 
        scanf("%d", &m_filtros);

        Busca* b = criar_busca(m_filtros);
        preencher_filtros(b);

        fseek(fp, 17, SEEK_SET); // Retorna ao início dos registros
        Registro* reg;
        long offset;

        //itera pelo arquivo capturando o offset antes de avançar o ponteiro
        while (offset = ftell(fp), (reg = bin_to_reg(fp)) != NULL) {
            //verifica se o registro é válido e atende aos critérios da struct Busca
            if (reg_get_removido(reg) == '0' && registro_passa_filtrob(reg, b)) {
                
                int rrn_atual = (offset - 17) / 80; 
                char* nome_e = reg_get_nomeEstacao(reg);

                //se a quantidade na AVL for 1, esta é a última ocorrência desse nome no arquivo
                if (AVL_quantidade_ocorencias(nomesEstacoes, nome_e) == 1) {
                    header_set_nroEstacoes(h, header_get_nroEstacoes(h) - 1);
                }
                AVL_remover(nomesEstacoes, nome_e); //decrementa count ou remove nó

                //decrementa se houver uma próxima estação
                if (reg_get_codProxEstacao(reg) != -1) {
                    header_set_nroParesEstacao(h, header_get_nroParesEstacao(h) - 1);
                }

                reg_set_removido(reg, '1'); // '1' indica removido logicamente 
                reg_set_proximo(reg, header_get_topo(h)); // Aponta para o antigo topo da pilha
                header_set_topo(h, rrn_atual); // O novo topo da pilha vira este RRN

                //Grava a alteração no registro diretamente no arquivo no offset correto
                fseek(fp, offset, SEEK_SET);
                reg_to_bin(reg, fp, offset); 
                
                // Retorna o ponteiro para o início do próximo registro
                fseek(fp, offset + 80, SEEK_SET); 
            }
            reg_free(&reg);
        }
        // Libera memória da busca atual
        apagar_busca(&b);
    }

    //salva o Header atualizado (topo, nroEstacoes, nroPares) e consistente
    header_set_status(h, '1'); 
    header_to_bin(fp, h); 

    fclose(fp);
    head_free(&h);
    AVL_apagar(&nomesEstacoes);
    BinarioNaTela(nome_arquivo); 
}
*/