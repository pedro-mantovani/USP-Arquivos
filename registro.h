#ifndef REGISTRO_H
    #define REGISTRO_H

    typedef struct registro Registro;
    typedef struct header Header;

    Registro* criar_registro();

    Registro* ler_registro(FILE* f);

    void remover_registro(Registro* reg);
    void reg_set_proximo(Registro* reg, int proximo);
    void reg_set_codEstacao(Registro* reg, int codEstacao);
    void reg_set_codLinha(Registro* reg, int codLinha);
    void reg_set_codProxEstacao(Registro* reg, int codProxEstacao);
    void reg_set_distProxEstacao(Registro* reg, int distProxEstacao);
    void reg_set_codLinhaIntegra(Registro* reg, int codLinhaIntegra);
    void reg_set_codEstIntegra(Registro* reg, int codEstIntegra);
    void reg_set_tamNomeEstacao(Registro* reg, int tamNomeEstacao);
    void reg_set_nomeEstacao(Registro* reg, char* nomeEstacao);
    void reg_set_tamNomeLinha(Registro* reg, int tamNomeLinha);
    void reg_set_nomeLinha(Registro* reg, char* nomeLinha);

    char reg_get_removido(Registro* reg);
    int reg_get_proximo(Registro* reg);
    int reg_get_codEstacao(Registro* reg);
    int reg_get_codLinha(Registro* reg);
    int reg_get_codProxEstacao(Registro* reg);
    int reg_get_distProxEstacao(Registro* reg);
    int reg_get_codLinhaIntegra(Registro* reg);
    int reg_get_codEstIntegra(Registro* reg);
    int reg_get_tamNomeEstacao(Registro* reg);
    char* reg_get_nomeEstacao(Registro* reg);
    int reg_get_tamNomeLinha(Registro* reg);
    char* reg_get_nomeLinha(Registro* reg);

    void print_reg(Registro* reg);

    void reg_free(Registro** reg);

#endif