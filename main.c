#include <stdio.h>
#include <stdlib.h>
#include "IO.h"
#include "registro.h"
#include "AVL.h"

int main(){
    int option;
    scanf("%d", &option);
    switch (option){
    case 1:
        char arquivo_csv[100];
        char arquivo_bin[100];
        scanf("%s", arquivo_csv);
        scanf("%s", arquivo_bin);
        FILE* fp_csv = fopen(arquivo_csv, "r");
        FILE* fp_bin = fopen(arquivo_bin, "wb");
        read_csv(fp_csv, fp_bin);
        BinarioNaTela(arquivo_bin);
        break;
    default:
        break;
    }
    return 0;
}