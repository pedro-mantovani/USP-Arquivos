#ifndef IO_H
    #define IO_H

    #include "registro.h"
    #include "AVL.h"

    Registro* criar_registro();
    
    Registro* ler_registro(FILE* f);

    void read_csv(FILE* fp_csv, FILE* fp_bin);
    void BinarioNaTela(char *arquivo);

    void select_all(char* nome_arquivo);
    void print_campo_int(int valor);
    void print_campo_str(char* str);
    void busca_parametrizada(char* nome_arquivo);

#endif