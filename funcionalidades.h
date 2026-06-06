#ifndef FUNCIONALIDADES_H
    #define FUNCIONALIDADES_H

    void ler_csv(char* arquivo_csv, char* arquivo_bin);
    void mostrar_todos(char* nome_arquivo);
    void busca_parametrizada(char* nome_arquivo);
    void busca_parametrizada_nova(char* arquivo_dados, char* arquivo_indice);
    void remover(char* nome_arquivo);
    void inserir(char* nome_arquivo);
    void atualizar(char* nome_arquivo);
    void criar_arvore(char* arquivo_dados, char* arquivo_arvore);

#endif