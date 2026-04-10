#ifndef BUSCA_H
#define BUSCA_H

    #include <stdbool.h>
    #include "registro.h"

    typedef struct {
        int n;               // Número de campos 
        char** campos;       // Nomes dos campos
        char** valores;      // Valores dos campos
    } Campos;

    Campos* criar_campos(int n);
    void apagar_campos(Campos **c);

    void preencher_campos(Campos *c);
    bool registro_passa_filtro(Registro *reg, Campos *c);

#endif