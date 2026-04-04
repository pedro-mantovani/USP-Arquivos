#ifndef IO_H
    #define IO_H

    #include "registro.h"
    #include "AVL.h"

    Registro* criar_registro();
    
    Registro* ler_registro(FILE* f);

    void read_csv(FILE* fp_csv, FILE* fp_bin);
    void BinarioNaTela(char *arquivo);

#endif