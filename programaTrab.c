#include <stdio.h>
#include <stdlib.h>
#include "funcionalidades.h"
#include "busca.h"

int main(){
    int option;
    scanf("%d", &option);
    char arquivo_bin[100];
    switch (option){
    case 1: //leitura de registros csv e escrita em binário
        char arquivo_csv[100];
        scanf("%s", arquivo_csv);
        scanf("%s", arquivo_bin);
        ler_csv(arquivo_csv, arquivo_bin);
        break;

    case 2: //impressão de todos os registros do arquivo binário
        scanf("%s", arquivo_bin);
        select_all(arquivo_bin);
        break;

    case 3: //recuperacao de registros do arquivo binário por campo 
        scanf("%s", arquivo_bin); 
        busca_parametrizada(arquivo_bin);
        break;

    case 4: // DELETE LOGIC (PILHA) [cite: 252]
        scanf("%s", arquivo_bin);
        // A funcionalidade4 cuida da AVL, da Pilha e do Header final
        remover(arquivo_bin);
        break;
    case 5: //insercao de novos registros
        scanf("%s", arquivo_bin);
        inserir(arquivo_bin);
        break;

    case 6: //atualizacao de registros
        scanf("%s", arquivo_bin);
        atualizar(arquivo_bin);
        break;  
    
    default:
        break;
    }
    return 0;
}