#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL.h"
#include "registro.h"
#include "header.h"
#include "busca.h"
#include "funcionalidades.h"
#include "utilitarias.h"

/* 
A função mostrar_todos percorre o arquivo binário lendo todos os registros válidos e imprimindo seus dados. 
Registros removidos são ignorados e, caso nenhum registro seja encontrado, 
é exibida uma mensagem indicando que não existem registros. 
*/

void mostrar_todos(char* nome_arquivo) {

    // Abre o arquivo binário para leitura
    FILE* fp = fopen(nome_arquivo, "rb");
    // Verifica status de consistência no cabeçalho (byte 0)
    if(!verificarStatusArquivo(fp)) return;

    // Lê o número de RRNs do registro
    fseek(fp, 5, SEEK_SET);
    int totalRRN;
    fread(&totalRRN, sizeof(int), 1, fp);

    // Pula para o primeiro registro
    fseek(fp, header_tam, SEEK_SET);

    Registro* reg; // Cria um registro temporário
    int registros_lidos = 0;
    for(int i = 0; i < totalRRN; i ++){
        reg = bin_to_reg(fp); // Lê o registro

        // Se ele não estiver removido
        if (reg != NULL) {
            registros_lidos ++; // Incrementa os registros lidos
            print_reg(reg); // Imprime o registro
        }
        reg_free(&reg); // Libera a memória do registro
    }

    // Se nenhum registro válido foi lido, imprime a mensagem
    if (registros_lidos == 0)
        printf("Registro inexistente.\n");

    fclose(fp);
}