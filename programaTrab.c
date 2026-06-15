#include <stdio.h>
#include "funcionalidades.h"

/*
Alunos:
Beatriz Aredes Texeira - N° USP: 16856317
Pedro Otavio Mantovani - N° USP: 16896987
*/

typedef enum {
    LEITURA_CSV = 1,
    MOSTRAR_TODOS,
    BUSCA_PARAMETRIZADA,
    REMOVER_REGISTROS,
    INSERIR_REGISTROS,
    ATUALIZAR_REGISTROS,
    CRIAR_ARVORE_B,
    BUSCA_COM_ARVORE,
    INSERIR_COM_ARVORE,
    REMOVER_COM_ARVORE
} Opcao;

int main() {
    int option;
    scanf("%d", &option);

    char arquivo_dados[100];
    char arquivo_arvore[100];

    switch ((Opcao) option) {

    case LEITURA_CSV: {
        char arquivo_csv[100];
        scanf("%s", arquivo_csv);
        scanf("%s", arquivo_dados);
        ler_csv(arquivo_csv, arquivo_dados);
        break;
    }

    case MOSTRAR_TODOS:
        scanf("%s", arquivo_dados);
        mostrar_todos(arquivo_dados);
        break;

    case BUSCA_PARAMETRIZADA:
        scanf("%s", arquivo_dados);
        busca_parametrizada(arquivo_dados);
        break;

    case REMOVER_REGISTROS:
        scanf("%s", arquivo_dados);
        remover(arquivo_dados);
        break;

    case INSERIR_REGISTROS:
        scanf("%s", arquivo_dados);
        inserir(arquivo_dados);
        break;

    case ATUALIZAR_REGISTROS:
        scanf("%s", arquivo_dados);
        atualizar(arquivo_dados);
        break;

    case CRIAR_ARVORE_B:
        scanf("%s", arquivo_dados);
        scanf("%s", arquivo_arvore);
        criar_arvore(arquivo_dados, arquivo_arvore);
        break;

    case BUSCA_COM_ARVORE:
        scanf("%s", arquivo_dados);
        scanf("%s", arquivo_arvore);
        busca_parametrizada_nova(arquivo_dados, arquivo_arvore);
        break;

    case INSERIR_COM_ARVORE:
        scanf("%s", arquivo_dados);
        scanf("%s", arquivo_arvore);
        inserir_nova(arquivo_dados, arquivo_arvore);
        break;

    case REMOVER_COM_ARVORE:
        scanf("%s", arquivo_dados);
        scanf("%s", arquivo_arvore);
        remover_novo(arquivo_dados, arquivo_arvore);
        break;

    default:
        break;
    }

    return 0;
}