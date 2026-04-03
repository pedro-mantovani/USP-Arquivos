#include <stdio.h>
#include <stdlib.h>
#include "IO.h"
#include "registro.h"

int main(){
    FILE* fp_csv = fopen("estacoes.csv", "r");
    FILE* fp_bin = fopen("binario.bin", "wb");
    read_csv(fp_csv, fp_bin);
    return 0;
}