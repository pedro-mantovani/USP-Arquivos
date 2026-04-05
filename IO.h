#ifndef IO_H
    #define IO_H

    #include <stdbool.h>
    #include "registro.h"
    #include "AVL.h"
    #include "busca.h"

    Registro* criar_registro();
    
    Registro* ler_registro(FILE* f);

    void read_csv(char* arquivo_csv, char* arquivo_bin);
    void BinarioNaTela(char *arquivo);
    void ScanQuoteString(char *str);
    bool verificarStatusArquivo(FILE* fp);

    void select_all(char* nome_arquivo);
    void print_campo_int(int valor);
    void print_campo_str(char* str);
    void busca_parametrizada(char* nome_arquivo);

    void remover(char* nome_bin);
    void header_to_bin(FILE* fp, const Header* head);   
    Header* ler_header_do_bin(FILE* fp);

#endif