#include <stdio.h>
#include "header.h"
#include "registro.h"
#include "utilitarias.h"

/*
POO: COMENTAR
*/

void juncao_intercalacao(){

    char arq1[100], arq2[100];
    char campo1[30], campo2[30];

    scanf("%s", arq1);
    scanf("%s", campo1);

    scanf("%s", arq2);
    scanf("%s", campo2);

    if(!ordenar_arquivo(arq1, "temp1.bin", campo1)) return;
    if(!ordenar_arquivo(arq2, "temp2.bin", campo2)) return;

    FILE *fp1 = fopen("temp1.bin","rb");
    if(!verificarStatusArquivo(fp1)) return;

    FILE *fp2 = fopen("temp2.bin","rb");
    if(!verificarStatusArquivo(fp2)){
        fclose(fp1);
        return;
    }

    Header *h1 = bin_to_header(fp1);
    Header *h2 = bin_to_header(fp2);

    Registro *r1 = NULL;
    Registro *r2 = NULL;

    bool encontrou = false;

    while(1){

        do{
            reg_free(&r1);
            r1 = bin_to_reg(fp1);
        }while(r1 == NULL && !feof(fp1));

        do{
            reg_free(&r2);
            r2 = bin_to_reg(fp2);
        }while(r2 == NULL && !feof(fp2));

        if(r1 == NULL || r2 == NULL)
            break;

        while(r1 != NULL && r2 != NULL){

            int c1 = reg_get_codProxEstacao(r1);
            int c2 = reg_get_codEstacao(r2);

            if(c1 == c2){

                encontrou = true;

                printf("%d %s %s %d %s\n",
                       reg_get_codEstacao(r1),
                       reg_get_nomeEstacao(r1),
                       reg_get_nomeLinha(r1),
                       reg_get_codProxEstacao(r1),
                       reg_get_nomeEstacao(r2));

                reg_free(&r1);
                reg_free(&r2);

                do{
                    r1 = bin_to_reg(fp1);
                }while(r1 == NULL && !feof(fp1));

                do{
                    r2 = bin_to_reg(fp2);
                }while(r2 == NULL && !feof(fp2));

            }else if(c1 < c2){

                reg_free(&r1);

                do{
                    r1 = bin_to_reg(fp1);
                }while(r1 == NULL && !feof(fp1));

            }else{

                reg_free(&r2);

                do{
                    r2 = bin_to_reg(fp2);
                }while(r2 == NULL && !feof(fp2));

            }

            if(r1 == NULL || r2 == NULL)
                break;
        }

        break;
    }

    if(!encontrou) printf("Registro inexistente.\n");

    // Libera a memória
    reg_free(&r1);
    reg_free(&r2);

    header_free(&h1);
    header_free(&h2);

    fclose(fp1);
    fclose(fp2);

    // Remove os arquivos temporários
    remove("temp1.bin");
    remove("temp2.bin");
}