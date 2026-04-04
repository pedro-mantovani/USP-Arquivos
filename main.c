#include <stdio.h>
#include <stdlib.h>
#include "IO.h"
#include "registro.h"
#include "AVL.h"

int main(){
    int option;
    scanf("%d", &option);
    char arquivo_bin[100];
    switch (option){
    case 1: //leitura de registros csv e escrita em binário
        char arquivo_csv[100];
        scanf("%s", arquivo_csv);
        scanf("%s", arquivo_bin);
        FILE* fp_csv = fopen(arquivo_csv, "r");
        if(fp_csv == NULL){
            printf("Falha no processamento do arquivo.");
            break;
        }
        FILE* fp_bin = fopen(arquivo_bin, "wb");
        if(fp_bin == NULL){
            printf("Falha no processamento do arquivo.");
            break;
        }
        read_csv(fp_csv, fp_bin);
        BinarioNaTela(arquivo_bin);
        break;

    case 2: //recuperacao de todos os registros do arquivo binário
        scanf("%s", arquivo_bin);
        select_all(arquivo_bin);
        break;

    case 3: //recuperacao de registros do arquivo binário por campo 
        scanf("%s", arquivo_bin); 
        busca_parametrizada(arquivo_bin);
        break;

    case 4: //remocao logica de registros
        break;

    case 5: //insercao de novos registros
        break;

    case 6: //atualizacao de registros
        break;  
        
    default:
        break;
    }
    return 0;
}