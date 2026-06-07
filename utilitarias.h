#ifndef UTILITARIAS_H
    #define UTILITARIAS_H

    #include "registro.h"

    // Funções fornecidas
    void BinarioNaTela(char *arquivo);
    void ScanQuoteString(char *str);

    // Funções criadas
    int convert_num(char* str_num);
    long int RRN_to_offset(int RRN);
    long int arv_RRN_to_offset(int RRN);
    void criar_par(Registro* reg, char* pair);
    int verificarStatusArquivo(FILE* fp);
    int scan_int();
    int busca_binaria(int vetor[], int tamanho, int alvo);
    void shift_back(int vetor[], int pos, int tam);

#endif