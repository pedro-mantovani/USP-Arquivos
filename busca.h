#ifndef BUSCA_H
#define BUSCA_H

#include "registro.h"

bool filter(Registro *reg, char* criterio, char* valor);
Registro** filtrar_registros(FILE* fp, char* criterio, char* valor, int* nroEstacoesFiltradas);
Registro** filtrar_vetor(Registro** registros, char* criterio, char* valor, int* nroEstacoesFiltradas);
Registro** filtros_multiplos(FILE* fp, int* nroEstacoesFiltradas);
void buscar(char* nome_arquivo);

#endif