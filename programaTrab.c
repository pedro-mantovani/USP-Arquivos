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
    char arquivo_dados[100];
    char arquivo_arvore[100];
    switch (option){
    case 1: // Leitura de registros csv e escrita em binário
        char arquivo_csv[100];
        scanf("%s", arquivo_csv);
        scanf("%s", arquivo_dados);
        ler_csv(arquivo_csv, arquivo_dados);
        break;

    case 2: // Impressão de todos os registros do arquivo binário
        scanf("%s", arquivo_dados);
        mostrar_todos(arquivo_dados);
        break;

    case 3: // Recuperacao de registros do arquivo binário por campo 
        scanf("%s", arquivo_dados); 
        busca_parametrizada(arquivo_dados);
        break;

    case 4: // Função de de remover registros que atendem a determinados critérios de busca
        scanf("%s", arquivo_dados);
        remover(arquivo_dados);
        break;

    case 5: // Insercao de novos registros
        scanf("%s", arquivo_dados);
        inserir(arquivo_dados);
        break;

    case 6: // Atualizacao de registros
        scanf("%s", arquivo_dados);
        atualizar(arquivo_dados);
        break;  
    
    case 7: // Escrita da árvore B com base em um arquivo binário
        scanf("%s", arquivo_dados);
        scanf("%s", arquivo_arvore);
        criar_arvore(arquivo_dados, arquivo_arvore);
        break;

    case 8: // Recuperacao de registros do arquivo binário por campo 
        scanf("%s", arquivo_dados); 
        scanf("%s", arquivo_arvore); 
        busca_parametrizada_nova(arquivo_dados, arquivo_arvore);
        break;

    case 9: // Inserção de novos registros
        scanf("%s", arquivo_dados); 
        scanf("%s", arquivo_arvore); 
        inserir_nova(arquivo_dados, arquivo_arvore);
        break;

    case 10: // Remoção registros
        scanf("%s", arquivo_dados); 
        scanf("%s", arquivo_arvore); 
        remover_novo(arquivo_dados, arquivo_arvore);
        break;

    default:
        break;
    }
    return 0;
}