#ifndef FUNCIONALIDADES_H
    #define FUNCIONALIDADES_H

    //Lê os dados de um arquivo CSV, processa as informações e as armazena 
    //em um arquivo binário, atualizando as métricas do cabeçalho
    void ler_csv(char* arquivo_csv, char* arquivo_bin);

    //Percorre o arquivo binário lendo todos os registros válidos 
    //e os imprime formatados na tela, ignorando os registros logicamente removidos
    void mostrar_todos(char* nome_arquivo);

    //Realiza uma busca sequencial no arquivo binário com base em filtros fornecidos pelo usuário
    void busca_parametrizada(char* nome_arquivo);

    //Realiza uma busca otimizada utilizando o arquivo de índice (Árvore-B) 
    //para campos de chave primária, ou sequencial para os demais critérios
    void busca_parametrizada_nova(char* arquivo_dados, char* arquivo_indice);

    //Lê novos registros da entrada padrão e os insere no arquivo binário
    void inserir(char* nome_arquivo);

    //Insere novos registros reaproveitando espaços removidos no arquivo de dados 
    //e atualiza o arquivo de índice Árvore-B com a nova chave e o respectivo offset
    void inserir_nova(char* arquivo_dados, char* arquivo_indice);

    //Remove logicamente registros do arquivo de dados com base em critérios de busca
    void remover(char* nome_arquivo);

    //Remove registros do arquivo de dados e também remove as suas respectivas chaves do arquivo de índice Árvore-B
    void remover_novo(char* nome_arquivo, char* nome_arquivo_arv);

    //Busca registros que satisfaçam determinados critérios e atualiza seus campos de acordo com a entrada
    void atualizar(char* nome_arquivo);

    //Cria um arquivo de índice em formato de Árvore-B a partir de um arquivo de dados já existente
    void criar_arvore(char* arquivo_dados, char* arquivo_arvore);

    //Realiza a junção de dois arquivos
    void junc_loop(void);

    //Realiza a junção de dois arquivos buscando a correspondência do arquivo 1 diretamente no índice Árvore-B
    void junc_idx(void);

    //Lê parâmetros da entrada, carrega os registros do arquivo de dados para a memória RAM e ordena-os
    void ordenar_funcionalidade(void);

    //Realiza a junção de dois arquivos utilizando a técnica de Intercalação
    void juncao_intercalacao(void);

#endif