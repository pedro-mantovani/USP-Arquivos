#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL.h"
#include "registro.h"
#include "header.h"
#include "busca.h"
#include "funcionalidades.h"
#include "utilities.h"

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