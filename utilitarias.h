#ifndef UTILITARIAS_H
    #define UTILITARIAS_H

    #include "registro.h"
    #include "header.h"
    #include <stdbool.h>

    // Funções fornecidas
    void BinarioNaTela(char *arquivo);
    void ScanQuoteString(char *str);

    // Funções criadas
    int convert_num(char* str_num);
    long int RRN_to_offset(int RRN);
    int offset_to_RRN(int offset);
    long int arv_RRN_to_offset(int RRN);
    void criar_par(Registro* reg, char* pair);
    int verificarStatusArquivo(FILE* fp);
    int scan_int();
    void atualizar_metricas_cabecalho(FILE* fp_dados, Header* h);
    int busca_binaria(int vetor[], int tamanho, int alvo);
    void shift(int vetor[], int pos, int tam, bool back); 

#endif