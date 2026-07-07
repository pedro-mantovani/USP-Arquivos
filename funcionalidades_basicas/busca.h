#ifndef BUSCA_H
#define BUSCA_H

    #include <stdbool.h>
    #include "registro.h"

    typedef struct {
        int n;               // Número de campos 
        bool chave_primaria; // Booleno indicando se um dos campos de busca é a chave primária
        char** campos;       // Nomes dos campos
        char** valores;      // Valores dos campos
    } Campos;

    Campos* criar_campos(int n);
    void apagar_campos(Campos **c);

    //funções auxiliaredes para a estrutura de busca
    int preencher_campos(Campos *c);                            //preenche a estrutura com os campos e valores buscado
    int registro_passa_filtro(Registro *reg, Campos *c);        //verifica para cada campo da estrutura se o valor correspondente é o buscado

#endif