#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL.h"
#include "registro.h"
#include "header.h"
#include "funcionalidades.h"
#include "utilities.h"

//funcionalidade 1: leitura de csv e escrita em binário
void ler_csv(char* arquivo_csv, char* arquivo_bin){
    
    // Abre o arquivo
    FILE* fp_csv = fopen(arquivo_csv, "r");
    if(fp_csv == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    FILE* fp_bin = fopen(arquivo_bin, "wb");
    if(fp_bin == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Cria o cabeçalho
    Header* head = criar_header();
    // Coloca o cabeçalho no arquivo binário (com status inconsistente)
    header_to_bin(fp_bin, head);

    // Define as variáveis temporárias usadas para leitura
    char linha[256]; // Linha da tabela
    char* ptr; // Ponteiro pa percorrer cada linha da tabela
    char* token; // Células da tabela
    int nroRegistos = 0; // Número de registros lidos
    int paresEstacoes = 0; // Número de pares de estação (valore não nulos do codProxEstacao)
    Registro* reg_temp = criar_registro(); // Registro temporário
    AVL* nomesEstacoes = AVL_criar(); // AVL para armazenar os nomes únicos

    if (fp_csv == NULL) return;

    fgets(linha, sizeof(linha), fp_csv); // Queima a linha de cabeçalho
    

    /*
    Lê o arquivo csv linha a linha
    Quebra a linha em várias strins
    Salva as strings nos respectivos campos do registro
    Se necessário converte a string para número antes disso
    */
    while (fgets(linha, sizeof(linha), fp_csv) != NULL) {
        ptr = linha;

        // Troca o \n por \0
        linha[strcspn(linha, "\n") - 1] = '\0';
        
        token = strsep(&ptr, ",");
        reg_set_codEstacao(reg_temp, convert_num(token));

        token = strsep(&ptr, ",");
        reg_set_tamNomeEstacao(reg_temp, strlen(token));
        reg_set_nomeEstacao(reg_temp, token);
        AVL_inserir(nomesEstacoes, token);

        reg_set_codLinha(reg_temp, convert_num(strsep(&ptr, ",")));

        token = strsep(&ptr, ",");
        reg_set_tamNomeLinha(reg_temp, strlen(token));
        reg_set_nomeLinha(reg_temp, token);

        token = strsep(&ptr, ",");
        reg_set_codProxEstacao(reg_temp, convert_num(token));
        // Se a string não é vazia incrementa o número de pares das estações
        if(token[0] != '\0')
            paresEstacoes ++;

        reg_set_distProxEstacao(reg_temp, convert_num(strsep(&ptr, ",")));

        reg_set_codLinhaIntegra(reg_temp, convert_num(strsep(&ptr, ",")));
        
        reg_set_codEstIntegra(reg_temp, convert_num(strsep(&ptr, ",")));

        // Salva o registro no arquivo binário
        // Note que o byteoffset passado para a função é -1
        // Então o registro será colocado na posição atual do fp, evitando fseeks desnecessários
        reg_to_bin(reg_temp, fp_bin, -1);

        // Incrementa o número de registros
        nroRegistos ++;
    }

    // Fecha o arquivo
    fclose(fp_csv);

    // Atualiza os campos do cabeçalho
    header_set_nroEstacoes(head, AVL_tamanho(nomesEstacoes));
    header_set_nroParesEstacao(head, paresEstacoes);
    header_set_proxRRN(head, nroRegistos);
    header_set_status(head, '1');
    // Salva o cabeçalho no arquivo binário
    header_to_bin(fp_bin, head);
    
    // Libera as memórias alocadas
    reg_free(&reg_temp);
    head_free(&head);
    AVL_apagar(&nomesEstacoes);

    // Fecha o arquivo
    fclose(fp_bin);

    // Utiliza a função BinarioNaTela
    BinarioNaTela(arquivo_bin);
}