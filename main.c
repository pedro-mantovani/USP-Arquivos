#include <stdio.h>
#include <stdlib.h>
#include "IO.h"
#include "registro.h"
#include "AVL.h"

int main(){
    FILE* fp_csv = fopen("estacoes.csv", "r");
    FILE* fp_bin = fopen("exit.bin", "wb");
    AVL* nomesEstacoes = read_csv(fp_csv, fp_bin);
    return 0;
}