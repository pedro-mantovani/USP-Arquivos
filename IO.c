#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "AVL.h"
#include "registro.h"

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

void read_csv(FILE* fp_csv, FILE* fp_bin){

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
    AVL_apagar(nomesEstacoes);
    AVL_apagar(paresEstacoes);
    fclose(fp_bin);
}

//funcionalidade 2: 

void print_campo_int(int valor) {
    if (valor == -1) printf("NULO");
    else printf("%d", valor);
}

void print_campo_str(char* str) {
    if (str == NULL || strlen(str) == 0) printf("NULO");
    else printf("%s", str);
}

void select_all(char* nome_arquivo) {
    FILE* fp = fopen(nome_arquivo, "rb");
    if (fp == NULL) {
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Verifica status de consistência no cabeçalho (byte 0)
    char status;
    fread(&status, sizeof(char), 1, fp);
    if (status == '0') {
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    // Pula o resto do cabeçalho para chegar no primeiro registro (17 bytes total)
    fseek(fp, 17, SEEK_SET);

    Registro* reg;
    int encontrou_pelo_menos_um = 0;

    // Loop de leitura sequencial
    while ((reg = bin_to_reg(fp)) != NULL) {
        // Ignora registros logicamente removidos
        if (reg_get_removido(reg) == '0') {
            encontrou_pelo_menos_um = 1;
            
            print_campo_int(reg_get_codEstacao(reg)); printf(" ");
            print_campo_str(reg_get_nomeEstacao(reg)); printf(" ");
            print_campo_int(reg_get_codLinha(reg)); printf(" ");
            print_campo_str(reg_get_nomeLinha(reg)); printf(" ");
            print_campo_int(reg_get_codProxEstacao(reg)); printf(" ");
            print_campo_int(reg_get_distProxEstacao(reg)); printf(" ");
            print_campo_int(reg_get_codLinhaIntegra(reg)); printf(" ");
            print_campo_int(reg_get_codEstIntegra(reg));
            printf("\n");
        }
        reg_free(&reg);
    }

    if (!encontrou_pelo_menos_um) {
        printf("Registro inexistente.\n");
    }

    fclose(fp);
}

//funcionalidade 3: busca parametrizada


// Problemas - POO
// 1. Não está modularizado
// 2. Não retorna o ponteiro para o registro

void busca_parametrizada(char* nome_arquivo) {
    FILE* fp = fopen(nome_arquivo, "rb");
    if (fp == NULL) {
        printf("Falha no processamento do arquivo."); 
        return;
    }

    char status;
    fread(&status, sizeof(char), 1, fp); 
    if (status == '0') {
        printf("Falha no processamento do arquivo."); 
        fclose(fp);
        return;
    }

    int n_buscas;
    scanf("%d", &n_buscas); 

    for (int b = 0; b < n_buscas; b++) {
        int m_filtros;
        scanf("%d", &m_filtros); 

        // Arrays simples para guardar os filtros da busca atual
        // Um malloc aqui nada a ver? - POO
        char nomes[m_filtros][50];
        char valoresStr[m_filtros][100];
        int valoresInt[m_filtros];

        for (int i = 0; i < m_filtros; i++) {
            scanf("%s", nomes[i]);
            ScanQuoteString(valoresStr[i]); 
            
            // Se não for NULO, tentamos converter para int para facilitar
            if (strcmp(valoresStr[i], "") != 0) {
                valoresInt[i] = atoi(valoresStr[i]);
            } else {
                valoresInt[i] = -1; // Representação de NULO para inteiros 
            }
            //printf("Filtro %d: campo=%s, valor_str=%s, valor_int=%d\n", i+1, nomes[i], valoresStr[i], valoresInt[i]);
        }

        fseek(fp, 17, SEEK_SET); 
        Registro* reg;
        int encontrou_algum = 0;

        while ((reg = bin_to_reg(fp)) != NULL) {
            if (reg_get_removido(reg) == '0') { 
                int bate_todos = 1;

                for (int i = 0; i < m_filtros; i++) {
                    int match = 0;
                    // Comparações diretas
                    if (strcmp(nomes[i], "codEstacao") == 0) {
                        if (reg_get_codEstacao(reg) == valoresInt[i]) match = 1;
                    } else if (strcmp(nomes[i], "nomeEstacao") == 0) {
                        if (strcmp(reg_get_nomeEstacao(reg), valoresStr[i]) == 0) match = 1;
                    } else if (strcmp(nomes[i], "codLinha") == 0) {
                        if (reg_get_codLinha(reg) == valoresInt[i]) match = 1;
                    } else if (strcmp(nomes[i], "nomeLinha") == 0) {
                        if (strcmp(reg_get_nomeLinha(reg), valoresStr[i]) == 0) match = 1;
                    } else if (strcmp(nomes[i], "codProxEstacao") == 0) {
                        if (reg_get_codProxEstacao(reg) == valoresInt[i]) match = 1;
                    } else if (strcmp(nomes[i], "distProxEstacao") == 0) {
                        if (reg_get_distProxEstacao(reg) == valoresInt[i]) match = 1;
                    } else if (strcmp(nomes[i], "codLinhaIntegra") == 0) {
                        if (reg_get_codLinhaIntegra(reg) == valoresInt[i]) match = 1;
                    } else if (strcmp(nomes[i], "codEstIntegra") == 0) {
                        if (reg_get_codEstIntegra(reg) == valoresInt[i]) match = 1;
                    }

                    if (!match) {
                        bate_todos = 0;
                        break;
                    }
                }

                if (bate_todos) {
                    encontrou_algum = 1;
                    print_campo_int(reg_get_codEstacao(reg)); printf(" ");
                    print_campo_str(reg_get_nomeEstacao(reg)); printf(" ");
                    print_campo_int(reg_get_codLinha(reg)); printf(" ");
                    print_campo_str(reg_get_nomeLinha(reg)); printf(" ");
                    print_campo_int(reg_get_codProxEstacao(reg)); printf(" ");
                    print_campo_int(reg_get_distProxEstacao(reg)); printf(" ");
                    print_campo_int(reg_get_codLinhaIntegra(reg)); printf(" ");
                    print_campo_int(reg_get_codEstIntegra(reg));
                    printf("\n");
                }
            }
            reg_free(&reg);
        }

        if (!encontrou_algum) {
            printf("Registro inexistente.\n"); 
        }

        printf("\n");
    }
    fclose(fp);
}

void update_reg(){
    
}