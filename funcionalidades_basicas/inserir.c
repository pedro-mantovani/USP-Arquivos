#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "registro.h"
#include "header.h"
#include "funcionalidades.h"
#include "utilitarias.h"
#include "arvoreB.h"
#include "AVL.h"

// Função para atualizar as contagens de nomes e pares do cabeçalho varrendo o arquivo inteiro
void atualizar_metricas_cabecalho(FILE* fp_dados, Header* h) {
    // Cria duas árvores AVL
    AVL* avl_nomes = AVL_criar();
    AVL* avl_pares = AVL_criar(); 

    // Posiciona o ponteiro logo após o cabeçalho
    fseek(fp_dados, tam_header, SEEK_SET); 
    int proxRRN = header_get_proxRRN(h);

    // Varre o arquivo
    for (int rrn = 0; rrn < proxRRN; rrn++) {
        
        Registro* reg = bin_to_reg(fp_dados);
        
        if (reg == NULL) continue;
        
        // Só contabiliza registros que não estão logicamente removidos
        if (reg_get_removido(reg) != '1') { 
            // Insere nos nomes
            AVL_inserir(avl_nomes, reg_get_nomeEstacao(reg)); // Insere o nome da estação na AVL

            // Transforma o par da estação em uma string do tipo "a,b" com a < b
            char pair[20];
            criar_par(reg, pair);                                   // Cria a string do par
            if(pair[0] != '\0') AVL_inserir(avl_pares, pair);       // Caso ela seja válida insere na AVL
        }

        reg_free(&reg);
    }

    // Atualiza os valores do cabeçalho na RAM
    header_set_nroEstacoes(h, AVL_tamanho(avl_nomes));
    header_set_nroParesEstacao(h, AVL_tamanho(avl_pares));

    // Libera a memória alocada dinamicamente
    AVL_apagar(&avl_nomes);
    AVL_apagar(&avl_pares);
}


/* A função inserir lê os dados de novos registros e preenche cada campo. 
Para cada inserção, verifica o espaço disponível na pilha de removidos (no arquivo de dados),
se existir, reutiliza, caso contrário insere no fim. 

Após escrever no binário de dados, a função insere a chave (codEstacao) e a 
referência (offset) no arquivo de índice Árvore-B.
*/
void inserir_nova(char* arquivo_dados, char* arquivo_indice) {

    // Abre o arquivo de dados para leitura e escrita
    FILE* fp_dados = fopen(arquivo_dados, "rb+");
    if (!verificarStatusArquivo(fp_dados)) return;

    // Abre o arquivo de índice para leitura e escrita
    FILE* fp_arvore = fopen(arquivo_indice, "rb+");
    if (fp_arvore == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp_dados);
        return;
    }

    // Verifica consistência do arquivo de índice e o marca como inconsistente
    Arv_head* arv_head = bin_to_arv_head(fp_arvore);
    if (arv_head == NULL || arv_head_get_status(arv_head) == '0') {
        printf("Falha no processamento do arquivo.\n");
        if (arv_head) arv_head_free(&arv_head);
        fclose(fp_dados);
        fclose(fp_arvore);
        return;
    }
    
    // Marca o header do arquivo de dados como inconsistente
    char inconsistente = '0';
    fseek(fp_dados, 0, SEEK_SET); 
    fwrite(&inconsistente, sizeof(char), 1, fp_dados); 
    Header* h = bin_to_header(fp_dados); 

    // Faz o mesmo com a árvore B
    fseek(fp_arvore, 0, SEEK_SET); // Volta o ponteiro para o início
    fwrite(&inconsistente, sizeof(char), 1, fp_arvore); // Coloca '0' no primeiro byte

    // Lê quantas inserções serão feitas
    int n_insercoes;
    if (scanf("%d", &n_insercoes) != 1) {
        fclose(fp_dados); 
        fclose(fp_arvore);
        header_free(&h); 
        return;
    }

    char valor_str[50]; 
    int valor_inteiro; 
    long int offset; 
    int prox; 
    int topo; 

    // Faz n inserções
    while (n_insercoes--) {
        Registro* reg = criar_registro();
        
        // Lê e preenche todos os campos do registro
        valor_inteiro = scan_int();
        reg_set_codEstacao(reg, valor_inteiro);
        
        ScanQuoteString(valor_str);
        reg_set_nomeEstacao(reg, valor_str);
        reg_set_tamNomeEstacao(reg, strlen(valor_str));

        valor_inteiro = scan_int();        
        reg_set_codLinha(reg, valor_inteiro);
        
        ScanQuoteString(valor_str);
        reg_set_nomeLinha(reg, valor_str);
        reg_set_tamNomeLinha(reg, strlen(valor_str));

        valor_inteiro = scan_int();
        reg_set_codProxEstacao(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_distProxEstacao(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_codLinhaIntegra(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_codEstIntegra(reg, valor_inteiro);

        int chave_inserida = reg_get_codEstacao(reg);
        
        // Salva o offset em que o registro será escrito
        topo = header_get_topo(h); 
        if (topo == -1) {
            offset = RRN_to_offset(header_get_proxRRN(h)); // Nenhum espaço sobrando: insere no fim
        } else { 
            offset = RRN_to_offset(topo); // Caso contrário coloca no antigo topo da pilha
        }

        //Tenta inserir primeiro na arvore b
        int inseriu = arv_inserir_chave(fp_arvore, arv_head, chave_inserida, offset);

        if(inseriu == 1){
            //printf("registro %d inserido em %ld\n", reg_get_codEstacao(reg), offset);
            // Efetiva a alteração da pilha de removidos ou do proxRRN
            if (topo == -1) { 
                header_set_proxRRN(h, header_get_proxRRN(h) + 1); 
            } else { 
                fseek(fp_dados, offset + 1, SEEK_SET); 
                fread(&prox, sizeof(int), 1, fp_dados); 
                header_set_topo(h, prox); 
            }

            // Grava o registro no arquivo de dados
            reg_to_bin(reg, fp_dados, offset); 
        }

        reg_free(&reg); // Libera o registro da memória
    }

    // Atualiza o número de pares e de estações
    atualizar_metricas_cabecalho(fp_dados, h);

    // Salva o Header do arquivo de dados atualizado e consistente
    header_set_status(h, '1'); 
    header_to_bin(fp_dados, h);
    header_free(&h);

    // Salva o Header do arquivo de índice como consistente novamente
    arv_head_set_status(arv_head, '1');
    arv_head_to_bin(fp_arvore, arv_head);
    arv_head_free(&arv_head);

    // Fecha ambos os arquivos
    fclose(fp_dados); 
    fclose(fp_arvore); 
    
    // Chama as funções de binario na tela para verificação
    BinarioNaTela(arquivo_dados); 
    BinarioNaTela(arquivo_indice); 
}

/* 
Antiga função de inserção:
Lê os dados de novos registros e preenche cada campo de uma struct Registro. 
Para cada inserção, ela verifica se existe espaço disponível na pilha de registros removidos; 
se existir, reutiliza esse espaço, caso contrário insere no fim do arquivo. 
Depois, escreve o registro no binário e atualiza o header com as informações necessárias. 
*/

void inserir(char* nome_arquivo){

    // Abre um arquivo existente para leitura e escrita
    FILE* fp = fopen(nome_arquivo, "rb+");
    if (!verificarStatusArquivo(fp)) return;

    // Marca o header como inconsistente e coloca a struct na memória
    char inconsistente = '0';
    fseek(fp, 0, SEEK_SET); // Volta o ponteiro para o início
    fwrite(&inconsistente, sizeof(char), 1, fp); // Coloca '0' no primeiro byte
    Header* h = bin_to_header(fp); 

    // Lê quantas inserções serão feitas
    int n_insercoes;
    if (scanf("%d", &n_insercoes) != 1) {
        fclose(fp); 
        header_free(&h); 
        return;
    }

    char valor_str[50];     // Variável temporária para armazenar strings
    int valor_inteiro;      // Variável temporária para armazenar valores inteiros
    long int offset;        // Byte offset em que o registro será inserido
    int prox;               // Próximo elemento da pilha
    int topo;               // Topo da pilha
    int novosPares = 0;     // Número de novos pares inseridos

    // Faz n inserções
    while (n_insercoes--) {
        // Cria um novo registro
        Registro* reg = criar_registro();
        
        // Lê e preenche todos os campos do registro
        // Caso o campo seja um inteiro é utilizada a função scan_int
        // Caso o campo seja uma string é utilizada a função ScanQuoteString
        valor_inteiro = scan_int();
        reg_set_codEstacao(reg, valor_inteiro);
        
        ScanQuoteString(valor_str);
        reg_set_nomeEstacao(reg, valor_str);
        reg_set_tamNomeEstacao(reg, strlen(valor_str));

        valor_inteiro = scan_int();        
        reg_set_codLinha(reg, valor_inteiro);
        
        ScanQuoteString(valor_str);
        reg_set_nomeLinha(reg, valor_str);
        reg_set_tamNomeLinha(reg, strlen(valor_str));

        valor_inteiro = scan_int();
        reg_set_codProxEstacao(reg, valor_inteiro);
        // Se o código da próxima estação é válido, significa que um novo par válido e único foi inserido
        if(valor_inteiro != -1)
            novosPares ++;
        
        valor_inteiro = scan_int();
        reg_set_distProxEstacao(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_codLinhaIntegra(reg, valor_inteiro);
        
        valor_inteiro = scan_int();
        reg_set_codEstIntegra(reg, valor_inteiro);
        
        /*Coloca o registro no topo da pilha e atualiza o topo*/

        topo = header_get_topo(h); // Armazena o topo atual da pilha

        // Se o topo for -1 nenhum registro foi removido
        if(topo == -1){ 
            offset = RRN_to_offset(header_get_proxRRN(h)); // O registro deve ser inserido no próximo RRN
            header_set_proxRRN(h, header_get_proxRRN(h) + 1); // O próximo RRN é incrementado
        }
        // Caso algum registro tenha sido removido
        else{ 
            offset = RRN_to_offset(topo); // O offset é aquele correspondente ao RRN do topo da pilha
            fseek(fp, offset+1, SEEK_SET); // Coloca fp no segundo byte do registro removido (próximo elemento da pilha)
            fread(&prox, sizeof(int), 1, fp); // Lê o valor e coloca em prox
            header_set_topo(h, prox); // Atualiza o topo
        }

        reg_to_bin(reg, fp, offset); // Coloca o registro no binário
        reg_free(&reg); // Libera a memória alocada para o registro
    }

    // Salva o Header com o nroPares atualizado e consistente
    // Note que pelas especificações do projeto é garantido que nenhuma estação com um novo nome será inserido
    header_set_nroParesEstacao(h, header_get_nroParesEstacao(h) + novosPares);
    header_set_status(h, '1'); 
    header_to_bin(fp, h);
    header_free(&h);

    fclose(fp); // Fecha o arquivo
    BinarioNaTela(nome_arquivo);
}
