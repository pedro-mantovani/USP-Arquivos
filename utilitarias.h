#ifndef UTILITARIAS_H
    #define UTILITARIAS_H

    void BinarioNaTela(char *arquivo);
    void ScanQuoteString(char *str);
    int convert_num(char* str_num);
    long int RRN_to_offset(int RRN);
    void criar_par(Registro* reg, char* pair);
    int verificarStatusArquivo(FILE* fp);
    int scan_int();

#endif