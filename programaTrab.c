#include <stdio.h>
#include "funcionalidades.h"

/*
Alunos:
Beatriz Aredes Texeira - N° USP: 16856317
Pedro Otavio Mantovani - N° USP: 16896987
*/

int main(){
    int option;
    scanf("%d", &option);
    char arquivo_bin[100];
    switch (option){
    case 1: // Leitura de registros csv e escrita em binário
        char arquivo_csv[100];
        scanf("%s", arquivo_csv);
        scanf("%s", arquivo_bin);
        ler_csv(arquivo_csv, arquivo_bin);
        break;

    case 2: // Impressão de todos os registros do arquivo binário
        scanf("%s", arquivo_bin);
        mostrar_todos(arquivo_bin);
        break;

    case 3: // Recuperacao de registros do arquivo binário por campo 
        scanf("%s", arquivo_bin); 
        busca_parametrizada(arquivo_bin);
        break;

    case 4: // Função de de remover registros que atendem a determinados critérios de busca
        scanf("%s", arquivo_bin);
        remover(arquivo_bin);
        break;

    case 5: // Insercao de novos registros
        scanf("%s", arquivo_bin);
        inserir(arquivo_bin);
        break;

    case 6: // Atualizacao de registros
        scanf("%s", arquivo_bin);
        atualizar(arquivo_bin);
        break;  
    
    case 7: // Escrita da árvore B com base em um arquivo binário
        char arquivo_dados[100];
        scanf("%s", arquivo_dados);
        scanf("%s", arquivo_bin);
        criar_arvore(arquivo_dados, arquivo_bin);
        break;

    case 8: // Recuperacao de registros do arquivo binário por campo 
        char arquivo_arv[100];
        scanf("%s", arquivo_bin); 
        scanf("%s", arquivo_arv); 
        busca_parametrizada_nova(arquivo_bin, arquivo_arv);
        break;

    case 9: // Inserção de novos registros
        char arquivo_arvore[100];
        scanf("%s", arquivo_bin); 
        scanf("%s", arquivo_arvore); 
        //inserir(arquivo_bin);
        inserir_nova(arquivo_bin, arquivo_arvore);
        break;

    default:
        break;
    }
    return 0;
}